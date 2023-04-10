#ifndef RENDER_H
#define RENDER_H

#include <board.h>
#include <SDL2/SDL_ttf.h>

void draw_text(SDL_Renderer *renderer, TTF_Font *font, int x, int y,
               SDL_Color color, const char *text, int square_size);

#endif
