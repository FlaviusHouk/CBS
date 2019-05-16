#ifndef _CLI_HELP_COMMAND_H_
#define _CLI_HELP_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_HELP_COMMAND cli_help_command_get_type()

G_DECLARE_FINAL_TYPE(CLIHelpCommand,
                     cli_help_command,
                     CLI,
                     HELP_COMMAND,
                     CLICommandInfo);

CLIHelpCommand*
cli_help_command_new();

G_END_DECLS

#endif