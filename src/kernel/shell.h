#ifndef SHELL_H
#define SHELL_H

#include "kernel/shell.h"
#include "kernel/terminal.h"
#include "util/common.h"
#include "util/constans.h"

#define MAX_COMMAND_LEN 255

void shell_print_prefix();
void shell_input(char c);

#endif