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


#include "ModelProjectManager.h"

#include "string.h"
#include "Helper.h"
#include "Helpers/XMLHelpers.h"

#include "ModelTestRunner.h"

#ifdef _HAVE_CONFIG_H_
#include "config.h"
#endif

#include "glib/gstdio.h"
#include "gio/gio.h"

///Holds reference for function to pass output.
static writeFunc _writer = NULL;

void 
model_init_output_pipe(writeFunc printFunc)
{ _writer=printFunc; }

writeFunc
model_get_output_pipe()
{ return _writer; }

struct _ModelProjectManager
{
    GObject parent_object;
};

G_DEFINE_TYPE(ModelProjectManager, model_project_manager, G_TYPE_OBJECT);

///GObject function for dispose.
static void
model_project_manager_dispose(GObject* obj)
{
    G_OBJECT_CLASS(model_project_manager_parent_class)->dispose(obj);
}

///GObject function for finalization.
static void
model_project_manager_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_project_manager_parent_class)->finalize(obj);
}

///Type's static constructor.
static void
model_project_manager_class_init(ModelProjectManagerClass* class)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(class);

    objectClass->dispose = model_project_manager_dispose;
    objectClass->finalize = model_project_manager_finalize;
}


///GObject instance constructor.
static void
model_project_manager_init(ModelProjectManager* this)
{}


ModelProjectManager*
model_project_manager_new()
{
    return g_object_new(MODEL_TYPE_PROJECT_MANAGER, NULL);
}

gchar*
model_project_manager_get_version()
{
    return g_strdup(CBS_VERSION);
}

///Get base directory for project.
///Allocates new string that should be freed.
static gchar*
model_project_manager_get_project_work_dir(GString* location)
{
    char* buf;

    const char* baseDir = g_dirname(location->str);
    gboolean relativeNaming = g_strcmp0(baseDir, ".") == 0;

    if(relativeNaming)
    {
        buf = g_strdup("");
    }
    else
    {
        buf = g_malloc(strlen(baseDir));
        strcpy(buf, baseDir);
    }

    return buf;
}

///Returns any project folder (/bin, /src, etc) relative to its location
///dirName - needed folder. Copied in this function and should be freed.
static GString*
model_project_manager_get_project_dir(ModelProject* proj, gchar* dirName)
{
    char* baseDir = model_project_manager_get_project_work_dir(model_project_get_location(proj));

    GString* dirPath = g_string_new(g_strdup(baseDir));
    if(strlen(baseDir) != 0)
        dirPath = g_string_append(dirPath, g_strdup("/"));

    dirPath = g_string_append(dirPath, g_strdup(dirName));

    return dirPath;
}

///Used for project initialization.
///Creates src folder in the _location if it does not exist.
static void
model_project_manager_init_src_folder(ModelProject* proj)
{
    GError* error = NULL;
    GString* dirPath = model_project_manager_get_project_dir(proj, "src");

    GDir* srcDir = g_dir_open(dirPath->str, 0, &error);
    if(srcDir)
    {
        const char* filename;
        while((filename = g_dir_read_name(srcDir)))
        {
            GString* filePath = g_string_new(g_strdup(dirPath->str));
            filePath = g_string_append(g_string_append(filePath, "/"), filename);
            ModelSourceFile* toAdd = model_source_file_new(filePath);
            model_project_add_source_file(proj, toAdd);
        }

        g_dir_close(srcDir);
    }
    else
    {
        g_mkdir_with_parents(dirPath->str, 8*8*7 + 8*6 + 4);
    }

    if(error)
        g_error_free(error);

    g_string_free(dirPath, TRUE);
}

///Used for project initialization.
///Creates headers folder in the _location if it does not exist.
static void
model_project_manager_init_headers_folder(ModelProject* proj)
{
    GError* error = NULL;
    GString* dirPath = model_project_manager_get_project_dir(proj, "headers");

    GDir* headDir = g_dir_open(dirPath->str, 0, &error);
    if(headDir)
    {
        g_dir_close(headDir);
    }
    else
    {
        g_mkdir_with_parents(dirPath->str, 8*8*7 + 8*6 + 4);
    }

    model_project_add_include_folder(proj, dirPath);

    if(error)
        g_error_free(error);
}

