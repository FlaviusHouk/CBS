#include "CLIAddFileCommand.h"

#include "ModelProject.h"

typedef struct _CLIAddFileCommand
{
    CLICommandInfo parent_object;

    GString* _projectLoc;
    GPtrArray* _files;
} CLIAddFileCommand;

enum CLI_ADD_FILE_COMMAND_INPUT_STATE
{
    CLI_ADD_FILE_COMMAND_START,
    CLI_ADD_FILE_COMMAND_PROJ_LOC,

    CLI_ADD_FILE_COMMAND_STATES_COUNT
};

G_DEFINE_TYPE(CLIAddFileCommand, cli_add_file_command, CLI_TYPE_COMMAND_INFO);

static gboolean
cli_add_file_command_is_valid(CLICommandInfo* command)
{
    CLIAddFileCommand* this = CLI_ADD_FILE_COMMAND(command);

    return this->_projectLoc != NULL && this->_files->len > 0;
}

static gboolean
cli_add_file_command_handle_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_ADD_FILE_COMMAND_PROJ_LOC);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_ADD_FILE_COMMAND_START);

        CLIAddFileCommand* this = CLI_ADD_FILE_COMMAND(command);
        switch (state)
        {
            case CLI_ADD_FILE_COMMAND_START:
            {
                if(this->_projectLoc != NULL)
                    this->_projectLoc = input;
                else
                    g_ptr_array_add(this->_files, input);

                break;
            } 
            case CLI_ADD_FILE_COMMAND_PROJ_LOC:
            {
                if(this->_projectLoc != NULL)
                    return TRUE;

                this->_projectLoc = input;
                break;
            }
        }
    }

    return FALSE;
}

///Function for GPtrArray foreach looping and adding source files 
///into project
static void 
cli_add_file_add_file_to_project(gpointer fileName, gpointer project)
{
	GString* fileLoc = (GString*)fileName;
	ModelProject* proj = (ModelProject*) project;

	ModelSourceFile* file = model_source_file_new(fileLoc);

	if(model_project_add_source_file(proj, file))
		g_print("%s was added.\n", fileLoc->str);
	else
		g_print("Cannot locate file %s.\n", fileLoc->str);
}

///Add file command handler
static void 
cli_add_file_command_execute(CLICommandInfo* command)
{
    CLIAddFileCommand* this = CLI_ADD_FILE_COMMAND(command);

    GPtrArray* files = this->_files;
    GString* projLoc = this->_projectLoc;

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	g_ptr_array_foreach(files, 
                        cli_add_file_add_file_to_project, 
                        proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
cli_add_file_command_dispose(GObject* obj)
{
    CLIAddFileCommand* this = CLI_ADD_FILE_COMMAND(obj);

    int fileCount = this->_files->len;
    for(int i = 0; i<fileCount; i++)
        g_string_free((GString*)this->_files->pdata[i], TRUE);
    
    this->_files = NULL;

    if(this->_projectLoc != NULL)
    {
        g_string_free(this->_projectLoc, TRUE);
        this->_projectLoc = NULL;
    }

    G_OBJECT_CLASS(cli_add_file_command_parent_class)->dispose(obj);
}

static void
cli_add_file_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_add_file_command_parent_class)->finalize(obj);
}

static void
cli_add_file_command_class_init(CLIAddFileCommandClass* klass)
{
    CLICommandInfoClass* parent_class = CLI_COMMAND_INFO_CLASS(klass);

    parent_class->_standardErrorMSG =
        g_string_new(g_strdup("Add file command usage:\n\t--addFile projName file1 file2 ... fileN\nprojName - path to project definition, file - path to file you want to add\n"));

    parent_class->_execute = cli_add_file_command_execute;
    parent_class->_handleInput = cli_add_file_command_handle_input;
    parent_class->_isValid = cli_add_file_command_is_valid;
    parent_class->_dispose = cli_add_file_command_dispose;
    parent_class->_finalize = cli_add_file_command_finalize;
}

static void
cli_add_file_command_init(CLIAddFileCommand* this)
{
    this->_files = g_ptr_array_new();
    this->_projectLoc = NULL;
}

CLIAddFileCommand*
cli_add_file_command_new()
{
    return g_object_new(CLI_TYPE_ADD_FILE_COMMAND, NULL);
}

