#ifndef _CLI_DELETE_FILE_COMMAND_H_
#define _CLI_DELETE_FILE_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_DELETE_FILE_COMMAND cli_delete_file_command_get_type()

G_DECLARE_FINAL_TYPE(CLIDeleteFileCommand, 
                     cli_delete_file_command, 
                     CLI, 
                     DELETE_FILE_COMMAND,
                     CLICommandInfo);

CLIDeleteFileCommand*
cli_delete_file_command_new();

G_END_DECLS

#endif