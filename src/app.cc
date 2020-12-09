#include "app.h"
#include "gamesingle.h"
#include "helpscreen.h"
#include "menuscreen.h"
#include "parser.h"
#include "prefs.h"
#include "ticker.h"
#include "utils.h"

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#define MIN_DELAY 10

// screen size in sprites
#define SCREEN_WIDTH_TILES (prefs.field_width.val * 2)
#define SCREEN_HEIGHT_TILES (prefs.field_height.val * 1.2)
// screen size in pixels
#define SCREEN_WIDTH_PX (prefs.tile_size.val * SCREEN_WIDTH_TILES)
#define SCREEN_HEIGHT_PX (prefs.tile_size.val * SCREEN_HEIGHT_TILES)

App::App() {
  audio_open = false;
  music = NULL;

  idlefunc = NULL;
  wscreen = NULL;
  m_game = NULL;

#ifdef WIN32
  srand(time(0));
#else
  srand(time(0) ^ getpid());
#endif

#ifdef DEBUG
  PG_LogConsole::SetLogLevel(PG_LOG_DBG);
#else
  PG_LogConsole::SetLogLevel(PG_LOG_ERR);
#endif

  this->AddArchive(get_data_dir().c_str());
  this->AddArchive("bling.zip");
  this->SetWriteDir(this->GetUserDir());
  this->EnableAppIdleCalls(true);
  this->LoadTheme("bling");
}

int App::run() {
  prefs.init();

  if (init_video() != 0)
    return 1;
  init_audio();

  //    SDL_EnableKeyRepeat(100, 20);
  SDL_EnableKeyRepeat(0, 0);

  load_config();
  scores.load();
  prefs.clamp();

  this->SetFontSize(this->get_font_size());

  this->change_screen(new MenuScreen());

  this->elapsed_ms();

  this->Run();
  return 0;
}

int App::init_video() {
  if (!InitScreen(int(SCREEN_WIDTH_PX), int(SCREEN_HEIGHT_PX), 0,
                  SDL_SWSURFACE | SDL_RESIZABLE))
    return 1;

  return 0;
}

int App::init_audio() {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    PG_LogERR("Couldn't initialize audio: %s", SDL_GetError());
    return 1;
  }

  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    PG_LogERR("Couldn't open audio: %s", Mix_GetError());
    return 1;
  }

  audio_open = true;

  return 0;
}

void App::change_screen(WScreen* mnew) {
  if (wscreen) {
    wscreen->Hide();
    if (wscreen != m_game) {
      set_idle_func(delete_widget, wscreen);
    }
  }

  wscreen = mnew ? mnew : new MenuScreen();
  wscreen->Show();
  wscreen->SetInputFocus();
}

void App::change_screen(GameSingle* m) {
  if (m_game && m_game != m) {
    delete m_game;
  }
  m_game = m;
  this->resize(GetScreenWidth(), GetScreenHeight());
  change_screen((WScreen*)m);
}

void App::close_screen() {
  if (wscreen) {
    wscreen->Hide();
    if (wscreen == m_game)
      m_game = NULL;
    set_idle_func(delete_widget, wscreen);
    wscreen = NULL;
  }
}

void App::update_music() {
  Mix_VolumeMusic(prefs.music_volume.val);
}

int App::get_font_size() {
  int fwidth = XP(prefs.font_size.val);
  int fheight = YP(prefs.font_size.val);

  return MIN(fwidth, fheight);
}

void App::Quit() {
  scores.save();
  prefs.save();

  PG_Application::Quit();
}

bool App::resize(int width, int height) {
  // take whichever gives us the smaller dimensions
  int bestw = width;
  int besth = int(width * SCREEN_HEIGHT_PX / SCREEN_WIDTH_PX);
  if (besth > height) {
    besth = height;
    bestw = int(height * SCREEN_WIDTH_PX / SCREEN_HEIGHT_PX);
  }

  if (bestw == GetScreenWidth() && besth == GetScreenHeight()) {
    return true;
  }

  SDL_Surface* new_screen = SDL_SetVideoMode(
      bestw, besth, GetScreen()->format->BitsPerPixel, GetScreen()->flags);
  this->SetScreen(new_screen);

  prefs.tile_size.val = int(besth / SCREEN_HEIGHT_TILES);
  sprites.reload(prefs.tile_size.val, prefs.tile_size.val);
  sprites_connect.reload(prefs.tile_size.val, prefs.tile_size.val);

  this->SetFontSize(this->get_font_size());

  wscreen->resize();
  if (m_game && m_game != wscreen)
    m_game->resize();

  this->request_refresh();

  return true;
}

void App::tick(int ms) {
  Ticker::iterator it, tmp;
  for (it = Ticker::ticker_list.begin(); it != Ticker::ticker_list.end();) {
    /**
     * @NOTE We must increment the iterator first, because the ticker
     * could possibly remove itself from the ticker_list upon tick(),
     * thus invalidating the iterator.
     */
    tmp = it++;
    (*tmp)->tick(ms);
  }

  if (wscreen) {
    wscreen->tick(ms);
    if (this->need_refresh()) {
      this->request_refresh(false);

      wscreen->Redraw();
      this->FlipPage();
    }
  }

  if (idlefunc) {
    idlefunc(idledata);
    idlefunc = NULL;
    idledata = NULL;
  }

  SDL_Delay(MIN_DELAY);
}

bool App::eventResize(const SDL_ResizeEvent* event) {
  this->resize(event->w, event->h);
  return true;
}

int App::elapsed_ms() {
  static int last = -1;
  int tmp = last;
  int now = get_ms();

  last = now;
  if (tmp == -1)
    return 0;
  return (now - tmp);
}

void App::load_config() {
  sprites.set_size(prefs.tile_size.val, prefs.tile_size.val);
  sprites_connect.set_size(prefs.tile_size.val, prefs.tile_size.val);

  Parser parser(BLINGBLING_CONF);
  if (has_audio()) {
    parser.set_handler("music", this, &App::cmd_music);
  }
  parser.set_handler("speedupsound", &sfx, &SoundSet::cmd_sound_speedup);
  parser.set_handler("tiledumpsound", &sfx, &SoundSet::cmd_sound_tiledump);
  parser.set_handler("locksound", &sfx, &SoundSet::cmd_sound_lock);
  parser.set_handler("clearsound", &sfx_clear, &SoundSet::cmd_sound_cleared);
  parser.set_handler("tile", &sprites, &SpriteSet::cmd_tile);
  parser.set_handler("connecttile", &sprites_connect, &SpriteSet::cmd_tile);
  parser.parse_file();

  // kind of a hack, but I need to set the upper limit on num_types based
  // on the number of sprites.  I can't do it upon prefs.init() because I
  // need to call that before this.
  prefs.num_types.def = prefs.num_types.max = NUM_COLORS;
}

int App::cmd_music(const ParserCmd& cmd) {
  string file = get_data_dir() + cmd.get_str_arg(1);
  if ((music = Mix_LoadMUS(file.c_str())) == NULL) {
    PG_LogWRN("Couldn't load background music: %s", Mix_GetError());
    return 1;
  }

  this->update_music();
  Mix_PlayMusic(music, -1);

  return 0;
}
