#ifndef _CLI_DELETE_INCLUDE_COMMAND_H_
#define _CLI_DELETE_INCLUDE_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_DELETE_INCLUDE_COMMAND cli_delete_include_command_get_type()

G_DECLARE_FINAL_TYPE(CLIDeleteIncludeCommand, 
                     cli_delete_include_command, 
                     CLI, 
                     DELETE_INCLUDE_COMMAND,
                     CLICommandInfo);

CLIDeleteIncludeCommand*
cli_delete_include_command_new();

G_END_DECLS

#endif