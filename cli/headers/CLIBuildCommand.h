#ifndef _CLI_BUILD_COMMAND_H_
#define _CLI_BUILD_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_BUILD_COMMAND cli_build_command_get_type()

G_DECLARE_FINAL_TYPE(CLIBuildCommand,
                     cli_build_command,
                     CLI,
                     BUILD_COMMAND,
                     CLICommandInfo);

CLIBuildCommand*
cli_build_command_new();

G_END_DECLS

#endif