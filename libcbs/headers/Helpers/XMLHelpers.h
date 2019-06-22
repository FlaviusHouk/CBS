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

#include "glib.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

/*-------------------------XML-----------------------------*/

///Writes string to XML document into @tagName tag.
///@writer - libxml2 writer
///@tagName - XML tag name.
///@value - value that will be written in document. 
///Return TRUE on success. 
gboolean
xml_text_writer_write_string(xmlTextWriter* writer, 
                             char* tagName, 
                             char* value,
                             GError** error);

///Writes int to XML document into @tagName tag.
///@writer - libxml2 writer
///@tagName - XML tag name.
///@value - value that will be written in document. 
///Return TRUE on success.
gboolean
xml_text_writer_write_int(xmlTextWriter* writer, 
                          char* tagName, 
                          int value,
                          GError** error);

///Writes GPtrArray as a collection.
///@writer - libxml2 writer
///@tagName - tag for collection
///@array - collection to write
///@content_writter - GFunc used to iterate throught collection. 
///Return TRUE on success.
gboolean
xml_text_writer_write_ptr_array(xmlTextWriter* writer,
                                 char* tagName, 
                                 GPtrArray* array,
                                 void (*content_writer)(gpointer, gpointer, GError**),
                                 GError** error);

///Writes GHashTable as a collection.
///@writer - libxml2 writer
///@tagName - tag for collection
///@array - collection to write
///@content_writter - GHFunc used to iterate throught collection. 
///Return TRUE on success.
gboolean
xml_text_writer_write_hash_table(xmlTextWriter* writer,
                                 char* tagName, 
                                 GHashTable* table,
                                 void (*content_writer)(gpointer, gpointer, gpointer, GError**),
                                 GError** error);

///Reads collection from XML document
///@xmlNodePtr - node that contains collection.
///@elementName - name of the child node for child object.
///@deserilizer - callback that performs actual element deserialization.
///@user_data - data that will be passed into @deserializer.
void
xml_node_read_collection(xmlNodePtr node,
                        char* collectionName,
                        char* elementName,
                        void (*deserializer)(xmlNodePtr, gpointer, GError**),
                        gpointer user_data,
                        GError** error);

///Read GString from xmlNode
///@node - first node on the same label with one with data.
///@name - tag name with data.
GString*
xml_node_read_g_string(xmlNodePtr node, 
                       char* name,
                       GError** error);

gint
xml_node_read_int(xmlNodePtr node, 
                  char* name,
                  GError** error);

///Function to converting string from ASCII encoding to @enc
///@string - string to encode.
///@enc - encoding name.
xmlChar*
xml_convert_input(char* string, char* enc);