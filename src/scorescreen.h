#ifndef _SCORESCREEN_H
#define _SCORESCREEN_H

#include "main.h"
#include "scorelist.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"
#include "pgspinnerbox.h"

#include <vector>

class ScoreScreen : public WScreen {
  PG_Label title;
  PG_Label names_title;
  PG_Label scores_title;
  vector<PG_Label*> numbers;
  vector<PG_Label*> names;
  vector<PG_Label*> scores;
  PG_Label ltypes_left;
  PG_SpinnerBox stypes_spin;
  PG_Button bmenu;

  int cur_types;

 public:
  ScoreScreen(int num_types = -1);

  void change_to(int num_types);

  virtual void resize();
  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);
};

#endif  // header guard
