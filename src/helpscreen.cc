#include "helpscreen.h"
#include "app.h"
#include "menuscreen.h"
#include "parser.h"
#include "widgets.h"

static PARAGUI_CALLBACK(cb_prev) {
  HelpScreen* help = (HelpScreen*)clientdata;
  help->show_page(help->get_curpage() - 1);
  return true;
}

static PARAGUI_CALLBACK(cb_next) {
  HelpScreen* help = (HelpScreen*)clientdata;
  help->show_page(help->get_curpage() + 1);
  return true;
}

static PARAGUI_CALLBACK(cb_menu) {
  app->change_screen(new MenuScreen());
  return true;
}

HelpScreen::HelpScreen()
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      bprev(this, 1, RECTP(10, 92, 15, BUTTONHP), "Back"),
      bnext(this, 1, RECTP(30, 92, 15, BUTTONHP), "Next"),
      bmenu(this, 1, RECTP(80, 92, 15, BUTTONHP), "Menu") {
  helppages.set_size(my_width, my_height);

  Parser parser(BLINGBLING_CONF);
  parser.set_handler("helppage", &helppages, &SpriteSet::cmd_tile);
  parser.parse_file();

  bprev.SetEventCallback(MSG_BUTTONCLICK, cb_prev, this);
  bnext.SetEventCallback(MSG_BUTTONCLICK, cb_next, this);
  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  this->show_page(0);
}

void HelpScreen::show_page(int n) {
  if (n >= 0 && n < helppages.num_tiles()) {
    app->request_refresh();
    curpage = n;
  }
}

void HelpScreen::resize() {
  WScreen::resize();

  if (my_width == helppages.width && my_height == helppages.height) {
    return;
  }

  helppages.reload(my_width, my_height);
}

void HelpScreen::eventBlit(SDL_Surface* surf,
                           const PG_Rect& src,
                           const PG_Rect& dst) {
  if (curpage >= helppages.num_tiles()) {
    return;
  }

  helppages.blit_tile(curpage, 0, GetScreenSurface(), dst.x, dst.y);
}

bool HelpScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
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
