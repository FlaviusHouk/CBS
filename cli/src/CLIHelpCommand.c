#include "CLIHelpCommand.h"

typedef struct _CLIHelpCommand
{
    CLICommandInfo parent_object;
} CLIHelpCommand;

G_DEFINE_TYPE(CLIHelpCommand, cli_help_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_help_command_is_valid(CLICommandInfo* command)
{
    return TRUE;
}

static void
cli_help_command_handle_input(CLICommandInfo* command, GString* input, gboolean* breakInput)
{
	*breakInput = TRUE;
}

///Help command handler
static void
cli_help_command_execute(CLICommandInfo* command)
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

static void
cli_help_command_class_init(CLIHelpCommandClass* klass)
{
    CLICommandInfoClass* parentClass = CLI_COMMAND_INFO_CLASS(klass);

	parentClass->_standardErrorMSG =
		g_string_new(g_strdup(""));

    parentClass->_execute = cli_help_command_execute;
    parentClass->_handleInput = cli_help_command_handle_input;
    parentClass->_isValid = cli_help_command_is_valid;
}

static void
cli_help_command_init(CLIHelpCommand* this)
{}

CLIHelpCommand*
cli_help_command_new()
{
    return g_object_new(CLI_TYPE_HELP_COMMAND, NULL);
}