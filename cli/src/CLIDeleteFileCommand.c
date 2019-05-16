#include "CLIDeleteFileCommand.h"

#include "ModelProject.h"

typedef struct _CLIDeleteFileCommand
{
    CLICommandInfo parent_object;

    GString* _projectLoc;
    GPtrArray* _files;
} CLIDeleteFileCommand;

G_DEFINE_TYPE(CLIDeleteFileCommand, cli_delete_file_command, CLI_TYPE_COMMAND_INFO);

enum CLI_DELETE_FILE_COMMAND_INPUT_STATES
{
    CLI_DELETE_FILE_COMMAND_START,
    CLI_DELETE_FILE_COMMAND_PROJ_LOC,

    CLI_DELETE_FILE_COMMAND_STATES_COUNT
};

static gboolean
cli_delete_file_command_is_valid(CLICommandInfo* command)
{
    CLIDeleteFileCommand* this = CLI_DELETE_FILE_COMMAND(command);

    return this->_projectLoc != NULL && this->_files->len > 0;
}

static gboolean
cli_delete_file_command_hanlde_input(CLICommandInfo* command, GString* input)
{
    if(g_str_equal(input->str, "-projLoc"))
    {
        cli_command_info_set_input_state(command, CLI_DELETE_FILE_COMMAND_PROJ_LOC);
    }
    else
    {
        gint state = cli_command_info_get_input_state(command);
        cli_command_info_set_input_state(command, CLI_DELETE_FILE_COMMAND_START);

        CLIDeleteFileCommand* this = CLI_DELETE_FILE_COMMAND(command);
        switch(state)
        {
            case CLI_DELETE_FILE_COMMAND_START:
            {
                if(this->_projectLoc == NULL)
                    this->_projectLoc = input;
                else
                    g_ptr_array_add(this->_files, input);

                break;
            }
            case CLI_DELETE_FILE_COMMAND_PROJ_LOC:
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

///Remove file command handler
static void 
cli_delete_file_command_delete_file(gpointer fileName, gpointer project)
{
	GString* fileLoc = (GString*)fileName;
	ModelProject* proj = (ModelProject*) project;

	ModelSourceFile* file = model_source_file_new(fileLoc);

	model_project_remove_source_file(proj, file);

	g_print("%s was removed.\n", fileLoc->str);
}

///Delete command handler
static void 
cli_delete_file_command_execute(CLICommandInfo* command)
{
    CLIDeleteFileCommand* this = CLI_DELETE_FILE_COMMAND(command);

	GPtrArray* files = this->_files;
    GString* projLoc = this->_projectLoc;

	ModelProject* proj = NULL;
	if(!model_project_load_or_create_project(projLoc, &proj))
	{
		g_print("Project does not exist\n");
		return;
	}

	g_ptr_array_foreach(files, 
                        cli_delete_file_command_delete_file, 
                        proj);

	model_project_save(proj, NULL);

	g_object_unref(proj);
}

static void
cli_delete_file_command_dispose(GObject* obj)
{
    CLIDeleteFileCommand* this = CLI_DELETE_FILE_COMMAND(obj);

    int fileCount = this->_files->len;
    for(int i = 0; i<fileCount; i++)
        g_string_free((GString*)this->_files->pdata[i], TRUE);
    
    this->_files = NULL;

    if(this->_projectLoc != NULL)
    {
        g_string_free(this->_projectLoc, TRUE);
        this->_projectLoc = NULL;
    }

    G_OBJECT_CLASS(cli_delete_file_command_parent_class)->dispose(obj);
}

static void
cli_delete_file_command_finalize(GObject* obj)
{
    G_OBJECT_CLASS(cli_delete_file_command_parent_class)->finalize(obj);
}

static void
cli_delete_file_command_class_init(CLIDeleteFileCommandClass* klass)
{
    CLICommandInfoClass* parent_class = CLI_COMMAND_INFO_CLASS(klass);

    parent_class->_standardErrorMSG =
        g_string_new(g_strdup("Delete file command usage:\n\t--deleteFile projName file\nprojName - path to project definition, file - path to file you want to remove\n"));

    parent_class->_execute = cli_delete_file_command_execute;
    parent_class->_handleInput = cli_delete_file_command_hanlde_input;
    parent_class->_isValid = cli_delete_file_command_is_valid;
    parent_class->_dispose = cli_delete_file_command_dispose;
    parent_class->_finalize = cli_delete_file_command_finalize;
}

static void
cli_delete_file_command_init(CLIDeleteFileCommand* this)
{
    this->_files = g_ptr_array_new();
    this->_projectLoc = NULL;
}

CLIDeleteFileCommand*
cli_delete_file_command_new()
{
    return g_object_new(CLI_TYPE_DELETE_FILE_COMMAND, NULL);
}