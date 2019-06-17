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

#include "CLIDeleteIncludeCommand.h"

#include "ModelProject.h"

typedef struct _CLIDeleteIncludeCommand
{
    CLICommandInfo* parent_object;

    GString* _projectLoc;
    GPtrArray* _folders;
} CLIDeleteIncludeCommand;

enum CLI_DELETE_INCLUDE_COMMAND_INPUT_STATES
{
    CLI_DELETE_INCLUDE_COMMAND_START,
    CLI_DELETE_INCLUDE_COMMAND_PROJ_LOC,

    CLI_DELETE_INCLUDE_COMMAND_STATES_COUNT
};

G_DEFINE_TYPE(CLIDeleteIncludeCommand, cli_delete_include_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_delete_include_command_is_valid(CLICommandInfo* command)
{
    CLIDeleteIncludeCommand* this = CLI_DELETE_INCLUDE_COMMAND(command);

    return this->_projectLoc != NULL && this->_folders->len > 0;
}

static gboolean
cli_delete_include_command_handle_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_DELETE_INCLUDE_COMMAND_PROJ_LOC);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_DELETE_INCLUDE_COMMAND_START);

        CLIDeleteIncludeCommand* this = CLI_DELETE_INCLUDE_COMMAND(command);
        switch(state)
        {
            case CLI_DELETE_INCLUDE_COMMAND_START:
            {
                if(this->_projectLoc == NULL)
                    this->_projectLoc = input;
                else
                    g_ptr_array_add(this->_folders, input);

                break;
            }
            case CLI_DELETE_INCLUDE_COMMAND_PROJ_LOC:
            {
                if(this->_projectLoc != NULL)
                    return TRUE;

                this->_projectLoc = input;
                break;
            }
        }
    }

    return FALSE;
}

///Function for foreach looping through GPtrArray and removing include folder
///from project
static void
cli_delete_include_command_delete_include(gpointer folder, gpointer proj)
{
	GString* incl = (GString*)folder;
	ModelProject* project = (ModelProject*) proj;

	model_project_remove_include_folder(project, incl);

	g_print("%s was removed.\n", incl->str);
}

///Delete include folder command handler
static void
cli_delete_include_command_execute(CLICommandInfo* command)
{
    GError* innerError = NULL;
	CLIDeleteIncludeCommand* this = CLI_DELETE_INCLUDE_COMMAND(command);

    GPtrArray* args = this->_folders;
	GString* projLoc = this->_projectLoc;

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(projLoc, &proj, &innerError))
	{
		g_print("Project does not exist\n");
		return;
	}
    if(innerError != NULL)
    {
        g_print(innerError->message);
        return;
    }

	g_ptr_array_foreach(args, cli_delete_include_command_delete_include, proj);

	model_project_save(proj, NULL, &innerError);
    if(innerError != NULL)
    {
        g_print(innerError->message);
        return;
    }

	g_object_unref(proj);
}

static void
cli_delete_include_command_dispose(GObject* obj)
{
    CLIDeleteIncludeCommand* this = CLI_DELETE_INCLUDE_COMMAND(obj);

    int fileCount = this->_folders->len;
    for(int i = 0; i<fileCount; i++)
        g_string_free((GString*)this->_folders->pdata[i], TRUE);
    
    this->_folders = NULL;

    if(this->_projectLoc != NULL)
    {
        g_string_free(this->_projectLoc, TRUE);
        this->_projectLoc = NULL;
    }

    G_OBJECT_CLASS(cli_delete_include_command_parent_class)->dispose(obj);
}

static void
cli_delete_include_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_delete_include_command_parent_class)->finalize(obj);
}

static void
cli_delete_include_command_class_init(CLIDeleteIncludeCommandClass* klass)
{
    CLICommandInfoClass* parent_class = CLI_COMMAND_INFO_CLASS(klass);

    parent_class->_standardErrorMSG =
        g_string_new(g_strdup("Delete include folder command usage:\n\t--deleteInclude projName fold\nprojName - path to project definition, fold - path to folder you want to remove\n"));

    parent_class->_execute = cli_delete_include_command_execute;
    parent_class->_handleInput = cli_delete_include_command_handle_input;
    parent_class->_isValid = cli_delete_include_command_is_valid;
    parent_class->_dispose = cli_delete_include_command_dispose;
    parent_class->_finalize = cli_delete_include_command_finalize;
}

static void
cli_delete_include_command_init(CLIDeleteIncludeCommand* this)
{
    this->_folders = g_ptr_array_new();
    this->_projectLoc = NULL;
}

CLIDeleteIncludeCommand*
cli_delete_include_command_new()
{
    return g_object_new(CLI_TYPE_DELETE_INCLUDE_COMMAND, NULL);
}