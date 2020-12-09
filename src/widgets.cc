#include "widgets.h"
#include "app.h"
#include "gamesingle.h"
#include "scorescreen.h"
#include "utils.h"

static PG_Rect dummy(0, 0, 0, 0);

void delete_widget(void* data) {
  PG_Widget* w = (PG_Widget*)data;
  delete w;
}

bool widget_layout(PG_Widget* widget, int x, int y, int w, int h) {
  if (dynamic_cast<PG_Slider*>(widget))
    return false;

  // can't use MoveWidget - paragui has a bug.
  widget->MoveRect(x, y);
  widget->SizeWidget(w, h);

  return true;
}

void widget_zoom(PG_Widget* widget, double zoomx, double zoomy) {
  int px = 0, py = 0;

  if (widget->GetParent()) {
    px = widget->GetParent()->x;
    py = widget->GetParent()->y;
  }

  PG_Rect r(px + int((widget->x - px) * zoomx + 0.5),
            py + int((widget->y - py) * zoomy + 0.5),
            int(widget->w * zoomx + 0.5), int(widget->h * zoomy + 0.5));

  if (!widget_layout(widget, r.x, r.y, r.w, r.h))
    return;

  PG_RectList* children = widget->GetChildList();
  if (children) {
    PG_RectList::iterator it;
    for (it = children->begin(); it != children->end(); it++) {
      widget_zoom(*it, zoomx, zoomy);
    }
  }
}

void widget_font(PG_Widget* widget, double factor, bool recursive) {
  widget->SetFontSize(int(factor * app->get_font_size()), recursive);
}

// --- WSpinPref

static PARAGUI_CALLBACK(cb_spinpref_change) {
  WSpinPref* spinpref = (WSpinPref*)clientdata;
  PrefInt* pref = spinpref->pref;
  static unsigned sfx_delay = 0;

  pref->val = (int)data;

  if (pref == &prefs.music_volume) {
    app->update_music();
  }
  if (pref == &prefs.sfx_volume) {
    if (app->get_ms() >= sfx_delay) {
      app->sfx_clear.play_sound(SOUND_CLEARED(1));
      sfx_delay = app->get_ms() + 1000;
    }
  }

  spinpref->call_callback();

  return true;
}

WSpinPref::WSpinPref(PG_Widget* parent,
                     const PG_Rect& rect,
                     char* txt,
                     PrefInt* ipref)
    : PG_Widget(parent, rect), pref(ipref), cbfunc(NULL) {
  label = new PG_Label(this, PG_Rect(0, 0, rect.w / 2, rect.h), txt);
  spin = new PG_SpinnerBox(
      this, PG_Rect(w * 80 / 100, h * 1 / 8, w * 20 / 100, h * 3 / 4));

  spin->SetMask(" ## ");
  spin->SetMinValue(pref->min);
  spin->SetMaxValue(pref->max);
  spin->SetValue(pref->val);
  spin->SetEventCallback(MSG_SPINNER_CHANGE, cb_spinpref_change, this);
}

void WSpinPref::update() {
  spin->SetValue(pref->val);
}

void WSpinPref::set_active(bool active) {
  if (!active) {
    spin->SetMinValue(spin->GetValue());
    spin->SetMaxValue(spin->GetValue());
    this->SetFontAlpha(100, true);
  } else {
    spin->SetMinValue(pref->min);
    spin->SetMaxValue(pref->max);
    this->SetFontAlpha(255, true);
  }
}

// --- WSlidePref

static PARAGUI_CALLBACK(cb_slidepref_change) {
  WSlidePref* slidepref = (WSlidePref*)clientdata;
  PrefInt* pref = slidepref->pref;
  static unsigned sfx_delay = 0;

  pref->val = (int)data;

  if (pref == &prefs.music_volume) {
    app->update_music();
  }
  if (pref == &prefs.sfx_volume) {
    if (app->get_ms() >= sfx_delay) {
      app->sfx_clear.play_sound(SOUND_CLEARED(1));
      sfx_delay = app->get_ms() + 1000;
    }
  }

  slidepref->call_callback();

  return true;
}

WSlidePref::WSlidePref(PG_Widget* parent,
                       const PG_Rect& rect,
                       char* txt,
                       PrefInt* ipref)
    : PG_Widget(parent, rect), pref(ipref), cbfunc(NULL) {
  label = new PG_Label(this, PG_Rect(0, 0, rect.w * 50 / 100, rect.h), txt);
  slide = NULL;
  this->eventSizeWidget(rect.w, rect.h);
}

void WSlidePref::update() {
  slide->SetPosition(pref->val);
}

void WSlidePref::eventSizeWidget(Uint16 w, Uint16 h) {
  // HUGE HACK TIME.  Resizing crashes for sliders, so I'll just make a
  // new object.  Quite lame.
  if (slide)
    delete slide;
  slide = new PG_Slider(
      this, 1, PG_Rect(w * 50 / 100, h * 40 / 100, w * 50 / 100, h * 20 / 100),
      PG_SB_HORIZONTAL);

  slide->SetRange(pref->min, pref->max);
  slide->SetPosition(pref->val);
  slide->SetEventCallback(MSG_SLIDE, cb_slidepref_change, this);
}

// --- WCredit

WCredit::WCredit(PG_Widget* parent,
                 const PG_Rect& rect,
                 char* credit,
                 char* who)
    : PG_Widget(parent, rect) {
  PG_Label* lab;
  lab = new PG_Label(this, PG_Rect(0, 0, rect.w, rect.h), credit);
  lab->SetAlignment(PG_TA_LEFT);
  lab = new PG_Label(this, PG_Rect(0, 0, rect.w, rect.h), who);
  lab->SetAlignment(PG_TA_RIGHT);
}