///Used for project initialization.
///Creates scripts folder in the _location if it does not exist.
static void
model_project_manager_init_scripts_folder(ModelProject* proj)
{
    GError* error = NULL;
    GString* dirPath = model_project_manager_get_project_dir(proj, "scripts");

    GDir* scriptDir = g_dir_open(dirPath->str, 0, &error);
    if(scriptDir)
    {
        g_dir_close(scriptDir);
    }
    else
    {
        g_mkdir_with_parents(dirPath->str, 8*8*7 + 8*6 + 4);
    }

    if(NULL)
        g_error_free(error);

    g_string_free(dirPath, TRUE);
}

void
model_project_manager_create_project(GString* location,
                                     GString* templateName,
                                     GError** error)
{
    ModelProject* proj = NULL;
    GError* innerError = NULL;
    
    if(templateName != NULL)
    {
        const gchar* dataDir = g_getenv("XDG_DATA_HOME");
        GString* templateLocation = g_string_new(dataDir);
        g_string_append_printf(templateLocation, 
                               "/cbs/templates/%s", 
                               g_strdup(templateName->str));

        if(!g_file_test(templateLocation->str, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
        {
            g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_CREATE,
                    "There is no template named %s\n",
                    g_strdup(templateName->str));

            return;
        }

        GString* templateProjectLocation = g_string_append(g_string_clone(templateLocation), "/name.cpd");
        ModelProject* proj = NULL;
        model_project_load_or_create_project(templateProjectLocation, &proj, &innerError);

        model_project_save(proj, location, &innerError);
        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return;
        }

        model_project_manager_init_src_folder(proj);
        model_project_manager_init_headers_folder(proj);
        model_project_manager_init_scripts_folder(proj);

        GPtrArray* toSkip = g_ptr_array_new();
        g_ptr_array_add(toSkip, g_strdup("name.cpd"));
        g_ptr_array_add(toSkip, g_strdup("."));

        GString* projDir = g_string_new(g_path_get_dirname(g_path_get_absolute(location->str)));

        copy_directory_recursive(templateLocation,
                                 projDir,
                                 toSkip,
                                 &innerError);

        if(innerError != NULL)
            g_propagate_error(error, innerError);

        g_ptr_array_free(toSkip, TRUE);

        return;
    }
    else if(model_project_load_or_create_project(location, &proj, &innerError))
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_CREATE,
                    "Project already exists in %s.",
                    g_strdup(location->str));
        return;
    }

    model_project_manager_init_src_folder(proj);
    model_project_manager_init_headers_folder(proj);
    model_project_manager_init_scripts_folder(proj);
    
    model_project_save(proj, location, &innerError);
    if(innerError)
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_CREATE,
                    "Saving error.\n %s",
                    g_strdup(innerError->message));
        
        g_error_free(innerError);
    }
}

///Builds string wich contains all necessary header's location to build source file.
///Allocates new string.
static GString*
model_project_manager_build_include_string(ModelProject* building, 
                                           GString* projLoc,
                                           GError** error)
{
    GError* innerError = NULL;
    GString* includes = g_string_new("");

    GPtrArray* includeCollection = model_project_get_includes(building);

    for(int i = 0; i<includeCollection->len; i++)
    {
        GString* include = (GString*)includeCollection->pdata[i];

        //better to set up calculated full path. It is important
        //in case of building project as a dependency
        GString* copy = g_string_new(g_strdup(projLoc->str));
        if(copy->len != 0)
            g_string_append(copy, g_strdup("/"));
        g_string_append(copy, g_strdup(include->str));

        g_string_append_printf(includes, "-I%s ", copy->str);
    }

    GPtrArray* dependencyCollection = model_project_get_dependencies(building);

    for(int i = 0; i<dependencyCollection->len; i++)
    {
        ModelProjectDependency* dep = (ModelProjectDependency*)g_ptr_array_index(dependencyCollection, i);
        GString* depInclude =  model_project_dependency_get_includes(dep, &innerError);
        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);

            return NULL;
        }
    
        if(depInclude == NULL)
            continue;

        depInclude = g_string_truncate(depInclude, depInclude->len - 1);
        //added to prevent last space and make output looks consistent 

        g_string_append(includes,depInclude->str);
    }

    return includes;
}

