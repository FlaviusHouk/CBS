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

    xmlTextWriterWriteElement(writer, "Path", file->_path->str);
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