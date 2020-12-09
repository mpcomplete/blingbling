#include "gamesingle.h"
#include "app.h"
#include "menuscreen.h"
#include "prefs.h"
#include "widgets.h"

#include "pgwindow.h"

#define MIN_DELAY 10

static PARAGUI_CALLBACK(cb_menu) {
  app->change_screen(new MenuScreen());
  return true;
}

bool is_standard_game() {
  return (prefs.block_size.is_default() && prefs.field_width.is_default() &&
          prefs.field_height.is_default());
}

#define DYP 7
#define LWIDTHP 40
#define SYP 47

GameSingle::GameSingle()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      wfield(this,
             RECTP(5, 5, 0, 0),
             prefs.field_width.val,
             prefs.field_height.val),
      wnextblock(this, RECTP(60, 5, 0, 0)),
      lscore(this, RECTP(60, SYP + DYP * 0, LWIDTHP, DYP), "BlingBling: $0"),
      lspeed(this, RECTP(60, SYP + DYP * 1, LWIDTHP, DYP), "Speed: 1"),
      lcombo(this, RECTP(60, SYP + DYP * 2, LWIDTHP, DYP), "Best Combo: 0"),
      ldifficulty(this, RECTP(60, SYP + DYP * 3, LWIDTHP, DYP), "Difficulty: "),
      whazard(this, RECTP(5, 90, 0, 0)),
      bmenu(this, 1, RECTP(80, 90, 15, 6), "Menu") {
  paused = false;

  last_score = 0;
  last_combo = 0;
  last_speed = 0;

  scorewin = NULL;

  if (!is_standard_game()) {
    new PG_Label(this, RECTP(60, SYP + DYP * 4, LWIDTHP, DYP),
                 "Non-standard game");
  }

  ldifficulty.SetTextFormat("Difficulty: %d", prefs.num_types.val);
  wnextblock.set_block(wfield.get_next());

  whazard.SetDrawPercentage(false);
  whazard.LoadThemeStyle("Hazard");

  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  this->resize();

  for (int i = 0; i < NUM_KEYS; i++) {
    key_delay[i] = 0;
  }
}

GameSingle::~GameSingle() {
  if (scorewin) {
    scorewin->Hide();
    delete scorewin;
  }
}

void GameSingle::floatscore_new(int gain) {
  // these are specified in percentages
  int scorexp = (lscore.my_xpos * 100) / XP(100);
  int scoreyp = (lscore.my_ypos * 100) / YP(100);
  int scorehp = (lscore.my_height * 100) / YP(100);
  PG_Rect from(scorexp, scoreyp - 10, 40, 16);
  PG_Rect to(scorexp, scoreyp, 40, 16);

  WFloatScore* fs = new WFloatScore(this, gain, from, to, 750, 150);
  fs->Show();
  fs->start();
}

void GameSingle::resize() {
  WScreen::resize();

  wfield.resize();
  wnextblock.resize();
  whazard.SizeWidget(wfield.w, YP(4));
}

void GameSingle::tick(int ms) {
  if (wfield.game_over()) {
    if (scorewin == NULL) {
      scorewin = new WScoreWin(NULL, PG_Rect(XP(10), YP(30), XP(80), YP(40)),
                               wfield.get_score());
      scorewin->Show();
      bmenu.Hide();
    }
    return;
  }

  if (paused) {
    return;
  }

  unsigned curtime = app->get_ms();

#define HANDLE_KEY(i, delay, interval, func)                                   \
  if (key_delay[i] && curtime >= key_delay[i]) {                               \
    func;                                                                      \
    key_delay[i] = !key_handled[i] ? (curtime + delay) : (curtime + interval); \
    key_handled[i] = true;                                                     \
  }
#define HANDLE_KEY_ONCE(i, func)         \
  if (key_delay[i] && !key_handled[i]) { \
    func;                                \
    key_handled[i] = true;               \
  }

  HANDLE_KEY(KEY_DOWN, 0, 0, wfield.block_move_down());
  HANDLE_KEY(KEY_LEFT, 200, 0, wfield.block_move_left());
  HANDLE_KEY(KEY_RIGHT, 200, 0, wfield.block_move_right());
  HANDLE_KEY(KEY_DROP, 250, 200, wfield.block_lock());
  HANDLE_KEY_ONCE(KEY_ROTATE_RIGHT, wfield.block_rotate_right());
  HANDLE_KEY_ONCE(KEY_ROTATE_LEFT, wfield.block_rotate_left());

  wfield.tick(ms);

  // apparently SetTextFormat is expensive. only update when necessary
  if (last_score != wfield.get_score()) {
    last_score = wfield.get_score();
    lscore.SetTextFormat("BlingBling: $%d", last_score);
    app->request_refresh();
  }
  if (last_speed != wfield.get_speed()) {
    last_speed = wfield.get_speed();
    lspeed.SetTextFormat("Speed: %d", last_speed);
    app->request_refresh();
  }
  if (last_combo != wfield.get_best_combo()) {
    last_combo = wfield.get_best_combo();
    lcombo.SetTextFormat("Best Combo: %d", last_combo);
    app->request_refresh();
  }
  whazard.SetProgress(wfield.get_hazard_percent());
  wnextblock.set_block(wfield.get_next());
}

bool GameSingle::eventKeyDown(const SDL_KeyboardEvent* key) {
  if (scorewin) {
    // can't press keys if the score window is up
    return true;
  }

  if (key->keysym.sym == SDLK_ESCAPE) {
    app->change_screen(new MenuScreen());
    return true;
  }

  int sym = int(key->keysym.sym);
  for (int i = 0; i < NUM_KEYS; i++) {
    if (sym == prefs.key[i].val) {
      key_delay[i] = app->get_ms();
      key_handled[i] = false;
      break;
    }
  }

  return true;
}

bool GameSingle::eventKeyUp(const SDL_KeyboardEvent* key) {
  int sym = int(key->keysym.sym);
  for (int i = 0; i < NUM_KEYS; i++) {
    if (sym == prefs.key[i].val) {
      key_delay[i] = 0;
      break;
    }
  }
}