///Builds string which contains pathes to all binaries that should be linked.
///Allocates new string.
static GString*
model_project_manager_build_link_string(ModelProject* building,
                                        gboolean includeRPath,
                                        GError** error)
{
    GError* innerError = NULL;
    GString* link = g_string_new(g_strdup(""));

    GPtrArray* dependenciesCollection = model_project_get_dependencies(building);
 
    for(int i = 0; i<dependenciesCollection->len; i++)
    {
        ModelProjectDependency* dep = 
                (ModelProjectDependency*)g_ptr_array_index(dependenciesCollection, i);
   
        GString* depLink = model_project_dependency_get_links(dep,
                                                              includeRPath,
                                                              &innerError);
        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);

            return NULL;
        }

        depLink = g_string_truncate(depLink, depLink->len-1);

        g_string_append(link, depLink->str);
    }

    return link;
}

///Helper method for concatenating command parts. 
///Maybe it will be moved into Helpers file
static void
model_project_manager_append_part(gpointer part, gpointer string)
{
    char* str = (char*)part;
    GString* command = (GString*)string;
    
    if(str != NULL)
    {
        g_string_append(command, " ");
        g_string_append(command, str);
    }
}

///Prints built compiler commands that will be passed to run_tool() function.
///@args - collection of GStrings.
static void
model_project_manager_print_command(GPtrArray* args)
{
    GString* command = g_string_new("");

    g_ptr_array_foreach(args, model_project_manager_append_part, command);
    g_string_append(command, "\n");
    g_string_erase(command, 0, 1);

    writeFunc print = model_get_output_pipe();

    if(print != NULL)
        print(command->str, MESSAGE);

    g_string_free(command, FALSE);
}

///script is freed inside so a copy should be pased.
///@striptFolder - location of the scritps.
///@scritp - file name to run.
static void
model_project_manager_run_script(GString* scriptFolder, 
                                 gchar* script,
                                 GError** error)
{
    GError* innerError = NULL;
    writeFunc print = model_get_output_pipe();
    GString* msg = g_string_new("");
    g_string_append_printf(msg, "Running %s script...\n", script);

    if(print != NULL)
        print(msg->str, MESSAGE);

    g_string_free(msg, TRUE);

    GPtrArray* args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);

    g_ptr_array_add(args, g_strdup("sh"));

    GString* preBuildScriptLocation = g_string_new(g_strdup(scriptFolder->str));
    g_string_append(preBuildScriptLocation, "/");
    g_string_append(preBuildScriptLocation, script);

    GString* output = NULL;

    FILE* check = fopen(preBuildScriptLocation->str, "r");
    if(check)
    {
        fclose(check);

        g_ptr_array_add(args, g_strdup(preBuildScriptLocation->str));
        g_ptr_array_add(args, NULL);

        output = run_tool("/usr/bin/sh", (char**)args->pdata, &innerError);
        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return;
        }
        else if(output->len > 0)
        {
            GString* msg = g_string_new("");
            g_string_append_printf(msg, "%s\n", output->str);

            if(print)
                print(msg->str, MESSAGE);

            g_string_free(msg, TRUE);
        }
        
        g_string_free(output, TRUE);
    }   

    g_string_free(preBuildScriptLocation, TRUE);
    g_ptr_array_free(args, TRUE);
}

///Converts strings to a GPtrArray collection to pass into run_tool() function
static void
model_project_manager_split_and_add_to(GPtrArray *toAdd, GString *string)
{
    char **configParts = g_strsplit(string->str, " ", -1);
    for (int i = 0;; i++)
    {
        char *curr = configParts[i];

        if (curr == NULL)
            break;

        if (strlen(curr) >= 2)
        {
            g_ptr_array_add(toAdd, curr);
        }
    }
}

///Checks is out file is up to date.
///@output - output file location.
///@objFiles - object files used to build output. Cpllection of GString.
static gboolean
model_project_manager_is_output_up_to_date(GString* output,
                                           GPtrArray* objFiles)
{
    GStatBuf outputStat;
    GStatBuf objStat;

    if(!g_file_test(output->str, G_FILE_TEST_EXISTS))
        return FALSE;

    if(g_stat(output->str, &outputStat) != 0)
            return FALSE;

    for(int i = 0; i<objFiles->len;i++)
    {
        GString* objFile = g_ptr_array_index(objFiles, i);
        
        if(g_stat(objFile->str, &objStat) != 0)
            return FALSE;

        if(objStat.st_mtime > outputStat.st_mtime)
            return FALSE;
    }

    return TRUE;
}

