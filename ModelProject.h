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


#ifndef _MODEL_PROJECT_H_
#define _MODEL_PROJECT_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "ModelSourceFile.h"
#include "ModelProjectDependency.h"
#include "ModelProjectConfiguration.h"

G_BEGIN_DECLS

#define MODEL_TYPE_PROJECT model_project_get_type()

G_DECLARE_FINAL_TYPE(ModelProject, model_project, MODEL, PROJECT, GObject);

G_END_DECLS

ModelProject* 
model_project_load_or_create_project(GString* location);

GString*
model_project_get_location(ModelProject* this);

void
model_project_add_source_file(ModelProject* this, ModelSourceFile* file);

GPtrArray*
model_project_get_source_files(ModelProject* this);

void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file);

void
model_project_add_dependency(ModelProject* this, ModelProjectDependency* dependency);

GPtrArray*
model_project_get_dependencies(ModelProject* this);

void
model_project_remove_dependency(ModelProject* this, ModelProjectDependency* dependency);

void
model_project_remove_dependency_by_name(ModelProject* this, GString* depName);

void
model_project_add_include_folder(ModelProject* this, GString* folder);

GPtrArray*
model_project_get_includes(ModelProject* this);

void
model_project_remove_include_folder(ModelProject* this, GString* folder);

void
model_project_save(ModelProject* this, GString* dest);

ModelProjectConfiguration*
model_project_get_build_config(ModelProject* this, GString* configName);

#endif