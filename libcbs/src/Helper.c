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

#define _POSIX_C_SOURCE 200809L

#include "Helper.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "errno.h"
#include <sys/types.h>
#include <sys/wait.h>

#include "gio/gio.h"

gchar* g_str_substr(const gchar* src, int offset, int len)
{
    g_assert(src);

    int origLen = strlen(src);

    g_assert(offset + len < origLen);

    gchar* subStr = (gchar*)g_malloc(sizeof(char) * len + 1);
    for(int i = 0; i<len; i++)
        subStr[i] = src[offset + i];

    subStr[len] = '\0';

    return subStr;
}

gchar* g_path_get_absolute(gchar* relPath)
{
    //Cannot include realpath function, so realpath tool is called
    GError* innerError;

    GPtrArray* args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);
    g_ptr_array_add(args, g_strdup("realpath"));
    g_ptr_array_add(args, strdup(relPath));
    g_ptr_array_add(args, NULL);

    GString* output = run_tool("/usr/bin/realpath", (char**)args->pdata, &innerError);
    if(innerError != NULL)
    {
        g_ptr_array_free(args, TRUE);        
        return NULL;
    }

    g_string_erase(output, output->len - 1, 1);

    gchar* absPath = output->str;

    g_string_free(output, FALSE);
    g_ptr_array_free(args, TRUE);

    return absPath;
}

/*gint g_string_index_of(const GString* str, const GString* seek)
{
    if(seek->len > str->len)
        return -1;

    gint toRet = -1;

    int i = 0;
    while(i < str->len - seek->len)
    {
        if(strncmp(str->str + i, seek->str, seek->len) == 0)
            toRet = i;

        i++;
    }

    return toRet;
}*/

///Performs reading from child process STDOUT
static GString*
read_from_pipe (int file)
{
    GString* toRet = g_string_new("");
    FILE *stream;
    int c;

    stream = fdopen (file, "r");
    while ((c = fgetc (stream)) != EOF)
        g_string_append_c(toRet, c);
    fclose (stream);

    return toRet;
}

GString*
run_tool(char* tool, char** args, GError** error)
{
    gchar *stdoutput = NULL, *stderror = NULL;
    gint exitStatus;
    GError* innerError = NULL;

    g_spawn_sync(NULL,
                 args,
                 NULL,
                 G_SPAWN_SEARCH_PATH,
                 NULL,
                 NULL,
                 &stdoutput,
                 &stderror,
                 &exitStatus,
                 &innerError);

    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return NULL;
    }
    else if(exitStatus != 0)
    {
        g_set_error(error,
                    g_quark_from_string(tool),
                    exitStatus,
                    stderror);
        
        return NULL;
    }

    if(stderror != NULL && strlen(stderror) > 0)
        g_printerr(stderror);

    return g_string_new(stdoutput);
}

void
clear_collection_with_null_elems(void* data)
{
    if(data != NULL)
        g_free(data);
}

void g_string_clean_up(void* string)
{
    GString* gString = (GString*)string;

    g_string_free(gString, TRUE);
}

GString*
g_string_clone(GString* string)
{
    g_assert(string);

    return g_string_new(g_strdup(string->str));
}

void
copy_directory_recursive(GString* source,
                         GString* dest,
                         GPtrArray* toSkip,
                         GError** error)
{
    GError* innerError = NULL;

    if(!g_file_test(source->str, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
       return;

    if(!g_file_test(dest->str, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
        g_mkdir_with_parents(dest->str, 8*8*7 + 8*6 + 4);

    GFile* file = g_file_new_for_path(source->str);
    GFileEnumerator* enumerator = g_file_enumerate_children(file,
                                                            "*",
                                                            G_FILE_QUERY_INFO_NONE,
                                                            NULL,
                                                            &innerError);

    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    GFileInfo* info = NULL;
    while(g_file_enumerator_iterate(enumerator, &info, NULL, NULL, &innerError))
    {
        if(info == NULL || innerError != NULL)
        {
            if(innerError != NULL)
                g_propagate_error(error, innerError);

            break;
        }

        gboolean skip = FALSE;
        const gchar* fileName = g_file_info_get_name(info);
        if(toSkip)
        {
            for(int i = 0; i<toSkip->len; i++)
            {
                gchar* condition = (gchar*)toSkip->pdata[i];
                if(g_str_has_prefix(fileName, condition))
                {
                    skip = TRUE;
                    break;
                }
            }

            if(skip)
                continue;
        }

        GString* recSource = g_string_clone(source);
        g_string_append_printf(recSource, "/%s", fileName);

        GString* recDest = g_string_clone(dest);
        g_string_append_printf(recDest, "/%s", fileName);

        if(g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY)
        {
            copy_directory_recursive(recSource,
                                     recDest,
                                     toSkip,
                                     &innerError);

            g_string_free(recSource, TRUE);
            g_string_free(recDest, TRUE);

            if(innerError != NULL)
            {
                g_propagate_error(error, innerError);
                break;
            }
        }
        else
        {
            GFile* sourceFile = g_file_new_for_path(recSource->str);
            GFile* destFile = g_file_new_for_path(recDest->str);

            g_file_copy(sourceFile,
                        destFile,
                        G_FILE_COPY_OVERWRITE,
                        NULL,
                        NULL,
                        NULL,
                        &innerError);
            
            g_object_unref(sourceFile);
            g_object_unref(destFile);

            if(innerError != NULL)
            {
                g_propagate_error(error, innerError);
                break;
            }
        }
    }

    g_object_unref(enumerator);
    g_object_unref(file);
}

void
copy_file(GString* sourcePath,
          GString* destPath,
          GError** error)
{
    if(!g_file_test(sourcePath->str, G_FILE_TEST_EXISTS))
    {
        g_set_error(error,
                    g_quark_from_string("Helper"),
                    1,
                    "Cannot locate source file.");
        
        return;
    }

    GError* innerError = NULL;

    GFile* source = g_file_new_for_path(sourcePath->str);
    GFile* dest = g_file_new_for_path(destPath->str);

    g_file_copy(source, 
                dest, 
                G_FILE_COPY_OVERWRITE,
                NULL,
                NULL,
                NULL,
                &innerError);

    if(innerError != NULL)
        g_propagate_error(error, innerError);
}