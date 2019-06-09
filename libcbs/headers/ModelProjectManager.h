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


#ifndef _MODEL_PROJECT_MANAGER_H_
#define _MODEL_PROJECT_MANAGER_H_

#include "Model.h"

#include "stdio.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

#include "ModelProject.h"

typedef gint ProjectBuildOption;

enum PROJECT_BUILD_OPTIONS
{
    PUBLISH = 1,
    REBUILD = 2
};

G_BEGIN_DECLS

//Instance constructor for ProjectManager type.
ModelProjectManager*
model_project_manager_new();

///Creates project in the provided location. 
///Also creates required folder hierachy.
///@location - project location.
///@error - inner exception
void
model_project_manager_create_project(GString* location, 
                                     GString* templateName, /* = NULL */
                                     GError** error);

///Builds project.
///@toBuild - project user wishes to build.
///@config - build config name to use.
///@isPublishing - flag that indicates rpath including in executable.
///@error - inner exception
void
model_project_manager_build_project(ModelProjectManager* this, 
                                    ModelProject* toBuild, 
                                    GString* config,
                                    ProjectBuildOption options,
                                    GError** error);

///Executes unit tests.
///@toBuild - unit test project.
///@error - inner exception
void
model_project_manager_run_tests(ModelProjectManager* this, 
                                ModelProject* toBuild,
                                GError** error);

G_END_DECLS

#endif