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

#include "CLIHelpCommand.h"

typedef struct _CLIHelpCommand
{
    CLICommandInfo parent_object;
} CLIHelpCommand;

G_DEFINE_TYPE(CLIHelpCommand, cli_help_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_help_command_is_valid(CLICommandInfo* command)
{
    return TRUE;
}

static gboolean
cli_help_command_handle_input(CLICommandInfo* command, GString* input)
{
	return TRUE;
}

///Help command handler
static void
cli_help_command_execute(CLICommandInfo* command)
{
	g_print("Usage: %s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
			"--create projName",
			"--addFile projName file1 file2 ... fileN",
			"--deleteFile projName file",
			"--addInclude projName fold1 fold2 ... foldN",
			"--deleteInclude projName fold",
			"--addDependency projName depType depName",
			"--deleteDependency projName depName",
			"--build projName",
			"--help");
}

static void
cli_help_command_class_init(CLIHelpCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

	parentClass->_standardErrorMSG =
		g_string_new(g_strdup(""));

    parentClass->_execute = cli_help_command_execute;
    parentClass->_handleInput = cli_help_command_handle_input;
    parentClass->_isValid = cli_help_command_is_valid;
}

static void
cli_help_command_init(CLIHelpCommand* this)
{}

CLIHelpCommand*
cli_help_command_new()
{
    return g_object_new(CLI_TYPE_HELP_COMMAND, NULL);
}