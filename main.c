#include "stdio.h"
#include "stdlib.h"

#include "CLI.h"

#include "ModelProject.h"

int main(int argc, char** argv)
{
	CLICommandParser* p = cli_command_parser_new(argv, argc);

	ModelProject* proj = model_project_load_or_create_project(g_string_new("/home/Flavius/C/CBS/proj2/proj.cpd"));
}
