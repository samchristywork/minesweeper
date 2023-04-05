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
