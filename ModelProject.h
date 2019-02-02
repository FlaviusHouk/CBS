#ifndef _MODEL_PROJECT_H_
#define _MODEL_PROJECT_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "ModelSourceFile.h"

G_BEGIN_DECLS

#define MODEL_TYPE_PROJECT model_project_get_type()

G_DECLARE_FINAL_TYPE(ModelProject, model_project, MODEL, PROJECT, GObject);

G_END_DECLS

ModelProject* 
model_project_load_or_create_project(GString* location);

void
model_project_add_source_file(ModelProject* this, ModelSourceFile* file);

void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file);

/*
void
model_project_add_dependency(ModelProject* this, ModelProjectDependency* dependency);

void
model_project_remove_dependency(ModelProject* this, ModelProjectDependency* dependency);
*/

void
model_project_add_include_folder(ModelProject* this, GString* folder);

void
model_project_remove_include_folder(ModelProject* this, GString* folder);

void
model_project_save(ModelProject* this, const GString* dest);

#endif