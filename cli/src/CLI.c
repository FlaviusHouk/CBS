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

#include "stdlib.h"

#include "CLI.h"

#include "CLIAddDepCommand.h"
#include "CLIAddFileCommand.h"
#include "CLIAddIncludeCommand.h"
#include "CLIBuildCommand.h"
#include "CLICreateCommand.h"
#include "CLIDeleteDepCommand.h"
#include "CLIDeleteFileCommand.h"
#include "CLIDeleteIncludeCommand.h"
#include "CLIHelpCommand.h"
#include "CLITestCommand.h"
#include "CLIVersionCommand.h"

#include "Helper.h"
#include "CLICommandInfo.h"
#include "ModelProjectDependency.h"
#include "ModelProjectManager.h"

struct _CLICommandParser
{
	GObject parent_instance;

	CLICommandInfo* _command; //command that will be invoked.
	gboolean _wrongInput; //flag that indicates error on arguments processing.
};

G_DEFINE_TYPE(CLICommandParser, cli_command_parser, G_TYPE_OBJECT)

///Static constructor for CommandParser class.
static void
cli_command_parser_class_init(CLICommandParserClass* class)
{}

///For now only one command per call might be executed.
///This setter checks is there any command set, if it is, error is raised.
static void
cli_command_parser_set_command(CLICommandParser* this, CLICommandInfo* command)
{
	g_assert(this);

	if(this->_command == NULL)
	{
		this->_command = command;
	}
	else
	{
		g_print("Wrong usage\n");
		return;
	}
}

static void
cli_command_parser_init(CLICommandParser* this)
{}

///Loops through all transfered params and loads command or add string as a parameter.
static void 
cli_command_parser_parse_commands(gpointer data, gpointer userData)
{
	GString* str = (GString*)data;
	CLICommandParser* this = (CLICommandParser*)userData;
	
	if(this->_wrongInput)
		return;

	if(strcmp(str->str, "--create") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_create_command_new()));
	else if(strcmp(str->str, "--addFile") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_add_file_command_new()));
	else if(strcmp(str->str, "--deleteFile") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_delete_file_command_new()));
	else if(strcmp(str->str, "--addInclude") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_add_include_command_new()));
	else if(strcmp(str->str, "--deleteInclude") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_delete_include_command_new()));
  else if(strcmp(str->str, "--addDependency") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_add_dep_command_new()));
	else if(strcmp(str->str, "--deleteDependency") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_delete_dep_command_new()));
	else if(strcmp(str->str, "--build") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_build_command_new()));
	else if(strcmp(str->str, "--test") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_test_command_new()));
	else if(strcmp(str->str, "--help") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_help_command_new()));
	else if(strcmp(str->str, "--version") == 0)
		cli_command_parser_set_command(this, CLI_COMMAND_INFO(cli_version_command_new()));
	else
	{
		if(this->_command == NULL)
		{
			g_print("Wrong usage\n");
			return;
		}
		
		this->_wrongInput = cli_command_info_handle_input(this->_command, str);
	}
}

///Output pipeline for pushing messages from libcbs.
static void
cli_output_pipe(gchar* message, gint type)
{
	switch(type)
	{
		case MESSAGE:
		{
			g_print(message);
			break;
		}
		case ERROR:
		{
			g_printerr(message);
			break;
		}
	}
}

CLICommandParser* cli_command_parser_new(char** args, int argn)
{
	model_init_output_pipe(cli_output_pipe);
	CLICommandParser* this = g_object_new(CLI_TYPE_COMMAND_PARSER, NULL);

	GPtrArray* strCollection = g_ptr_array_new();
	for(int i = 1; i<argn; i++)
		g_ptr_array_add(strCollection, g_string_new(args[i]));

	g_ptr_array_foreach(strCollection, 
					    cli_command_parser_parse_commands, 
						this);

	return this;
}

void cli_command_parser_execute(CLICommandParser* this)
{
	if(!this->_wrongInput)
		cli_command_info_process_command(this->_command);
}