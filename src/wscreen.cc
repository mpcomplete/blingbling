#include "wscreen.h"
#include "app.h"
#include "widgets.h"

WScreen::WScreen(PG_Widget* parent, const PG_Rect& r)
    : PG_Widget(parent, r, false) {
  prev_width = XP(100);
  prev_height = YP(100);
}

void WScreen::resize() {
  if (XP(100) == prev_width && YP(100) == prev_height)
    return;

  double zoomx = double(XP(100)) / double(prev_width);
  double zoomy = double(YP(100)) / double(prev_height);
  widget_zoom(this, zoomx, zoomy);
  widget_font(this, 1.0, true);

  prev_width = XP(100);
  prev_height = YP(100);
}

void WScreen::eventShow() {
  if (!GetChildList()) {
    return;
  }

  PG_RectList::iterator it;
  for (it = GetChildList()->begin(); it != GetChildList()->end(); it++) {
    (*it)->Show();
  }
}
