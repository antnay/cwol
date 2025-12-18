#!/bin/bash

DIR="$HOME/.local/share/cwol"

mkdir -p "$DIR"
if [ ! -d "$DIR" ]; then
  echo "could not create $DIR, exiting"
  exit 1
fi

sqlite3 "$DIR/cwol.db" << EOF
CREATE TABLE IF NOT EXISTS macs (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  host TEXT NOT NULL,
  mac BLOB
);
EOF

echo "created database"

