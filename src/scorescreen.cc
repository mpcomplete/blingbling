#include "scorescreen.h"
#include "app.h"
#include "menuscreen.h"
#include "prefs.h"
#include "utils.h"
#include "widgets.h"

static PARAGUI_CALLBACK(cb_menu) {
  app->change_screen(new MenuScreen());
  return true;
}

static PARAGUI_CALLBACK(cb_types_spin) {
  ScoreScreen* screen = (ScoreScreen*)clientdata;
  screen->change_to(data);
  return true;
}

#define HP 8
#define STARTYP 16
#define DYP 6

ScoreScreen::ScoreScreen(int num_types)
    : WScreen(NULL, RECTP(0, 0, 100, 100)),
      title(this, RECTP(0, TITLEYP, 100, 15), "High Scores"),
      names_title(this, RECTP(10, STARTYP, 40, HP), "Names"),
      scores_title(this, RECTP(60, STARTYP, 40, HP), "Bling Bling"),
      ltypes_left(this, RECTP(5, 90, 40, HP), "Scores for difficulty:"),
      stypes_spin(this, RECTP(42, 90, 20, HP)),
      bmenu(this, 0, RECTP(80, 90, 16, BUTTONHP), "Menu"),
      cur_types(-1) {
  if (num_types < 0) {
    num_types = prefs.num_types.val;
  }

  title.LoadThemeStyle("Title");

  stypes_spin.SetMinValue(prefs.num_types.min);
  stypes_spin.SetMaxValue(prefs.num_types.max);
  stypes_spin.SetMask(" ## ");
  stypes_spin.SetValue(num_types);
  stypes_spin.SetEventCallback(MSG_SPINNER_CHANGE, cb_types_spin, this);

  bmenu.SetEventCallback(MSG_BUTTONCLICK, cb_menu, NULL);

  int i;
  for (i = 1; i <= NUM_HIGHSCORES; i++) {
    string num = to_string(i) + '.';
    numbers.push_back(
        new PG_Label(this, RECTP(2, STARTYP + DYP * i, 8, HP), num.c_str()));
  }

  this->change_to(num_types);

  this->resize();
}

void ScoreScreen::change_to(int num_types) {
  if (num_types == cur_types) {
    return;
  }
  if (num_types < prefs.num_types.min || num_types > prefs.num_types.max) {
    return;
  }
  cur_types = num_types;

  for (size_t i = 0; i < names.size(); i++) {
    delete names[i];
    delete scores[i];
  }
  names.clear();
  scores.clear();

  ScoreList::iterator it;
  size_t i = 1;
  for (it = app->scores.begin(num_types); it != app->scores.end(num_types);
       it++, i++) {
    string score = '$' + to_string((*it).score);
    PG_Label* lname = new PG_Label(this, RECTP(10, STARTYP + DYP * i, 40, HP),
                                   (*it).name.c_str());
    PG_Label* lscore =
        new PG_Label(this, RECTP(60, STARTYP + DYP * i, 40, HP), score.c_str());
    lname->Show();
    lscore->Show();
    names.push_back(lname);
    scores.push_back(lscore);
  }
}

void ScoreScreen::resize() {
  WScreen::resize();

  widget_font(&title, 3);
}

bool ScoreScreen::eventKeyDown(const SDL_KeyboardEvent* key) {
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
