#include "ModelProjectTests.h"

void
model_project_creation_test_positive_case(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* location = g_hash_table_lookup(table, "InitialLocation");
    ModelProject* proj = model_project_new(location);

    assert_non_null(proj);
    assert_non_null(model_project_get_location(proj));

    assert_null(model_project_get_active_build_config(proj));
    assert_null(model_project_get_unit_tests_project_location(proj));

    assert_non_null(model_project_get_source_files(proj));
    assert_non_null(model_project_get_includes(proj));
    assert_non_null(model_project_get_dependencies(proj));

    g_object_unref(proj);
}

void
model_project_location_property_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    const gchar* initialLocation = g_hash_table_lookup(table, "Location");

    ModelProject* proj = model_project_new(g_string_new(g_strdup(initialLocation)));

    GString* location = model_project_get_location(proj);

    assert_non_null(location);
    assert_true(strcmp(location->str, initialLocation) == 0);

    g_object_unref(proj);
}

void 
model_project_add_source_file_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    gchar* location = g_hash_table_lookup(table, "FileLocation");
    gboolean fileExists = *((gboolean*)g_hash_table_lookup(table, "IsFileExists"));

    ModelSourceFile* file = model_source_file_new(g_string_new(g_strdup(location)));
    ModelSourceFile* toRem = model_source_file_new(g_string_new(g_strdup(location)));

    GString* projectLocation = g_hash_table_lookup(table, "ProjLoc");
    ModelProject* proj = model_project_new(projectLocation);

    GPtrArray* sourceFiles = model_project_get_source_files(proj);
    
    assert_int_equal(sourceFiles->len, 0);

    assert_true(model_project_add_source_file(proj, file) == fileExists);

    if(fileExists)
        assert_int_equal(sourceFiles->len, 1);

    model_project_remove_source_file(proj, toRem);
    
    assert_int_equal(sourceFiles->len, 0);

    g_object_unref(proj);
}

void
model_project_dependencies_manipulation_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* projLoc = (GString*)g_hash_table_lookup(table, "ProjLoc");
    ModelProject* proj = model_project_new(projLoc);

    GPtrArray* dependencies = model_project_get_dependencies(proj);
    
    assert_int_equal(dependencies->len, 0);

    GString* repr1 = (GString*)g_hash_table_lookup(table, "FirstDep");
    ModelProjectDependency* dep = 
            model_project_dependency_new(repr1, SYSTEM_DEP);

    GString* repr2 = (GString*)g_hash_table_lookup(table, "SecondDep");
    ModelProjectDependency* dep2 = 
            model_project_dependency_new(repr2, STATIC_LIB);

    GString* repr3 = (GString*)g_hash_table_lookup(table, "ThirdDep");
    ModelProjectDependency* dep3 = 
            model_project_dependency_new(repr3, DYNAMIC_LIB);                        

    GString* repr4 = (GString*)g_hash_table_lookup(table, "FourthDep");
    ModelProjectDependency* dep4 =
            model_project_dependency_new(repr4, CBS_PROJECT);    

    model_project_add_dependency(proj, dep);
    model_project_add_dependency(proj, dep2);
    model_project_add_dependency(proj, dep3);
    model_project_add_dependency(proj, dep4);
    model_project_add_dependency(proj, dep);

    assert_int_equal(dependencies->len, 4);

    model_project_remove_dependency(proj, dep2);
    model_project_remove_dependency(proj, dep3);
    model_project_remove_dependency(proj, dep2);

    assert_int_equal(dependencies->len, 2);

    model_project_remove_dependency_by_name(proj, repr1);

    assert_int_equal(dependencies->len, 1);

    g_object_unref(proj);
}

void
model_project_headers_manipulation_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    ModelProject* proj = model_project_new(g_string_new(g_strdup("")));
    GPtrArray* headers = model_project_get_includes(proj);
    
    assert_int_equal(headers->len, 0);

    GString* fold = g_string_new(g_strdup(g_hash_table_lookup(table, "FirstHeaderLocation")));
    GString* fold2 = g_string_new(g_strdup(g_hash_table_lookup(table, "SecondHeaderLocation")));
    GString* fold3 = g_string_new(g_strdup(g_hash_table_lookup(table, "ThirdHeaderLocation")));
    GString* fold4 = g_string_new(g_strdup(g_hash_table_lookup(table, "FourthHeaderLocation")));  

    model_project_add_include_folder(proj, fold);
    model_project_add_include_folder(proj, fold2);
    model_project_add_include_folder(proj, fold3);
    model_project_add_include_folder(proj, fold4);
    model_project_add_include_folder(proj, fold);

    int expectedCount = *((int*)g_hash_table_lookup(table, "ExistingCount"));

    assert_int_equal(headers->len, expectedCount);

    model_project_remove_include_folder(proj, fold);
    model_project_remove_include_folder(proj, fold2);
    model_project_remove_include_folder(proj, fold3);
    model_project_remove_include_folder(proj, fold4);
    model_project_remove_include_folder(proj, fold3);

    assert_int_equal(headers->len, 0);

    g_object_unref(proj);
}

void
model_project_configs_manipulation_test(void** state)
{
    ModelProject* proj = model_project_new(g_string_new(g_strdup("")));

    ModelProjectConfiguration* conf =
        model_project_get_build_config(proj, g_string_new(g_strdup("Debug")));

    assert_non_null(conf);

    conf = model_project_get_build_config(proj, g_string_new(g_strdup("Release")));

    assert_non_null(conf);

    model_project_set_active_build_config(proj, g_string_new(g_strdup("Release")));

    conf = model_project_get_build_config(proj, NULL);

    assert_non_null(conf);

    g_object_unref(proj);
}

void
model_project_resolve_path_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* projectPath = (GString*)g_hash_table_lookup(table, "ProjLoc");
    GString* relPath = (GString*)g_hash_table_lookup(table, "RelLoc");
    GString* expectedPath = (GString*)g_hash_table_lookup(table, "ExpLoc");

    ModelProject* proj = model_project_new(projectPath);

    GString* path = model_project_resolve_path(proj, relPath);

    assert_true(g_string_equal(expectedPath, path));

    g_object_unref(proj);
}