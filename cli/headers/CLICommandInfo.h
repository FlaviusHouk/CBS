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

///Type with information about some command for execution
G_DECLARE_DERIVABLE_TYPE(CLICommandInfo, cli_command_info, CLI, COMMAND_INFO, GObject);

///Class definition with static class members and virtual methods
struct _CLICommandInfoClass
{
	GObjectClass parentClass;

	GString* _standardErrorMSG;

	void (*_execute)(CLICommandInfo* this);
	gboolean (*_handleInput)(CLICommandInfo* this, GString* input);
	gboolean (*_isValid)(CLICommandInfo* this);

	void (*_dispose)(GObject* obj);
	void (*_finalize)(GObject* obj);

	gpointer padding[4];
};


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

///Method for actual command invoking
void cli_command_info_process_command(CLICommandInfo* this);

///Method for handling input data for it
///CLI args passed here one by one and sets to according fields of object
///@arg - cli argument
///returns flag that indicates error on input processing 
gboolean
cli_command_info_handle_input(CLICommandInfo* this, GString* arg);

///Property that checks is there enough data to execute command.
gboolean
cli_command_info_is_valid(CLICommandInfo* info);

///Getter for InputState property.
///Needed for cli arguments parsing.
gint
cli_command_info_get_input_state(CLICommandInfo* this);

///Setter for InputState property.
///Needed for cli arguments parsing.
void
cli_command_info_set_input_state(CLICommandInfo* this, gint state);

G_END_DECLS

#endif