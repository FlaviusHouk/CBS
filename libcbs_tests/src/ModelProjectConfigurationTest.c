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

#include "ModelProjectConfigurationTests.h"

void
model_project_configuration_creation_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* name = (GString*)g_hash_table_lookup(table, "ConfigName");
    ModelProjectConfiguration* config = model_project_configuration_new(name);

    gboolean isCorrect = *((gboolean*)g_hash_table_lookup(table, "IsCorrectInputParameters"));
    
    if(isCorrect)
    {
        GString* savedName = model_project_configuration_get_name(config);

        assert_true(g_string_equal(name, savedName));

        g_object_unref(config);
    }
    else
    {
        assert_null(config);
    }
}

void
model_project_configuration_equility_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* firstName = (GString*)g_hash_table_lookup(table, "FirstConfigName");
    GString* secondName = (GString*)g_hash_table_lookup(table, "SecondConfigName");
    gboolean isEqual = *((gboolean*)g_hash_table_lookup(table, "AreEqual"));

    ModelProjectConfiguration* config1 = model_project_configuration_new(firstName);
    ModelProjectConfiguration* config2 = model_project_configuration_new(secondName);

    if(isEqual)
        assert_true((model_project_configuration_equals(config1, config2)));
    else
        assert_false((model_project_configuration_equals(config1, config2)));
}

void
model_project_configuration_config_string_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* name = (GString*)g_hash_table_lookup(table, "ConfigName");
    GString* expected = (GString*)g_hash_table_lookup(table, "ExpectedRezult");

    gint c_std = *((gint*)g_hash_table_lookup(table, "CStandard"));
    gboolean ignoreOptions = *((gboolean*)g_hash_table_lookup(table, "IgnoreOptions"));
    gint optimization = *((gint*)g_hash_table_lookup(table, "OptimizationLevel"));
    GString* outputName = (GString*)g_hash_table_lookup(table, "OutputName");
    gint outputType = *((gint*)g_hash_table_lookup(table, "OutputType"));

    ModelProjectConfiguration* config = model_project_configuration_new(name);

    model_project_configuration_set_c_standard_version(config, c_std);
    model_project_configuration_set_is_ignore_options(config, ignoreOptions);
    model_project_configuration_set_optimization_level(config, optimization);
    model_project_configuration_set_output_name(config, outputName);
    model_project_configuration_set_output_type(config, outputType);

    GString* configString = model_project_configuration_build_config_string(config);

    assert_non_null(configString);
    assert_true(g_string_equal(configString, expected));
}