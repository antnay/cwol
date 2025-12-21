#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>
#include <stdint.h>

#define DB_F "~/.local/share/cwol/cwol.db"

typedef struct {
  char **hosts;
  uint8_t **macs;
  int total;
} Macs;

sqlite3 *db();

int db_save_mac(sqlite3 *db, char *host, uint8_t *mac);

int db_remove_mac(sqlite3 *db, char *host);

Macs *db_get_macs(sqlite3 *db);

int db_close(sqlite3 *db);

#endif // !_DB_H_
