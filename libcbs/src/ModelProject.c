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
#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlerror.h"

#include "Helper.h"
#include "Helpers/XMLHelpers.h"

#ifdef _HAVE_CONFIG_H_
#include "config.h"
#endif

struct _ModelProject
{
    GObject parent_object;

    //runtime fields
    GString* _location;

    //storable fields
    gint _xmlDocVersion;
    GPtrArray* _sourceFiles;
    GPtrArray* _headersFolders;
    GPtrArray* _dependencies;
    GPtrArray* _buildConfigs;
    GHashTable* _projData;
    GString* _activeConfiguration;
    GString* _unitTestsLocation;
};

G_DEFINE_TYPE(ModelProject, model_project, G_TYPE_OBJECT)

///Array of GObject properties for this type.
static GParamSpec* obj_properties[MODEL_PROJECT_PROP_COUNT] = { NULL, };

///GObject property setter
static void
model_project_set_property(GObject* obj,
                           guint propID,
                           const GValue* value,
                           GParamSpec* psSpec)
{
    ModelProject* this = MODEL_PROJECT(obj);

    switch(propID)
    {
        case MODEL_PROJECT_ACTIVE_BUILD_CONFIG_PROP:
        {
            const gchar* val = g_value_get_string(value);
            
            GString* stringValue = NULL;
            if(val != NULL)
                stringValue = g_string_new(g_strdup(val));

            model_project_set_active_build_config(this, stringValue);
            break;
        }
        case MODEL_PROJECT_UNIT_TEST_PROJ_PROP:
        {
            const gchar* val = g_value_get_string(value);

            GString* stringValue = NULL;
            if(val != NULL)
                stringValue = g_string_new(g_strdup(val));

            model_project_set_unit_tests_project_location(this, stringValue);
            break;
        }

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propID, psSpec);
            break;
        }
    }
}

///GObject properties getter
static void
model_project_get_property(GObject* obj,
                           guint propID,
                           GValue* value,
                           GParamSpec* pspec)
{
    ModelProject* this = MODEL_PROJECT(obj);

    switch(propID)
    {
        case MODEL_PROJECT_ACTIVE_BUILD_CONFIG_PROP:
        {
            GString* activeBuildConfig = model_project_get_active_build_config(this);
            g_value_set_string(value, activeBuildConfig->str);
            break;
        }
        case MODEL_PROJECT_UNIT_TEST_PROJ_PROP:
        {
            GString* unitTestProj = model_project_get_unit_tests_project_location(this);
            g_value_set_string(value, unitTestProj->str);
            break;
        }

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propID, pspec);
        }
    }
}

///Function for handling xml error and providing information about it. Mostly for debug.
static void
model_project_handle_xml_error(void* userData, xmlErrorPtr error)
{
    writeFunc print = model_get_output_pipe();

    if(print != NULL)
        print(error->message, ERROR);
}

///Collection of avalable default configs: Debug and Release; 
GPtrArray *defaultConfigs;

ModelProjectConfiguration*
model_project_get_build_config(ModelProject* this, GString* configName)
{
    g_assert(this);

    if(configName == NULL)
    {
        if(this->_activeConfiguration != NULL)
            return model_project_get_build_config(this, this->_activeConfiguration);
        else
            return NULL;
    }

    ModelProjectConfiguration* seek = model_project_configuration_new(g_string_clone(configName));
    ModelProjectConfiguration* toRet = NULL;

    guint index = -1;
    if(g_ptr_array_find_with_equal_func(this->_buildConfigs, seek, model_project_configuration_equals, &index))
        toRet = g_ptr_array_index(this->_buildConfigs, index);
    else if(g_ptr_array_find_with_equal_func(defaultConfigs, seek, model_project_configuration_equals, &index))
        toRet = g_ptr_array_index(defaultConfigs, index);

    g_object_unref(seek);

    return toRet;
}

