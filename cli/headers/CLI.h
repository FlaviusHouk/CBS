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


#ifndef _CLI_H_
#define _CLI_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

G_BEGIN_DECLS

#define CLI_TYPE_COMMAND_PARSER cli_command_parser_get_type()

///Type for handling CLI commands
G_DECLARE_FINAL_TYPE(CLICommandParser, cli_command_parser, CLI, COMMAND_PARSER, GObject)

///Constructor for CommandParser type
///args - array of input strings
///argn - number of input strings
CLICommandParser* cli_command_parser_new(char** args, int argn);

///Execute parsed commans
void cli_command_parser_execute(CLICommandParser* this);

G_END_DECLS

#endif