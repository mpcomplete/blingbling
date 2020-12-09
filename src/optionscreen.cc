#include "optionscreen.h"
#include "app.h"
#include "menuscreen.h"
#include "prefs.h"
#include "utils.h"

static PARAGUI_CALLBACK(cb_menu) {
  prefs.save();
  app->change_screen(new MenuScreen());

  return true;
}

static PARAGUI_CALLBACK(cb_key) {
  OptionScreen* options = (OptionScreen*)clientdata;

  if (options->key_wait != -1) {
    options->labels[options->key_wait]->SetText(
        capitalize(SDL_GetKeyName(SDLKey(prefs.key[options->key_wait].val)))
            .c_str());
  }

  options->key_wait = id;
  options->labels[id]->SetText("Press a options");

  return true;
}

static PARAGUI_CALLBACK(cb_defaults) {
  OptionScreen* options = (OptionScreen*)clientdata;

  for (int i = 0; i < NUM_KEYS; i++) {
    prefs.key[i].reset();
    options->labels[i]->SetText(
        capitalize(SDL_GetKeyName(SDLKey(prefs.key[i].val))).c_str());
  }
  options->key_wait = -1;
  return true;
}

#define DYP 7
#define SYP 16
#define SYP2 (SYP + DYP * 3 + 7)

OptionScreen::OptionScreen()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      ltitle(this, RECTP(0, TITLEYP, 100, 15), "Options"),
      smusic_volume(this,
                    RECTP(5, SYP + DYP * 0, 90, DYP),
                    "Music Volume:",
                    &prefs.music_volume),
      ssfx_volume(this,
                  RECTP(5, SYP + DYP * 1, 90, DYP),
                  "Sound Effects Volume:",
                  &prefs.sfx_volume),
      lnote(this, RECTP(40, SYP2 - DYP - 2, 25, DYP), "Key Settings:"),
      bdefaults(this, 1, RECTP(10, 90, 40, BUTTONHP), "Reset Defaults"),
      bmenu(this, 0, RECTP(80, 90, 15, BUTTONHP), "Menu") {
  ltitle.LoadThemeStyle("Title");

  key_wait = -1;

  for (int i = 0; i < NUM_KEYS; i++) {
    buttons[i] =
        new PG_Button(this, i, RECTP(5, SYP2 + DYP * i, 35, BUTTONHP), "");
    labels[i] = new PG_Label(
        this, RECTP(50, SYP2 + DYP * i, 45, BUTTONHP),
        capitalize(SDL_GetKeyName(SDLKey(prefs.key[i].val))).c_str());
    labels[i]->SetAlignment(PG_TA_RIGHT);
    buttons[i]->SetEventCallback(MSG_BUTTONCLICK, cb_key, this);
  }
  buttons[KEY_DOWN]->SetText("Move Block Down");
  buttons[KEY_LEFT]->SetText("Move Block Left");
  buttons[KEY_RIGHT]->SetText("Move Block Right");
  buttons[KEY_ROTATE_LEFT]->SetText("Rotate Block Left");
  buttons[KEY_ROTATE_RIGHT]->SetText("Rotate Block Right");
  buttons[KEY_DROP]->SetText("Drop Block");

  bdefaults.SetEventCallback(MSG_BUTTONCLICK, cb_defaults, this);
  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  this->resize();
}

void OptionScreen::resize() {
  WScreen::resize();

  widget_font(&ltitle, 3);
}

bool OptionScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
  if (key_wait != -1) {
    prefs.key[key_wait].val = int(key->keysym.sym);
    labels[key_wait]->SetText(
        capitalize(SDL_GetKeyName(key->keysym.sym)).c_str());
    key_wait = -1;
    return true;
  }

  switch (key->keysym.sym) {
    case SDLK_ESCAPE:
      prefs.save();
      app->change_screen(new MenuScreen());
      return true;
      break;
    default:
      break;
  }
  return true;
}