///Reads default build configurations. Should be called once as early as possible.
static void
model_project_init_default_build_configs(void)
{
    GError* innerError = NULL;
    GString* configDir = g_string_new(getenv("XDG_CONFIG_HOME"));
    g_string_append(configDir, "/cbs/config.conf");
    
    GKeyFile* configFile = g_key_file_new();
    if(!g_key_file_load_from_file(configFile, configDir->str, G_KEY_FILE_NONE, &innerError))
    {
        writeFunc print = model_get_output_pipe();

        if(print != NULL)
            print(innerError->message, ERROR);

        return;
    }

    defaultConfigs = g_ptr_array_new();
    const gchar* defaultDebugConfig = g_key_file_get_string(configFile, 
                                                            "Configs",
                                                            "Debug", 
                                                            &innerError);

    const gchar* defaultReleaseConfig = g_key_file_get_string(configFile, 
                                                        "Configs", 
                                                        "Release", 
                                                        &innerError);

    xmlDocPtr doc = xmlParseDoc((unsigned char*)defaultDebugConfig); //libxml requires unsigned chars
    xmlNodePtr configNode = xmlDocGetRootElement(doc);
    g_ptr_array_add(defaultConfigs, 
                    model_project_configuration_new_from_xml(configNode, &innerError));
    xmlFreeDoc(doc);    

    doc = xmlParseDoc((unsigned char*)defaultReleaseConfig); //libxml requires unsigned chars
    configNode = xmlDocGetRootElement(doc);
    g_ptr_array_add(defaultConfigs, 
                    model_project_configuration_new_from_xml(configNode, &innerError));
    xmlFreeDoc(doc);   
}

///GObject dispose function.
static void
model_project_dispose(GObject* obj)
{
    ModelProject* this = MODEL_PROJECT(obj);

    if(this->_location)
    {
        g_string_free(this->_location, TRUE);
        this->_location = NULL;
    }

    if(this->_activeConfiguration)
    {
        g_string_free(this->_activeConfiguration, TRUE);
        this->_activeConfiguration = NULL;
    }

    if(this->_sourceFiles)
    {
        g_ptr_array_free(this->_sourceFiles, TRUE);
        this->_sourceFiles = NULL;
    }

    if(this->_headersFolders)
    {
        g_ptr_array_free(this->_headersFolders, TRUE);
        this->_headersFolders = NULL;
    }

    if(this->_dependencies)
    {
        g_ptr_array_free(this->_dependencies, TRUE);
        this->_dependencies = NULL;
    }

    if(this->_buildConfigs)
    {
        g_ptr_array_free(this->_buildConfigs, TRUE);
        this->_buildConfigs = NULL;
    }

    if(this->_projData)
    {
        g_hash_table_destroy(this->_projData);
        this->_projData = NULL;
    }

    G_OBJECT_CLASS(model_project_parent_class)->dispose(obj);
}

///GObject finalize function.
static void
model_project_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_project_parent_class)->finalize(obj);
}

///Static constructor of ModelProject class.
///Init xmlErrorHanlder and defines default build configs.
static void
model_project_class_init(ModelProjectClass* class)
{
    LIBXML_TEST_VERSION

    xmlSetStructuredErrorFunc(NULL, model_project_handle_xml_error);

    model_project_init_default_build_configs();

    GObjectClass* class_object = G_OBJECT_CLASS(class);

    class_object->dispose = model_project_dispose;
    class_object->finalize = model_project_finalize;
    class_object->set_property = model_project_set_property;
    class_object->get_property = model_project_get_property;

    obj_properties[MODEL_PROJECT_ACTIVE_BUILD_CONFIG_PROP] = 
        g_param_spec_string("active_build_config",
                            "ActiveBuildConfig",
                            "Config name used to build project",
                            NULL,
                            G_PARAM_READWRITE);

    obj_properties[MODEL_PROJECT_UNIT_TEST_PROJ_PROP] = 
        g_param_spec_string("unit_test_project",
                            "UnitTestProject",
                            "Location of project with unit tests",
                            NULL,
                            G_PARAM_READWRITE);

    g_object_class_install_properties(class_object, 
                                      MODEL_PROJECT_PROP_COUNT,
                                      obj_properties);
}

