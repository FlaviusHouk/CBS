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
    GPtrArray* _buildConfigs;
};

G_DEFINE_TYPE(ModelProject, model_project, G_TYPE_OBJECT)

static void
model_project_handle_xml_error(void* userData, xmlErrorPtr error)
{
    g_print("%s", error->message);
}

GPtrArray *defaultConfigs;

ModelProjectConfiguration*
model_project_get_build_config(ModelProject* this, GString* configName)
{
    g_assert(this);

    if(configName == NULL)
        return g_ptr_array_index(defaultConfigs, 0);

    ModelProjectConfiguration* seek = model_project_configuration_new(configName);
    ModelProjectConfiguration* toRet = NULL;

    gint index = -1;
    if(g_ptr_array_find(this->_buildConfigs, seek, &index))
        toRet = g_ptr_array_index(this->_buildConfigs, index);
    else if(g_ptr_array_find(defaultConfigs, seek, &index))
        toRet = g_ptr_array_index(defaultConfigs, index);
    else
        g_assert(FALSE);

    g_object_unref(seek);

    return toRet;
}

static void
model_project_class_init(ModelProjectClass* class)
{
    LIBXML_TEST_VERSION

    xmlSetStructuredErrorFunc(NULL, model_project_handle_xml_error);

    defaultConfigs = g_ptr_array_new();

    g_ptr_array_add(defaultConfigs, model_project_configuration_new(g_string_new("Debug")));

    ModelProjectConfiguration* conf = model_project_configuration_new(g_string_new("Release"));
    model_project_configuration_set_optimization_level(conf, RELEASE_1);
    g_ptr_array_add(defaultConfigs, conf);
}

static void
model_project_init(ModelProject* this)
{
    this->_sourceFiles = g_ptr_array_new();
    this->_headersFolders = g_ptr_array_new();
    this->_dependencies = g_ptr_array_new();
    this->_buildConfigs = g_ptr_array_new();
}

static ModelProject* 
model_project_new(GString* location)
{
    ModelProject* this;
    this = g_object_new(MODEL_TYPE_PROJECT, NULL);

    this->_location = location;

    return this;
}

static void
model_project_read(xmlNodePtr root, ModelProject* this)
{
    xmlNodePtr node = root->children;

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

        if(sourceFiles != NULL)
            sourceFiles = sourceFiles->next;
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

        GString* data = g_string_new(xmlNodeGetContent(includeFolder));
        g_ptr_array_add(this->_headersFolders, data);
        includeFolder = includeFolder->next;
    }

    //read Dependencies list
    while(node != NULL && strcmp(node->name, "SystemDependencies") != 0)
    {
        node = node->next;
    }

    xmlNodePtr deps = node->children;

    while(deps != NULL)
    {
        while(deps != NULL && strcmp(deps->name, "Dependency") != 0)
        {
            deps = deps->next;
        }

        ModelProjectDependency* dep = model_project_dependency_new_from_xml(deps);

        if(dep != NULL)
            g_ptr_array_add(this->_dependencies, dep);

        deps = deps->next;
    }

    //read Build configurations
    while(node != NULL && strcmp(node->name, "BuildConfigs") != 0)
    {
        node = node->next;
    }

    xmlNodePtr configs = node->children;

    while(configs != NULL)
    {
        while(configs != NULL && strcmp(configs->name, "Configuration") != 0)
        {
            configs = configs->next;
        }

        ModelProjectConfiguration* conf = model_project_configuration_new_from_xml(configs);

        if(conf != NULL)
            g_ptr_array_add(this->_buildConfigs, conf);

        configs = configs->next;
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
    ModelProjectDependency* dep = (ModelProjectDependency*)obj;

    model_project_dependency_write_xml(dep, writer);
}

