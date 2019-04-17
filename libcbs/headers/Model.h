#ifndef _MODEL_H_
#define _MODEL_H_

#include "glib-object.h"

G_BEGIN_DECLS

#define MODEL_TYPE_PROJECT model_project_get_type()

///Type that represents project as a set of source files, dependencies and 
///resources
G_DECLARE_FINAL_TYPE(ModelProject, model_project, MODEL, PROJECT, GObject);

#define MODEL_TYPE_PROJECT_CONFIGURATION model_project_configuration_get_type()

///Type for storing basic build information (Output file name, optimization level and so on)
G_DECLARE_FINAL_TYPE(ModelProjectConfiguration, model_project_configuration, MODEL, PROJECT_CONFIGURATION, GObject);

#define MODEL_TYPE_PROJECT_DEPENDENCY model_project_dependency_get_type()

///Type that represents project dependency
G_DECLARE_FINAL_TYPE(ModelProjectDependency, model_project_dependency, MODEL, PROJECT_DEPENDENCY, GObject);

#define MODEL_TYPE_PROJECT_MANAGER model_project_manager_get_type()

///Type that performes operation on projects.
G_DECLARE_FINAL_TYPE(ModelProjectManager, model_project_manager, MODEL, PROJECT_MANAGER, GObject);

#define MODEL_TYPE_SOURCE_FILE model_source_file_get_type()

///Type that represent one file that used in project.
G_DECLARE_FINAL_TYPE(ModelSourceFile, model_source_file, MODEL, SOURCE_FILE, GObject);

G_END_DECLS

#endif