///GObject instance constructor.
static void
model_project_init(ModelProject* this)
{
    this->_activeConfiguration = NULL;
    this->_unitTestsLocation = NULL;

    this->_sourceFiles = g_ptr_array_new_with_free_func(g_object_unref);
    this->_headersFolders = g_ptr_array_new_with_free_func(g_string_clean_up);
    this->_dependencies = g_ptr_array_new_with_free_func(g_object_unref);
    this->_buildConfigs = g_ptr_array_new_with_free_func(g_object_unref);
    this->_projData = g_hash_table_new(g_str_hash, g_str_equal);
    this->_xmlDocVersion = 0;
}

ModelProject* 
model_project_new(GString* location)
{
    ModelProject* this;
    this = g_object_new(MODEL_TYPE_PROJECT, NULL);

    this->_location = location;

    return this;
}

///Function for XML SourceFile object deserialization.
static void
model_project_read_source_file(xmlNodePtr node, gpointer user_data, GError** error)
{
    GError* innerError = NULL;
    ModelProject* this = MODEL_PROJECT(user_data);
    ModelSourceFile* file = model_source_file_new_from_xml(node, &innerError);

    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }
        
    if(file != NULL)
        g_ptr_array_add(this->_sourceFiles, file);
}

///Function for XML Header folder deserialization.
static void
model_project_read_header(xmlNodePtr node, gpointer user_data, GError** error)
{
    ModelProject* this = MODEL_PROJECT(user_data);
    GString* data = g_string_new((gchar*)xmlNodeGetContent(node));
    g_ptr_array_add(this->_headersFolders, data);
}

///Function for XML Dependency object deserialization.
static void
model_project_read_project_dependency(xmlNodePtr node, 
                                      gpointer user_data, 
                                      GError** error)
{
    GError* innerError = NULL;
    ModelProject* this = MODEL_PROJECT(user_data);

    ModelProjectDependency* dep = model_project_dependency_new_from_xml(node, &innerError);

    if(innerError != NULL)
        g_propagate_error(error, innerError);

    if(dep != NULL)
    {
        model_project_dependency_set_owner(dep, this);
        g_ptr_array_add(this->_dependencies, dep);
    }
}

///Function for XML ProjectConfiguration object deserialization.
static void
model_project_read_build_config(xmlNodePtr node, gpointer user_data, GError** error)
{
    GError* innerError = NULL;
    ModelProject* this = MODEL_PROJECT(user_data);
    ModelProjectConfiguration* conf = model_project_configuration_new_from_xml(node, &innerError);

    if(innerError != NULL)
        g_propagate_error(error, innerError);

    if(conf != NULL)
        g_ptr_array_add(this->_buildConfigs, conf);
}

///Function for XML DataEntry object deserialization.
static void
model_project_read_data_entry(xmlNodePtr node, gpointer user_data, GError** error)
{
    ModelProject* this = MODEL_PROJECT(user_data);

    gchar* key = xmlGetProp(node, "Key");
    gchar* value = xmlGetProp(node, "Value");

    g_hash_table_insert(this->_projData, key, value);
}