///Checks if object file os up to date.
///@toBuild - project which "owns" file.
///@file - file that should be checked.
///@objFile - location of object file.
static gboolean
model_project_manager_is_object_file_up_to_date(ModelProject* toBuild,
                                                ModelSourceFile* file,
                                                GString* objFile)
{
    if(!g_file_test(objFile->str, G_FILE_TEST_EXISTS))
        return FALSE;

    GStatBuf objFileStat;
    if(g_stat(objFile->str, &objFileStat) != 0)
        return FALSE;

    GString* filePath = model_source_file_get_path(file);
    filePath = model_project_resolve_path(toBuild, filePath);

    GStatBuf fileStat;
    if(g_stat(filePath->str, &fileStat) != 0)
        return FALSE;

    if(fileStat.st_mtime > objFileStat.st_mtime)
        return FALSE;

    GPtrArray* dependsOn = model_source_file_get_deps(file);

    for(int i = 0; i<dependsOn->len; i++)
    {
        GString* dep = g_ptr_array_index(dependsOn, i);
        
        if(g_stat(dep->str, &fileStat) != 0)
            return FALSE;

        if(fileStat.st_mtime > objFileStat.st_mtime)
            return FALSE;
    }
    
    return TRUE;
}

///Callback for GPtrArray foreach loop.
///Process (builds object file from source) c file with code.
///@toBuild - project that is built.
///@file - file to process.
///@objFolder - location of folder for object files.
///@configString - compilation options for compiler.
///@includes - string which contains all include folders for compiler. 
///(Should be refactorer to support different compilers)
///@projLoc - location of project that initiated the build. 
///(used for building dependencies)
///@rebuild - flag that indicates force rebuild even if all files up to date.
static GString *
model_project_manager_process_code_file(ModelProject* toBuild,
                                        ModelSourceFile* file, 
                                        GString* objFolder, 
                                        GString* configString,
                                        GString* includes,
                                        GString* projLoc,
                                        gboolean rebuild,
                                        GError** error)
{
    GError* innerError = NULL;
    GString *objFile = g_string_clone(objFolder);
    writeFunc print = model_get_output_pipe();
    g_string_append_printf(objFile, "/%s.o", g_path_get_basename(model_source_file_get_path(file)->str));

    if(model_project_manager_is_object_file_up_to_date(toBuild, file, objFile) &&
       !rebuild)
    {
        GString* msg = g_string_new("");
        g_string_append_printf(msg, "File %s is up to date.\n", objFile->str);

        if(print != NULL)
            print(msg->str, MESSAGE);

        g_string_free(msg, TRUE);

        return objFile;
    }

    GPtrArray *args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);
    g_ptr_array_add(args, g_strdup("gcc"));

    //check for relative pathes stored in project definition
    GString* loc = g_string_new(g_strdup(projLoc->str));
    if(loc->len != 0)
        loc = g_string_append(loc, g_strdup("/"));
    
    loc = g_string_append(loc, g_strdup(model_source_file_get_path(file)->str));

    model_project_manager_split_and_add_to(args, configString);

    g_ptr_array_add(args, g_strdup("-c"));
    g_ptr_array_add(args, g_strdup(loc->str));
    g_ptr_array_add(args, g_strdup("-o"));

    g_ptr_array_add(args, g_strdup(objFile->str));

    model_project_manager_split_and_add_to(args, includes);

    g_ptr_array_add(args, NULL);

    model_project_manager_print_command(args);
    GString* output = run_tool("/usr/bin/gcc", (char **)args->pdata, &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        g_string_free(loc, TRUE);
        g_ptr_array_free(args, TRUE);

        return NULL;
    }
    else if (output->len > 0)
    {
        GString* msg = g_string_new("");
        g_string_append_printf(msg, "%s\n", output->str);

        if(print)
            print(msg->str, MESSAGE);

        g_string_free(msg, TRUE);
    }

    g_string_free(output, TRUE);
    g_string_free(loc, TRUE);
    g_ptr_array_free(args, TRUE);

    return objFile;
}

