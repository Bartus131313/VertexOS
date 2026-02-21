#ifndef SHELL_H
#define SHELL_H

#include "shell.h"
#include "terminal.h"
#include "util/common.h"

#define MAX_COMMAND_LEN 255

void shell_print_prefix();
void shell_input(char c);

#endif