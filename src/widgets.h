// Miscellaneous widgets

#ifndef _WIDGETS_H
#define _WIDGETS_H

#include "block.h"
#include "field.h"
#include "main.h"
#include "prefs.h"
#include "ticker.h"

#include "pgbutton.h"
#include "pglabel.h"
#include "pgslider.h"
#include "pgspinnerbox.h"
#include "pgwidget.h"
#include "pgwindow.h"

// x or y position based on % of window size
#define XP(x) int((x)*app->GetScreenWidth() / 100)
#define YP(y) int((y)*app->GetScreenHeight() / 100)

#define RECTP(x, y, w, h) PG_Rect(XP(x), YP(y), XP(w), YP(h))

// default button height (%)
#define BUTTONHP 6
// title y position (%)
#define TITLEYP 1

// deletes a widget
void delete_widget(void* data);
// moves and resizes a widget, returning true if its ok to resize the
// children.
bool widget_layout(PG_Widget* widget, int x, int y, int w, int h);
// zoom widget by zoomx,zoomy
void widget_zoom(PG_Widget* widget, double zoomx, double zoomy);
// set a widget's font as a factor of the default font
void widget_font(PG_Widget* widget, double factor, bool recursive = false);

// A spinner and a slider that are associated with a PrefInt
class WSpinPref : public PG_Widget {
 public:
  WSpinPref(PG_Widget* parent, const PG_Rect& rect, char* txt, PrefInt* pref);

  void update();
  void set_active(bool active);

  void call_callback() {
    if (cbfunc)
      (*cbfunc)(cbdata);
  }
  void set_spin_callback(BasicCallback* func, void* data = NULL) {
    cbfunc = func;
    cbdata = data;
  }

  PrefInt* pref;

 private:
  PG_Label* label;
  PG_SpinnerBox* spin;

  BasicCallback* cbfunc;
  void* cbdata;
};

class WSlidePref : public PG_Widget {
 public:
  WSlidePref(PG_Widget* parent, const PG_Rect& rect, char* txt, PrefInt* pref);

  void update();

  virtual void eventSizeWidget(Uint16 w, Uint16 h);

  void call_callback() {
    if (cbfunc)
      (*cbfunc)(cbdata);
  }
  void set_slide_callback(BasicCallback* func, void* data = NULL) {
    cbfunc = func;
    cbdata = data;
  }

  PrefInt* pref;

 private:
  PG_Label* label;
  PG_Slider* slide;

  BasicCallback* cbfunc;
  void* cbdata;
};

// A credit attribution.  It's just two labels, one on the left, one on the
// right.
class WCredit : public PG_Widget {
 public:
  WCredit(PG_Widget* parent, const PG_Rect& rect, char* credit, char* who);
};

// popup when player's game is over
class WScoreWin : public PG_Window {
  PG_LineEdit* edit;

 public:
  int score;
  WScoreWin(PG_Widget* parent, const PG_Rect& rect, int score);
  const char* get_text() { return edit->GetText(); }
};

// a floating score widget
class WFloatScore : public PG_Label, public Ticker {
 public:
  WFloatScore(PG_Widget* parent,
              int gain,
              const PG_Rect& from,
              const PG_Rect& to,
              int hover,
              int duration);

  // update position and size after ms milliseconds
  void tick(int ms);

 protected:
  // protected to make sure we only create floatscores on the heap
  ~WFloatScore() {}

 private:
  PG_Rect from;  // starting point, in percents
  PG_Rect to;    // ending point, in percents
  int hover;     // how long to hover before moving (milliseconds)
  int duration;  // how long it will take to animate from 'from' to 'to'
                 // (milliseconds)
  int timer;     // current time (milliseconds)
};

#endif  // header guard
