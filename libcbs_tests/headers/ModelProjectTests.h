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