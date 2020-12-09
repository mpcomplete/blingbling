#include "creditscreen.h"
#include "app.h"
#include "menuscreen.h"
#include "widgets.h"

static PARAGUI_CALLBACK(cb_menu) {
  app->change_screen(new MenuScreen());

  return true;
}

#define LABELHP 7
#define SYP TITLEYP + 16
#define DYP 7

CreditScreen::CreditScreen()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      ltitle(this, RECTP(0, TITLEYP, 100, 20), "Credits"),
      bmenu(this, 1, RECTP(80, 90, 15, BUTTONHP), "Menu") {
  ltitle.LoadThemeStyle("Title");

  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  PG_Label* lab = new PG_Label(this, RECTP(5, SYP + DYP * 0, 90, LABELHP),
                               "http://somewhere.fscked.org/blingbling/");
  lab->SetAlignment(PG_TA_CENTER);

  new WCredit(this, RECTP(5, SYP + DYP * 1, 90, LABELHP), "Producer",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 2, 90, LABELHP), "Chief Programmer",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 3, 90, LABELHP),
              "Assistant Programmer", "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 4, 90, LABELHP), "Team Coordinator",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 5, 90, LABELHP), "Graphic Design",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 6, 90, LABELHP), "Sound Effects",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 7, 90, LABELHP), "Composer",
              "Matt Perry");
  new WCredit(this, RECTP(5, SYP + DYP * 8, 90, LABELHP),
              "Testing & Moral Support", "Parisa Tabriz");
  new WCredit(this, RECTP(5, SYP + DYP * 9, 90, LABELHP), "Special Thanks To",
              "Matt Perry");

  this->resize();
}

void CreditScreen::resize() {
  WScreen::resize();

  widget_font(&ltitle, 3);
}

bool CreditScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
  switch (key->keysym.sym) {
    case SDLK_ESCAPE:
      app->change_screen(new MenuScreen());
      return true;
      break;
    default:
      break;
  }
  return true;
}
