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
model_source_file_dispose(GObject* obj)
{
    ModelSourceFile* this = MODEL_SOURCE_FILE(obj);

    if(this->_path)
    {
        g_string_free(this->_path, TRUE);
        this->_path = NULL;
    }

    G_OBJECT_CLASS(model_source_file_parent_class)->dispose(obj);
}

static void
model_source_file_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_source_file_parent_class)->finalize(obj);
}

static GParamSpec* obj_props[MODEL_SOURCE_FILE_PROP_COUNT] = { NULL, };

static void
model_source_file_set_property(GObject* obj,
                               guint propID,
                               const GValue* value,
                               GParamSpec* pspec)
{
    ModelSourceFile* this = MODEL_SOURCE_FILE(obj);

    switch(propID)
    {
        case MODEL_SOURCE_FILE_PATH_PROP:
        {
            const gchar* val = g_value_get_string(value);
            GString* stringVal = NULL;

            if(val != NULL)
                stringVal = g_string_new(g_strdup(val));

            this->_path = stringVal;
            break;
        }

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propID, pspec);
            break;
        }
    }
}

static void
model_source_file_get_property(GObject* obj,
                               guint propID,
                               GValue* value,
                               GParamSpec* pspec)
{
    ModelSourceFile* this = MODEL_SOURCE_FILE(obj);

    switch(propID)
    {
        case MODEL_SOURCE_FILE_PATH_PROP:
        {
            gchar* val = NULL;

            if(this->_path != NULL)
                val = this->_path->str;

            g_value_set_string(value, val);
            break;
        }
        case MODEL_SOURCE_FILE_TYPE_PROP:
        {
            gint val = model_source_file_get_file_type(this);
            g_value_set_int(value, val);
            break;
        }

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propID, pspec);
            break;
        }
    }
}

static void
model_source_file_class_init(ModelSourceFileClass* class)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(class);

    objectClass->dispose = model_source_file_dispose;
    objectClass->finalize = model_source_file_finalize;

    objectClass->set_property = model_source_file_set_property;
    objectClass->get_property = model_source_file_get_property;

    obj_props[MODEL_SOURCE_FILE_PATH_PROP] = 
        g_param_spec_string("path",
                            "Path",
                            "Path to file",
                            NULL,
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    obj_props[MODEL_SOURCE_FILE_TYPE_PROP] = 
        g_param_spec_int("type",
                         "FileType",
                         "File type that set according to it's extension",
                         CODE,
                         ModelSourceFileTypesCOUNT,
                         RESOURCE,
                         G_PARAM_READABLE);

    g_object_class_install_properties(objectClass,
                                      MODEL_SOURCE_FILE_PROP_COUNT,
                                      obj_props);
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