#include "stdlib.h"

#include "CLI.h"
#include "Helper.h"
#include "CLICommandInfo.h"

struct _CLICommandParser
{
	GObject parent_instance;

	GPtrArray* _commands;
};

G_DEFINE_TYPE(CLICommandParser, cli_command_parser, G_TYPE_OBJECT)

static void ProcessCreateCommand(CLICommandInfo* command)
{

}

static void ProcessAddCommand(CLICommandInfo* command)
{

}

static void ProcessDeleteCommand(CLICommandInfo* command)
{

}

static void ProcessAddDepCommand(CLICommandInfo* command)
{

}

static void ProcessRemDepCommand(CLICommandInfo* command)
{

}

static void ProcessBuildCommand(CLICommandInfo* command)
{

}

static void ProcessPublishCommand(CLICommandInfo* command)
{

}

static GPtrArray* AvailableCommands;

static void
cli_command_parser_class_init(CLICommandParserClass* class)
{
	AvailableCommands = g_ptr_array_new();

	g_ptr_array_add(AvailableCommands, 
			cli_command_info_new(g_string_new("--create"),
					CREATE,
					1,
					1,
					ProcessCreateCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--addFile"),
					ADD_FILE,
					-1,
					2,
					ProcessAddCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--deleteFile"),
					DELETE_FILE,
					-1,
					3,
					ProcessDeleteCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--addDependency"),
					ADD_DEP,
					1,
					4,
					ProcessAddDepCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--remDependency"),
					REM_DEP,
					1,
					5,
					ProcessRemDepCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--build"),
					BUILD,
					-1,
					6,
					ProcessBuildCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--publish"),
					PUBLISH,
					2,
					7,
					ProcessPublishCommand));
}

static void
cli_command_parser_init(CLICommandParser* this)
{

}

static gint current_state = START;
static CLICommandInfo* currentCommand;

static void cli_command_parser_set_input_state(gint state)
{
	if(current_state != START)
	{
		g_assert(currentCommand);
		g_assert(cli_command_info_get_args(currentCommand)->len);
	}
	else
	{
		currentCommand = (CLICommandInfo*)g_ptr_array_index(AvailableCommands, CREATE);
	}

	current_state = state;
	currentCommand = (CLICommandInfo*)g_ptr_array_index(AvailableCommands, state);
}

static void cli_command_parser_add_to_current_collection(GString* arg)
{
	g_ptr_array_add(cli_command_info_get_args(currentCommand) ,arg);
}

static void cli_command_parser_parse_commands(gpointer data, gpointer userData)
{
	GString* str = (GString*)data;
	CLICommandParser* this = (CLICommandParser*)userData;
	
	if(strcmp(str->str, "--create") == 0)
		cli_command_parser_set_input_state(CREATE);
	else if(strcmp(str->str, "--addFile") == 0)
		cli_command_parser_set_input_state(ADD_FILE);
	else if(strcmp(str->str, "--deleteFile") == 0)
		cli_command_parser_set_input_state(DELETE_FILE);
    else if(strcmp(str->str, "--addDependency") == 0)
		cli_command_parser_set_input_state(ADD_DEP);
	else if(strcmp(str->str, "--build") == 0)
		cli_command_parser_set_input_state(BUILD);
	else
		cli_command_parser_add_to_current_collection(str);
}

static void
cli_command_parser_filter_commands (gpointer obj, gpointer user_data)
{
	CLICommandInfo* com = (CLICommandInfo*)obj;
	CLICommandParser* this = (CLICommandParser*)user_data;

	if(cli_command_info_get_args_count(com) > 0)
		g_ptr_array_add(this->_commands, com);
}

static void
cli_command_parser_process_command(gpointer obj, gpointer user_data)
{
	CLICommandInfo* com = (CLICommandInfo*)obj;

	cli_command_info_process_command(com);
}


static void
cli_command_parser_clean_commands(gpointer obj, gpointer user_data)
{
	CLICommandInfo* com = (CLICommandInfo*)obj;
	CLICommandParser* this = (CLICommandParser*)user_data;

	g_ptr_array_remove(this->_commands, com);
}

static gint 
cli_command_parser_compare_commands(gconstpointer obj1, 
									gconstpointer obj2)
{
	CLICommandInfo* info1 = (CLICommandInfo*)obj1;
	CLICommandInfo* info2 = (CLICommandInfo*)obj2;

	return cli_command_info_get_order(info1) - cli_command_info_get_order(info2);
}

CLICommandParser* cli_command_parser_new(char** args, int argn)
{
	CLICommandParser* this = g_object_new(CLI_TYPE_COMMAND_PARSER, NULL);
	this->_commands = g_ptr_array_new();

	GPtrArray* strCollection = g_ptr_array_new();
	for(int i = 1; i<argn; i++)
		g_ptr_array_add(strCollection, g_string_new(args[i]));

	g_ptr_array_foreach(strCollection, 
					    cli_command_parser_parse_commands, 
						this);

	g_ptr_array_foreach(AvailableCommands,
						cli_command_parser_filter_commands,
						this);

	g_ptr_array_sort(this->_commands, cli_command_parser_compare_commands);
}

void cli_command_parser_execute(CLICommandParser* this)
{
	g_ptr_array_foreach(this->_commands,
						cli_command_parser_process_command,
						this);

	g_ptr_array_foreach(this->_commands,
						cli_command_parser_clean_commands,
						this);
}