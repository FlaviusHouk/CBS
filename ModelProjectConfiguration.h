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

#include "glib.h"
#include "glib-object.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

enum CStandard
{
    ANSI_C,
    C89,
    C99,
    C11
};

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
};

enum OutputType
{
    ELF,
    DYNAMIC_LIB,
    STATIC_LIB,

    OUTPUT_TYPE_COUNT
};

#define MODEL_TYPE_PROJECT_CONFIGURATION model_project_configuration_get_type()

G_DECLARE_FINAL_TYPE(ModelProjectConfiguration, model_project_configuration, MODEL, PROJECT_CONFIGURATION, GObject);

ModelProjectConfiguration*
model_project_configuration_new(GString* name);

ModelProjectConfiguration*
model_project_configuration_new_from_xml(xmlNodePtr node);

void
model_project_configuration_write_xml(ModelProjectConfiguration* this, xmlTextWriterPtr writer);

GString*
model_project_configuration_get_name(ModelProjectConfiguration* this);

GString*
model_project_configuration_get_output_name(ModelProjectConfiguration* this);

void
model_project_configuration_set_output_name(ModelProjectConfiguration* this, GString* outputName);

gint
model_project_configuration_get_optimization_level(ModelProjectConfiguration* this);

void
model_project_configuration_set_optimization_level(ModelProjectConfiguration* this, gint optimization);

gint
model_project_configuration_get_c_standard_version(ModelProjectConfiguration* this);

void
model_project_configuration_set_c_standard_version(ModelProjectConfiguration* this, gint version);

gint
model_project_configuration_get_output_type(ModelProjectConfiguration* this);

void
model_project_configuration_set_output_type(ModelProjectConfiguration* this, gint outputType);

gboolean
model_project_configuration_get_is_ignore_options(ModelProjectConfiguration* this);

void
model_project_configuration_set_is_ignore_options(ModelProjectConfiguration* this, gboolean isIgnoreChanges);

GString*
model_project_configuration_get_custom_config(ModelProjectConfiguration* this);

gboolean
model_project_configuration_equals(const void* first, const void* second);

GString*
model_project_configuration_build_config_string(ModelProjectConfiguration* this);

G_END_DECLS

#endif