#include "db.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *db() {
  char *home = getenv("HOME");
  if (home == NULL) {
    fprintf(stderr, "Could not get HOME directory\n");
    return NULL;
  }

  char path[1024];
  snprintf(path, sizeof(path), "%s/.local/share/cwol/cwol.db", home);

  sqlite3 *db;
  int f = sqlite3_open(path, &db);
#ifdef DEBUG
  printf("db path: %s\n", path);
#endif /* ifdef DEBUG */
  if (f != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }
  return db;
}

int db_save_mac(sqlite3 *db, char *host, uint8_t *mac) {
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(
      db, "INSERT INTO macs (host, mac) VALUES (?, ?) ON CONFLICT DO NOTHING",
      -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, host, strlen(host), SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 2, mac, MAC_SIZE, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
  }

  sqlite3_finalize(stmt);
  return 0;
}

int db_remove_mac(sqlite3 *db, char *host) {
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, "DELETE FROM macs WHERE host = ?", -1, &stmt,
                              NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, host, strlen(host), SQLITE_STATIC);

  sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return 0;
}

Macs *db_get_macs(sqlite3 *db) {
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(
      db, "SELECT count(id) OVER() as total, host, mac FROM macs", -1, &stmt,
      NULL);

  if (rc != SQLITE_OK) {
    return NULL;
  }

  Macs *res = NULL;
  int i = 0;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    if (res == NULL) {
      int total = sqlite3_column_int(stmt, 0);
      res = malloc(sizeof(Macs));
      res->total = total;
      res->hosts = malloc(total * sizeof(char *));
      res->macs = malloc(total * sizeof(uint8_t *));
    }

    const char *host_text = (const char *)sqlite3_column_text(stmt, 1);
    const void *mac_blob = sqlite3_column_blob(stmt, 2);
    int mac_bytes = sqlite3_column_bytes(stmt, 2);

    res->hosts[i] = strdup(host_text ? host_text : "");
    res->macs[i] = malloc(mac_bytes);
    if (mac_blob) {
      memcpy(res->macs[i], mac_blob, mac_bytes);
    }
    i++;
  }

  if (res == NULL) {
    res = malloc(sizeof(Macs));
    res->total = 0;
    res->hosts = NULL;
    res->macs = NULL;
  }

  sqlite3_finalize(stmt);
  return res;
}

int db_close(sqlite3 *db) { return sqlite3_close(db); }
