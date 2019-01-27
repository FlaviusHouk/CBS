#ifndef _MODEL_PROJECT_MANAGER_H_
#define _MODEL_PROJECT_MANAGER_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

#include "ModelProject.h"

G_BEGIN_DECLS

#define MODEL_TYPE_PROJECT_MANAGER model_project_manager_get_type()

G_DECLARE_FINAL_TYPE(ModelProjectManager, model_project_manager, MODEL, PROJECT_MANAGER, GObject);

G_END_DECLS

ModelProjectManager*
model_project_manager_new();

int
model_project_manager_create_project(GString* location);

int
model_project_manager_build_project(ModelProjectManager* this, ModelProject* toBuild);

int
model_project_run_tests(ModelProjectManager* this, ModelProject* toBuild);

#endif