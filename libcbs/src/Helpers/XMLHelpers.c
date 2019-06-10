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

#include "Helpers/XMLHelpers.h"

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