#include "db.h"
#include <string.h>

sqlite3 *db() {
  sqlite3 *db;
  int f = sqlite3_open(DB_F, &db);
  return db;
}

int save_mac(sqlite3 *db, char *host, uint8_t *mac) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, "INSERT INTO mac (host, mac) VALUES ?, ?", -1, &stmt,
                     NULL);

  sqlite3_bind_text(stmt, 1, host, strlen(host), SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 2, mac, strlen(host), SQLITE_STATIC);

  int step = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return 0;
}

int remove_mac(sqlite3 *db, char *host) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, "DELETE FROM macs WHERE host = ?", -1, &stmt, NULL);

  sqlite3_bind_text(stmt, 1, host, strlen(host), SQLITE_STATIC);

  int step = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return 0;
}

Macs *get_macs(sqlite3 *db) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db,
                     "SELECT count(id) OVER() as total, host, mac FROM macs",
                     -1, &stmt, NULL);

  Macs *res;
  int rows = sqlite3_step(stmt);
  int total = sqlite3_column_int(stmt, 0);
  res->total = total;
  if (total <= MAX_STACK_ROWS) {
    // res->hosts = [];
    // res->macs = [];
  } else {
    // res->hosts = (char)malloc(sizeof(char) *)
  }
  while ((rows = sqlite3_step(stmt)) == SQLITE_ROW) {
    // char *host =
  }

  sqlite3_finalize(stmt);
  return res;
}

int db_close(sqlite3 *db) { return sqlite3_close(db); }
