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

#include "Discoverer.h"

#include "ModelProjectConfiguration.h"
#include "ModelProjectDependency.h"
#include "ModelSourceFile.h"

#include "unistd.h"

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

static void
initialize_model_project_tests(void)
{
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
}

static void**
init_model_project_configuration_creation_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* configName = g_string_new(g_strdup("CustomConfig"));
    g_hash_table_insert(defaultTestCase, "ConfigName", configName);

    gboolean* isCorrect = g_malloc(sizeof(gboolean));
    *isCorrect = TRUE;
    g_hash_table_insert(defaultTestCase, "IsCorrectInputParameters", isCorrect);

    g_ptr_array_add(testCases, defaultTestCase);

    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_project_configuration_equility_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* equalTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* firstConfigName = g_string_new(g_strdup("Debug"));
    g_hash_table_insert(equalTestCase, g_strdup("FirstConfigName"), firstConfigName);

    GString* secondConfigName = g_string_new(g_strdup("Debug"));
    g_hash_table_insert(equalTestCase, g_strdup("SecondConfigName"), secondConfigName);

    gboolean* isEqual = g_malloc(sizeof(gboolean));
    *isEqual = TRUE;
    g_hash_table_insert(equalTestCase, g_strdup("AreEqual"), isEqual);

    GHashTable* notEqualTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    firstConfigName = g_string_new(g_strdup("Debug"));
    g_hash_table_insert(notEqualTestCase, g_strdup("FirstConfigName"), firstConfigName);

    secondConfigName = g_string_new(g_strdup("Release"));
    g_hash_table_insert(notEqualTestCase, g_strdup("SecondConfigName"), secondConfigName);

    isEqual = g_malloc(sizeof(gboolean));
    *isEqual = FALSE;
    g_hash_table_insert(notEqualTestCase, g_strdup("AreEqual"), isEqual);

    g_ptr_array_add(testCases, equalTestCase);
    g_ptr_array_add(testCases, notEqualTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_project_configuration_config_string_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* complexTest1 = g_hash_table_new(g_str_hash, g_str_equal);

    GString* configName = g_string_new(g_strdup("TestConfig"));
    g_hash_table_insert(complexTest1, g_strdup("ConfigName"), configName);

    GString* configString = g_string_new(g_strdup(" -std=c89 -fpic -O0 "));
    g_hash_table_insert(complexTest1, g_strdup("ExpectedRezult"), configString);

    gint* cstd = (gint*)g_malloc(sizeof(gint));
    *cstd = C89;
    g_hash_table_insert(complexTest1, g_strdup("CStandard"), cstd); 

    gboolean* ignoreOptions = (gboolean*)g_malloc(sizeof(gboolean));
    *ignoreOptions = FALSE;
    g_hash_table_insert(complexTest1, g_strdup("IgnoreOptions"), ignoreOptions);

    gint* optimization = (gint*)g_malloc(sizeof(gint));
    *optimization = RELEASE_0;
    g_hash_table_insert(complexTest1, g_strdup("OptimizationLevel"), optimization); 

    GString* outputName = g_string_new(g_strdup("OldLib.so"));
    g_hash_table_insert(complexTest1, g_strdup("OutputName"), outputName);

    gint* outputType = (gint*)g_malloc(sizeof(gint));
    *outputType = DYNAMIC_LIB;
    g_hash_table_insert(complexTest1, g_strdup("OutputType"), outputType); 

    GHashTable* ignoreOptionsTest = g_hash_table_new(g_str_hash, g_str_equal);

    configName = g_string_new(g_strdup("TestConfig"));
    g_hash_table_insert(ignoreOptionsTest, g_strdup("ConfigName"), configName);

    configString = g_string_new(g_strdup(""));
    g_hash_table_insert(ignoreOptionsTest, g_strdup("ExpectedRezult"), configString);

    cstd = (gint*)g_malloc(sizeof(gint));
    *cstd = C89;
    g_hash_table_insert(ignoreOptionsTest, g_strdup("CStandard"), cstd); 

    ignoreOptions = (gboolean*)g_malloc(sizeof(gboolean));
    *ignoreOptions = TRUE;
    g_hash_table_insert(ignoreOptionsTest, g_strdup("IgnoreOptions"), ignoreOptions);

    optimization = (gint*)g_malloc(sizeof(gint));
    *optimization = RELEASE_0;
    g_hash_table_insert(ignoreOptionsTest, g_strdup("OptimizationLevel"), optimization); 

    outputName = g_string_new(g_strdup("OldLib.so"));
    g_hash_table_insert(ignoreOptionsTest, g_strdup("OutputName"), outputName);

    outputType = (gint*)g_malloc(sizeof(gint));
    *outputType = DYNAMIC_LIB;
    g_hash_table_insert(ignoreOptionsTest, g_strdup("OutputType"), outputType); 

    g_ptr_array_add(testCases, ignoreOptionsTest);
    g_ptr_array_add(testCases, complexTest1);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void
initialize_model_project_configuration_tests(void)
{
    g_hash_table_insert(table,
                        g_strdup("model_project_configuration_creation_test"),
                        init_model_project_configuration_creation_test());

    g_hash_table_insert(table,
                        g_strdup("model_project_configuration_equility_test"),
                        init_model_project_configuration_equility_test());

    g_hash_table_insert(table,
                        g_strdup("model_project_configuration_config_string_test"),
                        init_model_project_configuration_config_string_test());
}

static void**
init_model_project_dependency_creation_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(defaultTestCase, "Representation", representation);

    gint* type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(defaultTestCase, "DepType", type);

    g_ptr_array_add(testCases, defaultTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_project_dependency_include_string_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* systemLibTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(systemLibTestCase, "Representation", representation);

    gint* type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(systemLibTestCase, "DepType", type);

    gboolean* shouldGenerateError = (gboolean*)g_malloc(sizeof(gboolean));
    *shouldGenerateError = FALSE;
    g_hash_table_insert(systemLibTestCase, "ShouldGenerateError", shouldGenerateError);

    gchar* errorMsg = g_strdup("");
    g_hash_table_insert(systemLibTestCase, "ErrorMsg", errorMsg);

    GString* includeString = g_string_new(g_strdup("-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include \n"));
    g_hash_table_insert(systemLibTestCase, "IncludeString", includeString);

    GHashTable* missingLibTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    representation = g_string_new(g_strdup("proj.cpd"));
    g_hash_table_insert(missingLibTestCase, "Representation", representation);

    type = (gint*)g_malloc(sizeof(gint));
    *type = CBS_PROJECT;
    g_hash_table_insert(missingLibTestCase, "DepType", type);

    shouldGenerateError = (gboolean*)g_malloc(sizeof(gboolean));
    *shouldGenerateError = TRUE;
    g_hash_table_insert(missingLibTestCase, "ShouldGenerateError", shouldGenerateError);

    errorMsg = g_strdup("Cannot locate project proj.cpd.\n");
    g_hash_table_insert(missingLibTestCase, "ErrorMsg", errorMsg);

    includeString = g_string_new(g_strdup(""));
    g_hash_table_insert(missingLibTestCase, "IncludeString", includeString);

    g_ptr_array_add(testCases, missingLibTestCase);
    g_ptr_array_add(testCases, systemLibTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_project_dependency_link_string_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* systemLibTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(systemLibTestCase, "Representation", representation);

    gint* type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(systemLibTestCase, "DepType", type);

    gboolean* isPublishing = (gboolean*)g_malloc(sizeof(gboolean));
    *isPublishing = FALSE;
    g_hash_table_insert(systemLibTestCase, "IsPublishing", isPublishing);

    gboolean* shouldGenerateError = (gboolean*)g_malloc(sizeof(gboolean));
    *shouldGenerateError = FALSE;
    g_hash_table_insert(systemLibTestCase, "ShouldGenerateError", shouldGenerateError);

    GString* errorMsg = g_string_new(g_strdup(""));
    g_hash_table_insert(systemLibTestCase, "ErrorMsg", errorMsg);

    GString* linkString = g_string_new(g_strdup("-lglib-2.0 \n"));
    g_hash_table_insert(systemLibTestCase, "LinkString", linkString);

    GHashTable* dynamicLibTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    representation = g_string_new(g_strdup("someLib.so"));
    g_hash_table_insert(dynamicLibTestCase, "Representation", representation);

    type = (gint*)g_malloc(sizeof(gint));
    *type = EXTERNAL_DYNAMIC_LIB;
    g_hash_table_insert(dynamicLibTestCase, "DepType", type);

    isPublishing = (gboolean*)g_malloc(sizeof(gboolean));
    *isPublishing = FALSE;
    g_hash_table_insert(dynamicLibTestCase, "IsPublishing", isPublishing);

    shouldGenerateError = (gboolean*)g_malloc(sizeof(gboolean));
    *shouldGenerateError = FALSE;
    g_hash_table_insert(dynamicLibTestCase, "ShouldGenerateError", shouldGenerateError);

    errorMsg = g_string_new(g_strdup(""));
    g_hash_table_insert(dynamicLibTestCase, "ErrorMsg", errorMsg);

    gchar* path = (gchar*)malloc(sizeof(gchar)*1024);
    linkString = g_string_new(g_strdup("-L. -lsomeLib.so -Wl,-rpath="));
    getcwd(path, 1024);
    g_string_append_printf(linkString, "%s  ", path);

    g_hash_table_insert(dynamicLibTestCase, "LinkString", linkString);

    g_ptr_array_add(testCases, dynamicLibTestCase);
    g_ptr_array_add(testCases, systemLibTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_project_dependency_equlity_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* equalTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(equalTestCase, "FirstRepresentation", representation);

    gint* type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(equalTestCase, "FirstDepType", type);

    gboolean* isEqual = (gboolean*)g_malloc(sizeof(gboolean));
    *isEqual = TRUE;
    g_hash_table_insert(equalTestCase, "IsEqual", isEqual);

    representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(equalTestCase, "SecondRepresentation", representation);

    type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(equalTestCase, "SecondDepType", type);

    GHashTable* notEqualTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    representation = g_string_new(g_strdup("glib-2.0"));
    g_hash_table_insert(notEqualTestCase, "FirstRepresentation", representation);

    type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(notEqualTestCase, "FirstDepType", type);

    isEqual = (gboolean*)g_malloc(sizeof(gboolean));
    *isEqual = FALSE;
    g_hash_table_insert(notEqualTestCase, "IsEqual", isEqual);

    representation = g_string_new(g_strdup("gtk+-2.0"));
    g_hash_table_insert(notEqualTestCase, "SecondRepresentation", representation);

    type = (gint*)g_malloc(sizeof(gint));
    *type = SYSTEM_DEP;
    g_hash_table_insert(notEqualTestCase, "SecondDepType", type);

    g_ptr_array_add(testCases, notEqualTestCase);
    g_ptr_array_add(testCases, equalTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void
initialize_model_project_dependency_tests(void)
{
    g_hash_table_insert(table,
                        g_strdup("model_project_dependency_creation_test"),
                        init_model_project_dependency_creation_test());

    g_hash_table_insert(table,
                        g_strdup("model_project_dependency_include_string_test"),
                        init_model_project_dependency_include_string_test());

    g_hash_table_insert(table,
                        g_strdup("model_project_dependency_link_string_test"),
                        init_model_project_dependency_link_string_test());
    
    g_hash_table_insert(table,
                        g_strdup("model_project_dependency_equlity_test"),
                        init_model_project_dependency_equlity_test());
}

static void**
init_model_source_file_creation_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* location = g_string_new(g_strdup("file.c"));
    g_hash_table_insert(defaultTestCase, "FileLoc", location);

    g_ptr_array_add(testCases, defaultTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_source_file_type_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();

    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* location = g_string_new(g_strdup("file.c"));
    g_hash_table_insert(defaultTestCase, "FileLoc", location);

    gint* type = (gint*)g_malloc(sizeof(gint));
    *type = CODE;
    g_hash_table_insert(defaultTestCase, "FileType", type);

    GHashTable* resourceTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    location = g_string_new(g_strdup("file.res"));
    g_hash_table_insert(resourceTestCase, "FileLoc", location);

    type = (gint*)g_malloc(sizeof(gint));
    *type = RESOURCE;
    g_hash_table_insert(resourceTestCase, "FileType", type);

    g_ptr_array_add(testCases, resourceTestCase);
    g_ptr_array_add(testCases, defaultTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void**
init_model_source_file_equility_test(void)
{
    GPtrArray* testCases = g_ptr_array_new();
    
    GHashTable* defaultTestCase = g_hash_table_new(g_str_hash, g_str_equal);

    GString* file = g_string_new(g_strdup("main.c"));
    g_hash_table_insert(defaultTestCase, "FirstFileLoc", file);

    file = g_string_new(g_strdup("main.c"));
    g_hash_table_insert(defaultTestCase, "SecondFileLoc", file);

    gboolean* isEqual = (gboolean*)g_malloc(sizeof(gboolean));
    *isEqual = TRUE;
    g_hash_table_insert(defaultTestCase, "IsEqual", isEqual);

    g_ptr_array_add(testCases, defaultTestCase);
    g_ptr_array_add(testCases, NULL);

    return g_ptr_array_free(testCases, FALSE);
}

static void
initialize_model_source_file_tests(void)
{
    g_hash_table_insert(table,
                        g_strdup("model_source_file_creation_test"),
                        init_model_source_file_creation_test());

    g_hash_table_insert(table,
                        g_strdup("model_source_file_type_test"),
                        init_model_source_file_type_test());            
                        
    g_hash_table_insert(table,
                        g_strdup("model_source_file_equility_test"),
                        init_model_source_file_equility_test());                                    
}

char** 
get_available_tests(void)
{
    table = g_hash_table_new(g_str_hash, g_str_equal);

    initialize_model_project_tests();
    initialize_model_project_configuration_tests();
    initialize_model_project_dependency_tests();
    initialize_model_source_file_tests();

    guint size = g_hash_table_size(table);

    GPtrArray* toRet = g_ptr_array_new();
    char** keys = (char**)g_hash_table_get_keys_as_array(table, &size);
    for(int i = 0; i<size; i++)
        g_ptr_array_add(toRet, keys[i]);

    g_ptr_array_add(toRet, NULL);

    return (char**)toRet->pdata;
}

void
clean_up()
{
    g_hash_table_destroy(table);
}

void**
get_states_for_test(void* key)
{
    return g_hash_table_lookup(table, key);
}