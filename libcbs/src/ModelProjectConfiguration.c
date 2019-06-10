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
#include "ModelProjectConfiguration.h"

#include "Helper.h"
#include "Helpers/XMLHelpers.h"

struct _ModelProjectConfiguration
{
    GObject parent;

    GString* _name;
    GString* _outputName;
    gint _optimization;
    gint _cStandard;
    gint _outputType;
    GPtrArray* _macrosToDefine;

    gboolean _ignoreOptions;
    GString* _customConfig;
};

G_DEFINE_TYPE(ModelProjectConfiguration, model_project_configuration, G_TYPE_OBJECT);

static void
model_project_configuration_dispose(GObject* obj)
{
    ModelProjectConfiguration* this = MODEL_PROJECT_CONFIGURATION(obj);

    if(this->_customConfig)
    {
        g_string_free(this->_customConfig, TRUE);
        this->_customConfig = NULL;
    }

    if(this->_macrosToDefine)
    {
        g_ptr_array_free(this->_macrosToDefine, TRUE);
        this->_macrosToDefine = NULL;
    }

    if(this->_name)
    {
        g_string_free(this->_name, TRUE);
        this->_name = NULL;
    }

    if(this->_outputName)
    {
        g_string_free(this->_outputName, TRUE);
        this->_outputName = NULL;
    }

    G_OBJECT_CLASS(model_project_configuration_parent_class)->dispose(obj);
}

static void
model_project_configuration_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_project_configuration_parent_class)->finalize(obj);
}

static GParamSpec* obj_props[MODEL_PROJECT_CONFIGURATION_PROP_COUNT] = { NULL, };

