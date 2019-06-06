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

#include "ModelProjectDependencyTests.h"

void
model_project_dependency_creation_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* representation = g_hash_table_lookup(table, "Representation");
    gint type = *((gint*)g_hash_table_lookup(table, "DepType"));
    ModelProjectDependency* dep = model_project_dependency_new(representation, type);

    GString* saved = model_project_dependency_get_representation(dep);
    gint savedType = model_project_dependency_get_dependency_type(dep);

    assert_true(savedType == type);
    assert_true(g_string_equal(representation, saved));

    g_object_unref(dep);
}

void
model_project_dependency_include_string_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* representation = g_hash_table_lookup(table, "Representation");
    gint type = *((gint*)g_hash_table_lookup(table, "DepType"));
    ModelProjectDependency* dep = model_project_dependency_new(representation, type);

    ModelProject* proj = model_project_new(g_string_new(""));
    model_project_dependency_set_owner(dep, proj);

    GError* err = NULL;
    GString* includeString = model_project_dependency_get_includes(dep, &err);

    gboolean shouldGenerateError = *((gboolean*)g_hash_table_lookup(table, "ShouldGenerateError"));
    if(shouldGenerateError)
    {
        gchar* expectedMsg = (gchar*)g_hash_table_lookup(table, "ErrorMsg");

        assert_null(includeString);
        assert_non_null(err);
        assert_true(g_str_equal(expectedMsg, err->message));

        g_object_unref(dep);
    }
    else
    {
        assert_non_null(includeString);
        assert_null(err);

        GString* expected = g_hash_table_lookup(table, "IncludeString");
        assert_true(g_string_equal(expected, includeString));

        g_object_unref(dep);
    }
}

void
model_project_dependency_link_string_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* representation = g_hash_table_lookup(table, "Representation");
    gint type = *((gint*)g_hash_table_lookup(table, "DepType"));
    ModelProjectDependency* dep = model_project_dependency_new(representation, type);

    GError* err = NULL;
    GString* linkString = model_project_dependency_get_links(dep, &err);

    gboolean shouldGenerateError = *((gboolean*)g_hash_table_lookup(table, "ShouldGenerateError"));
    if(shouldGenerateError)
    {
        gchar* expectedMsg = (gchar*)g_hash_table_lookup(table, "ErrorMsg");

        assert_null(linkString);
        assert_non_null(err);
        assert_true(g_strcmp0(expectedMsg, err->message));

        g_object_unref(dep);
    }
    else
    {
        assert_non_null(linkString);
        assert_null(err);

        GString* expected = g_hash_table_lookup(table, "LinkString");
        assert_true(g_string_equal(expected, linkString));

        g_object_unref(dep);
    }
}

void
model_project_dependency_equlity_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* representation1 = g_hash_table_lookup(table, "FirstRepresentation");
    gint type1 = *((gint*)g_hash_table_lookup(table, "FirstDepType"));
    ModelProjectDependency* dep1 = model_project_dependency_new(representation1, type1);

    GString* representation2 = g_hash_table_lookup(table, "SecondRepresentation");
    gint type2 = *((gint*)g_hash_table_lookup(table, "SecondDepType"));
    ModelProjectDependency* dep2 = model_project_dependency_new(representation2, type2);

    gboolean expected = *((gboolean*)g_hash_table_lookup(table, "IsEqual"));
    gboolean isEqual = model_project_dependency_equals(dep1, dep2);

    assert_true(expected == isEqual);

    g_object_unref(dep1);
    g_object_unref(dep2);
}