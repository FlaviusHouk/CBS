#include "CLIDeleteDepCommand.h"

#include "ModelProject.h"

typedef struct _CLIDeleteDepCommand
{
    CLICommandInfo parent_object;

    GString* _projLoc;
    GString* _depName;
} CLIDeleteDepCommand;

G_DEFINE_TYPE(CLIDeleteDepCommand, cli_delete_dep_command, CLI_TYPE_COMMAND_INFO);

enum CLI_DELETE_DEP_COMMAND_INPUT_STATES
{
    CLI_DELETE_DEP_COMMAND_START,
    CLI_DELETE_DEP_COMMAND_PROJ_LOC,
    CLI_DELETE_DEP_COMMAND_DEP_NAME,

    CLI_DELETE_DEP_COMMAND_STATES_COUNT
};

static gboolean
cli_delete_dep_command_is_valid(CLICommandInfo* command)
{
    g_assert(command);
    CLIDeleteDepCommand* this = CLI_DELETE_DEP_COMMAND(command);

    return this->_projLoc != NULL && this->_depName != NULL;
}

static void
cli_delete_dep_command_handle_input(CLICommandInfo* command, GString* input, gboolean* breakInput)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_DELETE_DEP_COMMAND_PROJ_LOC);
    }
    else if(g_str_equal(input->str, "-depName"))
    {
        cli_command_info_set_input_state(command, CLI_DELETE_DEP_COMMAND_DEP_NAME);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_DELETE_DEP_COMMAND_START);

        CLIDeleteDepCommand* this = CLI_DELETE_DEP_COMMAND(command);

        switch(state)
        {
            case CLI_DELETE_DEP_COMMAND_START:
            {
                if(this->_projLoc == NULL)
                {
                    this->_projLoc = input;
                    break;
                }
                else if(this->_depName = input)
                {
                    this->_depName = input;
                    break;
                }
                else
                {
                    *breakInput = TRUE;
                    return;
                }
            }
            case CLI_DELETE_DEP_COMMAND_PROJ_LOC:
            {
                if(this->_projLoc != NULL)
                {
                    *breakInput = TRUE;
                    return;
                }

                this->_projLoc = input;
                break;
            }
            case CLI_DELETE_DEP_COMMAND_DEP_NAME:
            {
                if(this->_depName != NULL)
                {
                    *breakInput = TRUE;
                    return;
                }

                this->_depName = input;
                break;
            }
        }
    }
}

///Delete dependency command handler
static void 
cli_delete_dep_command_execute(CLICommandInfo* command)
{
	CLIDeleteDepCommand* this = CLI_DELETE_DEP_COMMAND(command);

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(this->_projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	model_project_remove_dependency_by_name(proj, this->_depName);

	g_print("%s was removed.\n", this->_depName->str);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
cli_delete_dep_command_dispose(GObject* obj)
{
    CLIDeleteDepCommand* this = CLI_DELETE_DEP_COMMAND(obj);

    if(this->_projLoc != NULL)
    {
        g_string_free(this->_projLoc, TRUE);
        this->_projLoc = NULL;
    }

    if(this->_depName != NULL)
    {
        g_string_free(this->_depName, TRUE);
        this->_depName = NULL;
    }

    G_OBJECT_CLASS(cli_delete_dep_command_parent_class)->dispose(obj);
}

static void
cli_delete_dep_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_delete_dep_command_parent_class)->finalize(obj);
}

static void
cli_delete_dep_command_class_init(CLIDeleteDepCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

    parentClass->_standardErrorMSG = 
        g_string_new(g_strdup("Delete dependency command usage:\n\t--deleteDependency projName depName\nprojName - path to project definition, depName - representation of dependency you want to remove\n"));

    parentClass->_execute = cli_delete_dep_command_execute;
    parentClass->_handleInput = cli_delete_dep_command_handle_input;
    parentClass->_isValid = cli_delete_dep_command_is_valid;
    parentClass->_dispose = cli_delete_dep_command_dispose;
    parentClass->_finalize = cli_delete_dep_command_finalize;
}

static void
cli_delete_dep_command_init(CLIDeleteDepCommand* this)
{
    this->_projLoc = NULL;
    this->_depName = NULL;
}

CLIDeleteDepCommand*
cli_delete_dep_command_new()
{
    return g_object_new(CLI_TYPE_DELETE_DEP_COMMAND, NULL);
}