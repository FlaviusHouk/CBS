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


#ifndef _CLI_COMMAND_INFO_H_
#define _CLI_COMMAND_INFO_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

G_BEGIN_DECLS

#define CLI_TYPE_COMMAND_INFO cli_command_info_get_type()

///Type with information about some predefined command (representation, order, argNum, funcion
///execute and so on)
G_DECLARE_FINAL_TYPE(CLICommandInfo, cli_command_info, CLI, COMMAND_INFO, GObject);

///Enum with all predefined commands
enum CLI_COMMAND_PARSER_INPUT_STATES
{
	START,
	CREATE,
	ADD_FILE,
	DELETE_FILE,
	ADD_INCL,
	DELETE_INCL,
	ADD_DEP,
	REM_DEP,
	BUILD,
	HELP,
	TEST,

	COMMANDS_COUNT
};

///Constructor for CommandInfo type
///command - command representation ("--create" for example)
///commandType - command identifier according to CLI_COMMAND_PARSER_INPUT_STATES enum
///argsCount - desired number of arguments for it
///order - executing order in case of executing multiple commands
///action - function to call when command is given
CLICommandInfo* cli_command_info_new(GString* command, 
                                     int commandType, 
                                     int argsCount, 
                                     int order, 
                                     void (*action)(CLICommandInfo* com));

///Getter for command representation
GString* cli_command_info_get_command(CLICommandInfo* this);

///Setter for command representation
void cli_command_info_set_command(CLICommandInfo* this, GString* value);

///Getter for arguments count
int cli_command_info_get_args_count(CLICommandInfo* this);

///Setter for arguments count
void cli_command_info_set_args_count(CLICommandInfo* this, int value);

///Getter for execution order 
int cli_command_info_get_order(CLICommandInfo* this);

///Setter for execution order
void cli_command_info_set_order(CLICommandInfo* this, int value);

///Getter for collection with provided args
GPtrArray* cli_command_info_get_args(CLICommandInfo* this);

///Method for actual command invoking
void cli_command_info_process_command(CLICommandInfo* this);

G_END_DECLS

#endif