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

#include "Model.h"

gboolean
xml_text_writer_write_string(xmlTextWriter* writer,
                             char* tagName, 
                             char* value,
                             GError** error)
{
    int rc = -1;
    rc = xmlTextWriterWriteElement(writer, BAD_CAST tagName, value);

    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot write &s.\n",
                    tagName);        
    }

    return rc >= 0;
}

gboolean
xml_text_writer_write_int(xmlTextWriter* writer, 
                          char* tagName, 
                          int value,
                          GError** error)
{
    int rc = -1;
    char num[16];
    sprintf(num, "%d", value);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST tagName, num);
    
    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot write &s.\n",
                    tagName);        
    }

    return rc >= 0;
}

gboolean
xml_text_writer_write_ptr_array(xmlTextWriter* writer,
                                 char* tagName, 
                                 GPtrArray* array,
                                 void (*content_writer)(gpointer, gpointer, GError**),
                                 GError** error)
{
    int rc = -1;
    GError* innerError;
    rc = xmlTextWriterStartElement(writer, BAD_CAST tagName);
    
    if(rc < 0)
        return FALSE;

    for(int i = 0; i<array->len;i++)
        content_writer(array->pdata[i], writer, &innerError);

    rc = xmlTextWriterEndElement(writer);
    
    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot write &s.\n",
                    tagName);        
    }

    return rc >= 0;
}

gboolean
xml_text_writer_write_hash_table(xmlTextWriter* writer,
                                 char* tagName, 
                                 GHashTable* table,
                                 void (*content_writer)(gpointer, gpointer, gpointer, GError**),
                                 GError** error)
{
    int rc = -1;
    GError* innerError;
    rc = xmlTextWriterStartElement(writer, BAD_CAST tagName);
    
    if(rc < 0)
        return FALSE;
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, table);
    while(g_hash_table_iter_next(&iter, &key, &value))
    {
        content_writer(key, value, writer, &innerError);
        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return FALSE;
        }
    }

    rc = xmlTextWriterEndElement(writer);
    
    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot write &s.\n",
                    tagName);        
    }

    return rc >= 0;
}

void
xml_node_read_collection(xmlNodePtr node,
                        char* collectionName,
                        char* elementName,
                        gboolean isOptional,
                        void (*deserializer)(xmlNodePtr, gpointer, GError**),
                        gpointer user_data,
                        GError** error)
{
    while(node != NULL && strcmp(node->name, collectionName) != 0)
        node = node->next;

    if(!node)
    {
        if(!isOptional)
            g_set_error(error,
                        g_quark_from_string("XML"),
                        XML_CANNOT_READ,
                        "Cannot find tag %s.\n",
                        collectionName);

        return;
    }

    GError* innerError = NULL;
    xmlNodePtr arrayItems = node->children;

    while(arrayItems != NULL)
    {
        while(arrayItems != NULL && strcmp(arrayItems->name, elementName) != 0)
        {
            arrayItems = arrayItems->next;
        }

        deserializer(arrayItems, user_data, &innerError);

        if(innerError != NULL)
        {
            g_propagate_error(error, innerError);
            return;
        }

        if(arrayItems != NULL)
            arrayItems = arrayItems->next;
    }
}

GString*
xml_node_read_g_string(xmlNodePtr node, 
                       char* name,
                       gboolean isOptional,
                       GError** error)
{
    while(node != NULL && strcmp(node->name, name) != 0)
        node = node->next;

    if(!node)
    {
        if(!isOptional)
            g_set_error(error,
                        g_quark_from_string("XML"),
                        XML_CANNOT_READ,
                        "Tag %s not found.\n",
                        name);

        return NULL;
    }

    return g_string_new(xmlNodeGetContent(node));
}

gint
xml_node_read_int(xmlNodePtr node, 
                  char* name,
                  gboolean isOptional,
                  GError** error)
{
    while(node != NULL && strcmp(node->name, name) != 0)
        node = node->next;

    if(!node)
    {
        if(!isOptional)
            g_set_error(error,
                        g_quark_from_string("XML"),
                        XML_CANNOT_READ,
                        "Tag %s not found.\n",
                        name);

        return -1;
    }

    gchar* content = xmlNodeGetContent(node);

    return atoi(content);
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