#include "cli.h"
#include "db.h"
#include "network.h"
#include "tui.h"
#include <stdio.h>
#include <string.h>

void parse_args(int argc, char *argv[]) {
  switch (argc) {
  case 1: {
    sqlite3 *database = db();
    if (database == NULL) {
      return;
    }
    Macs *macs = db_get_macs(database);
    if (macs == NULL) {
      fprintf(stderr, "No hosts found or database error\n");
      return;
    }
    Dropdown *dropdown = create_dropdown(macs->hosts, macs->total);
    int selected = run_dropdown(dropdown);
    printf(CLEAR_SCREEN);
    if (selected >= 0)
      printf("You selected: %s %s\n", macs->hosts[selected],
             macs->macs[selected]);

    free_dropdown(dropdown);
    break;
  }
  default:
    if (strcmp(argv[1], "add") == 0 || strcmp(argv[1], "a") == 0) {
      if (argc < 4) {
        printf("%s\n", help_add());
        return;
      }
      uint8_t mac_buf[MAC_SIZE];
      db_save_mac(db(), argv[2], parse_mac(argv[3], mac_buf));
    } else if (strcmp(argv[1], "remove") == 0 || strcmp(argv[1], "r") == 0) {
      if (argc < 3) {
        printf("%s\n", help_remove());
        return;
      }
      db_remove_mac(db(), argv[2]);
    } else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "h") == 0) {
      printf("%s\n", help_commands());
    } else {
      send_packet(argv[1]);
    }
    break;
  }
}

char *help_commands() {
  return "Usage: cwol <command> [arguments]\n"
         "       cwol <mac> - Send a magic packet to the MAC address\n\n"
         "commands:\n"
         "  add, a    - Add a host to the database\n"
         "  remove, r - Remove a host from the database\n"
         "  help, h   - Show help message\n";
}

char *help_add() {
  return "Usage: cwol add <host> <mac>\n\n"
         "Arguments:\n"
         "  <host> - The name of the host\n"
         "  <mac>  - The MAC address of the host\n";
}

char *help_remove() {
  return "Usage: cwol remove <host>\n\n"
         "Arguments:\n"
         "  <host> - The name of the host\n";
}