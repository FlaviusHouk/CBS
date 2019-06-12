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


#include "stdio.h"
#include "glib.h"

///Extension for C str to make substrings (allocates new memory)
///src - source str
///offset - offset from start
///len - amount of chars to copy
gchar* 
g_str_substr(const gchar* src, int offset, int len);

///Extension for g_path to resolve links and relative pathes
///returns newle allocated string with absolute path
///relPath - relative path to resolve
gchar* 
g_path_get_absolute(gchar* relPath);

///Executes command on system shell
///return newly allocated string with output of the started command.
///tool - path to tool to run
///args - NULL terminated array of arguments (every word should be as separate string,
///(for example "gcc" "name.c", "-o", "name.o"). Tool name should also be included.
GString*
run_tool(char* tool, char** args, GError** error);

///Wrapper on free(). Performs check on NULL. If data != NULL cleans memory.
///data - memory to clean
void
clear_collection_with_null_elems(void* data);

///Wrapper on g_string_free. Created to prevent compiler warnings.
///Does full clean.
///@string - GString* to free.
void 
g_string_clean_up(void* string);

///Creates copy of given @string.
///@string - GString* to clone.
GString*
g_string_clone(GString* string);

///Recursivly copy content of directory (uses GIO functions)
///@source - source folder (currently only local folder)
///@dest - destination path (currently only local folder)
///@toSkip - collection of patterns in the names that should not be copy.
///Works according to pattern "{skip}***".
void
copy_directory_recursive(GString* source,
                         GString* dest,
                         GPtrArray* toSkip,
                         GError** error);

///Incapsulates copying file
///@sourcePath - path to shource file. It has to exist and be readable.
///@destPath - path should be available for writing.
///@error - exception.
void
copy_file(GString* sourcePath,
          GString* destPath,
          GError** error);