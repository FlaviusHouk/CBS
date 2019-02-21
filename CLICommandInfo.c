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

struct _CLICommandInfo
{
	GObject parent_instance;
	
    GString* _command;
	int _commandType;
	int _argsCount;
	int _order;

	GPtrArray* _args;
	void (*_action)(CLICommandInfo* com);

	void* data;
};

G_DEFINE_TYPE(CLICommandInfo, cli_command_info, G_TYPE_OBJECT)

static void
cli_command_info_class_init(CLICommandInfoClass* class)
{

}

static void
cli_command_info_init(CLICommandInfo* this)
{

}

CLICommandInfo* cli_command_info_new(GString* command, 
									int commandType,
									int argsCount, 
									int order, 
									void (*action)(CLICommandInfo* com))
{
	CLICommandInfo* this;
	this = g_object_new(CLI_TYPE_COMMAND_INFO, NULL);

	this->_command = command;
	this->_argsCount = argsCount;
	this->_order = order;

	this->_args = g_ptr_array_new();
	this->_action = action;

	return this;
}

GString* cli_command_info_get_command(CLICommandInfo* this)
{
	return this->_command;
}

void cli_command_info_set_command(CLICommandInfo* this, GString* value)
{
	this->_command = value;
}

int cli_command_info_get_args_count(CLICommandInfo* this)
{
	return this->_argsCount;
}

void cli_command_info_set_args_count(CLICommandInfo* this, int value)
{
	this->_argsCount = value;
}

int cli_command_info_get_order(CLICommandInfo* this)
{
	return this->_order;
}

void cli_command_info_set_order(CLICommandInfo* this, int value)
{
	this->_order = value;
}

GPtrArray* cli_command_info_get_args(CLICommandInfo* this)
{
	return this->_args;
}

void cli_command_info_process_command(CLICommandInfo* this)
{
	if(this->_action != NULL)
		this->_action(this);
}
