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

#ifndef _MODEL_TEST_RUNNER_H_
#define _MODEL_TEST_RUNNER_H_

#include "Model.h"

#include "glib.h"

///Constructor for TestRunner.
ModelTestRunner*
model_test_runner_new(void);

///Executes unit tests.
///@loc - location of binary with all tests.
gint
model_test_runner_execute_tests(ModelTestRunner* this, GString* loc);

#endif