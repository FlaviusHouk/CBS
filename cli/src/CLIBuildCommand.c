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

#include "CLIBuildCommand.h"

#include "ModelProjectManager.h"

typedef struct _CLIBuildCommand
{
    CLICommandInfo parent_object;

    GString* _projLoc;
    GString* _configName;
} CLIBuildCommand;

enum CLI_BUILD_COMMAND_INPUT_STATES
{
    CLI_BUILD_COMMAND_START,
    CLI_BUILD_COMMAND_PROJ_LOC,
    CLI_BUILD_COMMAND_CONFIG_NAME,

    CLI_BUILD_COMMAND_STATES_COUNT
};

G_DEFINE_TYPE(CLIBuildCommand, cli_build_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_build_command_is_valid(CLICommandInfo* command)
{
    CLIBuildCommand* this = CLI_BUILD_COMMAND(command);

    return this->_projLoc != NULL;
}

static gboolean
cli_build_command_handle_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_BUILD_COMMAND_PROJ_LOC);
    }
    else if(g_str_equal(input->str, "-config"))
    {
        cli_command_info_set_input_state(command, CLI_BUILD_COMMAND_CONFIG_NAME);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_BUILD_COMMAND_START);

        CLIBuildCommand* this = CLI_BUILD_COMMAND(command);
        switch(state)
        {
            case CLI_BUILD_COMMAND_START:
            {
                if(this->_projLoc == NULL)
                {
                    this->_projLoc = input;
                    break;
                }
                else if(this->_configName == NULL)
                {
                    this->_configName = input;
                    break;
                }
                else
                {
                    return TRUE;
                }
            }
            case CLI_BUILD_COMMAND_PROJ_LOC:
            {
                if(this->_projLoc != NULL)
                    return TRUE;

                this->_projLoc = input;
                break;
            }
            case CLI_BUILD_COMMAND_CONFIG_NAME:
            {
                if(this->_configName != NULL)
                    return TRUE;

                this->_configName = input;
                break;
            }
        }
    }

    return FALSE;
}

///Build command handler
static void 
cli_build_command_execute(CLICommandInfo* command)
{
	CLIBuildCommand* this = CLI_BUILD_COMMAND(command);

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(this->_projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	ModelProjectManager* manager = model_project_manager_new();

	GError* error = NULL;
	model_project_manager_build_project(manager, proj, this->_configName, &error);
	if(error)
	{
		g_print(error->message);
		return;
	}

	g_object_unref(manager);
	g_object_unref(proj);
}

static void
cli_build_command_dispose(GObject* obj)
{
    CLIBuildCommand* this = CLI_BUILD_COMMAND(obj);

    if(this->_projLoc != NULL)
    {
        g_string_free(this->_projLoc, TRUE);
        this->_projLoc = NULL;
    }

    if(this->_configName != NULL)
    {
        g_string_free(this->_configName, TRUE);
        this->_configName = NULL;
    }

    G_OBJECT_CLASS(cli_build_command_parent_class)->dispose(obj);
}

static void
cli_build_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(obj)->finalize(obj);
}

static void
cli_build_command_class_init(CLIBuildCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

    parentClass->_standardErrorMSG = 
        g_string_new(g_strdup("Build command usage:\n\t--build projName\nprojName - path to project definition you want to build\n"));

    parentClass->_execute = cli_build_command_execute;
    parentClass->_handleInput = cli_build_command_handle_input;
    parentClass->_isValid = cli_build_command_is_valid;
    parentClass->_dispose = cli_build_command_dispose;
    parentClass->_finalize = cli_build_command_finalize;
}

static void
cli_build_command_init(CLIBuildCommand* this)
{
    this->_projLoc = NULL;
    this->_configName = NULL;
}

CLIBuildCommand*
cli_build_command_new()
{
    return g_object_new(CLI_TYPE_BUILD_COMMAND, NULL);
}