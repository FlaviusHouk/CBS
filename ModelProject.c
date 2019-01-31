#include "ModelProject.h"
#include "ModelSourceFile.h"
#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlerror.h"

struct _ModelProject
{
    GObject parent_object;

    GString* _location;
    GPtrArray* _sourceFiles;
    GPtrArray* _headersFolders;
    GPtrArray* _dependencies;
};

G_DEFINE_TYPE(ModelProject, model_project, G_TYPE_OBJECT)

static void
model_project_handle_xml_error(void* userData, xmlErrorPtr error)
{
    g_print("%s", error->message);
}

static void
model_project_class_init(ModelProjectClass* class)
{
    LIBXML_TEST_VERSION

    xmlSetStructuredErrorFunc(NULL, model_project_handle_xml_error);
}

static void
model_project_init(ModelProject* this)
{}

static ModelProject* 
model_project_new(GString* location)
{
    ModelProject* this;
    this = g_object_new(MODEL_TYPE_PROJECT, NULL);

    this->_location = location;

    this->_sourceFiles = g_ptr_array_new();
    this->_headersFolders = g_ptr_array_new();
    this->_dependencies = g_ptr_array_new();

    return this;
}

static void
model_project_read(xmlNodePtr root, ModelProject* this)
{
    xmlNodePtr node = root->children;
    
    //read Location string
    while(node != NULL && strcmp(node->name, "Location") != 0)
    {
        node = node->next;
    }

    //this->_location = g_string_new(xmlNodeGetContent(node));

    //read SourceFiles list
    while(node != NULL && strcmp(node->name, "SourceFiles") != 0)
    {
        node = node->next;
    }

    xmlNodePtr sourceFiles = node->children;

    while(sourceFiles != NULL)
    {
        while(sourceFiles != NULL && strcmp(sourceFiles->name, "SourceFile") != 0)
        {
            sourceFiles = sourceFiles->next;
        }

        ModelSourceFile* file = model_source_file_new_from_xml(sourceFiles);
        
        if(file != NULL)
            g_ptr_array_add(this->_sourceFiles, file);
    }

    //read IncludeForders list
    while(node != NULL && strcmp(node->name, "IncludeFolders") != 0)
    {
        node = node->next;
    }

    xmlNodePtr includeFolder = node->children;

    while(includeFolder != NULL)
    {
        while(includeFolder != NULL && strcmp(includeFolder->name, "string") != 0)
        {
            includeFolder = includeFolder->next;
        }

        g_ptr_array_add(this->_headersFolders, g_string_new(xmlNodeGetContent(includeFolder)));
    }

    //read Dependencies list
    while(node != NULL && strcmp(node->name, "SystemDependencies") != 0)
    {
        node = node->next;
    }

    xmlNodePtr deps = node->children;

    while(deps != NULL)
    {
        while(deps != NULL && strcmp(deps->name, "string") != 0)
        {
            deps = deps->next;
        }

        g_ptr_array_add(this->_dependencies, g_string_new(xmlNodeGetContent(includeFolder)));
    }
}

static void
model_project_read_project(ModelProject* this)
{
    xmlDocPtr doc = xmlParseFile(this->_location->str);
    g_assert(doc);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    g_assert(root);
    g_assert(strcmp(root->name, "Project") == 0);

    model_project_read(root, this);

    xmlFreeDoc(doc);
}

static xmlChar*
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


static void
model_project_write_source_files(gpointer obj, gpointer data)
{
    model_source_file_write_xml((ModelSourceFile*)obj, (xmlTextWriterPtr)data);
}

static void
model_project_write_include_folders(gpointer obj, gpointer data)
{
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    GString* str = (GString*)obj;

    xmlTextWriterWriteElement(writer, "string", str->str);
}

static void
model_project_write_system_dependencies(gpointer obj, gpointer data)
{
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    GString* str = (GString*)obj;

    xmlTextWriterWriteElement(writer, "string", str->str);
}

static void
model_project_write_project(ModelProject* this)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp = NULL;
    xmlDocPtr doc;

    writer = xmlNewTextWriterDoc(&doc, 0);
    g_assert(writer);

    rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    g_assert(rc >= 0);

    //tmp = xml_convert_input("Project", "ASCII");
    rc = xmlTextWriterStartElement(writer, "Project");
    g_assert(rc >= 0);
    if(tmp)
        g_free(tmp);

    //tmp = xml_convert_input(this->_location, "ASCII");
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Location", "%s", this->_location->str);
    g_assert(rc >= 0);
    if(tmp)
        g_free(tmp);

    rc = xmlTextWriterStartElement(writer, BAD_CAST "SourceFiles");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_sourceFiles, 
                        model_project_write_source_files,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    rc = xmlTextWriterStartElement(writer, BAD_CAST "IncludeFolders");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_headersFolders, 
                        model_project_write_include_folders,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    rc = xmlTextWriterStartElement(writer, BAD_CAST "SystemDependencies");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_dependencies, 
                        model_project_write_system_dependencies,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    rc = xmlTextWriterEndDocument(writer);
    g_assert(rc >= 0);

    xmlFreeTextWriter(writer);
    xmlSaveFileEnc(this->_location->str, doc, "UTF-8");
    xmlFreeDoc(doc);
}


ModelProject*
model_project_load_or_create_project(GString* location)
{
    FILE* proj = fopen(location->str, "r");
    ModelProject* toRet = model_project_new(location);

    if(proj) //exists
    {
        fclose(proj);
        model_project_read_project(toRet);
    }
    else //not exists
    {
        proj = fopen(location->str, "w+");
        model_project_write_project(toRet);
        fclose(proj);
    }

    return toRet;
}

void
model_project_add_source_file(ModelProject* this, ModelSourceFile* file)
{
    g_assert(this);
    g_assert(file);

    if(g_ptr_array_find(this->_sourceFiles, file, NULL))
        return;

    g_ptr_array_add(this->_sourceFiles, file);
}

void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file)
{
    g_assert(this);
    g_assert(file);

    if(!g_ptr_array_find(this->_sourceFiles, file, NULL))
        return;

    g_ptr_array_remove(this->_sourceFiles, file);

    g_object_unref(file);
}

void
model_project_add_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    g_ptr_array_add(this->_headersFolders, folder);
}

void
model_project_remove_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    g_ptr_array_remove(this->_headersFolders, folder);
}

void
model_project_save(ModelProject* this, const GString* dest)
{
    //розкоментувати після видалення location із ModelProject
    /*g_object_unref(this->_location);
    this->_location = dest;*/

    model_project_write_project(this);
}