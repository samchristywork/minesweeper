#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <command_line.h>

#define SQUARE_SIZE 40
#define WIDTH 1200
#define HEIGHT 700

enum { COVERED, UNCOVERED };

class Square {
public:
  int state;

  Square();
  void Initialize();
};

Square::Square() { this->Initialize(); }

void Square::Initialize() {
  this->state = COVERED;
}

class Board {
public:
  int width;
  int height;
  Square **squares;
  Board(int width, int height);
};

Board::Board(int width, int height) {
  this->width = width;
  this->height = height;

  this->squares = new Square *[width];
  for (int i = 0; i < width; i++) {
    this->squares[i] = new Square[height];
  }
}

int main(int argc, char *argv[]) {
  srand(time(0));

  int mouse_x = 0;
  int mouse_y = 0;

  parse_opts(argc, argv);

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  SDL_Window *window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 500, 500,
                                        SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Board *board = new Board(WIDTH / SQUARE_SIZE, HEIGHT / SQUARE_SIZE);

  SDL_Event event;
  bool running = true;
  while (running) {
    Uint32 begin = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
        running = false;
        break;

      case SDL_KEYDOWN:

        switch (event.key.keysym.sym) {

        case SDLK_ESCAPE:
          running = false;
          break;

        default:
          printf("Unhandled Key: %d\n", event.key.keysym.sym);
          break;
        }
        break;

      case SDL_POLLSENTINEL:
        break;

      default:
        printf("Unhandled Event: %d\n", event.type);
        break;
      }
    }

    int shade = 0x07;
    SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
