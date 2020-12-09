#ifndef _WFIELD_H
#define _WFIELD_H

#include "block.h"
#include "field.h"
#include "main.h"

#include "pgwidget.h"

// Block displayer widget
class WBlock : public PG_Widget {
  Block* block;

 public:
  WBlock(PG_Widget* parent, PG_Rect rect);
  void set_block(Block* b) { block = b; }
  void resize();
  virtual void eventBlit(SDL_Surface* surf,
                         const PG_Rect& src,
                         const PG_Rect& dst);
};

// the playing field with an SDL display
class WField : public Field, public PG_Widget {
 public:
  WField(PG_Widget* parent, PG_Rect rect, int width = 6, int height = 12);
  void resize();
  virtual void eventDraw(SDL_Surface* surf, const PG_Rect& rect);
  virtual void eventBlit(SDL_Surface* surf,
                         const PG_Rect& src,
                         const PG_Rect& dst);
  virtual int add_score(int num_cleared);
};

#endif  // header guard
