#include "stdio.h"
#include "stdlib.h"

#include "CLI.h"

#include "ModelProject.h"

int main(int argc, char** argv)
{
	CLICommandParser* p = cli_command_parser_new(argv, argc);
	
	cli_command_parser_execute(p);

	g_object_unref(p);
}
