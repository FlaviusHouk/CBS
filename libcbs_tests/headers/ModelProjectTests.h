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

#include "stdio.h"
#include "stdarg.h"
#include "stddef.h"
#include "setjmp.h"
#include "stdint.h"

#include "cmocka.h"

void
model_project_creation_test_positive_case(void** state);

void
model_project_location_property_test(void** state);

void 
model_project_add_source_file_test(void** state);

void
model_project_dependencies_manipulation_test(void** state);

void
model_project_headers_manipulation_test(void** state);

void
model_project_configs_manipulation_test(void** state);

void
model_project_resolve_path_test(void** state);