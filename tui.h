#ifndef _TUI_H_
#define _TUI_H_

#include <sqlite3.h>
#include <stdint.h>
#define CLEAR_SCREEN "\033[2J\033[H"
#define CURSOR_UP "\033[A"
#define CURSOR_DOWN "\033[B"
#define CLEAR_LINE "\033[K"
#define HIGHLIGHT "\033[1;36m"
#define RESET "\033[0m"
#define ALT_SCREEN_ENTER "\033[?1049h"
#define ALT_SCREEN_EXIT "\033[?1049l"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

typedef enum {
  KEY_ENTER = 10,
  KEY_ESC = 27,
  KEY_UP = 65,
  KEY_DOWN = 66,
  KEY_D = 100,
  KEY_Q = 113,
  KEY_BACKSPACE = 127,
} Key;

typedef struct {
  sqlite3 *db;
  char **hosts;
  uint8_t **macs;
  int num_options;
  int selected;
} Dropdown;

void tui();
void reset_tui();
Dropdown *create_dropdown(sqlite3 *db);
void free_dropdown(Dropdown *dropdown);
void display_dropdown(Dropdown *dropdown);
void update_dropdown(Dropdown *dropdown);
int read_key();
void handle_arrow_key(Dropdown *dropdown);
int run_dropdown(Dropdown *dropdown);

#endif // !_TUI_H_
