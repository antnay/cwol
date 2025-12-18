// #include "db.h"
#include "network.h"

#define WOL_SIZE 102
#define MAC_SIZE 6

// todo: db
// todo: tui mode
// todo: cli flags

// char *help();

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    // help();
    return EXIT_FAILURE;
  }

  printf("mac: %s\n", argv[1]);

  send_packet(argv[1]);

  return EXIT_SUCCESS;
}

// char *help() {}
