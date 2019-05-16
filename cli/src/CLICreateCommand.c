#include "CLICreateCommand.h"

#include "ModelProjectManager.h"

enum CLI_CREATE_COMMAND_INPUT_STATES
{
    CREATE_COMMAND_START,
    CREATE_COMMAND_NAME,

    CLI_CREATE_COMMAND_INPUT_STATES_COUNT
};

typedef struct _CLICreateCommand
{
    CLICommandInfo parent_class;

    GString* _name;
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

	GError* error = NULL;
	model_project_manager_create_project(loc, &error);
	if(error)
	{
		g_print(error->message);
		return;
	}

	g_print("Project was successfully created!\n");

	g_object_unref(manager);
}

static void
cli_create_command_handle_input(CLICommandInfo* command, GString* input, gboolean* breakInput)
{
    if(g_str_equal(input->str, "-name"))
    {
        cli_command_info_set_input_state(command, CREATE_COMMAND_NAME);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        CLICreateCommand* this = CLI_CREATE_COMMAND(command);

        switch(state)
        {
            case CREATE_COMMAND_START:
            {
                if(this->_name != NULL)
                {
                    this->_name = input;
                    break;
                }
                else
                {
                    *breakInput = TRUE;
                    return;
                }
            }
            case CREATE_COMMAND_NAME:
            {
                if(this->_name != NULL)
                {
                    *breakInput = TRUE;
                    return;
                }

                this->_name = input;
                cli_command_info_set_input_state(command, CREATE_COMMAND_START);
            }
        }
    }
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
