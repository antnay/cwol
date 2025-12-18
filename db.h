#ifndef _DB_H_
#define _DB_H_
#include <sqlite3.h>
#include <stdint.h>

#define DB_F "$HOME/.local/share/cwol/cwol.db"
#define MAX_STACK_ROWS 8

typedef struct {
  char **hosts;
  uint8_t **macs;
  int total;
} Macs;

sqlite3 *db();

int save_mac(sqlite3 *db, char *host, uint8_t *mac);

int remove_mac(sqlite3 *db, char *host);

Macs *get_macs(sqlite3 *db);

int db_close(sqlite3 *db);

#endif // !_DB_H_
