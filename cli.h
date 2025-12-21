#ifndef _CLI_H_
#define _CLI_H_

#include "db.h"
#include "tui.h"

void parse_args(int argc, char *argv[]);
char *help_commands();
char *help_add();
char *help_remove();

#endif // !_CLI_H_