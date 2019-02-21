#ifndef _CLI_COMMAND_INFO_H_
#define _CLI_COMMAND_INFO_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

G_BEGIN_DECLS

#define CLI_TYPE_COMMAND_INFO cli_command_info_get_type()

G_DECLARE_FINAL_TYPE(CLICommandInfo, cli_command_info, CLI, COMMAND_INFO, GObject);

G_END_DECLS

enum CLI_COMMAND_PARSER_INPUT_STATES
{
	START,
	CREATE,
	ADD_FILE,
	DELETE_FILE,
	ADD_INCL,
	DELETE_INCL,
	ADD_DEP,
	REM_DEP,
	BUILD,
	HELP
};

CLICommandInfo* cli_command_info_new(GString* command, 
                                     int commandType, 
                                     int argsCount, 
                                     int order, 
                                     void (*action)(CLICommandInfo* com));

GString* cli_command_info_get_command(CLICommandInfo* this);
void cli_command_info_set_command(CLICommandInfo* this, GString* value);

int cli_command_info_get_args_count(CLICommandInfo* this);
void cli_command_info_set_args_count(CLICommandInfo* this, int value);

int cli_command_info_get_order(CLICommandInfo* this);
void cli_command_info_set_order(CLICommandInfo* this, int value);

GPtrArray* cli_command_info_get_args(CLICommandInfo* this);
void cli_command_info_process_command(CLICommandInfo* this);

#endif