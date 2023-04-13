#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <command_line.h>
#include <render.h>

#define FLAG_PATH "res/flag.bmp"
#define FONT_PATH "res/LiberationSans-Regular.ttf"
#define MINE_PATH "res/mine.bmp"
#define SMALL_FONT_PATH "res/LiberationSans-Regular.ttf"
#define SQUARE_SIZE 40

bool rand_bool(float p) { return (float)rand() / (float)RAND_MAX < p; }

Square::Square() { this->Initialize(); }

void Square::Initialize() {
  this->state = COVERED;
  this->is_hover = false;
  this->is_mine = false;
  this->is_flag = false;
  this->success_probability = 0;
}

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

  for (int i = 0; i < this->num_mines; i++) {
    this->SeedRandomMine();
  }
}

Board::Board(int width, int height, int num_mines) {
  this->width = width;
  this->height = height;
  this->num_mines = num_mines;

  this->squares = new Square *[width];
  for (int i = 0; i < width; i++) {
    this->squares[i] = new Square[height];
  }

  this->Reset();
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

bool event_loop(Board *board, int *mouse_x, int *mouse_y) {
  static SDL_Event event;

  bool running = true;

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
      *mouse_x = event.motion.x;
      *mouse_y = event.motion.y;
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

  return running;
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

  if (get_is_set('n')) {
    num_mines = atoi(get_value('n'));
  }

  if (!width) {
    width = 30;
  }

  if (!height) {
    height = 15;
  }

  if (!num_mines) {
    num_mines = 15;
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  TTF_Init();

  TTF_Font *font = TTF_OpenFont(FONT_PATH, SQUARE_SIZE);
  TTF_Font *small_font = TTF_OpenFont(SMALL_FONT_PATH, SQUARE_SIZE / 2);

  SDL_Window *window = SDL_CreateWindow(
      "Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      width * SQUARE_SIZE, height * SQUARE_SIZE, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Board *board = new Board(width, height, num_mines);

  SDL_Texture *flag_texture = IMG_LoadTexture(renderer, FLAG_PATH);
  SDL_Texture *mine_texture = IMG_LoadTexture(renderer, MINE_PATH);

  if (!font) {
    fprintf(stderr, "Resource: \"%s\" could not be loaded...\n", FONT_PATH);
  }

  if (!small_font) {
    fprintf(stderr, "Resource: \"%s\" could not be loaded...\n",
            SMALL_FONT_PATH);
  }

  if (!flag_texture) {
    fprintf(stderr, "Resource: \"%s\" could not be loaded...\n", FLAG_PATH);
  }

  if (!mine_texture) {
    fprintf(stderr, "Resource: \"%s\" could not be loaded...\n", MINE_PATH);
  }

  int shade = 0x07;
  SDL_SetRenderDrawColor(renderer, shade, shade, shade, 0xff);
  SDL_RenderClear(renderer);

  bool running = true;
  while (running) {
    running = event_loop(board, &mouse_x, &mouse_y);

    bool game_complete = true;
    for (int x = 0; x < board->width; x++) {
      for (int y = 0; y < board->height; y++) {
        Square *s = board->GetSquare(x, y);
        if (!s->is_mine && s->state == COVERED) {
          game_complete = false;
        }
      }
    }

    if (game_complete) {
      board->AutoFlag();
    }

    render(renderer, board, mouse_x, mouse_y, font, flag_texture, mine_texture);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
