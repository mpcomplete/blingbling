#ifndef _WSCREEN_H
#define _WSCREEN_H

#include "main.h"

#include "pgwidget.h"

// all toplevels derive from this class
// derived include: MenuScreen, GameSingle, OptionScreen, ScoreScreen
class WScreen : public PG_Widget {
 protected:
  int prev_width;
  int prev_height;

 public:
  WScreen(PG_Widget* parent, const PG_Rect& r);
  virtual ~WScreen() {}

  // update the size of the widget and child widgets, usually based on
  // screen size
  virtual void resize();

  // execute a tick(), ms milliseconds have elapsed
  virtual void tick(int ms) {}

  virtual void eventShow();
};

#endif  // header guard
