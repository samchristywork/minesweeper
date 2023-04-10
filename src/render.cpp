#include <render.h>

void draw_text(SDL_Renderer *renderer, TTF_Font *font, int x, int y,
               SDL_Color color, const char *text, int square_size) {
  if (!font) {
    return;
  }

  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_Rect rect = textSurface->clip_rect;
  rect.x = x - rect.w / 2 + square_size / 2;
  rect.y = y - rect.h / 2 + square_size / 2;
  SDL_RenderCopy(renderer, textTexture, NULL, &rect);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

void render(SDL_Renderer *renderer, Board *board, int mouse_x, int mouse_y,
            TTF_Font *font, SDL_Texture *flag_texture,
            SDL_Texture *mine_texture, int square_size) {
  int shade = 0x07;
  SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
  SDL_RenderClear(renderer);

  for (int x = 0; x < board->width; x++) {
    for (int y = 0; y < board->height; y++) {
      board->squares[x][y].is_hover = false;
    }
  }

  Square *s = board->GetCollision(mouse_x, mouse_y);
  if (s) {
    s->is_hover = true;
  }

  bool finished = false;
  while (!finished) {
    finished = true;
    for (int x = 0; x < board->width; x++) {
      for (int y = 0; y < board->height; y++) {
        int num_neighbors = board->GetNumNeighbors(x, y);
        Square *square = board->GetSquare(x, y);
        if (square) {
          if (square->state == UNCOVERED && num_neighbors == 0) {
            for (int cx = x - 1; cx <= x + 1; cx++) {
              for (int cy = y - 1; cy <= y + 1; cy++) {
                Square *s = board->GetSquare(cx, cy);
                if (s) {
                  if (s->state == COVERED) {
                    s->state = UNCOVERED;
                    finished = false;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  for (int x = 0; x < board->width; x++) {
    for (int y = 0; y < board->height; y++) {
      SDL_Rect rect;
      rect.x = x * square_size;
      rect.y = y * square_size;
      rect.w = square_size - 1;
      rect.h = square_size - 1;

      Square *square = board->GetSquare(x, y);
      if (square) {
        if (square->is_hover && square->state == COVERED) {
          int shade = 0x7f;
          SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
          SDL_RenderFillRect(renderer, &rect);
          if (square->is_flag && flag_texture) {
            SDL_Rect flag_rect;
            flag_rect.x = rect.x;
            flag_rect.y = rect.y;
            flag_rect.w = square_size;
            flag_rect.h = square_size;
            SDL_RenderCopy(renderer, flag_texture, NULL, &flag_rect);
          }
        } else if (square->state == COVERED) {
          int shade = 0xaf;
          SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
          SDL_RenderFillRect(renderer, &rect);

          SDL_Rect inside_rect;
          inside_rect.x = rect.x + 1;
          inside_rect.y = rect.y + 1;
          inside_rect.w = square_size - 2;
          inside_rect.h = square_size - 2;
          shade = 0x7f;
          SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
          SDL_RenderFillRect(renderer, &inside_rect);
          if (square->is_flag && flag_texture) {
            SDL_Rect flag_rect;
            flag_rect.x = rect.x;
            flag_rect.y = rect.y;
            flag_rect.w = square_size;
            flag_rect.h = square_size;
            SDL_RenderCopy(renderer, flag_texture, NULL, &flag_rect);
          }
        } else if (square->state == UNCOVERED) {
          if (square->success_probability == 0) {
            int shade = 0xff;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);
          } else if (square->success_probability <= 34) {
            int shade = 0xaf;
            SDL_SetRenderDrawColor(renderer, 0xff, shade, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);
          } else if (square->success_probability <= 50) {
            int shade = 0xaf;
            SDL_SetRenderDrawColor(renderer, 0xff, 0xff, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);
          } else {
            int shade = 0xaf;
            SDL_SetRenderDrawColor(renderer, shade, 0xff, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);
          }

          SDL_Color black = {0, 0, 0};
          if (square->is_mine && mine_texture) {
            SDL_Rect mine_rect;
            mine_rect.x = rect.x;
            mine_rect.y = rect.y;
            mine_rect.w = square_size;
            mine_rect.h = square_size;
            SDL_RenderCopy(renderer, mine_texture, NULL, &mine_rect);
          } else {
            int num_neighbors = board->GetNumNeighbors(x, y);
            if (num_neighbors > 0 && font) {
              char num_neigbors_text[8];
              sprintf(num_neigbors_text, "%d", num_neighbors);
              draw_text(renderer, font, rect.x, rect.y, black,
                        num_neigbors_text, square_size);
            }
          }
        }
      }
    }
  }

  SDL_RenderPresent(renderer);
}