///Analyzes project data and creates config.h according to it.
static void
model_project_manager_generate_config_header(ModelProject* toBuild)
{
    GHashTable* data = model_project_get_data(toBuild);

    if(g_hash_table_size(data) > 0)
    {
        GError *error = NULL;
        GString* configHeader = model_project_manager_get_project_dir(toBuild, "headers");

        g_string_append(configHeader, "/config.h");

        if(g_file_test(configHeader->str, G_FILE_TEST_EXISTS))
            g_remove(configHeader->str);

        GString* headerContent = g_string_new("");
        GHashTableIter iter;
        gpointer key, value;
        
        g_hash_table_iter_init(&iter, data);
        while(g_hash_table_iter_next(&iter, &key, &value))
            g_string_append_printf(headerContent, "#define %s \"%s\"\n", (gchar*)key, (gchar*)value);

        g_file_set_contents(configHeader->str,
                            headerContent->str,
                            headerContent->len,
                            &error);

        g_string_free(configHeader, TRUE);
        g_string_free(headerContent, TRUE);

        ModelProjectConfiguration* buildConfig = model_project_get_build_config(toBuild, NULL);
        model_project_configuration_define_macro(buildConfig, g_string_new("_HAVE_CONFIG_H_"));
    }

    g_hash_table_unref(data);
}

///Copies all binaries from dependent project into current project's binary folder.
///Used for publish command.
///@deps - GString collection. Contains pathes of files which should be copied.
///@projBinaryFolder - destination.
static void
model_project_manager_copy_binaries(GPtrArray* deps,
                                    GString* projBinaryFolder,
                                    GError** error)
{
    GError* innerError = NULL;

    for(int i = 0; i<deps->len; i++)
    {
        ModelProjectDependency* dep = g_ptr_array_index(deps, i);
        gint depType = model_project_dependency_get_dependency_type(dep);

        if(depType == SYSTEM_DEP)
            continue;

        if(depType == CBS_PROJECT)
        {
            GString* pathToProj = model_project_dependency_get_representation(dep);
            GString* projFolder = g_string_new(g_path_get_dirname(pathToProj->str));
            g_string_append_printf(projFolder, "/%s", "bin");

            copy_directory_recursive(projFolder,
                                     projBinaryFolder,
                                     NULL,
                                     &innerError);
        }
        else
        {
            GString* pathToLib = model_project_dependency_get_representation(dep);

            GString* destination = g_string_clone(projBinaryFolder);
            gchar* libName = g_path_get_basename(pathToLib->str);
            g_string_append_printf(destination, "/%s", libName);

            copy_file(pathToLib, destination, &innerError);
        }

        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            break;
        }
    }
}

///Builds project dependencies if any.
///@toBuild - project that is building.
///@buildOptions - internal CBS build params.
static void
model_project_manager_build_dependencies(ModelProjectManager* this,
                                         ModelProject* toBuild,
                                         ProjectBuildOption buildOptions,
                                         GError ** error)
{
    GError* innerError = NULL;
    GPtrArray* deps = model_project_get_dependencies(toBuild);

    for(int i = 0; i<deps->len; i++)
    {
        ModelProjectDependency* dep = MODEL_PROJECT_DEPENDENCY(g_ptr_array_index(deps, i));
        gint type = model_project_dependency_get_dependency_type(dep);
        if(type == CBS_PROJECT)
        {
            GString* representation = model_project_dependency_get_representation(dep);
            GString* resovledPath = model_project_resolve_path(toBuild, representation);

            ModelProject* projDep = NULL;
            if(!model_project_load_or_create_project(resovledPath, &projDep, &innerError))
            {
                g_set_error(error,
                        g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                        MODEL_PROJECT_DEPENDENCY_FAILED_TO_PROCESS,
                        "Cannot locate project %s./n",
                        resovledPath->str);
            }

            ModelProjectConfiguration* conf = model_project_get_build_config(projDep, NULL);

            gint outputType = model_project_configuration_get_output_type(conf);
            g_assert(outputType != ELF);

            model_project_manager_build_project(this,
                                                projDep,
                                                NULL,
                                                buildOptions,
                                                &innerError);

            g_object_unref(projDep);

            if(innerError != NULL)
            {
                g_set_error(error,
                            g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                            MODEL_PROJECT_DEPENDENCY_FAILED_TO_PROCESS,
                            "Failed to build dependency.\n%s",
                            g_strdup(innerError->message));
                
                g_clear_error(&innerError);
            }
        }
    }
}

