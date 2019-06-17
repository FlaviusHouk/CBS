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

#include "CLICreateCommand.h"

#include "ModelProjectManager.h"

enum CLI_CREATE_COMMAND_INPUT_STATES
{
    CREATE_COMMAND_START,
    CREATE_COMMAND_NAME,
    CREATE_COMMAND_TEMPLATE_NAME,

    CLI_CREATE_COMMAND_INPUT_STATES_COUNT
};

typedef struct _CLICreateCommand
{
    CLICommandInfo parent_class;

    GString* _name;
    GString* _templateName;
} CLICreateCommand;

G_DEFINE_TYPE(CLICreateCommand, cli_create_command, CLI_TYPE_COMMAND_INFO);

static void
cli_create_command_dispose(GObject* obj)
{
    CLICreateCommand* this = CLI_CREATE_COMMAND(obj);

    if(this->_name != NULL)
    {
        g_string_free(this->_name, TRUE);
        this->_name = NULL;
    }
    if(this->_templateName != NULL)
    {
        g_string_free(this->_templateName, TRUE);
        this->_templateName = NULL;
    }

    G_OBJECT_CLASS(cli_create_command_parent_class)->dispose(obj);   
}

static void
cli_create_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_create_command_parent_class)->finalize(obj);
}

static gboolean
cli_create_command_is_valid(CLICommandInfo* command)
{
    CLICreateCommand* this = CLI_CREATE_COMMAND(command);

    return this->_name != NULL;
}

static void 
cli_create_command_execute(CLICommandInfo* command)
{
    CLICreateCommand* this = CLI_CREATE_COMMAND(command);

	ModelProjectManager* manager = model_project_manager_new();

	GString* loc = this->_name;

	GError* innerError = NULL;
	model_project_manager_create_project(loc, this->_templateName, &innerError);
	if(innerError)
	{
		g_print(innerError->message);
		return;
	}

	g_print("Project was successfully created!\n");

	g_object_unref(manager);
}

static gboolean
cli_create_command_handle_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-name"))
    {
        cli_command_info_set_input_state(command, CREATE_COMMAND_NAME);
    }
    else if (g_str_equal(input->str, "-templateName"))
    {
        cli_command_info_set_input_state(command, CREATE_COMMAND_TEMPLATE_NAME);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        CLICreateCommand* this = CLI_CREATE_COMMAND(command);

        switch(state)
        {
            case CREATE_COMMAND_START:
            {
                if(this->_name == NULL)
                {
                    this->_name = input;
                    break;
                }
                else if(this->_templateName == NULL)
                {
                    this->_templateName = input;
                    break;
                }
                else
                {
                    return TRUE;
                }
            }
            case CREATE_COMMAND_NAME:
            {
                if(this->_name != NULL)
                    return TRUE;

                this->_name = input;
                cli_command_info_set_input_state(command, CREATE_COMMAND_START);
            }
            case CREATE_COMMAND_TEMPLATE_NAME:
            {
                if(this->_templateName != NULL)
                    return TRUE;

                this->_templateName = input;
                cli_command_info_set_input_state(command, CREATE_COMMAND_START);
            }
        }
    }

    return FALSE;
}

static void
cli_create_command_class_init(CLICreateCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

    parentClass->_standardErrorMSG =
        g_string_new(g_strdup("Create command usage:\n\t--create projName\nprojName - path to project definition file to be created\n"));

    parentClass->_execute = cli_create_command_execute;
    parentClass->_handleInput = cli_create_command_handle_input;
    parentClass->_isValid = cli_create_command_is_valid;
    parentClass->_dispose = cli_create_command_dispose;
    parentClass->_finalize = cli_create_command_finalize;
}

static void
cli_create_command_init(CLICreateCommand* this)
{
    cli_command_info_set_input_state(CLI_COMMAND_INFO(this), CREATE_COMMAND_START);
    this->_name = NULL;
}

CLICreateCommand*
cli_create_command_new()
{
    return g_object_new(CLI_TYPE_CREATE_COMMAND, NULL);
}
