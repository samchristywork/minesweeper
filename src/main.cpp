#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <command_line.h>

#define SQUARE_SIZE 40

enum { COVERED, UNCOVERED };

bool rand_bool(float p) { return (float)rand() / (float)RAND_MAX < p; }

class Square {
public:
  int state;
  bool is_hover;
  bool is_mine;
  bool is_flag;
  int success_probability;

  Square();
  void Initialize();
};

Square::Square() { this->Initialize(); }

void Square::Initialize() {
  this->state = COVERED;
  this->is_hover = false;
  this->is_mine = false;
  this->is_flag = false;
  this->success_probability = 0;
}

class Board {
public:
  int width;
  int height;
  Square **squares;
  Board(int width, int height, int num_mines);
  Square *GetCollision(int mx, int my);
  Square *GetSquare(int x, int y);
  int GetNumNeighbors(int x, int y);
  int GetNumNeighborsCovered(int x, int y);
  int GetNumNeighborsFlagged(int x, int y);
  void Reset();
  void AutoFlag();
  void AutoComplete();
  void MarkProbability();
  void SeedRandomMine();
};

void Board::SeedRandomMine() {
  int num_unmined_squares = 0;

  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      if (!this->GetSquare(x, y)->is_mine) {
        num_unmined_squares++;
      }
    }
  }

  if (num_unmined_squares == 0) {
    return;
  }

  int choice = random() % num_unmined_squares;

  int counter = 0;
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      if (!this->GetSquare(x, y)->is_mine) {
        if (counter == choice) {
          this->GetSquare(x, y)->is_mine = true;
          static int nm = 0;
          nm++;
          printf("Mined %d\n", nm);
          return;
        }
        counter++;
      }
    }
  }
}

