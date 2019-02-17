#ifndef _MODEL_PROJECT_DEP_H_
#define _MODEL_PROJECT_DEP_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

#define MODEL_TYPE_PROJECT_DEPENDENCY model_project_dependency_get_type()

G_DECLARE_FINAL_TYPE(ModelProjectDependency, model_project_dependency, MODEL, PROJECT_DEPENDENCY, GObject);

enum MODEL_PROJECT_DEPENDENCY_TYPES
{
    SYSTEM_DEP,
    EXTERNAL_STATIC_LIB,
    EXTERNAL_DYNAMIC_LIB,
    CBS_PROJECT,
    LAST_DEP_TYPE
};

ModelProjectDependency*
model_project_dependency_new(GString* representation, gint type);

ModelProjectDependency*
model_project_dependency_new_from_xml(xmlNodePtr node);

void
model_project_dependency_write_xml(ModelProjectDependency* this, xmlTextWriterPtr writer);

GString*
model_project_dependency_get_representation(ModelProjectDependency* this);

GString*
model_project_dependency_get_includes(ModelProjectDependency* this);

GString*
model_project_dependency_get_links(ModelProjectDependency* this);

gint
model_project_dependency_get_dependency_type(ModelProjectDependency* this);

int
model_project_dependency_equals(const void* obj1, const void* obj2);
G_END_DECLS

#endif
