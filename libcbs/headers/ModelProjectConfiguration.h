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

#ifndef _MODEL_PROJECT_CONFIGURATION_H_
#define _MODEL_PROJECT_CONFIGURATION_H_

#include "Model.h"

#include "glib.h"
#include "glib-object.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

enum MODEL_PROJECT_CONFIGURATION_PROPERTIES
{
    MODEL_PROJECT_CONFIGURATION_NAME_PROP = 1,
    MODEL_PROJECT_CONFIGURATION_OUTPUT_NAME_PROP,
    MODEL_PROJECT_CONFIGURATION_OPTIMIZATION_LEVEL_PROP,
    MODEL_PROJECT_CONFIGURATION_C_STANDARD_PROP,
    MODEL_PROJECT_CONFIGURATION_OUTPUT_TYPE_PROP,
    MODEL_PROJECT_CONFIGURATION_IS_IGNORE_OPTIONS_PROP,
    MODEL_PROJECT_CONFIGURATION_CUSTOM_CONFIG_PROP,    

    MODEL_PROJECT_CONFIGURATION_PROP_COUNT
};

///Enum with C standard versions
enum CStandard
{
    ANSI_C,
    C89,
    C99,
    C11,

    CStandard_COUNT
};

///Enum with GCC optimization levels
enum Optimization
{
    RELEASE_FAST, //-Ofast
    RELEASE_SIZE, //-Os
    RELEASE_3, //-O3
    RELEASE_2, //-O2
    RELEASE_1, //-O1
    RELEASE_0, //-O0
    DEBUG_1, //-g1
    DEBUG_2, //-g
    DEBUG_3, //-g3

    Optimization_COUNT
};

///Enum with possible output files for project build.
enum OutputType
{
    ELF,
    DYNAMIC_LIB,
    STATIC_LIB,

    OUTPUT_TYPE_COUNT
};

///Constructor for ProjectConfiguration type.
///name - config name
ModelProjectConfiguration*
model_project_configuration_new(GString* name);

///Deserialization method.
///node - xmlNode to read from.
ModelProjectConfiguration*
model_project_configuration_new_from_xml(xmlNodePtr node);

///Serialization method.
///writer - xml writer to save data
void
model_project_configuration_write_xml(ModelProjectConfiguration* this, xmlTextWriterPtr writer);

///Getter for Config name property
GString*
model_project_configuration_get_name(ModelProjectConfiguration* this);

///Getter for Output File name property
GString*
model_project_configuration_get_output_name(ModelProjectConfiguration* this);

///Setter for Output File name property
void
model_project_configuration_set_output_name(ModelProjectConfiguration* this, GString* outputName);

///Getter for Optimization level property
gint
model_project_configuration_get_optimization_level(ModelProjectConfiguration* this);

///Setter for Optimization level property
void
model_project_configuration_set_optimization_level(ModelProjectConfiguration* this, gint optimization);

///Getter for C Standard property
gint
model_project_configuration_get_c_standard_version(ModelProjectConfiguration* this);

///Setter for C Standard property
void
model_project_configuration_set_c_standard_version(ModelProjectConfiguration* this, gint version);

///Getter for Output file type property
gint
model_project_configuration_get_output_type(ModelProjectConfiguration* this);

///Setter for Output file type property
void
model_project_configuration_set_output_type(ModelProjectConfiguration* this, gint outputType);

///Getter for Is Ignore Options property.
gboolean
model_project_configuration_get_is_ignore_options(ModelProjectConfiguration* this);

///Setter for Is Ignore Options property.
void
model_project_configuration_set_is_ignore_options(ModelProjectConfiguration* this, gboolean isIgnoreChanges);

///Getter for Custom config property.
GString*
model_project_configuration_get_custom_config(ModelProjectConfiguration* this);

///Function for comparing two ProjectConfiguration instances. 
gboolean
model_project_configuration_equals(const void* first, const void* second);

///Function builds config string for compiler according to ProjectConfiguration parameters.
///Allocates new string.
GString*
model_project_configuration_build_config_string(ModelProjectConfiguration* this);

///Adds macro to macros collection.
///If there already was such macro it won't be added.
///Passed string should not be cleaned.
void
model_project_configuration_define_macro(ModelProjectConfiguration* this,
                                         GString* macro);

G_END_DECLS

#endif