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


#include "ModelSourceFile.h"
#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"


struct _ModelSourceFile
{
    GObject parent_object;

    GString* _path;
};

G_DEFINE_TYPE(ModelSourceFile, model_source_file, G_TYPE_OBJECT);

static void
model_source_file_class_init(ModelSourceFileClass* class)
{

}

static void
model_source_file_init(ModelSourceFile* this)
{

}

ModelSourceFile*
model_source_file_new(GString* location)
{
    ModelSourceFile* this = g_object_new(MODEL_TYPE_SOURCE_FILE, NULL);

    this->_path = location;

    return this;
}

ModelSourceFile*
model_source_file_new_from_xml(xmlNodePtr node)
{
    xmlNodePtr file = node->children;
    ModelSourceFile* this = NULL; 

    while(file != NULL)
    {
        if(strcmp(file->name, "Path") == 0)
        {
            this = g_object_new(MODEL_TYPE_SOURCE_FILE, NULL);
            this->_path = g_string_new(xmlNodeGetContent(file));
            return this;
        }

        file = file->next;
    }

    return this;
}

void
model_source_file_write_xml(ModelSourceFile* file, xmlTextWriterPtr writer)
{
    g_assert(file);
    g_assert(writer);

    int rc;

    rc = xmlTextWriterStartElement(writer, BAD_CAST "SourceFile");
    g_assert(rc >= 0);

    xmlTextWriterWriteElement(writer, "Path", file->_path->str);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);
}

static const char* 
get_filename_ext(const char *filename) 
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int
model_source_file_get_file_type(ModelSourceFile* file)
{
    const char* ext = get_filename_ext(file->_path->str);

    if(strcmp(ext, "c") == 0)
        return CODE;
    else if(strcmp(ext, "h") == 0)
        return HEADER;
    else if(strcmp(ext, "gml") == 0)
        return VIEW;
    else
        return RESOURCE;
}

gboolean
model_source_file_equals(const void* this, const void* that)
{ 
    ModelSourceFile* file1 = (ModelSourceFile*)this;
    ModelSourceFile* file2 = (ModelSourceFile*)that;

    return g_string_equal(file1->_path, file2->_path);
}

GString*
model_source_file_get_path(ModelSourceFile* this)
{
    g_assert(this);

    return this->_path;
}