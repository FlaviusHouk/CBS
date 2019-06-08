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

    GPtrArray* args = g_ptr_array_new_with_free_func(clear_collection_with_null_elems);
    g_ptr_array_add(args, g_strdup("realpath"));
    g_ptr_array_add(args, strdup(relPath));
    g_ptr_array_add(args, NULL);

    GString* output = run_tool("/usr/bin/realpath", (char**)args->pdata);
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
run_tool(char* tool, char** args)
{
    int link[2];
    pid_t pid;
    GString* toRet = NULL;

    int rc = pipe(link); //creates pipe to read STDOUT of child process
    g_assert(rc != -1);

    pid = fork(); //creates a copy of current process.
    g_assert(pid != -1);

    if(pid == 0) // flow for child process
    {
        //redirecting STDOUT and STDERR
        dup2 (link[1], STDOUT_FILENO); 
        dup2 (link[1], STDERR_FILENO);

        //In a lot of examples links closed before actual executing
        close(link[0]);
        close(link[1]);

        //executing command
        int status = execv (tool, args);
        
        if(status == -1)
        {
            g_print("%d\n", errno);
            g_print("%s\n", strerror(errno));
        }

        exit(0); //suspending process as it done it's job.
    } 
    else
    {
        close(link[1]);
        toRet = read_from_pipe(link[0]);
    }

    return toRet;
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

gboolean
xml_text_writer_write_string(xmlTextWriter* writer, char* tagName, char* value)
{
    int rc = -1;
    rc = xmlTextWriterWriteElement(writer, BAD_CAST tagName, value);
    return rc >= 0;
}

gboolean
xml_text_writer_write_int(xmlTextWriter* writer, char* tagName, int value)
{
    int rc = -1;
    char num[16];
    sprintf(num, "%d", value);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST tagName, num);
    return rc >= 0;
}

gboolean
xml_text_writer_write_ptr_array(xmlTextWriter* writer,
                                 char* tagName, 
                                 GPtrArray* array,
                                 void (*content_writer)(gpointer, gpointer))
{
    int rc = -1;
    rc = xmlTextWriterStartElement(writer, BAD_CAST tagName);
    
    if(rc < 0)
        return FALSE;

    g_ptr_array_foreach(array, 
                        content_writer,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    
    return rc >= 0;
}

gboolean
xml_text_writer_write_hash_table(xmlTextWriter* writer,
                                 char* tagName, 
                                 GHashTable* table,
                                 void (*content_writer)(gpointer, gpointer, gpointer))
{
    int rc = -1;
    rc = xmlTextWriterStartElement(writer, BAD_CAST tagName);
    
    if(rc < 0)
        return FALSE;

    g_hash_table_foreach(table, 
                         content_writer,
                         writer);

    rc = xmlTextWriterEndElement(writer);
    
    return rc >= 0;
}

void
xml_node_read_collection(xmlNodePtr node,
                        char* collectionName,
                        char* elementName,
                        void (*deserializer)(xmlNodePtr, gpointer),
                        gpointer user_data)
{
    while(node != NULL && strcmp(node->name, collectionName) != 0)
        node = node->next;

    if(!node)
        return;

    xmlNodePtr arrayItems = node->children;

    while(arrayItems != NULL)
    {
        while(arrayItems != NULL && strcmp(arrayItems->name, elementName) != 0)
        {
            arrayItems = arrayItems->next;
        }

        deserializer(arrayItems, user_data);

        if(arrayItems != NULL)
            arrayItems = arrayItems->next;
    }
}

GString*
xml_node_read_g_string(xmlNodePtr node, char* name)
{
    while(node != NULL && strcmp(node->name, name) != 0)
        node = node->next;

    return g_string_new(xmlNodeGetContent(node));
}

///Converting input for right xml encoding.
xmlChar*
xml_convert_input(char* string, char* enc)
{
    xmlChar *out;
    int ret;
    int size;
    int out_size;
    int temp;
    xmlCharEncodingHandlerPtr handler;

    if (string == NULL)
        return NULL;

    handler = xmlFindCharEncodingHandler(enc);

    if (!handler) {
        printf("ConvertInput: no encoding handler found for '%s'\n",
               enc ? enc : "");
        return 0;
    }

    size = (int) strlen(string) + 1;
    out_size = size * 2 - 1;
    out = (unsigned char *) xmlMalloc((size_t) out_size);

    if (out != 0) {
        temp = size - 1;
        ret = handler->input(out, &out_size, (const xmlChar *) string, &temp);
        if ((ret < 0) || (temp - size + 1)) {
            if (ret < 0) {
                printf("ConvertInput: conversion wasn't successful.\n");
            } else {
                printf
                    ("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
                     temp);
            }

            xmlFree(out);
            out = 0;
        } else {
            out = (unsigned char *) xmlRealloc(out, out_size + 1);
            out[out_size] = 0;  /*null terminating out */
        }
    } else {
        printf("ConvertInput: no mem\n");
    }

    return out;
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