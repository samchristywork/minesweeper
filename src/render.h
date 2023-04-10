#ifndef RENDER_H
#define RENDER_H

#include <board.h>
#include <SDL2/SDL_ttf.h>

void render(SDL_Renderer *renderer, Board *board, int mouse_x, int mouse_y,
            TTF_Font *font, SDL_Texture *flag_texture,
            SDL_Texture *mine_texture, int square_size);

#endif
