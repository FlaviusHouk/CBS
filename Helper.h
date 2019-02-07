#include "stdio.h"
#include "glib.h"

gboolean g_string_compare(GString* str1, GString* str2);
gboolean g_string_starts_with(GString* str, GString* startsWith);

GString*
run_tool(char* tool, char** args);