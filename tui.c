#include "tui.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void tui() {
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_tui() {
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

Dropdown *create_dropdown(char **options, int num_options) {
  Dropdown *dropdown = (Dropdown *)malloc(sizeof(Dropdown));
  if (!dropdown) {
    return NULL;
  }

  dropdown->options = options;
  dropdown->num_options = num_options;
  dropdown->selected = 0;
  return dropdown;
}

void free_dropdown(Dropdown *dropdown) { free(dropdown); }

void display_dropdown(Dropdown *dropdown) {
  printf(CLEAR_SCREEN);

  for (int i = 0; i < dropdown->num_options; i++) {
    if (i == dropdown->selected) {
      printf("%s> %s%s\n", HIGHLIGHT, dropdown->options[i], RESET);
    } else {
      printf("  %s\n", dropdown->options[i]);
    }
  }
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
      is_running = 0;
      break;

    case KEY_Q:
      dropdown->selected = -1;
      is_running = 0;
      break;
    }
  }

  reset_tui();
  return dropdown->selected;
}
