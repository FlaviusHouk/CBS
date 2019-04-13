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

static void
model_project_configuration_class_init(ModelProjectConfigurationClass* class)
{
    GObjectClass* class_object = G_OBJECT_CLASS(class);

    class_object->dispose = model_project_configuration_dispose;
    class_object->finalize = model_project_configuration_finalize;
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

    g_string_append(string, macro->str);
    g_string_append(string, g_strdup(" "));
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