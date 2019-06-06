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

#include "CLIAddDepCommand.h"

#include "ModelProject.h"

typedef struct _CLIAddDepCommand
{
    CLICommandInfo parent_object;

    GString* _projLoc;
    gint* _depType;
    GString* _depRepresentation;
} CLIAddDepCommand;

enum CLI_ADD_DEP_COMMAND_INPUT_STATES
{
    CLI_ADD_DEP_COMMAND_START,
    CLI_ADD_DEP_COMMAND_PROJ_LOC,
    CLI_ADD_DEP_COMMAND_DEP_TYPE,
    CLI_ADD_DEP_COMMAND_DEP_REPRESENTATION,

    CLI_ADD_DEP_COMMAND_STATES_COUNT
};

G_DEFINE_TYPE(CLIAddDepCommand, cli_add_dep_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_add_dep_command_is_valid(CLICommandInfo* command)
{
    CLIAddDepCommand* this = CLI_ADD_DEP_COMMAND(command);

    return this->_projLoc != NULL && 
           this->_depType != NULL && 
           this->_depRepresentation != NULL;
}

static gboolean
cli_add_dep_command_handle_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_ADD_DEP_COMMAND_PROJ_LOC);
    }
    else if(g_str_equal(input->str, "-depType"))
    {
        cli_command_info_set_input_state(command, CLI_ADD_DEP_COMMAND_DEP_TYPE);
    }
    else if(g_str_equal(input->str, "-depRepr"))
    {
        cli_command_info_set_input_state(command, CLI_ADD_DEP_COMMAND_DEP_REPRESENTATION);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_ADD_DEP_COMMAND_START);
        
        CLIAddDepCommand* this = CLI_ADD_DEP_COMMAND(command);   

        switch(state)
        {
            case CLI_ADD_DEP_COMMAND_START:
            {
                if(this->_projLoc == NULL)
                {
                    this->_projLoc = input;
                    break;
                }
                else if (this->_depType == NULL)
                {
                    this->_depType = (gint*)g_malloc(sizeof(gint));
                    *(this->_depType) = atoi(input->str);
                    break;
                }
                else if(this->_depRepresentation == NULL)
                {
                    this->_depRepresentation = input;
                    break;
                }
                else
                {
                    return TRUE;
                }
            }
            case CLI_ADD_DEP_COMMAND_PROJ_LOC:
            {
                if(this->_projLoc != NULL)
                    return TRUE;

                this->_projLoc = input;
                break;
            }
            case CLI_ADD_DEP_COMMAND_DEP_TYPE:
            {
                if(this->_depType != NULL)
                    return TRUE;

                this->_depType = (gint*)g_malloc(sizeof(gint));
                *(this->_depType) = atoi(input->str);
                break;                
            }
            case CLI_ADD_DEP_COMMAND_DEP_REPRESENTATION:
            {
                if(this->_depRepresentation != NULL)
                    return TRUE;

                this->_depRepresentation = input;
                break;
            }
        }
    }

    return FALSE;
}

///Add dependency command handler
static void 
cli_add_dep_command_execute(CLICommandInfo* command)
{
    CLIAddDepCommand* this = CLI_ADD_DEP_COMMAND(command);

	GString* projLoc = this->_projLoc;
	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	ModelProjectDependency* depObj = model_project_dependency_new(this->_depRepresentation,
                                                                  *(this->_depType));

	model_project_add_dependency(proj, depObj);
	model_project_save(proj, NULL);

	g_print("%s was added with %d type.\n", this->_depRepresentation->str, *(this->_depType));

	g_object_unref(proj);
}

static void
cli_add_dep_command_dispose(GObject* obj)
{
    CLIAddDepCommand* this = CLI_ADD_DEP_COMMAND(obj);

    if(this->_projLoc != NULL)
    {
        g_string_free(this->_projLoc, TRUE);
        this->_projLoc = NULL;
    }

    if(this->_depType != NULL)
    {
        g_free(this->_depType);
        this->_depType = NULL;
    }

    if(this->_depRepresentation != NULL)
    {
        g_string_free(this->_depRepresentation, TRUE);
        this->_depRepresentation = NULL;
    }

    G_OBJECT_CLASS(cli_add_dep_command_parent_class)->dispose(obj);
}

static void
cli_add_dep_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_add_dep_command_parent_class)->finalize(obj);
}

static void
cli_add_dep_command_class_init(CLIAddDepCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

    parentClass->_standardErrorMSG = 
        g_string_new(g_strdup("Add dependency command usage:\n\t--addDependency projName depType depName\nprojName - path to project definition, depType - integer number(dependency type), depName - dependency representation according to type\n"));

    parentClass->_execute = cli_add_dep_command_execute;
    parentClass->_handleInput = cli_add_dep_command_handle_input;
    parentClass->_isValid = cli_add_dep_command_is_valid;
    parentClass->_dispose = cli_add_dep_command_dispose;
    parentClass->_finalize = cli_add_dep_command_finalize;
}

static void
cli_add_dep_command_init(CLIAddDepCommand* this)
{
    cli_command_info_set_input_state(CLI_COMMAND_INFO(this), CLI_ADD_DEP_COMMAND_START);

    this->_projLoc = NULL;
    this->_depType = NULL;
    this->_depRepresentation = NULL;
}

CLIAddDepCommand*
cli_add_dep_command_new()
{
    return g_object_new(CLI_TYPE_ADD_DEP_COMMAND, NULL);
}