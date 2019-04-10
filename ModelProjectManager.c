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

struct _ModelProjectManager
{
    GObject parent_object;
};

G_DEFINE_TYPE(ModelProjectManager, model_project_manager, G_TYPE_OBJECT);

static void
model_project_manager_class_init(ModelProjectManagerClass* class)
{}

static void
model_project_manager_init(ModelProjectManager* this)
{}

ModelProjectManager*
model_project_manager_new()
{
    return g_object_new(MODEL_TYPE_PROJECT_MANAGER, NULL);
}

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

static void
model_project_manager_init_src_folder(ModelProject* proj)
{
    GError* error = NULL;
    GString* dirPath = model_project_manager_get_project_dir(proj, "src");

    GDir* srcDir = g_dir_open(dirPath->str, 0, &error);
    if(srcDir)
    {
        const char* filename;
        while(filename = g_dir_read_name(srcDir))
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

int
model_project_manager_create_project(GString* location)
{
    ModelProject* proj = model_project_load_or_create_project(location);

    model_project_manager_init_src_folder(proj);
    model_project_manager_init_headers_folder(proj);
    model_project_manager_init_scripts_folder(proj);
    
    model_project_save(proj, location);

    return 0;
}

static void 
model_project_manager_process_system_deps(gpointer obj, gpointer data)
{
    ModelProjectDependency* dep = (ModelProjectDependency*)obj;
    GString* includes = (GString*)data;
    GString* depInclude =  model_project_dependency_get_includes(dep);
    
    depInclude = g_string_truncate(depInclude, depInclude->len - 1);

    g_string_append(includes,depInclude->str);
}

static GString*
model_project_manager_build_include_string(ModelProject* building, GString* projLoc)
{
    GString* includes = g_string_new("");

    GPtrArray* includeCollection = model_project_get_includes(building);

    for(int i = 0; i<includeCollection->len; i++)
    {
        GString* include = (GString*)includeCollection->pdata[i];

        GString* copy = g_string_new(g_strdup(projLoc->str));
        if(copy->len != 0)
            g_string_append(copy, g_strdup("/"));
        g_string_append(copy, g_strdup(include->str));

        g_string_append_printf(includes, "-I%s ", copy->str);
    }

    g_ptr_array_foreach(model_project_get_dependencies(building),
                        model_project_manager_process_system_deps,
                        includes);

    return includes;
}

static void 
model_project_manager_process_deps_to_link(gpointer obj, gpointer data)
{
    ModelProjectDependency* dep = (ModelProjectDependency*)obj;
    GString* links = (GString*)data;
   
    GString* depLink = model_project_dependency_get_links(dep);
    depLink = g_string_truncate(depLink, depLink->len-1);

    g_string_append(links, depLink->str);
}

static GString*
model_project_manager_build_link_string(ModelProject* building)
{
    GString* link = g_string_new("");

    g_ptr_array_foreach(model_project_get_dependencies(building),
                        model_project_manager_process_deps_to_link,
                        link);

    return link;
}

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

static void
model_project_manager_print_command(GPtrArray* args)
{
    GString* command = g_string_new("");

    g_ptr_array_foreach(args, model_project_manager_append_part, command);
    g_string_append(command, "\n");
    g_string_erase(command, 0, 1);

    g_print(command->str);

    g_string_free(command, FALSE);
}

static void
model_project_manager_run_script(GString* scriptFolder, gchar* script)
{
    g_print("Running %s script...\n", script);

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

        output = run_tool("/usr/bin/sh", (char**)args->pdata);
        if(output->len > 0)
            g_print("%s\n", output->str);
        
        g_string_free(output, TRUE);
    }   

    g_string_free(preBuildScriptLocation, TRUE);
    g_ptr_array_free(args, TRUE);
}

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

static GString *
model_project_manager_process_code_file(ModelSourceFile* file, 
                                        GString* objFolder, 
                                        GString* configString,
                                        GString* includes,
                                        GString* projLoc)
{
    GPtrArray *args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);
    g_ptr_array_add(args, g_strdup("gcc"));

    GString* loc = g_string_new(g_strdup(projLoc->str));
    if(loc->len != 0)
        loc = g_string_append(loc, g_strdup("/"));
    
    loc = g_string_append(loc, g_strdup(model_source_file_get_path(file)->str));

    model_project_manager_split_and_add_to(args, configString);

    g_ptr_array_add(args, g_strdup("-c"));
    g_ptr_array_add(args, g_strdup(loc->str));
    g_ptr_array_add(args, g_strdup("-o"));

    GString *objFile = g_string_new(g_strdup(objFolder->str));
    g_string_append_printf(objFile, "/%s.o", g_path_get_basename(model_source_file_get_path(file)->str));

    g_ptr_array_add(args, g_strdup(objFile->str));

    model_project_manager_split_and_add_to(args, includes);

    g_ptr_array_add(args, NULL);

    model_project_manager_print_command(args);
    GString* output = run_tool("/usr/bin/gcc", (char **)args->pdata);
    if (output->len > 0)
        g_print("%s\n", output->str);

    g_string_free(output, TRUE);
    g_string_free(loc, TRUE);
    g_ptr_array_free(args, TRUE);

    return objFile;
}

