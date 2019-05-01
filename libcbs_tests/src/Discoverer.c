#include "Discoverer.h"

GHashTable* table;

static void**
init_model_project_creation_test_positive_case_data()
{
    int numberOfTests = 1;
    void** testCases = g_malloc(sizeof(void*) * (numberOfTests + 1));

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(defaultTestCase, "InitialLocation", g_string_new(g_strdup("")));

    testCases[0] = (void*)defaultTestCase;
    
    testCases[numberOfTests] = NULL;

    return testCases;    
}

static void**
init_model_project_location_property_test_data()
{
    int numberOfTests = 1;
    void** testCases = g_malloc(sizeof(void*) * (numberOfTests + 1));

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(defaultTestCase, "Location", g_strdup("some/location"));

    testCases[0] = (void*)defaultTestCase;
    
    testCases[numberOfTests] = NULL;

    return testCases;    
}

static void**
init_model_project_add_source_file_test_data()
{
    int numberOfTests = 2;
    void** testCases = g_malloc(sizeof(void*) * (numberOfTests + 1));

    //file that not exists
    GHashTable* fileNotExistsTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(fileNotExistsTestCase, "FileLocation", g_strdup("some/location"));

    gboolean* isFileExists = g_malloc(sizeof(gboolean));
    *isFileExists = FALSE;
    g_hash_table_insert(fileNotExistsTestCase, "IsFileExists", isFileExists);

    g_hash_table_insert(fileNotExistsTestCase, "ProjLoc", g_string_new(g_strdup("proj/loc")));

    testCases[0] = (void*)fileNotExistsTestCase;
    
    //existing file
    GHashTable* fileExistsTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(fileExistsTestCase, "FileLocation", g_strdup("/usr/bin/["));

    isFileExists = g_malloc(sizeof(gboolean));
    *isFileExists = TRUE;
    g_hash_table_insert(fileExistsTestCase, "IsFileExists", isFileExists);

    g_hash_table_insert(fileExistsTestCase, "ProjLoc", g_string_new(g_strdup("proj/loc")));

    testCases[1] = (void*)fileExistsTestCase;

    testCases[numberOfTests] = NULL;

    return testCases;   
}

static void**
init_model_project_dependencies_manipulation_test_data()
{
    int numberOfTests = 1;
    void** testCases = g_malloc(sizeof(void*) * (numberOfTests + 1));

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(defaultTestCase, "ProjLoc", g_string_new(g_strdup("some/location")));
    g_hash_table_insert(defaultTestCase, "FirstDep", g_string_new(g_strdup("SysLib")));
    g_hash_table_insert(defaultTestCase, "SecondDep", g_string_new(g_strdup("some/static/lib")));
    g_hash_table_insert(defaultTestCase, "ThirdDep", g_string_new(g_strdup("libBestLib.so")));
    g_hash_table_insert(defaultTestCase, "FourthDep", g_string_new(g_strdup("../cbsProj/proj.cpd")));

    testCases[0] = (void*)defaultTestCase;
    
    testCases[numberOfTests] = NULL;

    return testCases;   
}

static void**
init_model_project_headers_manipulation_test_data()
{
    int numberOfTests = 1;
    void** testCases = g_malloc(sizeof(void*) * (numberOfTests + 1));

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(defaultTestCase, "FirstHeaderLocation", "/var");
    g_hash_table_insert(defaultTestCase, "SecondHeaderLocation", "file://someFile");
    g_hash_table_insert(defaultTestCase, "ThirdHeaderLocation", "/usr/lib");
    g_hash_table_insert(defaultTestCase, "FourthHeaderLocation", "/usr/include");

    gint* count = g_malloc(sizeof(gint));
    *count = 3;

    g_hash_table_insert(defaultTestCase, "ExistingCount", count);

    testCases[0] = (void*)defaultTestCase;
    
    testCases[numberOfTests] = NULL;

    return testCases;
}

static void**
init_model_project_configs_manipulation_test_data()
{
   
}

static void**
init_model_project_resolve_path_test_data()
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* sameFolderTest = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(sameFolderTest, "ProjLoc", g_string_new(g_strdup("proj.cpd")));
    g_hash_table_insert(sameFolderTest, "RelLoc", g_string_new(g_strdup("../lib/proj2.cpd")));
    g_hash_table_insert(sameFolderTest, "ExpLoc", g_string_new(g_strdup("../lib/proj2.cpd")));

    g_ptr_array_add(testCases, sameFolderTest);

    GHashTable* upperFolderTest = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(upperFolderTest, "ProjLoc", g_string_new(g_strdup("exec/proj.cpd")));
    g_hash_table_insert(upperFolderTest, "RelLoc", g_string_new(g_strdup("../lib/proj2.cpd")));
    g_hash_table_insert(upperFolderTest, "ExpLoc", g_string_new(g_strdup("lib/proj2.cpd")));

    g_ptr_array_add(testCases, upperFolderTest);

    GHashTable* innerFolderTest = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(innerFolderTest, "ProjLoc", g_string_new(g_strdup("../../proj.cpd")));
    g_hash_table_insert(innerFolderTest, "RelLoc", g_string_new(g_strdup("../lib/proj2.cpd")));
    g_hash_table_insert(innerFolderTest, "ExpLoc", g_string_new(g_strdup("../../../lib/proj2.cpd")));

    g_ptr_array_add(testCases, innerFolderTest);

    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

char** 
get_available_tests(void)
{
    table = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(table,
                        g_strdup("model_project_creation_test_positive_case"), 
                        init_model_project_creation_test_positive_case_data());

    g_hash_table_insert(table,
                        g_strdup("model_project_location_property_test"), 
                        init_model_project_location_property_test_data());
  
    g_hash_table_insert(table,
                        g_strdup("model_project_add_source_file_test"), 
                        init_model_project_add_source_file_test_data());

    g_hash_table_insert(table,
                        g_strdup("model_project_dependencies_manipulation_test"), 
                        init_model_project_dependencies_manipulation_test_data());

    g_hash_table_insert(table,
                        g_strdup("model_project_headers_manipulation_test"), 
                        init_model_project_headers_manipulation_test_data());

    g_hash_table_insert(table,
                        g_strdup("model_project_configs_manipulation_test"), 
                        init_model_project_configs_manipulation_test_data());

    g_hash_table_insert(table,
                        g_strdup("model_project_resolve_path_test"), 
                        init_model_project_resolve_path_test_data());

    guint size = g_hash_table_size(table);

    GPtrArray* toRet = g_ptr_array_new();
    char** keys = (char**)g_hash_table_get_keys_as_array(table, &size);
    for(int i = 0; i<size; i++)
        g_ptr_array_add(toRet, keys[i]);

    g_ptr_array_add(toRet, NULL);

    return (char**)toRet->pdata;
}

void**
get_states_for_test(void* key)
{
    return g_hash_table_lookup(table, key);
}