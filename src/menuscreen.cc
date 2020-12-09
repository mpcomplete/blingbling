#include "menuscreen.h"
#include "app.h"
#include "creditscreen.h"
#include "helpscreen.h"
#include "newgamescreen.h"
#include "optionscreen.h"
#include "prefs.h"
#include "scorescreen.h"
#include "widgets.h"

#include "pgmessagebox.h"

static PARAGUI_CALLBACK(cb_resumegame) {
  if (app->game()) {
    app->change_screen(app->game());
  }
  return true;
}

static PARAGUI_CALLBACK(cb_singlegame) {
  if (app->game()) {
    PG_Rect r(XP(10), YP(30), XP(80), YP(30));
    PG_MessageBox* confirm = new PG_MessageBox(
        NULL, r, "New game",
        "\nThis will end your current game.  Are you sure?",
        PG_Rect(r.w * 10 / 100, r.h * 70 / 100, r.w * 30 / 100, r.h * 20 / 100),
        "Yes",
        PG_Rect(r.w * 60 / 100, r.h * 70 / 100, r.w * 30 / 100, r.h * 20 / 100),
        "No");
    confirm->Show();
    int button = confirm->WaitForClick();
    delete confirm;

    if (button == 2)  // No
      return true;
    // else, Yes...
  }
  app->change_screen(new NewGameScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_options) {
  app->change_screen(new OptionScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_highscores) {
  app->change_screen(new ScoreScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_help) {
  app->change_screen(new HelpScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_credits) {
  app->change_screen(new CreditScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_exit) {
  app->Quit();
  return true;
}

#define BUTTONWP 40
#define DYP 7
#define SYP TITLEYP + 28

MenuScreen::MenuScreen()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      ltitle(this, RECTP(0, TITLEYP, 100, 20), "BlingBling"),
      bresumegame(this,
                  0,
                  RECTP(30, SYP + DYP * 0, BUTTONWP, BUTTONHP),
                  "Resume Game"),
      bsinglegame(this,
                  0,
                  RECTP(30, SYP + DYP * 1, BUTTONWP, BUTTONHP),
                  "New Game"),
      boptions(this,
               0,
               RECTP(30, SYP + DYP * 2, BUTTONWP, BUTTONHP),
               "Options"),
      bhighscores(this,
                  0,
                  RECTP(30, SYP + DYP * 3, BUTTONWP, BUTTONHP),
                  "High Scores"),
      bhelp(this, 0, RECTP(30, SYP + DYP * 4, BUTTONWP, BUTTONHP), "Help"),
      bcredits(this,
               0,
               RECTP(30, SYP + DYP * 5, BUTTONWP, BUTTONHP),
               "Credits"),
      bexit(this, 0, RECTP(30, SYP + DYP * 6, BUTTONWP, BUTTONHP), "Exit") {
  ltitle.LoadThemeStyle("Title");

  if (!app->game()) {
    bresumegame.SizeWidget(0, 0);
  }

  bresumegame.SetEventCallback(MSG_BUTTONCLICK, cb_resumegame, NULL);
  bsinglegame.SetEventCallback(MSG_BUTTONCLICK, cb_singlegame, NULL);
  boptions.SetEventCallback(MSG_BUTTONCLICK, cb_options, NULL);
  bhighscores.SetEventCallback(MSG_BUTTONCLICK, cb_highscores, NULL);
  bhelp.SetEventCallback(MSG_BUTTONCLICK, cb_help, NULL);
  bcredits.SetEventCallback(MSG_BUTTONCLICK, cb_credits, NULL);
  bexit.SetEventCallback(MSG_BUTTONCLICK, cb_exit, NULL);

  this->resize();
}

void MenuScreen::resize() {
  WScreen::resize();

  widget_font(&ltitle, 4, true);
}

bool MenuScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
  switch (key->keysym.sym) {
    case SDLK_ESCAPE:
      app->Quit();
      return true;
      break;
    default:
      break;
  }
  return true;
}
