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

G_END_DECLS

#endif