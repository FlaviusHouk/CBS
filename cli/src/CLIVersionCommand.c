/**********************************************************************
This file is part of C Build System.

C Build System is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

C Build System is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with C Build System.  If not, see <https://www.gnu.org/licenses/>.
***********************************************************************/

#include "CLIVersionCommand.h"

#include "ModelProjectManager.h"

typedef struct _CLIVersionCommand
{
    CLICommandInfo parent_object;
} CLIVersionCommand;

G_DEFINE_TYPE(CLIVersionCommand, cli_version_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_version_command_is_valid(CLICommandInfo* command)
{
    return TRUE;
}

static gboolean
cli_version_command_handle_input(CLICommandInfo* command, GString* input)
{
    return TRUE;
}

static void
cli_version_command_execute(CLICommandInfo* command)
{
    gchar* version = model_project_manager_get_version();
    g_print("%s\n", version);
    g_free(version);
}

static void
cli_version_command_class_init(CLIVersionCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

	parentClass->_standardErrorMSG =
		g_string_new(g_strdup(""));

    parentClass->_execute = cli_version_command_execute;
    parentClass->_handleInput = cli_version_command_handle_input;
    parentClass->_isValid = cli_version_command_is_valid;
}

static void
cli_version_command_init(CLIVersionCommand* this)
{}

CLIVersionCommand*
cli_version_command_new()
{
    return g_object_new(CLI_TYPE_VERSION_COMMAND, NULL);
}