static void
model_project_manager_process_g_error(GError** error)
{
    if(*error != NULL)
    {
        //g_printerr(error->message);
        g_error_free(*error);
        *error = NULL;
    }
}

int
model_project_manager_build_project(ModelProjectManager* this, ModelProject* toBuild, GString* configName)
{
    GError* error = NULL;
    GString* loc, *output;
    
    loc = g_string_new(model_project_manager_get_project_work_dir(model_project_get_location(toBuild)));

    GString* objFolder = model_project_manager_get_project_dir(toBuild, "obj");
    GString* binFolder = model_project_manager_get_project_dir(toBuild, "bin");
    GString* scriptFolder = model_project_manager_get_project_dir(toBuild, "scripts");

    if(configName != NULL)
        model_project_set_active_build_config(toBuild, configName);

    ModelProjectConfiguration* config = model_project_get_build_config(toBuild, configName);

    GString* includes = model_project_manager_build_include_string(toBuild, loc);

    GDir* objFold = g_dir_open(objFolder->str, 0, &error);
    if(objFold)
    {
        g_dir_close(objFold);
    }
    else
    {
        g_mkdir_with_parents(objFolder->str, 8*8*7 + 8*6 + 4);
    }

    model_project_manager_process_g_error(&error);

    model_project_manager_run_script(scriptFolder, "preBuild.sh");

    g_print("Building...\n");

    GPtrArray* sources = model_project_get_source_files(toBuild);
    GPtrArray* objFiles = g_ptr_array_new();
    GString* configString = model_project_configuration_build_config_string(config);
    for(int i = 0; i<sources->len; i++)
    {
        ModelSourceFile* file = (ModelSourceFile*)g_ptr_array_index(sources, i);

        if(model_source_file_get_file_type(file) == CODE)        
            g_ptr_array_add(objFiles, model_project_manager_process_code_file(file,
                                                                              objFolder,
                                                                              configString,
                                                                              includes,
                                                                              loc));
    }

    gint outputType = model_project_configuration_get_output_type(config);
    GPtrArray *args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);

    GDir *binFold = g_dir_open(binFolder->str, 0, &error);
    if (binFold)
    {
        g_dir_close(binFold);
    }
    else
    {
        g_mkdir_with_parents(binFolder->str, 8 * 8 * 7 + 8 * 6 + 4);
    }

    model_project_manager_process_g_error(&error);

    char *projName = g_path_get_basename(model_project_get_location(toBuild)->str);
    binFolder = g_string_append(binFolder, "/");

    GString *outputName = model_project_configuration_get_output_name(config);
    if (outputName != NULL)
        binFolder = g_string_append(binFolder, g_strdup(outputName->str));
    else
        binFolder = g_string_append(binFolder, projName);

    g_print("Linking...\n");

    if (outputType != STATIC_LIB)
    {
        GString *link = model_project_manager_build_link_string(toBuild);
        g_ptr_array_add(args, g_strdup("gcc"));

        if(outputType == DYNAMIC_LIB)
            g_ptr_array_add(args, g_strdup("-shared"));

        //I think there are no build options for linking stage
        //model_project_manager_split_and_add_to(args, configString);

        for (int i = 0; i < objFiles->len; i++)
        {
            GString *file = (GString *)g_ptr_array_index(objFiles, i);
            g_ptr_array_add(args, g_strdup(file->str));
        }

        g_ptr_array_add(args, g_strdup("-o"));

        g_ptr_array_add(args, g_strdup(binFolder->str));

        model_project_manager_split_and_add_to(args, link);

        g_ptr_array_add(args, NULL);

        model_project_manager_print_command(args);
        output = run_tool("/usr/bin/gcc", (char **)args->pdata);

        if (output->len > 0)
            g_print("%s\n", output->str);
    }
    else
    {
        g_ptr_array_add(args, g_strdup("ar"));
        g_ptr_array_add(args, g_strdup("rcs"));

        g_ptr_array_add(args, g_strdup(binFolder->str));

        for (int i = 0; i < objFiles->len; i++)
        {
            GString *file = (GString *)g_ptr_array_index(objFiles, i);
            g_ptr_array_add(args, g_strdup(file->str));
        }

        g_ptr_array_add(args, NULL);

        model_project_manager_print_command(args);
        output = run_tool("/usr/bin/ar", (char **)args->pdata);

        if (output->len > 0)
            g_print("%s\n", output->str);
    }

    model_project_manager_run_script(scriptFolder, "postBuild.sh");

    g_ptr_array_free(args, TRUE);

    g_string_free(loc, TRUE);
    g_string_free(objFolder, TRUE);
    g_string_free(binFolder, TRUE);
    g_string_free(scriptFolder, TRUE);
    g_string_free(includes, TRUE);
}