#include "newgamescreen.h"
#include "app.h"
#include "gamesingle.h"
#include "menuscreen.h"
#include "prefs.h"
#include "utils.h"

extern bool is_standard_game();

static PARAGUI_CALLBACK(cb_menu) {
  prefs.save();
  app->change_screen(new MenuScreen());

  return true;
}

static PARAGUI_CALLBACK(cb_start) {
  prefs.save();
  app->change_screen(new GameSingle());

  return true;
}

static PARAGUI_CALLBACK(cb_standards) {
  NewGameScreen* newgame = (NewGameScreen*)clientdata;

  prefs.block_size.reset();
  prefs.field_width.reset();
  prefs.field_height.reset();
  newgame->update();

  return true;
}

static void cb_check_standard(void* clientdata) {
  NewGameScreen* newgame = (NewGameScreen*)clientdata;

  newgame->set_standard(is_standard_game());
}

#define DYP 7
#define SYP 20

NewGameScreen::NewGameScreen()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      ltitle(this, RECTP(0, TITLEYP, 100, 15), "New Game"),
      bstart(this, 0, RECTP(35, SYP + DYP * 0, 30, BUTTONHP), "Start Game"),

      snum_types(this,
                 RECTP(5, SYP + DYP * 1, 90, DYP),
                 "Difficulty level:",
                 &prefs.num_types),

      lfun(this, RECTP(30, SYP + DYP * 3, 90, DYP), "Just-for-fun options:"),
      bstandards(this, 0, RECTP(5, 90, 45, BUTTONHP), "Use Standard Settings"),
      sblock_size(this,
                  RECTP(5, SYP + DYP * 4, 90, DYP),
                  "Tiles per Block:",
                  &prefs.block_size),
      sfield_width(this,
                   RECTP(5, SYP + DYP * 5, 90, DYP),
                   "Field Width:",
                   &prefs.field_width),
      sfield_height(this,
                    RECTP(5, SYP + DYP * 6, 90, DYP),
                    "Field Height:",
                    &prefs.field_height),
      lnote1(this,
             RECTP(5, SYP + DYP * 7, 90, DYP),
             "Note: you will not be eligible for the high score list"),
      lnote2(this,
             RECTP(5, SYP + DYP * 7 + 5, 90, DYP),
             "unless you play with standard for-fun options."),

      bmenu(this, 0, RECTP(80, 90, 15, BUTTONHP), "Menu") {
  ltitle.LoadThemeStyle("Title");

  bstandards.SetEventCallback(MSG_BUTTONCLICK, cb_standards, this);
  bstart.SetEventCallback(MSG_BUTTONCLICK, cb_start, NULL);
  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  sblock_size.set_spin_callback(cb_check_standard, this);
  sfield_width.set_spin_callback(cb_check_standard, this);
  sfield_height.set_spin_callback(cb_check_standard, this);

  this->resize();
  this->set_standard(is_standard_game());
}

void NewGameScreen::resize() {
  WScreen::resize();

  widget_font(&ltitle, 3);
  widget_font(&lnote1, 0.9);
  widget_font(&lnote2, 0.9);
}

bool NewGameScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
  switch (key->keysym.sym) {
    case SDLK_ESCAPE:
      prefs.save();
      app->change_screen(new MenuScreen());
      return true;
      break;
    case SDLK_RETURN:
      prefs.save();
      app->change_screen(new GameSingle());
      return true;
      break;
    default:
      break;
  }
  return true;
}

void NewGameScreen::update() {
  sblock_size.update();
  sfield_width.update();
  sfield_height.update();
}

void NewGameScreen::set_standard(bool val) {
  if (val) {
    bstart.SetText("Start Game");
    widget_layout(&bstart, XP(35), YP(SYP + DYP * 0), XP(30), YP(BUTTONHP));
  } else {
    bstart.SetText("Start Non-standard Game");
    widget_layout(&bstart, XP(25), YP(SYP + DYP * 0), XP(50), YP(BUTTONHP));
  }

  app->request_refresh();
}