///Function for XML Project object deserialization.
static void
model_project_read(xmlNodePtr root, 
                   ModelProject* this,
                   GError** error)
{
    GError* innerError = NULL;
    xmlNodePtr node = root->children;

    gint ver = xml_node_read_int(node, "Version", FALSE, &innerError);
    if(innerError != NULL)
    {
        g_error_free(innerError);
        innerError = NULL;
        this->_xmlDocVersion = 0;
    }
    else
    {
        this->_xmlDocVersion = ver;
    }

    xml_node_read_collection(node,
                            "SourceFiles",
                            "SourceFile",
                            FALSE,
                            model_project_read_source_file,
                            this,
                            &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_node_read_collection(node,
                            "IncludeFolders",
                            "string",
                            FALSE,
                            model_project_read_header,
                            this,
                            &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_node_read_collection(node,
                            "SystemDependencies",
                            "Dependency",
                            TRUE,
                            model_project_read_project_dependency,
                            this,
                            &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_node_read_collection(node,
                            "BuildConfigs",
                            "Configuration",
                            TRUE,
                            model_project_read_build_config,
                            this,
                            &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_node_read_collection(node,
                             "ProjectData",
                             "DataEntry",
                             TRUE,
                             model_project_read_data_entry,
                             this,
                             &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    this->_activeConfiguration = xml_node_read_g_string(node, 
                                                        "ActiveBuildConfig",
                                                        FALSE,
                                                        &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }
    this->_unitTestsLocation = xml_node_read_g_string(node, 
                                                      "UnitTestsLocation",
                                                      TRUE,
                                                      &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }
}

///Reads project from locating set in Project's Location field.
static void
model_project_read_project(ModelProject* this,
                           GError** error)
{
    GError* innerError = NULL;

    xmlDocPtr doc = xmlParseFile(this->_location->str);
    g_assert(doc);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    g_assert(root);
    g_assert(strcmp(root->name, "Project") == 0);

    model_project_read(root, this, &innerError);
    if(innerError != NULL)
        g_propagate_error(error, innerError);

    xmlFreeDoc(doc);
}

//There are a banch of function to iterate through GPtrArray with foreach loop
//and executing some kind of job 

static void
model_project_write_source_files(gpointer obj, gpointer data, GError** error)
{
    GError* innerError = NULL;
    
    model_source_file_write_xml((ModelSourceFile*)obj, (xmlTextWriterPtr)data, &innerError);

    if(innerError != NULL)
        g_propagate_error(error, innerError);
}

static void
model_project_write_include_folders(gpointer obj, gpointer data, GError** error)
{
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    GString* str = (GString*)obj;

    xmlTextWriterWriteElement(writer, "string", str->str);
}

static void
model_project_write_system_dependencies(gpointer obj, gpointer data, GError** error)
{
    GError* innerError = NULL;
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    ModelProjectDependency* dep = (ModelProjectDependency*)obj;

    model_project_dependency_write_xml(dep, writer, &innerError);

    if(innerError != NULL)
        g_propagate_error(error, innerError);
}

static void
model_project_write_build_configs(gpointer obj, gpointer data, GError** error)
{
    GError* innerError = NULL;
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;
    ModelProjectConfiguration* conf = (ModelProjectConfiguration*)obj;

    model_project_configuration_write_xml(conf, writer, &innerError);
    if(innerError != NULL)
        g_propagate_error(error, innerError);
}

static void
model_project_write_data(gpointer key, gpointer value, gpointer data, GError** error)
{
    xmlTextWriterPtr writer = (xmlTextWriterPtr)data;

    gchar* strKey = (gchar*)key;
    gchar* strVal = (gchar*)value;

    xmlTextWriterStartElement(writer, "DataEntry");
    xmlTextWriterWriteAttribute(writer, "Key", strKey);
    xmlTextWriterWriteAttribute(writer, "Value", strVal);
    xmlTextWriterEndElement(writer);
}

static void
model_project_write_project(ModelProject* this,
                            GError** error)
{
    int rc;
    GError* innerError = NULL;
    xmlTextWriterPtr writer;
    xmlDocPtr doc;

    writer = xmlNewTextWriterDoc(&doc, 0);
    if(!writer)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot create XML document");
        return;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    if(rc<0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot start document");
        return;
    }

    rc = xmlTextWriterStartElement(writer, "Project");
    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot start document");
        return;
    }

    xml_text_writer_write_int(writer,
                              "Version",
                              this->_xmlDocVersion,
                              &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_text_writer_write_ptr_array(writer, 
                                    "SourceFiles", 
                                    this->_sourceFiles, 
                                    model_project_write_source_files,
                                    &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_text_writer_write_ptr_array(writer, 
                                   "IncludeFolders", 
                                   this->_headersFolders, 
                                   model_project_write_include_folders,
                                   &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }    

    xml_text_writer_write_ptr_array(writer, 
                                   "SystemDependencies", 
                                   this->_dependencies, 
                                   model_project_write_system_dependencies,
                                   &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_text_writer_write_ptr_array(writer, 
                                   "BuildConfigs", 
                                   this->_buildConfigs, 
                                   model_project_write_build_configs,
                                   &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    xml_text_writer_write_hash_table(writer,
                                     "ProjectData",
                                     this->_projData,
                                     model_project_write_data,
                                     &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    gchar* activeConf = NULL;
    if(this->_activeConfiguration == NULL)
        activeConf = "";
    else
        activeConf = this->_activeConfiguration->str;

    xml_text_writer_write_string(writer, 
                                 "ActiveBuildConfig", 
                                 activeConf,
                                 &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    gchar* unitTests = NULL;
    if(this->_unitTestsLocation == NULL)
        unitTests = "";
    else
        unitTests = this->_unitTestsLocation->str;

    xml_text_writer_write_string(writer, 
                                 "UnitTestsLocation", 
                                 unitTests,
                                 &innerError);
    if(innerError != NULL)
    {
        g_propagate_error(error, innerError);
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if(rc < 0)
    {
        g_set_error(error,
                    g_quark_from_string("XML"),
                    XML_CANNOT_WRITE,
                    "Cannot end document");
        return;
    }

    xmlFreeTextWriter(writer);
    xmlSaveFileEnc(this->_location->str, doc, "UTF-8");
    xmlFreeDoc(doc);
}


gboolean
model_project_load_or_create_project(GString* location, 
                                     ModelProject** output,
                                     GError** error)
{
    GError* innerError = NULL;
    GString* dot = g_string_new(".");
    ModelProject* toRet;
    if(g_string_equal(location, dot))
        toRet = model_project_new(g_string_new(""));
    else
        toRet = model_project_new(location);

    if(g_file_test(location->str, G_FILE_TEST_EXISTS))
    {
        model_project_read_project(toRet, &innerError);

        if(innerError != NULL)
            g_propagate_error(error, innerError);

        (*output) = toRet;
        return TRUE;
    }
    else //not exists
    {
        FILE* proj = fopen(location->str, "w+");

        gint ver = atoi(DEFAULT_PROJECT_VERSION);
        toRet->_xmlDocVersion = ver;

        model_project_write_project(toRet, &innerError);
        if(innerError != NULL)
            g_propagate_error(error, innerError);

        fclose(proj);

        (*output) = toRet;
        return FALSE;
    }
}

gboolean
model_project_add_source_file(ModelProject* this, ModelSourceFile* file)
{
    g_assert(this);
    g_assert(file);

    if(!g_file_test(model_source_file_get_path(file)->str, G_FILE_TEST_EXISTS))
        return FALSE;

    if(g_ptr_array_find_with_equal_func(this->_sourceFiles, file, model_source_file_equals, NULL))
        return TRUE;

    g_ptr_array_add(this->_sourceFiles, file);

    return TRUE;
}

void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file)
{
    g_assert(this);
    g_assert(file);

    guint index = 0;

    if(g_ptr_array_find_with_equal_func(this->_sourceFiles, file, model_source_file_equals, &index))
        g_ptr_array_remove_index(this->_sourceFiles, index);

    g_object_unref(file);
}

///Wrapper on g_string_equal. Created to prevent incorrect pointer warning.
static gboolean
model_project_find_gstring(const void* obj1, const void* obj2)
{
    GString* str1 = (GString*)obj1;
    GString* str2 = (GString*)obj2;

    return g_string_equal(str1, str2);
}

gboolean
model_project_add_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    GError* error = NULL;

    GDir* test = g_dir_open(folder->str, 0, &error);
    if(test)
        g_dir_close(test);
    else
        return FALSE;

    if(g_ptr_array_find_with_equal_func(this->_headersFolders, folder, model_project_find_gstring, NULL))
        return TRUE;;

    g_ptr_array_add(this->_headersFolders, folder);

    return TRUE;
}

void
model_project_remove_include_folder(ModelProject* this, GString* folder)
{
    g_assert(this);
    g_assert(folder);

    guint index = 0;
    if(!g_ptr_array_find_with_equal_func(this->_headersFolders, folder, model_project_find_gstring, &index))
        return;

    g_ptr_array_remove_index(this->_headersFolders, index);
}

void
model_project_save(ModelProject* this, 
                   GString* dest,
                   GError** error)
{
    g_assert(this);
    GError* innerError = NULL;

    if(dest != NULL && !g_string_equal(this->_location, dest))
    {
        GString* newLoc = g_string_new(dest->str);
        g_string_free(this->_location, TRUE);
        this->_location = newLoc;
    }

    model_project_write_project(this, &innerError);
    if(innerError)
        g_propagate_error(error, innerError);
}

void
model_project_add_dependency(ModelProject* this, ModelProjectDependency* dependency)
{
    g_assert(this);
    g_assert(dependency);

    if(g_ptr_array_find_with_equal_func(this->_dependencies, dependency, model_project_dependency_equals, NULL))
        return;

    model_project_dependency_set_owner(dependency, this);

    //add validation here

    g_ptr_array_add(this->_dependencies, dependency);    
}

void
model_project_remove_dependency(ModelProject* this, ModelProjectDependency* dependency)
{
    g_assert(this);
    g_assert(dependency);

    guint index = -1;
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
model_project_resolve_path(ModelProject* this, GString* relPath)
{
    g_assert(this);
    g_assert(relPath);
    g_assert(relPath->len != 0);
    
    ///If string starts with '/' it is already absoulte path.
    if(g_str_has_prefix(relPath->str, "/"))
        return g_string_clone(relPath);

    GString* loc = g_string_new(g_path_get_dirname(this->_location->str));

    if(strcmp(loc->str, ".") == 0)
        g_string_truncate(loc, 0);

    gchar** parts = g_strsplit(relPath->str, "/", -1);

    int len = g_strv_length(parts);
    gchar* part = NULL;
    for(int i = 0; i<len; i++)
    {
        part = parts[i];

        if(g_strcmp0(part, ".") == 0)
        {
            continue;   
        }
        else if(g_strcmp0(part, "..") == 0)
        {
            gint index = strrchr(loc->str, '/') - loc->str;
            
            if(index < 0)
            {
                if(loc->len != 0)
                    index = 0;
                else
                    g_string_append(loc, g_strdup(".."));
            }

            g_string_truncate(loc, index);  
        }
        else
        {
            if(!g_str_has_suffix(loc->str, "/") && loc->len != 0)
                g_string_append(loc, g_strdup("/"));

            g_string_append(loc, g_strdup(part));
        }
    }

    g_strfreev(parts);

    return loc;
}

GString*
model_project_get_location(ModelProject* this)
{
    g_assert(this);

    return this->_location;
}

GString*
model_project_get_active_build_config(ModelProject* this)
{
    g_assert(this);

    return this->_activeConfiguration;
}

void
model_project_set_active_build_config(ModelProject* this, GString* configName)
{
    g_assert(this);

    if(this->_activeConfiguration)
        g_string_free(this->_activeConfiguration, TRUE);

    this->_activeConfiguration = configName;    
}

GString*
model_project_get_unit_tests_project_location(ModelProject* this)
{
    g_assert(this);

    return this->_unitTestsLocation;
}

void
model_project_set_unit_tests_project_location(ModelProject* this, GString* location)
{
    g_assert(this);

    if(this->_unitTestsLocation != NULL)
        g_string_free(this->_unitTestsLocation, TRUE);

    this->_unitTestsLocation = location;
}

GHashTable*
model_project_get_data(ModelProject* this)
{
    g_assert(this);

    g_hash_table_ref(this->_projData);

    return this->_projData;
}