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

#ifndef _MODEL_PROJECT_DEP_H_
#define _MODEL_PROJECT_DEP_H_

#include "Model.h"

#include "stdio.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

enum MODEL_PROJECT_DEPENDENCY_PROPERTIES
{
    MODEL_PROJECT_DEPENDENCY_REPRESENTATION_PROP = 1,
    MODEL_PROJECT_DEPENDENCY_INCLUDE_STRING_PROP,
    MODEL_PROJECT_DEPENDENCY_LINK_STRING_PROP,
    MODEL_PROJECT_DEPENDENCY_TYPE_PROP,
    MODEL_PROJECT_DEPENDENCY_OWNER_PROR,

    MODEL_PROJECT_DEPENDENCY_PROP_COUNT
};

///Possible dependency types
enum MODEL_PROJECT_DEPENDENCY_TYPES
{
    SYSTEM_DEP,
    EXTERNAL_STATIC_LIB,
    EXTERNAL_DYNAMIC_LIB,
    CBS_PROJECT,
    LAST_DEP_TYPE
};

///Constructor for ProjectDependency type
///representation - depends of dependency type (for example a path to project
///in case of CBS_PROJECT dependency, or component name in case of SYSTEM_DEP)
///type - dependency type
ModelProjectDependency*
model_project_dependency_new(GString* representation, gint type);

///Deserialization method.
///@node - parent xml node with ModelProjectDepedency.
ModelProjectDependency*
model_project_dependency_new_from_xml(xmlNodePtr node,
                                      GError** error);

///Serialization method.
void
model_project_dependency_write_xml(ModelProjectDependency* this, 
                                   xmlTextWriterPtr writer,
                                   GError** error);

///Getter for Representation property
GString*
model_project_dependency_get_representation(ModelProjectDependency* this);

///Getter for Includes property. Gets the string with information about includes
///for each dependency. 
///retuns string with -I options for GCC. Should be cleaned.
GString*
model_project_dependency_get_includes(ModelProjectDependency* this,
                                      GError** error);

///Getter for Links property. Gets the string with information about linking for 
///each dependency.
///return string with linker options.
///@includeRPath - flag that indicates rpath including in library 
GString*
model_project_dependency_get_links(ModelProjectDependency* this,
                                   gboolean includeRPath,
                                   GError** error);

///Getter for Dependency type property.
gint
model_project_dependency_get_dependency_type(ModelProjectDependency* this);

///Checks the equility of two ProjectConfiguration instances.
int
model_project_dependency_equals(const void* obj1, const void* obj2);

///Getter for Owner property.
///Increments reference counter of returned object.
ModelProject*
model_project_dependency_get_owner(ModelProjectDependency* this);

///Setter for Owner property.
///Increments counter of passed value.
void
model_project_dependency_set_owner(ModelProjectDependency* this, ModelProject* owner);

G_END_DECLS

#endif
