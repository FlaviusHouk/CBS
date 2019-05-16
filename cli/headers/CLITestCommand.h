#ifndef _CLI_TEST_COMMAND_H_
#define _CLI_TEST_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_TEST_COMMAND cli_test_command_get_type()

G_DECLARE_FINAL_TYPE(CLITestCommand,
                     cli_test_command,
                     CLI,
                     TEST_COMMAND,
                     CLICommandInfo);

CLITestCommand*
cli_test_command_new();

G_END_DECLS

#endif