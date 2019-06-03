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

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

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
run_tool(char* tool, char** args);

///Wrapper on free(). Performs check on NULL. If data != NULL cleans memory.
///data - memory to clean
void
clear_collection_with_null_elems(void* data);

void 
g_string_clean_up(void* string);

GString*
g_string_clone(GString* string);

gboolean
xml_text_writer_write_string(xmlTextWriter* writer, char* tagName, char* value);

gboolean
xml_text_writer_write_int(xmlTextWriter* writer, char* tagName, int value);

gboolean
xml_text_writer_write_ptr_array(xmlTextWriter* writer,
                                 char* tagName, 
                                 GPtrArray* array,
                                 void (*content_writer)(gpointer, gpointer));

gboolean
xml_text_writer_write_hash_table(xmlTextWriter* writer,
                                 char* tagName, 
                                 GHashTable* table,
                                 void (*content_writer)(gpointer, gpointer, gpointer));

void
xml_node_read_collection(xmlNodePtr node,
                        char* collectionName,
                        char* elementName,
                        void (*deserializer)(xmlNodePtr, gpointer),
                        gpointer user_data);

GString*
xml_node_read_g_string(xmlNodePtr node, char* name);

xmlChar*
xml_convert_input(char* string, char* enc);