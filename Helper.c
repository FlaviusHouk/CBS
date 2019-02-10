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

    int rc = pipe(link);
    g_assert(rc != -1);

    pid = fork(); 
    g_assert(pid != -1);

    if(pid == 0) 
    {
        dup2 (link[1], STDOUT_FILENO);
        dup2 (link[1], STDERR_FILENO);
        close(link[0]);
        close(link[1]);
        int status = execv (tool, args);
        
        if(status == -1)
        {
            g_print("%d\n", errno);
            g_print("%s\n", strerror(errno));
        }

        exit(0);
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