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

#ifndef _CLI_VERSION_COMMAND_H_
#define _CLI_VERSION_COMMAND_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "CLICommandInfo.h"

G_BEGIN_DECLS

#define CLI_TYPE_VERSION_COMMAND cli_version_command_get_type()

G_DECLARE_FINAL_TYPE(CLIVersionCommand,
                     cli_version_command,
                     CLI,
                     VERSION_COMMAND,
                     CLICommandInfo);

CLIVersionCommand*
cli_version_command_new();

G_END_DECLS

#endif