// --- WScoreWin

// is a high score
static PARAGUI_CALLBACK(cb_scoreok) {
  WScoreWin* win = (WScoreWin*)clientdata;

  if (win->get_text()[0]) {
    app->scores.insert(win->get_text(), win->score);
    app->scores.save();
    app->close_screen();
    app->change_screen(new ScoreScreen());
  }

  return true;
}

// not a high score
static PARAGUI_CALLBACK(cb_scorecancel) {
  app->close_screen();
  app->change_screen(new ScoreScreen());

  return true;
}

// check whether we're playing with default settings.
static bool has_defaults() {
  if (prefs.field_width.val != prefs.field_width.def)
    return false;
  if (prefs.field_height.val != prefs.field_height.def)
    return false;
  if (prefs.block_size.val != prefs.block_size.def)
    return false;

  return true;
}

// define lower limits on score ranges
#define MED_SCORE 100
#define HIGH_SCORE 1000

WScoreWin::WScoreWin(PG_Widget* parent, const PG_Rect& rect, int score)
    : PG_Window(parent, rect, "", WF_MODAL), score(score) {
  this->SetFontSize(int(0.5 * app->get_font_size()), true);

  string cash_str = "You raked in $" + to_string(score) + " cash money.";
  new PG_Label(this,
               PG_Rect(rect.w * 5 / 100, rect.h * 10 / 100, rect.w * 90 / 100,
                       rect.h * 20 / 100),
               cash_str.c_str());
  PG_Label* msg = new PG_Label(this,
                               PG_Rect(rect.w * 5 / 100, rect.h * 25 / 100,
                                       rect.w * 90 / 100, rect.h * 20 / 100),
                               "Message.");
  PG_Button* ok = new PG_Button(this, BTN_ID_OK,
                                PG_Rect(rect.w * 60 / 100, rect.h * 80 / 100,
                                        rect.w * 30 / 100, rect.h * 15 / 100),
                                "OK");
  ok->SetFontSize(int(0.7 * app->get_font_size()));

  bool defaults = has_defaults();
  if (defaults && app->scores.is_high(score)) {
    this->SetTitle("Congratulations!");

    if (score < MED_SCORE) {
      msg->SetText("Eh, that's pretty good... I guess.");
    } else if (score < HIGH_SCORE) {
      msg->SetText("That's a pretty tight haul.");
    } else {
      msg->SetText("Damn, now we're talkin' bling bling!");
    }
    ok->SetText("ChaChing!");

    PG_Button* cancel =
        new PG_Button(this, BTN_ID_CANCEL,
                      PG_Rect(rect.w * 10 / 100, rect.h * 80 / 100,
                              rect.w * 30 / 100, rect.h * 15 / 100),
                      "Nevermind");
    cancel->SetFontSize(int(0.7 * app->get_font_size()));

    new PG_Label(this,
                 PG_Rect(rect.w * 5 / 100, rect.h * 55 / 100, rect.w * 20 / 100,
                         rect.h * 20 / 100),
                 "Name:");
    edit = new PG_LineEdit(this, PG_Rect(rect.w * 30 / 100, rect.h * 55 / 100,
                                         rect.w * 60 / 100, rect.h * 15 / 100));

    edit->EditBegin();
    edit->SetEventCallback(MSG_RETURN, cb_scoreok, this);
    ok->SetEventCallback(MSG_BUTTONCLICK, cb_scoreok, this);
    cancel->SetEventCallback(MSG_BUTTONCLICK, cb_scorecancel, this);
  } else {
    this->SetTitle("Loser!");

    if (!defaults) {
      msg->SetText("No high score for you, cheater.");
      ok->SetText("That's coo.");
    } else if (rand_int(0, 1)) {
      msg->SetText("Sucks to be you.");
      ok->SetText(rand_int(0, 1) ? "Sure does." : "I Know.");
    } else {
      msg->SetText("Just give up already.");
      ok->SetText("Aight.");
    }

    ok->SetEventCallback(MSG_BUTTONCLICK, cb_scorecancel, this);
  }
}

// --- WFloatScore

WFloatScore::WFloatScore(PG_Widget* parent,
                         int gain,
                         const PG_Rect& ifrom,
                         const PG_Rect& ito,
                         int ihover,
                         int iduration)
    : PG_Label(parent, dummy, string("+" + to_string(gain) + "!").c_str()),
      from(ifrom),
      to(ito),
      hover(ihover),
      duration(iduration),
      timer(0) {
  this->LoadThemeStyle("FloatScore");
  this->tick(0);
}

void WFloatScore::tick(int ms) {
  if (!app->playing()) {
    // only tick if they're watching
    return;
  }

  timer += ms;
  if (timer > (duration + hover)) {
    // Ok because we only create these on the heap. (Is there a better
    // way to do this?)
    delete this;
    return;
  }

  PG_Rect current = from;
  double fontp = 2.0;

  if (timer > hover) {
    double percent = double(timer - hover) / double(duration);
    if (percent > 1.0)
      percent = 1.0;

    current.x = int((1.0 - percent) * from.x + percent * to.x);
    current.y = int((1.0 - percent) * from.y + percent * to.y);
    current.w = int((1.0 - percent) * from.w + percent * to.w);
    current.h = int((1.0 - percent) * from.h + percent * to.h);
  }

  widget_layout(this, XP(current.x), YP(current.y), XP(current.w),
                YP(current.h));
  widget_font(this, fontp);
  app->request_refresh();
}
