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

int
model_project_manager_create_project(GString* location)
{
    GError* error;
    ModelProject* proj = model_project_load_or_create_project(location);

    const char* baseDir = g_dirname(location->str);
    char* buf = g_alloca(strlen(baseDir));
    strcpy(buf, baseDir);

    GString* dirPath = g_string_append(g_string_new(buf), "/src");
    GDir* srcDir = g_dir_open(dirPath->str, 0, &error);
    if(srcDir)
    {
        const char* filename;
        while(filename = g_dir_read_name(srcDir))
            model_project_add_source_file(proj, model_source_file_new(
                                                    g_string_append(g_string_append(dirPath, "/"), filename)));

        g_dir_close(srcDir);
    }
    else
    {
        g_mkdir_with_parents(dirPath->str, 8*8*7 + 8*6 + 4);
    }

    //g_object_unref(dirPath);

    dirPath = g_string_append(g_string_new(buf), "/headers");
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

    dirPath = g_string_append(g_string_new(buf), "/scripts");
    GDir* scriptDir = g_dir_open(dirPath->str, 0, &error);
    if(scriptDir)
    {
        g_dir_close(scriptDir);
    }
    else
    {
        g_mkdir_with_parents(dirPath->str, 8*8*7 + 8*6 + 4);
    }

    //g_object_unref(baseDir);
    
    model_project_save(proj, location);

    return 0;
}

static void
model_project_manager_process_includes(gpointer obj, gpointer data)
{
    GString* include = (GString*)obj;
    GString* all = (GString*)data;

    GString* copy = g_string_new(g_strdup(include->str));
    copy = g_string_prepend(copy, "-I");
    copy = g_string_append(copy, " ");

    g_string_append(all, copy->str);
}

static void 
model_project_manager_process_system_deps(gpointer obj, gpointer data)
{
    ModelProjectDependency* dep = (ModelProjectDependency*)obj;
    GString* includes = (GString*)data;
    GString* depInclude =  model_project_dependency_get_includes(dep);

    g_string_append(includes,depInclude->str);
}

static GString*
model_project_manager_build_include_string(ModelProject* building)
{
    GString* includes = g_string_new("");

    g_ptr_array_foreach(model_project_get_includes(building),
                        model_project_manager_process_includes,
                        includes);

    g_ptr_array_foreach(model_project_get_dependencies(building),
                        model_project_manager_process_system_deps,
                        includes);

    return includes;
}

int
model_project_manager_build_project(ModelProjectManager* this, ModelProject* toBuild)
{
    GError* error;
    GString* loc = g_string_new(g_dirname(model_project_get_location(toBuild)->str));

    GString* objFolder = g_string_new(g_strdup(loc->str));
    objFolder = g_string_append(objFolder, "/obj");

    GString* binFolder = g_string_new(g_strdup(loc->str));
    binFolder = g_string_append(binFolder, "/bin");

    GString* includes = model_project_manager_build_include_string(toBuild);

    GDir* objFold = g_dir_open(objFolder->str, 0, &error);
    if(objFold)
    {
        g_dir_close(objFold);
    }
    else
    {
        g_mkdir_with_parents(objFolder->str, 8*8*7 + 8*6 + 4);
    }

    g_print("Building...");

    GPtrArray* sources = model_project_get_source_files(toBuild);
    for(int i = 0; i<sources->len; i++)
    {
        ModelSourceFile* file = (ModelSourceFile*)g_ptr_array_index(sources, i);

        if(model_source_file_get_file_type(file) == CODE)
        {
            GPtrArray* args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);
            g_ptr_array_add(args, g_strdup("gcc"));
            g_ptr_array_add(args, model_source_file_get_path(file)->str);
            g_ptr_array_add(args, g_strdup("-o"));

            GString* objFile = g_string_new(g_strdup(objFolder->str));
            objFile = g_string_append(objFile, "/");
            objFile = g_string_append(objFile, g_path_get_basename(model_source_file_get_path(file)->str));
            objFile = g_string_append(objFile, ".o");
            g_ptr_array_add(args, objFile->str);

            char** includePatrs = g_strsplit(includes->str, " ", -1);
            for(int i = 0;;i++)
            {
                char* curr = includePatrs[i];

                if(curr == NULL)
                    break;

                if(strlen(curr) > 2)
                {
                    g_ptr_array_add(args, curr);   
                }
            }
            g_ptr_array_add(args, NULL);

            GString* output = run_tool("/usr/bin/gcc", (char**)args->pdata);
            if(output->len > 0)
                g_print("%s\n", output->str);
            
            g_string_free(output, TRUE);

            g_ptr_array_free(args, TRUE);
        }
    }

    

    g_string_free(loc, TRUE);
    g_string_free(objFolder, TRUE);
    g_string_free(binFolder, TRUE);
    g_string_free(includes, TRUE);
}