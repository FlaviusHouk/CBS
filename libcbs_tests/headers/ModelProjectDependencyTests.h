#include "stdio.h"
#include "stdarg.h"
#include "stddef.h"
#include "setjmp.h"
#include "stdint.h"

#include "cmocka.h"

#include "ModelProjectDependency.h"

void
model_project_dependency_creation_test(void **state);

void
model_project_dependency_include_string_test(void** state);

void
model_project_dependency_link_string_test(void** state);

void
model_project_dependency_equlity_test(void** state);