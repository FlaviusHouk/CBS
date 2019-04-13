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

gboolean g_string_compare(GString* str1, GString* str2)
{
    if(str1->len != str2->len)
        return FALSE;

    return strncmp(str1->str, str2->str, str1->len);
}

gboolean g_string_starts_with(GString* str, GString* startsWith)
{
    if(startsWith->len > str->len)
        return FALSE;

    return strncmp(str->str, startsWith->str, startsWith->len);
}

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