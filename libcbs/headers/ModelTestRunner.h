#ifndef _MODEL_TEST_RUNNER_H_
#define _MODEL_TEST_RUNNER_H_

#include "Model.h"

#include "glib.h"

ModelTestRunner*
model_test_runner_new();

gint
model_test_runner_execute_tests(ModelTestRunner* this, GString* loc);

#endif