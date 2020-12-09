#ifndef _CREDITSCREEN_H
#define _CREDITSCREEN_H

#include "main.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"

class CreditScreen : public WScreen {
 protected:
  PG_Label ltitle;
  PG_Button bmenu;

 public:
  CreditScreen();
  virtual ~CreditScreen() {}

  virtual void resize();

  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);
};

#endif  // header guard