void Board::AutoFlag() {
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      Square *s = this->GetSquare(x, y);
      if (s) {
        if (s->state == UNCOVERED) {
          int num_neighbors = this->GetNumNeighbors(x, y);
          int num_neighbors_covered = this->GetNumNeighborsCovered(x, y);
          if (num_neighbors == num_neighbors_covered) {
            for (int cx = x - 1; cx <= x + 1; cx++) {
              for (int cy = y - 1; cy <= y + 1; cy++) {
                Square *s = this->GetSquare(cx, cy);
                if (s) {
                  if (s->state == COVERED) {
                    s->is_flag = true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void Board::MarkProbability() {
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      Square *s = this->GetSquare(x, y);
      if (s) {
        s->success_probability = 0;
        int num_neighbors = this->GetNumNeighbors(x, y);
        int num_neighbors_flagged = this->GetNumNeighborsFlagged(x, y);
        int num_neighbors_covered = this->GetNumNeighborsCovered(x, y);
        if (s->state == UNCOVERED && num_neighbors != 0 &&
            num_neighbors_flagged != num_neighbors_covered) {
          int choices = num_neighbors_covered - num_neighbors_flagged;
          int dangerous = num_neighbors - num_neighbors_flagged;
          s->success_probability = 100 - 100 * dangerous / choices;
        }
      }
    }
  }
}

void Board::AutoComplete() {
  bool finished = false;
  while (!finished) {
    finished = true;
    this->AutoFlag();
    for (int x = 0; x < this->width; x++) {
      for (int y = 0; y < this->height; y++) {
        Square *s = this->GetSquare(x, y);
        if (s) {
          if (s->state == UNCOVERED) {
            int num_neighbors = this->GetNumNeighbors(x, y);
            int num_neighbors_flagged = this->GetNumNeighborsFlagged(x, y);
            if (num_neighbors == num_neighbors_flagged) {
              for (int cx = x - 1; cx <= x + 1; cx++) {
                for (int cy = y - 1; cy <= y + 1; cy++) {
                  Square *s = this->GetSquare(cx, cy);
                  if (s) {
                    if (s->state == COVERED && s->is_flag == false) {
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
  }
  this->MarkProbability();
}

void Board::Reset() {
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      Square *s = this->GetSquare(x, y);
      if (s) {
        s->Initialize();
      }
    }
  }
}

Board::Board(int width, int height, int num_mines) {
  this->width = width;
  this->height = height;

  this->squares = new Square *[width];
  for (int i = 0; i < width; i++) {
    this->squares[i] = new Square[height];
  }

  for (int i = 0; i < num_mines; i++) {
    this->SeedRandomMine();
  }
}

Square *Board::GetCollision(int mx, int my) {
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      SDL_Rect rect;
      rect.x = x * SQUARE_SIZE;
      rect.y = y * SQUARE_SIZE;
      rect.w = SQUARE_SIZE + 1;
      rect.h = SQUARE_SIZE + 1;
      if (mx > rect.x && mx < rect.x + rect.w && my > rect.y &&
          my < rect.y + rect.h) {
        return &this->squares[x][y];
      }
    }
  }

  return nullptr;
}

Square *Board::GetSquare(int x, int y) {
  if (x >= 0 && x < this->width) {
    if (y >= 0 && y < this->height) {
      return &this->squares[x][y];
    }
  }
  return nullptr;
}

int Board::GetNumNeighborsCovered(int x, int y) {
  int n = 0;
  for (int cx = x - 1; cx <= x + 1; cx++) {
    for (int cy = y - 1; cy <= y + 1; cy++) {
      Square *s = this->GetSquare(cx, cy);
      if (s) {
        if (s->state == COVERED) {
          n++;
        }
      }
    }
  }

  return n;
}

int Board::GetNumNeighborsFlagged(int x, int y) {
  int n = 0;
  for (int cx = x - 1; cx <= x + 1; cx++) {
    for (int cy = y - 1; cy <= y + 1; cy++) {
      Square *s = this->GetSquare(cx, cy);
      if (s) {
        if (s->is_flag == true) {
          n++;
        }
      }
    }
  }

  return n;
}

int Board::GetNumNeighbors(int x, int y) {
  int n = 0;
  for (int cx = x - 1; cx <= x + 1; cx++) {
    for (int cy = y - 1; cy <= y + 1; cy++) {
      Square *s = this->GetSquare(cx, cy);
      if (s) {
        if (s->is_mine) {
          n++;
        }
      }
    }
  }

  return n;
}

void draw_text(SDL_Renderer *renderer, TTF_Font *font, int x, int y,
               SDL_Color color, const char *text) {
  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_Rect rect = textSurface->clip_rect;
  rect.x = x - rect.w / 2 + SQUARE_SIZE / 2;
  rect.y = y - rect.h / 2 + SQUARE_SIZE / 2;
  SDL_RenderCopy(renderer, textTexture, NULL, &rect);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

int main(int argc, char *argv[]) {
  srand(time(0));

  int mouse_x = 0;
  int mouse_y = 0;

  add_arg('n', "num-mines", "Number of mines");
  add_arg('x', "width", "Board width");
  add_arg('y', "height", "Board height");

  parse_opts(argc, argv);

  int width = 0;
  int height = 0;
  int num_mines = 0;

  if (get_is_set('x')) {
    width = atoi(get_value('x'));
  }

  if (get_is_set('y')) {
    height = atoi(get_value('y'));
  }

  if(!width){
    width=30;
  }

  if(!height){
    height=15;
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  TTF_Init();

  TTF_Font *font = TTF_OpenFont("res/LiberationSans-Regular.ttf", SQUARE_SIZE);
  TTF_Font *small_font =
      TTF_OpenFont("res/LiberationSans-Regular.ttf", SQUARE_SIZE / 2);

  SDL_Window *window = SDL_CreateWindow(
      "Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      width * SQUARE_SIZE, height * SQUARE_SIZE, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Board *board = new Board(width, height, num_mines);

  SDL_Texture *flag_texture = IMG_LoadTexture(renderer, "res/flag.bmp");
  SDL_Texture *mine_texture = IMG_LoadTexture(renderer, "res/mine.bmp");

  SDL_Event event;
  bool running = true;
  while (running) {
    int clicked_x = -1;
    int clicked_y = -1;
    int clicked_button = -1;

    Uint32 begin = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
        running = false;
        break;

      case SDL_MOUSEMOTION:
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
        break;

      case SDL_MOUSEBUTTONDOWN:
        clicked_x = event.button.x;
        clicked_y = event.button.y;
        clicked_button = event.button.button;
        break;

      case SDL_KEYDOWN:

        switch (event.key.keysym.sym) {

        case SDLK_ESCAPE:
          running = false;
          break;

        case SDLK_r:
          board->Reset();
          break;

        case SDLK_f:
          board->AutoFlag();
          break;

        case SDLK_h:
          board->MarkProbability();
          break;

        case SDLK_c:
          board->AutoComplete();
          break;

        default:
          // printf("Unhandled Key: %d\n", event.key.keysym.sym);
          break;
        }
        break;

      case SDL_POLLSENTINEL:
        break;

      default:
        // printf("Unhandled Event: %d\n", event.type);
        break;
      }
    }

    if (clicked_x != -1) {
      Square *s = board->GetCollision(clicked_x, clicked_y);
      if (s) {
        if (clicked_button == SDL_BUTTON_LEFT) {
          s->state = UNCOVERED;
        } else if (clicked_button == SDL_BUTTON_RIGHT) {
          s->is_flag = !s->is_flag;
        }
      }
    }

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
        rect.x = x * SQUARE_SIZE;
        rect.y = y * SQUARE_SIZE;
        rect.w = SQUARE_SIZE - 1;
        rect.h = SQUARE_SIZE - 1;

        Square *square = board->GetSquare(x, y);
        if (square) {
          if (square->is_hover && square->state == COVERED) {
            int shade = 0x7f;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);
            if (square->is_flag) {
              SDL_Rect flag_rect;
              flag_rect.x = rect.x;
              flag_rect.y = rect.y;
              flag_rect.w = SQUARE_SIZE;
              flag_rect.h = SQUARE_SIZE;
              SDL_RenderCopy(renderer, flag_texture, NULL, &flag_rect);
            }
          } else if (square->state == COVERED) {
            int shade = 0xaf;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
            SDL_RenderFillRect(renderer, &rect);

            SDL_Rect inside_rect;
            inside_rect.x = rect.x + 1;
            inside_rect.y = rect.y + 1;
            inside_rect.w = SQUARE_SIZE - 2;
            inside_rect.h = SQUARE_SIZE - 2;
            shade = 0x7f;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
            SDL_RenderFillRect(renderer, &inside_rect);
            if (square->is_flag) {
              SDL_Rect flag_rect;
              flag_rect.x = rect.x;
              flag_rect.y = rect.y;
              flag_rect.w = SQUARE_SIZE;
              flag_rect.h = SQUARE_SIZE;
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
            if (square->is_mine) {
              SDL_Rect mine_rect;
              mine_rect.x = rect.x;
              mine_rect.y = rect.y;
              mine_rect.w = SQUARE_SIZE;
              mine_rect.h = SQUARE_SIZE;
              SDL_RenderCopy(renderer, mine_texture, NULL, &mine_rect);
            } else {
              int num_neighbors = board->GetNumNeighbors(x, y);
              if (num_neighbors > 0) {
                char num_neigbors_text[8];
                sprintf(num_neigbors_text, "%d", num_neighbors);
                draw_text(renderer, font, rect.x, rect.y, black,
                          num_neigbors_text);
              }
            }
          }
        }
      }
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
