#include "tui.h"
#include "db.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

void tui() {
  printf("%s%s", ALT_SCREEN_ENTER, HIDE_CURSOR);
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fflush(stdout);
}

void reset_tui() {
  printf("%s%s", ALT_SCREEN_EXIT, SHOW_CURSOR);
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fflush(stdout);
}

Dropdown *create_dropdown(sqlite3 *db) {
  Macs *macs = db_get_macs(db);
  if (macs == NULL) {
    fprintf(stderr, "database error\n");
    return NULL;
  }
  Dropdown *dropdown = (Dropdown *)malloc(sizeof(Dropdown));
  if (!dropdown) {
    return NULL;
  }

  dropdown->db = db;
  dropdown->hosts = macs->hosts;
  dropdown->macs = macs->macs;
  dropdown->num_options = macs->total;
  dropdown->selected = 0;
  return dropdown;
}

void free_dropdown(Dropdown *dropdown) { free(dropdown); }

void display_dropdown(Dropdown *dropdown) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  int term_height = w.ws_row;
  int term_width = w.ws_col;

  int content_height = dropdown->num_options + 4;

  int max_line_len = 18;
  for (int i = 0; i < dropdown->num_options; i++) {
    int len = strlen(dropdown->hosts[i]) + 5;
    if (len > max_line_len)
      max_line_len = len;
  }
  int content_width = max_line_len;

  int start_row = (term_height - content_height) / 2 + 1;
  int start_col = (term_width - content_width) / 2 + 1;
  if (start_row < 1)
    start_row = 1;
  if (start_col < 1)
    start_col = 1;

  printf(CLEAR_SCREEN);

#define MOVE_CURSOR(row, col) printf("\033[%d;%dH", (row), (col))

  MOVE_CURSOR(start_row, start_col);

  int title_len = 16;
  int title_padding = (content_width - title_len) / 2;
  if (title_padding < 0)
    title_padding = 0;

  if (title_padding > 0)
    printf("%*scwol", title_padding, "");
  else
    printf("cwol");

  MOVE_CURSOR(start_row + 1, start_col);
  int sep_len = 18;
  int sep_padding = (content_width - sep_len) / 2;
  if (sep_padding < 0)
    sep_padding = 0;

  if (sep_padding > 0)
    printf("%*s───────────────", sep_padding, "");
  else
    printf("───────────────");

  for (int i = 0; i < dropdown->num_options; i++) {
    MOVE_CURSOR(start_row + 2 + i, start_col);
    if (i == dropdown->selected) {
      printf("%s ● %s%s", HIGHLIGHT, dropdown->hosts[i], RESET);
    } else {
      printf("   %s", dropdown->hosts[i]);
    }
  }
  fflush(stdout);
}

inline void update_dropdown(Dropdown *dropdown) {
  Macs *macs = db_get_macs(dropdown->db);
  if (macs == NULL) {
    fprintf(stderr, "database error\n");
    return;
  }
  dropdown->hosts = macs->hosts;
  dropdown->macs = macs->macs;
  dropdown->num_options = macs->total;
}

int read_key() {
  char c;
  if (read(STDIN_FILENO, &c, 1) == 1) {
    return c;
  }
  return 0;
}

void handle_arrow_key(Dropdown *dropdown) {
  switch (read_key()) {
  case KEY_UP:
    if (dropdown->selected > 0) {
      dropdown->selected--;
      display_dropdown(dropdown);
    }
    break;

  case KEY_DOWN:
    if (dropdown->selected < dropdown->num_options - 1) {
      dropdown->selected++;
      display_dropdown(dropdown);
    }
    break;
  }
}

int run_dropdown(Dropdown *dropdown) {
  tui();
  int is_running = 1;
  display_dropdown(dropdown);

  while (is_running == 1) {
    int key = read_key();

    switch (key) {
    case KEY_ESC:
      if (read_key() == '[') {
        handle_arrow_key(dropdown);
      } else {
        dropdown->selected = -1;
        is_running = 0;
      }
      break;

    case KEY_ENTER:
      send_packet(dropdown->macs[dropdown->selected]);
      is_running = 0;
      break;

    case KEY_Q:
      dropdown->selected = -1;
      is_running = 0;
      break;

    case KEY_D:
    case KEY_BACKSPACE:
      db_remove_mac(dropdown->db, dropdown->hosts[dropdown->selected]);
      update_dropdown(dropdown);
      display_dropdown(dropdown);
      break;
    }
  }

  reset_tui();
  return dropdown->selected;
}
