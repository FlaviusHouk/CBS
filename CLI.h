#ifndef _CLI_H_
#define _CLI_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

G_BEGIN_DECLS

#define CLI_TYPE_COMMAND_PARSER cli_command_parser_get_type()

G_DECLARE_FINAL_TYPE(CLICommandParser, cli_command_parser, CLI, COMMAND_PARSER, GObject)

G_END_DECLS

CLICommandParser* cli_command_parser_new(char** args, int argn);

void cli_command_parser_execute(CLICommandParser* this);

#endif