void
model_project_manager_build_project(ModelProjectManager* this, 
                                    ModelProject* toBuild, 
                                    GString* configName,
                                    ProjectBuildOption options,
                                    GError** error)
{
    GError* innerError = NULL;
    GString* loc, *output;
    gboolean isPublishing = options & PUBLISH;
    gboolean forceRebuild = options & REBUILD;
    writeFunc print = model_get_output_pipe();

    loc = g_string_new(model_project_manager_get_project_work_dir(model_project_get_location(toBuild)));

    GString* objFolder = model_project_manager_get_project_dir(toBuild, "obj");
    GString* binFolder = model_project_manager_get_project_dir(toBuild, "bin");
    GString* scriptFolder = model_project_manager_get_project_dir(toBuild, "scripts");

    if(configName != NULL)
        model_project_set_active_build_config(toBuild, configName);

    model_project_manager_generate_config_header(toBuild);

    ModelProjectConfiguration* config = model_project_get_build_config(toBuild, configName);
    if(config == NULL)
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_FIND,
                    "Active build config is not set and there is no one provided.\n");
        return;
    }

    GString* includes = model_project_manager_build_include_string(toBuild, loc, &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);

        return;
    }

    if(g_mkdir_with_parents(objFolder->str, 8*8*7 + 8*6 + 4) == -1)
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_WRITE,
                    "Cannot create folder %s\n",
                    objFolder->str);

        return;
    }

    model_project_manager_run_script(scriptFolder, "preBuild.sh", &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    if(print)
        print("Building...\n", MESSAGE);

    GPtrArray* sources = model_project_get_source_files(toBuild);
    GPtrArray* objFiles = g_ptr_array_new();
    GString* configString = model_project_configuration_build_config_string(config);
    for(int i = 0; i<sources->len; i++)
    {
        ModelSourceFile* file = (ModelSourceFile*)g_ptr_array_index(sources, i);

        if(model_source_file_get_file_type(file) == CODE)      
        {  
            GString* objFile = model_project_manager_process_code_file(toBuild,
                                                                       file,
                                                                       objFolder,
                                                                       configString,
                                                                       includes,
                                                                       loc,
                                                                       forceRebuild,
                                                                       &innerError);

            if(objFile != NULL)
                g_ptr_array_add(objFiles, objFile);
        }

        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return;
        }
    }

    gint outputType = model_project_configuration_get_output_type(config);
    GPtrArray *args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);

    if(g_mkdir_with_parents(binFolder->str, 8*8*7 + 8*6 + 4) == -1)
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_WRITE,
                    "Cannot create folder %s\n",
                    binFolder->str);

        return;
    }

    GString* binary = g_string_new(g_strdup(binFolder->str));
    char *projName = g_path_get_basename(model_project_get_location(toBuild)->str);
    binary = g_string_append(binary, "/");

    GString *outputName = model_project_configuration_get_output_name(config);
    if (outputName != NULL)
        binary = g_string_append(binary, g_strdup(outputName->str));
    else
        binary = g_string_append(binary, projName);

    model_project_manager_build_dependencies(this,
                                             toBuild,
                                             options,
                                             &innerError);

    if(print)
        print("Linking...\n", MESSAGE);

    if (!model_project_manager_is_output_up_to_date(binary, objFiles) || forceRebuild)
    {
        if (outputType != STATIC_LIB)
        {
            GString *link = model_project_manager_build_link_string(toBuild,
                                                                    !isPublishing,
                                                                    &innerError);

            if (innerError != NULL)
            {
                g_propagate_error(error, innerError);

                return;
            }

            g_ptr_array_add(args, g_strdup("gcc"));

            if (outputType == DYNAMIC_LIB)
                g_ptr_array_add(args, g_strdup("-shared"));

            //I think there are no build options for linking stage
            //model_project_manager_split_and_add_to(args, configString);

            for (int i = 0; i < objFiles->len; i++)
            {
                GString *file = (GString *)g_ptr_array_index(objFiles, i);
                g_ptr_array_add(args, g_strdup(file->str));
            }

            g_ptr_array_add(args, g_strdup("-o"));

            g_ptr_array_add(args, g_strdup(binary->str));

            model_project_manager_split_and_add_to(args, link);

            g_ptr_array_add(args, NULL);

            model_project_manager_print_command(args);
            output = run_tool("/usr/bin/gcc", (char **)args->pdata, &innerError);
            if(innerError != NULL)
            {
                g_propagate_error(error, innerError);
            }
            else if (output->len > 0 && print)
            {
                print(output->str, MESSAGE);
                print("\n", MESSAGE);
            }
        }
        else
        {
            g_ptr_array_add(args, g_strdup("ar"));
            g_ptr_array_add(args, g_strdup("rcs"));

            g_ptr_array_add(args, g_strdup(binary->str));

            for (int i = 0; i < objFiles->len; i++)
            {
                GString *file = (GString *)g_ptr_array_index(objFiles, i);
                g_ptr_array_add(args, g_strdup(file->str));
            }

            g_ptr_array_add(args, NULL);

            model_project_manager_print_command(args);
            output = run_tool("/usr/bin/ar", (char **)args->pdata, &innerError);
            if(innerError != NULL)
                g_propagate_error(error, innerError);
            else if (output->len > 0 && print)
            {
                print(output->str, MESSAGE);
                print("\n", MESSAGE);
            }
        }
    }

    if(innerError != NULL)
        return;

    if(isPublishing)
    {
        GPtrArray* dependencies = model_project_get_dependencies(toBuild);

        model_project_manager_copy_binaries(dependencies, binFolder, &innerError);

        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return;
        }
    }

    model_project_manager_run_script(scriptFolder, "postBuild.sh", &innerError);
    if(innerError != NULL)
        g_propagate_error(error, innerError);

    g_ptr_array_free(args, TRUE);

    g_string_free(loc, TRUE);
    g_string_free(objFolder, TRUE);
    g_string_free(binFolder, TRUE);
    g_string_free(binary, TRUE);
    g_string_free(scriptFolder, TRUE);
    g_string_free(includes, TRUE);
}

