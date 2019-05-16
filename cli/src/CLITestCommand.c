#include "CLITestCommand.h"

#include "ModelProjectManager.h"

typedef struct _CLITestCommand
{
    CLICommandInfo parent_type;

    GString* _projLoc;
} CLITestCommand;

enum CLI_TEST_COMMAND_INPUT_STATES
{
	CLI_TEST_COMMAND_START,
	CLI_TEST_COMMAND_PROJ_LOC,

	CLI_TEST_COMMAND_STATES_COUNT
};

G_DEFINE_TYPE(CLITestCommand, cli_test_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_test_command_is_valid(CLICommandInfo* command)
{
    CLITestCommand* this = CLI_TEST_COMMAND(command);

    return this->_projLoc != NULL;
}

static void
cli_test_command_handle_input(CLICommandInfo* command, GString* input, gboolean* breakInput)
{
	if(g_str_equal(input->str, "-projLoc"))
	{
		cli_command_info_set_input_state(command, CLI_TEST_COMMAND_PROJ_LOC);
	}
	else
	{
		gint state = cli_command_info_get_input_state(command);
		cli_command_info_set_input_state(command, CLI_TEST_COMMAND_START);

		CLITestCommand* this = CLI_TEST_COMMAND(command);
		switch(state)
		{
			case CLI_TEST_COMMAND_START:
			{
				if(this->_projLoc == NULL)
				{
					this->_projLoc = input;
					break;
				}
				else
				{
					*breakInput = TRUE;
					return;
				}
			}
			case CLI_TEST_COMMAND_PROJ_LOC:
			{
				if(this->_projLoc != NULL)
				{
					*breakInput = TRUE;
					return;
				}

				this->_projLoc = input;
				break;
			}
		}
	}
}

///Handler for Test command
static void
cli_test_command_execute(CLICommandInfo* command)
{
    CLITestCommand* this = CLI_TEST_COMMAND(command);

	GString* projLoc = this->_projLoc;

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	ModelProjectManager* manager = model_project_manager_new();

	GError* error = NULL;
	model_project_manager_run_tests(manager, proj, &error);
	if(error != NULL)
	{
		g_print(error->message);
		return;
	}

	g_object_unref(manager);
	g_object_unref(proj);
}

static void
cli_test_command_dispose(GObject* obj)
{
    CLITestCommand* this = CLI_TEST_COMMAND(obj);

    if(this->_projLoc != NULL)
    {
        g_string_free(this->_projLoc, TRUE);
        this->_projLoc = NULL;
    }

    G_OBJECT_CLASS(cli_test_command_parent_class)->dispose(obj);
}

static void
cli_test_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_test_command_parent_class)->finalize(obj);
}

static void
cli_test_command_class_init(CLITestCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

	parentClass->_standardErrorMSG =
		g_string_new(g_strdup("Test command usage:\n\t--test projName\nprojName - path to project definition you want to test\n"));

    parentClass->_execute = cli_test_command_execute;
    parentClass->_handleInput = cli_test_command_handle_input;
    parentClass->_isValid = cli_test_command_is_valid;
	parentClass->_dispose = cli_test_command_dispose;
	parentClass->_finalize = cli_test_command_finalize;
}

static void
cli_test_command_init(CLITestCommand* this)
{
    this->_projLoc = NULL;
}

CLITestCommand*
cli_test_command_new()
{
    return g_object_new(CLI_TYPE_TEST_COMMAND, NULL);
}