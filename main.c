#include "cli.h"
#include <stdlib.h>

// todo: db
// todo: tui mode
// todo: cli flags

int main(int argc, char *argv[]) {
  parse_args(argc, argv);

  return EXIT_SUCCESS;
}