void
model_project_manager_run_tests(ModelProjectManager* this, 
                                ModelProject* toTest,
                                GError** error)
{
    g_assert(this);
    g_assert(toTest);

    GError* localError = NULL;
    GString* unitTestProjectLoc = model_project_get_unit_tests_project_location(toTest); 

    if(unitTestProjectLoc != NULL &&
       unitTestProjectLoc->len > 0)
    {   
        GString* resolvedPath = model_project_resolve_path(toTest,
                                                           unitTestProjectLoc);

        ModelProject* unitTestProject = NULL;
        if(!model_project_load_or_create_project(resolvedPath, &unitTestProject, &localError))
        {
            g_set_error(error,
                        g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                        MODEL_PROJECT_MANAGER_CANNOT_OPEN,
                        "Project %s does not exist.\n",
                        resolvedPath->str);
            
            return;
        }

        model_project_manager_build_project(this, 
                                            unitTestProject, 
                                            NULL,
                                            FALSE,
                                            &localError);
        if(localError != NULL)
        {
            g_propagate_error(error, localError);

            return;
        }

        GString* pathToUnitTestLib = model_project_manager_get_project_dir(unitTestProject, "bin");

        gchar* unitTestFileName = NULL;
        ModelProjectConfiguration* unitTestConfig = model_project_get_build_config(unitTestProject, NULL);
        if(unitTestConfig)
            unitTestFileName = g_strdup(model_project_configuration_get_output_name(unitTestConfig)->str);
        else
            unitTestFileName = g_path_get_basename(resolvedPath->str);

        g_string_append_printf(pathToUnitTestLib, "/%s", unitTestFileName);

        ModelTestRunner* runner = model_test_runner_new();

        model_test_runner_execute_tests(runner, pathToUnitTestLib);

        g_object_unref(runner);
    }
    else
    {
        g_set_error(error,
                    g_type_qname(MODEL_TYPE_PROJECT_MANAGER),
                    MODEL_PROJECT_MANAGER_CANNOT_OPEN,
                    "Unit test project location isn't set.\n");
    }
}