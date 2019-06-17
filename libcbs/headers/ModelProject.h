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

#include "Model.h"

#include "stdio.h"

#include "glib.h"
#include "gmodule.h"

#include "ModelProjectDependency.h"
#include "ModelSourceFile.h"
#include "ModelProjectConfiguration.h"

G_BEGIN_DECLS

enum MODEL_PROJECT_PROPERTIES
{
    MODEL_PROJECT_ACTIVE_BUILD_CONFIG_PROP = 1,
    MODEL_PROJECT_UNIT_TEST_PROJ_PROP,

    MODEL_PROJECT_PROP_COUNT
};

///Function that loads project if it exists or creates new one with given location
///returns project from given location
///location - project location
gboolean
model_project_load_or_create_project(GString* location, 
                                     ModelProject** output,
                                     GError** error);

///Constructor for Project class. Used for unit testing or for internal object creation.
ModelProject* 
model_project_new(GString* location);

///Getter for project location
///Value should not be cleaned manually
GString*
model_project_get_location(ModelProject* this);

///Adds SourceFile to project. If there is one with the same name, duplicate 
///will not be added.
///file - file that added to the project. It should not be freed.
gboolean
model_project_add_source_file(ModelProject* this, 
                              ModelSourceFile* file);

///Getter for source files collection
GPtrArray*
model_project_get_source_files(ModelProject* this);

///Removes source file from project. Cleans memory for given file to look up.
///If there is no such file nothing will be done.
///file - file to remove
void
model_project_remove_source_file(ModelProject* this, ModelSourceFile* file);

///Adds ProjectDependency to a project. If there is one with the same name, duplicate 
///will not be added.
///dependency - dependency that added to the project. It should not be freed.
void
model_project_add_dependency(ModelProject* this, ModelProjectDependency* dependency);

///Getter for all project dependencies.
GPtrArray*
model_project_get_dependencies(ModelProject* this);

///Removes dependency from project. Cleans memory for given file to look up.
///If there is no such file nothing will be done.
///dependency - dependency to remove
void
model_project_remove_dependency(ModelProject* this, ModelProjectDependency* dependency);

///Removes dependency from project according to it's name (first occurance is deleted). 
///Does not cleans given GString*, so it's value should be manualy freed.
///If there is no such file nothing will be done.
///depName - dependency name to remove
void
model_project_remove_dependency_by_name(ModelProject* this, GString* depName);

///Adds include folder(path to it) to project. If there is one with the same name, 
///duplicate will not be added.
///folder - path to include folder. Should not be freed.
gboolean
model_project_add_include_folder(ModelProject* this, GString* folder);

///Getter for collection of includes folder.
GPtrArray*
model_project_get_includes(ModelProject* this);

///Removes include folder from project. Frees memory for given folder.
///If there is no such folder, nothing will be done.
void
model_project_remove_include_folder(ModelProject* this, GString* folder);

///Performes saving project into file definition path.
///dest - path to save project. If dest == NULL Project location is used.
void
model_project_save(ModelProject* this, 
                   GString* dest,
                   GError** error);

///Seeks for build config (default and project defined).
///If there is no such config throws exception.
///Increments reference counter of returning object.
///configName - config to seek. If configName == NULL Project 
///ActiveConfig is returned. 
ModelProjectConfiguration*
model_project_get_build_config(ModelProject* this, GString* configName);

///Getter for active build config name.
GString*
model_project_get_active_build_config(ModelProject* this);

///Setter for active build config name.
///Does not checks existance of config.
void
model_project_set_active_build_config(ModelProject* this, GString* configName);

///Resoves a path relative to current working directory(one cbs is statred from)
///and main project location. Alocates new string that should be freed.
GString*
model_project_resolve_path(ModelProject* this, GString* relPath);

///Getter for location of cbs project with unit tests
GString*
model_project_get_unit_tests_project_location(ModelProject* this);

///Setter for location of cbs project with unit tests
///Passed string should not be freed
void
model_project_set_unit_tests_project_location(ModelProject* this, GString* location);

///Getter for data hash table
///Increments reference counter for GHashTable.
GHashTable*
model_project_get_data(ModelProject* this);

G_END_DECLS

#endif