#ifndef _CLI_DELETE_DEP_COMMAND_H_
#define _CLI_DELETE_DEP_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_DELETE_DEP_COMMAND cli_delete_dep_command_get_type()

G_DECLARE_FINAL_TYPE(CLIDeleteDepCommand,
                     cli_delete_dep_command,
                     CLI,
                     DELETE_DEP_COMMAND,
                     CLICommandInfo);

CLIDeleteDepCommand*
cli_delete_dep_command_new();

G_END_DECLS

#endif