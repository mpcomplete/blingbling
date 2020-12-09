#include "app.h"
#include "gamesingle.h"
#include "scorescreen.h"
#include "utils.h"
#include "wscreen.h"

// --- WBlock

WBlock::WBlock(PG_Widget* parent, PG_Rect rect)
    : PG_Widget(parent, rect, false) {
  resize();
}

void WBlock::resize() {
  SizeWidget(prefs.tile_size.val + 4,
             prefs.tile_size.val * prefs.block_size.val + 4);
}

#define BG_COLOR 175

void WBlock::eventBlit(SDL_Surface* surf,
                       const PG_Rect& src,
                       const PG_Rect& dst) {
  SDL_Rect clip = PG_Rect(dst.x + 2, dst.y + 2, dst.w - 4, dst.h - 4);

  SDL_Rect sdl_dst = clip;
  SDL_FillRect(
      GetScreenSurface(), &sdl_dst,
      SDL_MapRGB(GetScreenSurface()->format, BG_COLOR, BG_COLOR, BG_COLOR));

  DrawBorder(src, 2, false);

  for (int i = 0; i < prefs.block_size.val; i++) {
    app->sprites.blit_tile(
        block->get_tile(i).type, block->get_tile(i).frame, GetScreenSurface(),
        clip.x, clip.y + prefs.tile_size.val * (prefs.block_size.val - 1 - i));
  }
}

// --- WField

WField::WField(PG_Widget* parent, PG_Rect rect, int width, int height)
    : Field(width, height), PG_Widget(parent, rect, true) {
  resize();
}

void WField::resize() {
  SizeWidget(get_width() * prefs.tile_size.val + 4,
             get_height() * prefs.tile_size.val + 4);
  set_field_changed(true);
}

void WField::eventDraw(SDL_Surface* surf, const PG_Rect& dst) {
  if (!has_field_changed()) {
    return;
  }
  set_field_changed(false);

  SDL_Rect clip = PG_Rect(dst.x + 2, dst.y + 2, dst.w - 4, dst.h - 4);

  SDL_Rect sdl_dst = clip;
  SDL_FillRect(surf, &sdl_dst,
               SDL_MapRGB(surf->format, BG_COLOR, BG_COLOR, BG_COLOR));

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      const Tile& tile = this->get_tile(row, col);

      if (tile.type == TILE_EMPTY) {
        continue;
      }

#if 0
            // Draw connections between this block and the blocks to the
            // right or below.
	    if (col+1 < width && tile.type != TILE_CLEARED
                    && tile.type == this->get_tile(row, col+1).type) {
                // line from center of this to center of right
                SDL_Rect connect = PG_Rect(
                        clip.x + prefs.tile_size.val*col + prefs.tile_size.val/2,
                        clip.y + prefs.tile_size.val*(height-1-row) + prefs.tile_size.val/2,
                        prefs.tile_size.val, 5);
                SDL_FillRect(surf, &connect,
                        SDL_MapRGB(surf->format, 255, 0, 0));
            }
	    if (row+1 < height && tile.type != TILE_CLEARED
                    && tile.type == this->get_tile(row+1, col).type) {
                // line from center of bottom to center of this
                SDL_Rect connect = PG_Rect(
                        clip.x + prefs.tile_size.val*col + prefs.tile_size.val/2,
                        clip.y + prefs.tile_size.val*(height-1-(row+1)) + prefs.tile_size.val/2,
                        5, prefs.tile_size.val);
                SDL_FillRect(surf, &connect,
                        SDL_MapRGB(surf->format, 255, 0, 0));
            }
#endif

      // Draw the block itself
      if (is_connected(row, col)) {
        app->sprites_connect.blit_tile(
            tile.type, tile.frame, surf, clip.x + prefs.tile_size.val * col,
            clip.y + prefs.tile_size.val * (height - 1 - row));
      } else {
        app->sprites.blit_tile(
            tile.type, tile.frame, surf, clip.x + prefs.tile_size.val * col,
            clip.y + prefs.tile_size.val * (height - 1 - row));
      }
    }
  }
}

void WField::eventBlit(SDL_Surface* surf,
                       const PG_Rect& src,
                       const PG_Rect& dst) {
  DrawBorder(src, 2, false);

  SDL_Rect sdl_src = src;
  SDL_Rect sdl_dst = dst;
  SDL_BlitSurface(surf, &sdl_src, GetScreenSurface(), &sdl_dst);

  SDL_Rect clip = PG_Rect(dst.x + 2, dst.y + 2, dst.w - 4, dst.h - 4);
  SDL_SetClipRect(GetScreenSurface(), &clip);

  // now show the block
  Block* block = this->get_block();
  if (block) {
    for (int i = 0; i < prefs.block_size.val; i++) {
      const Tile& tile = block->get_tile(i);
      float tile_x = block->get_x(i);
      float tile_y = block->get_y(i);
      app->sprites.blit_tile(
          tile.type, tile.frame, GetScreenSurface(),
          int(clip.x + prefs.tile_size.val * tile_x),
          int(clip.y + prefs.tile_size.val * (height - 1 - tile_y)));
    }
  }
  SDL_SetClipRect(GetScreenSurface(), NULL);
}

int WField::add_score(int num_cleared) {
  int gain = Field::add_score(num_cleared);
  if (gain >= MIN_FLOATSCORE) {
    app->game()->floatscore_new(gain);
  }
  return gain;
}
