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


#include "CLICommandInfo.h"

typedef struct
{
	gint _inputState; //Indicates what kind of input is expected. 
} CLICommandInfoPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (CLICommandInfo, cli_command_info, G_TYPE_OBJECT)

static void
cli_command_info_dispose(GObject* obj)
{
	CLICommandInfoClass* class = CLI_COMMAND_INFO_GET_CLASS(obj);
	class->_dispose(obj);

	G_OBJECT_CLASS(cli_command_info_parent_class)->dispose(obj);
}

static void
cli_command_info_class_init(CLICommandInfoClass* class)
{
	GObjectClass* baseClass = G_OBJECT_CLASS(class);

	class->_execute = NULL;
	class->_handleInput = NULL;
	class->_isValid = NULL;
	class->_dispose = NULL;
	class->_finalize = NULL;
}

static void
cli_command_info_init(CLICommandInfo* this)
{

}

/*virtual*/ void 
cli_command_info_process_command(CLICommandInfo* this)
{
	g_assert(this);
	CLICommandInfoClass* class = CLI_COMMAND_INFO_GET_CLASS(this);

	//check for correctness is common for all commands.
	gboolean isValid = cli_command_info_is_valid(this);
	if(!isValid)
	{
		g_print("%s\n", class->_standardErrorMSG->str);
		return;
	}

	g_assert(class->_execute);
	class->_execute(this);
}

/*
Every command should handle it's input manualy. There is two available options for handling:
Direct and named. In first case parameters will be set according to predefined order.
Params will be parsed if neccesary. In case of named handling it is possible to use parameter
name. It will look like this --command -par1 val1 -par2 val 2. "-par1" and "-par2" should be
handled with setting appropriate input state.
*/

/*virtual*/ gboolean
cli_command_info_handle_input(CLICommandInfo* this, GString* arg)
{
	g_assert(this);
	CLICommandInfoClass* class = CLI_COMMAND_INFO_GET_CLASS(this);

	g_assert(class->_handleInput);
	return class->_handleInput(this, arg);	
}


/*virtual*/ gboolean
cli_command_info_is_valid(CLICommandInfo* this)
{
	g_assert(this);
	CLICommandInfoClass* class = CLI_COMMAND_INFO_GET_CLASS(this);

	g_assert(class->_isValid);
	return class->_isValid(this);	
}

gint
cli_command_info_get_input_state(CLICommandInfo* this)
{
	g_assert(this);
	
	CLICommandInfoPrivate* data = cli_command_info_get_instance_private(this);

	return data->_inputState;
}

void
cli_command_info_set_input_state(CLICommandInfo* this, gint state)
{
	g_assert(this);

	CLICommandInfoPrivate* data = cli_command_info_get_instance_private(this);

	data->_inputState = state;	
}