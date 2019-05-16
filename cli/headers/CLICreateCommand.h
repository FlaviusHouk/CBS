#ifndef _CLI_CREATE_COMMAND_H_
#define _CLI_CREATE_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_CREATE_COMMAND cli_create_command_get_type()

G_DECLARE_FINAL_TYPE(CLICreateCommand, cli_create_command, CLI, CREATE_COMMAND, CLICommandInfo);

CLICreateCommand* 
cli_create_command_new();

G_END_DECLS

#endif