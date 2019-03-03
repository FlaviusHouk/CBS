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
#include "Helper.h"
#include "CLICommandInfo.h"
#include "ModelProjectDependency.h"
#include "ModelProjectManager.h"

struct _CLICommandParser
{
	GObject parent_instance;

	GPtrArray* _commands;
};

G_DEFINE_TYPE(CLICommandParser, cli_command_parser, G_TYPE_OBJECT)

static void ProcessCreateCommand(CLICommandInfo* command)
{
	GPtrArray* args = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != args->len)
	{
		g_print("Create command usage:\n\t--create projName\nprojName - path to project definition file to be created\n");
		g_assert(FALSE);
	}

	ModelProjectManager* manager = model_project_manager_new();

	GString* loc = (GString*) g_ptr_array_index(args, 0);

	model_project_manager_create_project(loc);

	g_object_unref(manager);
}

static void AddFileToProject(gpointer fileName, gpointer project)
{
	GString* fileLoc = (GString*)fileName;
	ModelProject* proj = (ModelProject*) project;

	ModelSourceFile* file = model_source_file_new(fileLoc);

	model_project_add_source_file(proj, file);
}

static void ProcessAddCommand(CLICommandInfo* command)
{
	GPtrArray* files = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != files->len)
	{
		g_print("Add file command usage:\n\t--addFile projName file1 file2 ... fileN\nprojName - path to project definition, file - path to file you want to add\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(files, 0);
	g_ptr_array_remove(files, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_ptr_array_foreach(files, AddFileToProject, proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void RemoveFileFromProject(gpointer fileName, gpointer project)
{
	GString* fileLoc = (GString*)fileName;
	ModelProject* proj = (ModelProject*) project;

	ModelSourceFile* file = model_source_file_new(fileLoc);

	model_project_remove_source_file(proj, file);
}

static void ProcessDeleteCommand(CLICommandInfo* command)
{
	GPtrArray* files = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != files->len)
	{
		g_print("Delete file command usage:\n\t--deleteFile projName file\nprojName - path to project definition, file - path to file you want to remove\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(files, 0);
	g_ptr_array_remove(files, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_ptr_array_foreach(files, RemoveFileFromProject, proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
AddIncludeFolder(gpointer folder, gpointer proj)
{
	GString* incl = (GString*)folder;
	ModelProject* project = (ModelProject*) proj;

	model_project_add_include_folder(project, incl);
}

static void
ProcessAddInclCommand(CLICommandInfo* command)
{
	GPtrArray* args = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != args->len)
	{
		g_print("Add include folder command usage:\n\t--addInclude projName fold1 fold2 ... foldN\nprojName - path to project definition, fold - path to folder you want to add\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(args, 0);
	g_ptr_array_remove(args, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_ptr_array_foreach(args, AddIncludeFolder, proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
DeleteIncludeFolder(gpointer folder, gpointer proj)
{
	GString* incl = (GString*)folder;
	ModelProject* project = (ModelProject*) proj;

	model_project_remove_include_folder(project, incl);
}

static void
ProcessDeleteInclCommand(CLICommandInfo* command)
{
	GPtrArray* args = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != args->len)
	{
		g_print("Delete include folder command usage:\n\t--deleteInclude projName fold\nprojName - path to project definition, fold - path to folder you want to remove\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(args, 0);
	g_ptr_array_remove(args, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_ptr_array_foreach(args, DeleteIncludeFolder, proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void ProcessAddDepCommand(CLICommandInfo* command)
{
	GPtrArray* dep = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != dep->len)
	{
		g_print("Add dependency command usage:\n\t--addDependency projName depType depName\nprojName - path to project definition, depType - integer number(dependency type), depName - dependency representation according to type\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(dep, 0);
	g_ptr_array_remove(dep, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_assert(dep->len == 2);

	int type = atoi(((GString*)g_ptr_array_index(dep, 0))->str);
	GString* dependency = (GString*)g_ptr_array_index(dep, 1);

	ModelProjectDependency* depObj = model_project_dependency_new(dependency, type);

	model_project_add_dependency(proj, depObj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
RemoveDependency(gpointer obj, gpointer data)
{
	GString* dep = (GString*)obj;
	ModelProject* proj = (ModelProject*)data;

	model_project_remove_dependency_by_name(proj, dep);
}

static void ProcessRemDepCommand(CLICommandInfo* command)
{
	GPtrArray* deps = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != deps->len)
	{
		g_print("Delete dependency command usage:\n\t--deleteDependency projName depName\nprojName - path to project definition, depName - representation of dependency you want to remove\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(deps, 0);
	g_ptr_array_remove(deps, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	g_ptr_array_foreach(deps, RemoveDependency, proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void ProcessBuildCommand(CLICommandInfo* command)
{
	GPtrArray* params = cli_command_info_get_args(command);
	int expectedCount = cli_command_info_get_args_count(command);
	if(expectedCount != params->len)
	{
		g_print("Build command usage:\n\t--build projName\nprojName - path to project definition you want to build\n");
		g_assert(FALSE);
	}

	GString* projLoc = g_ptr_array_index(params, 0);
	g_ptr_array_remove(params, projLoc);

	ModelProject* proj = model_project_load_or_create_project(projLoc);

	ModelProjectManager* manager = model_project_manager_new();

	model_project_manager_build_project(manager, proj, NULL);

	g_object_unref(manager);
	g_object_unref(proj);
}

static void
ProcessHelpCommand(CLICommandInfo* command)
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
					2,
					3,
					ProcessDeleteCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--addInclude"),
					ADD_INCL,
					-1,
					4,
					ProcessAddInclCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--deleteInclude"),
					DELETE_INCL,
					2,
					5,
					ProcessDeleteInclCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--addDependency"),
					ADD_DEP,
					3,
					6,
					ProcessAddDepCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--deleteDependency"),
					REM_DEP,
					2,
					7,
					ProcessRemDepCommand));

	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--build"),
					BUILD,
					1,
					8,
					ProcessBuildCommand));
	g_ptr_array_add(AvailableCommands,
			cli_command_info_new(g_string_new("--help"),
					HELP,
					0,
					8,
					ProcessHelpCommand));
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
		currentCommand = (CLICommandInfo*)g_ptr_array_index(AvailableCommands, CREATE - 1);
	}

	current_state = state;
	currentCommand = (CLICommandInfo*)g_ptr_array_index(AvailableCommands, state - 1);
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
	else if(strcmp(str->str, "--addInclude") == 0)
		cli_command_parser_set_input_state(ADD_INCL);
	else if(strcmp(str->str, "--deleteInclude") == 0)
		cli_command_parser_set_input_state(DELETE_INCL);
    else if(strcmp(str->str, "--addDependency") == 0)
		cli_command_parser_set_input_state(ADD_DEP);
	else if(strcmp(str->str, "--deleteDependency") == 0)
		cli_command_parser_set_input_state(REM_DEP);
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

	if(cli_command_info_get_args(com)->len > 0)
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

	return this;
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