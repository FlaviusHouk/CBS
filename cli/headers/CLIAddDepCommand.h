#ifndef _CLI_ADD_DEP_COMMAND_H_
#define _CLI_ADD_DEP_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_ADD_DEP_COMMAND cli_add_dep_command_get_type()

G_DECLARE_FINAL_TYPE(CLIAddDepCommand, 
                     cli_add_dep_command, 
                     CLI, 
                     ADD_DEP_COMMAND, 
                     CLICommandInfo);

CLIAddDepCommand*
cli_add_dep_command_new();

G_END_DECLS

#endif