static void
model_project_write_build_configs(gpointer obj, gpointer data)
{
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    ModelProjectConfiguration* conf = (ModelProjectConfiguration*)obj;

    model_project_configuration_write_xml(conf, writer);
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

    //write files collection
    rc = xmlTextWriterStartElement(writer, BAD_CAST "SourceFiles");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_sourceFiles, 
                        model_project_write_source_files,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    //write include folders collection
    rc = xmlTextWriterStartElement(writer, BAD_CAST "IncludeFolders");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_headersFolders, 
                        model_project_write_include_folders,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    //write dependencies collection
    rc = xmlTextWriterStartElement(writer, BAD_CAST "SystemDependencies");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_dependencies, 
                        model_project_write_system_dependencies,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    //write build configuration collection
    rc = xmlTextWriterStartElement(writer, BAD_CAST "BuildConfigs");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_buildConfigs, 
                        model_project_write_build_configs,
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

    GString* dot = g_string_new(".");
    ModelProject* toRet;
    if(g_string_equal(location, dot))
        toRet = model_project_new(g_string_new(""));
    else
        toRet = model_project_new(location);

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

    FILE* fileCheck = fopen(model_source_file_get_path(file)->str, "r");
    if(fileCheck)
        fclose(fileCheck);
    else
        g_assert(FALSE);

    if(g_ptr_array_find_with_equal_func(this->_sourceFiles, file, model_source_file_equals, NULL))
        return;

    g_ptr_array_add(this->_sourceFiles, file);
}

void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file)
{
    g_assert(this);
    g_assert(file);

    int index = -1;

    if(g_ptr_array_find_with_equal_func(this->_sourceFiles, file, model_source_file_equals, &index))
        g_ptr_array_remove_index(this->_sourceFiles, index);

    g_object_unref(file);
}

static gboolean
model_project_find_gstring(const void* obj1, const void* obj2)
{
    GString* str1 = (GString*)obj1;
    GString* str2 = (GString*)obj2;

    return g_string_equal(str1, str2);
}

void
model_project_add_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    GError* error = NULL;

    GDir* test = g_dir_open(folder->str, 0, &error);
    if(test)
        g_dir_close(test);
    else
        g_assert(test);

    if(g_ptr_array_find_with_equal_func(this->_headersFolders, folder, model_project_find_gstring, NULL))
        return;

    g_ptr_array_add(this->_headersFolders, folder);
}

void
model_project_remove_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    int index = -1;
    if(!g_ptr_array_find_with_equal_func(this->_headersFolders, folder, model_project_find_gstring, &index))
        return;

    g_ptr_array_remove_index(this->_headersFolders, index);
}

void
model_project_save(ModelProject* this, GString* dest)
{
    g_assert(this);
    g_assert(dest);

    if(!g_string_equal(this->_location, dest))
    {
        GString* newLoc = g_string_new(dest->str);
        g_string_free(this->_location, TRUE);
        this->_location = newLoc;
    }

    model_project_write_project(this);
}

void
model_project_add_dependency(ModelProject* this, ModelProjectDependency* dependency)
{
    g_assert(this);
    g_assert(dependency);

    if(g_ptr_array_find_with_equal_func(this->_dependencies, dependency, model_project_dependency_equals, NULL))
        return;

    //add validation here

    g_ptr_array_add(this->_dependencies, dependency);    
}

void
model_project_remove_dependency(ModelProject* this, ModelProjectDependency* dependency)
{
    g_assert(this);
    g_assert(dependency);

    int index = -1;
    if(!g_ptr_array_find_with_equal_func(this->_dependencies, dependency, model_project_dependency_equals, &index))
        return;

    //add validation here

    g_ptr_array_remove_index(this->_dependencies, index); 
}

void
model_project_remove_dependency_by_name(ModelProject* this, GString* depName)
{
    g_assert(this);
    g_assert(depName);

    int length = this->_dependencies->len;

    for(int i = 0; i<length; i++)
    {
        ModelProjectDependency* curr = (ModelProjectDependency*)g_ptr_array_index(this->_dependencies, i);

        if(g_string_equal(depName, model_project_dependency_get_representation(curr)))
        {
            g_ptr_array_remove(this->_dependencies, curr);
            return;
        }
    }
}

GPtrArray*
model_project_get_source_files(ModelProject* this)
{
    g_assert(this);

    return this->_sourceFiles;
}

GPtrArray*
model_project_get_dependencies(ModelProject* this)
{
    g_assert(this);

    return this->_dependencies;
}

GPtrArray*
model_project_get_includes(ModelProject* this)
{
    g_assert(this);

    return this->_headersFolders;
}

GString*
model_project_get_location(ModelProject* this)
{
    g_assert(this);

    return this->_location;
}