static void
model_project_configuration_set_property(GObject* obj,
                                         guint propID,
                                         const GValue* value,
                                         GParamSpec* pspec)
{
    ModelProjectConfiguration* this = MODEL_PROJECT_CONFIGURATION(obj);

    switch(propID)
    {
        case MODEL_PROJECT_CONFIGURATION_C_STANDARD_PROP:
        {
            gint val = g_value_get_int(value);
            model_project_configuration_set_c_standard_version(this, val);
            break;
        }
        /*case MODEL_PROJECT_CONFIGURATION_CUSTOM_CONFIG_PROP:
        {
            gchar* val = g_value_get_string(value);

            GString* stringValue = NULL;
            if(val != NULL)
                model_project_configuration_set_
        }*/
        case MODEL_PROJECT_CONFIGURATION_IS_IGNORE_OPTIONS_PROP:
        {
            gboolean val = g_value_get_boolean(value);
            model_project_configuration_set_is_ignore_options(this, val);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_NAME_PROP:
        {
            const gchar* val = g_value_get_string(value);

            GString* stringValue = NULL;
            if(val != NULL)
                stringValue = g_string_new(g_strdup(val));

            this->_name = stringValue;
        }
        case MODEL_PROJECT_CONFIGURATION_OPTIMIZATION_LEVEL_PROP:
        {
            gint val = g_value_get_int(value);
            model_project_configuration_set_optimization_level(this, val);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_OUTPUT_NAME_PROP:
        {
            const gchar* val = g_value_get_string(value);

            GString* stringValue = NULL;
            if(val != NULL)
                stringValue = g_string_new(g_strdup(val));

            model_project_configuration_set_output_name(this, stringValue);
        }
        case MODEL_PROJECT_CONFIGURATION_OUTPUT_TYPE_PROP:
        {
            gint val = g_value_get_int(value);
            model_project_configuration_set_output_type(this, val);
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
model_project_configuration_get_property(GObject* obj,
                                         guint propID,
                                         GValue* value,
                                         GParamSpec* pspec)
{
    ModelProjectConfiguration* this = MODEL_PROJECT_CONFIGURATION(obj);

    switch(propID)
    {
        case MODEL_PROJECT_CONFIGURATION_C_STANDARD_PROP:
        {
            gint val = model_project_configuration_get_c_standard_version(this);
            g_value_set_int(value, val);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_CUSTOM_CONFIG_PROP:
        {
            gchar* val = NULL;

            if(this->_customConfig != NULL)
                val = this->_customConfig->str;

            g_value_set_string(value, val);
        }
        case MODEL_PROJECT_CONFIGURATION_IS_IGNORE_OPTIONS_PROP:
        {
            gboolean val = model_project_configuration_get_is_ignore_options(this);
            g_value_set_boolean(value, val);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_NAME_PROP:
        {
            GString* val = model_project_configuration_get_name(this);

            gchar* string = NULL;
            if(val != NULL)
                string = val->str;

            g_value_set_string(value, string);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_OPTIMIZATION_LEVEL_PROP:
        {
            gint val = model_project_configuration_get_optimization_level(this);
            g_value_set_int(value, val);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_OUTPUT_NAME_PROP:
        {
            GString* val = model_project_configuration_get_output_name(this);

            gchar* string = NULL;
            if(val != NULL)
                string = val->str;

            g_value_set_string(value, string);
            break;
        }
        case MODEL_PROJECT_CONFIGURATION_OUTPUT_TYPE_PROP:
        {
            gint val = model_project_configuration_get_output_type(this);
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
model_project_configuration_class_init(ModelProjectConfigurationClass* class)
{
    GObjectClass* class_object = G_OBJECT_CLASS(class);

    class_object->dispose = model_project_configuration_dispose;
    class_object->finalize = model_project_configuration_finalize;

    class_object->set_property = model_project_configuration_set_property;
    class_object->get_property = model_project_configuration_get_property;

    obj_props[MODEL_PROJECT_CONFIGURATION_C_STANDARD_PROP] = 
        g_param_spec_int("c_standard",
                         "CStandard",
                         "C Standard that compiler is using for build",
                         ANSI_C,
                         CStandard_COUNT,
                         C11,
                         G_PARAM_READWRITE);
    
    obj_props[MODEL_PROJECT_CONFIGURATION_CUSTOM_CONFIG_PROP] = 
        g_param_spec_string("custom_config",
                            "CustomConfig",
                            "Configurations that does not covered with cli tools",
                            NULL,
                            G_PARAM_READABLE);

    obj_props[MODEL_PROJECT_CONFIGURATION_IS_IGNORE_OPTIONS_PROP] =
        g_param_spec_boolean("is_ignore_options",
                             "IsIgnoreOptions",
                             "Indicates that only custom config should be used",
                             FALSE,
                             G_PARAM_READWRITE);

    obj_props[MODEL_PROJECT_CONFIGURATION_NAME_PROP] = 
        g_param_spec_string("name",
                            "Name",
                            "Config name",
                            NULL,
                            G_PARAM_CONSTRUCT_ONLY && G_PARAM_READWRITE);

    obj_props[MODEL_PROJECT_CONFIGURATION_OPTIMIZATION_LEVEL_PROP] = 
        g_param_spec_int("optimization_level",
                         "OptimizationLevel",
                         "Optimization level that used for compilation",
                         0,
                         Optimization_COUNT,
                         DEBUG_1,
                         G_PARAM_READWRITE);

    obj_props[MODEL_PROJECT_CONFIGURATION_OUTPUT_NAME_PROP] = 
        g_param_spec_string("output_name",
                            "OutputName",
                            "Name used to save output file",
                            NULL,
                            G_PARAM_READWRITE);

    obj_props[MODEL_PROJECT_CONFIGURATION_OUTPUT_TYPE_PROP] = 
        g_param_spec_string("output_type",
                            "OutputType",
                            "Target to build",
                            ELF,
                            G_PARAM_READWRITE);

    g_object_class_install_properties(class_object,
                                      MODEL_PROJECT_CONFIGURATION_PROP_COUNT,
                                      obj_props);
}

static void 
model_project_configuration_init(ModelProjectConfiguration* this)
{
    this->_name = NULL;
    this->_outputName = NULL;
    this->_cStandard = C11;
    this->_optimization = DEBUG_2;
    this->_macrosToDefine = g_ptr_array_new_with_free_func(g_string_clean_up);
    this->_outputType = ELF;

    this->_ignoreOptions = FALSE;
    this->_customConfig = NULL;
}

ModelProjectConfiguration*
model_project_configuration_new(GString* name)
{
    ModelProjectConfiguration* this = g_object_new(MODEL_TYPE_PROJECT_CONFIGURATION, NULL);

    this->_name = name;
}

ModelProjectConfiguration*
model_project_configuration_new_from_xml(xmlNodePtr node)
{
    ModelProjectConfiguration* this = g_object_new(MODEL_TYPE_PROJECT_CONFIGURATION, NULL);

    xmlNodePtr conf = node->children;
    
    while(conf != NULL)
    {
        if(strcmp(conf->name, "Name") == 0)
        {
            this->_name = g_string_new(xmlNodeGetContent(conf));
        }
        else if(strcmp(conf->name, "OutputName") == 0)
        {
            this->_outputName = g_string_new(xmlNodeGetContent(conf));
        }
        else if(strcmp(conf->name, "CStandard") == 0)
        {
            this->_cStandard = atoi(xmlNodeGetContent(conf));
            g_assert(this->_cStandard >= 0 && this->_cStandard <= C11);
        }
        else if(strcmp(conf->name, "OutputType") == 0)
        {
            this->_outputType = atoi(xmlNodeGetContent(conf));
            g_assert(this->_outputType >= 0 && this->_outputType < OUTPUT_TYPE_COUNT);
        }
        else if(strcmp(conf->name, "Optimization") == 0)
        {
            this->_optimization = atoi(xmlNodeGetContent(conf));
            g_assert(this->_optimization >= 0 && this->_optimization <= DEBUG_3);
        }
        else if(strcmp(conf->name, "Macros") == 0)
        {
            xmlNode* macros = conf->children;

            while(macros != NULL)
            {
                while(macros != NULL && strcmp(macros->name, "string") != 0)
                {
                    macros = macros->next;
                }

                GString* data = g_string_new(xmlNodeGetContent(macros));
                g_ptr_array_add(this->_macrosToDefine, data);

                macros = macros->next;
            }
        }
        else if(strcmp(conf->name, "IgnoreOptions") == 0)
        {
            this->_ignoreOptions = atoi(xmlNodeGetContent(conf));
        }
        else if(strcmp(conf->name, "CustomConfig") == 0)
        {
            this->_customConfig = g_string_new(xmlNodeGetContent(conf));
        }

        conf = conf->next;
    }

    return this;
}

static void
model_project_configuration_write_macros(gpointer obj, gpointer data)
{
    GString* macro = (GString*)obj;
    xmlTextWriter* writer = (xmlTextWriter*)data;

    xmlTextWriterWriteElement(writer, "string", macro->str);
}

void
model_project_configuration_write_xml(ModelProjectConfiguration* this, xmlTextWriterPtr writer)
{
    g_assert(this);
    g_assert(writer);

    int rc;

    rc = xmlTextWriterStartElement(writer, BAD_CAST "Configuration");
    g_assert(rc >= 0);

    rc = xmlTextWriterWriteElement(writer, "Name", this->_name->str);
    g_assert(rc >= 0);

    rc = xmlTextWriterWriteElement(writer, "OutputName", this->_outputName->str);
    g_assert(rc >= 0);

    char num[16];
    sprintf(num, "%d", this->_cStandard);
    rc = xmlTextWriterWriteElement(writer, "CStandard", num);
    g_assert(rc >= 0);

    sprintf(num, "%d", this->_outputType);
    rc = xmlTextWriterWriteElement(writer, "OutputType", num);
    g_assert(rc >= 0);

    sprintf(num, "%d", this->_optimization);
    rc = xmlTextWriterWriteElement(writer, "Optimization", num);
    g_assert(rc >= 0);

    rc = xmlTextWriterStartElement(writer, "Macros");
    g_assert(rc >= 0);

    g_ptr_array_foreach(this->_macrosToDefine,
                        model_project_configuration_write_macros,
                        writer);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);

    sprintf(num, "%d", this->_ignoreOptions);
    rc = xmlTextWriterWriteElement(writer, "IgnoreOptions", num);
    g_assert(rc >= 0);

    if(this->_customConfig != NULL)
    {
        rc = xmlTextWriterWriteElement(writer, "CustomConfig", this->_customConfig->str);
        g_assert(rc >= 0);
    }

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);
}

GString*
model_project_configuration_get_name(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_name;
}

GString*
model_project_configuration_get_output_name(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_outputName;
}

void
model_project_configuration_set_output_name(ModelProjectConfiguration* this, GString* outputName)
{
    g_assert(this);

    if(this->_outputName)
        g_string_free(this->_outputName, TRUE);

    this->_outputName = outputName;
}

gint
model_project_configuration_get_optimization_level(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_optimization;
}

void
model_project_configuration_set_optimization_level(ModelProjectConfiguration* this, gint optimization)
{
    g_assert(this);

    this->_optimization = optimization;
}

gint
model_project_configuration_get_c_standard_version(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_cStandard;
}

gint
model_project_configuration_get_output_type(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_outputType;
}

void
model_project_configuration_set_output_type(ModelProjectConfiguration* this, gint outputType)
{
    g_assert(this);

    this->_outputType = outputType;
}

void
model_project_configuration_set_c_standard_version(ModelProjectConfiguration* this, gint version)
{
    g_assert(this);

    this->_cStandard = version;
}

gboolean
model_project_configuration_get_is_ignore_options(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_ignoreOptions;
}

void
model_project_configuration_set_is_ignore_options(ModelProjectConfiguration* this, gboolean isIgnoreOptions)
{
    g_assert(this);

    this->_ignoreOptions = isIgnoreOptions;
}

GString*
model_project_configuration_get_custom_config(ModelProjectConfiguration* this)
{
    g_assert(this);

    return this->_customConfig;
}

gboolean
model_project_configuration_equals(const void* first, const void* second)
{
    g_assert(first);
    g_assert(second);

    ModelProjectConfiguration* this = (ModelProjectConfiguration*)first;
    ModelProjectConfiguration* that = (ModelProjectConfiguration*)second;

    return g_string_equal(that->_name, this->_name);
}

static void
model_project_configuration_append_macros(gpointer obj, gpointer data)
{
    GString* macro = (GString*)obj;
    GString* string = (GString*)data;

    g_string_append_printf(string, "-D%s ", macro->str);
}

GString*
model_project_configuration_build_config_string(ModelProjectConfiguration* this)
{
    g_assert(this);

    //if ignoreOptions flag is set, then all config properties are ignored
    //and only CustomConfig is used

    if(this->_ignoreOptions) 
        return this->_customConfig;
    else
    {
        GString* toRet = g_string_new(g_strdup(" "));

        char* toAppend;
        switch(this->_cStandard)
        {
            case ANSI_C:
            {
                toAppend = g_strdup("-ansi");
                break;
            }
            case C89:
            {
                toAppend = g_strdup("-std=c89");
                break;
            }
            case C99:
            {
                toAppend = g_strdup("-std=c99");
                break;
            }
            case C11:
            {
                toAppend = g_strdup("-std=c11");
                break;
            }
            default:
            {
                g_assert(FALSE);
            }
        }

        toRet = g_string_append(toRet, toAppend);
        toRet = g_string_append(toRet, g_strdup(" "));
        
        if(this->_outputType == DYNAMIC_LIB)
            toRet = g_string_append(toRet, g_strdup("-fpic "));

        char* toAppendOptimization;
        switch(this->_optimization)
        {
            case RELEASE_FAST:
            {
                toAppendOptimization = g_strdup("-Ofast");
                break;
            }
            case RELEASE_SIZE:
            {
                toAppendOptimization = g_strdup("-Os");
                break;
            }
            case RELEASE_3:
            {
                toAppendOptimization = g_strdup("-O3");
                break;
            }
            case RELEASE_2:
            {
                toAppendOptimization = g_strdup("-O2");
                break;
            }
            case RELEASE_1:
            {
                toAppendOptimization = g_strdup("-O1");
                break;
            }
            case RELEASE_0:
            {
                toAppendOptimization = g_strdup("-O0");
                break;
            }
            case DEBUG_1:
            {
                toAppendOptimization = g_strdup("-g1");
                break;
            }
            case DEBUG_2:
            {
                toAppendOptimization = g_strdup("-g");
                break;
            }
            case DEBUG_3:
            {
                toAppendOptimization = g_strdup("-g3");
                break;
            }
            default:
            {
                g_assert(FALSE);
            }
        }

        toRet = g_string_append(toRet, toAppendOptimization);
        toRet = g_string_append(toRet, g_strdup(" "));

        g_ptr_array_foreach(this->_macrosToDefine,
                            model_project_configuration_append_macros,
                            toRet);

        if(this->_customConfig != NULL)
        {
            toRet = g_string_append(toRet, g_strdup(" "));
            toRet = g_string_append(toRet, g_strdup(this->_customConfig->str));
        }

        return toRet;
    }
}

void
model_project_configuration_define_macro(ModelProjectConfiguration* this,
                                         GString* macro)
{
    g_assert(this);
    g_assert(macro);

    gint index;
    if(g_ptr_array_find_with_equal_func(this->_macrosToDefine, 
                                        macro, 
                                        (GEqualFunc)g_string_equal, 
                                        &index))
        return;

    g_ptr_array_add(this->_macrosToDefine, macro);
}