#include "ModelSourceFileTests.h"

void
model_source_file_creation_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* fileLoc = (GString*)g_hash_table_lookup(table, "FileLoc");
    ModelSourceFile* file = model_source_file_new(fileLoc);

    assert_non_null(file);

    GString* loc = model_source_file_get_path(file);

    assert_true(g_string_equal(fileLoc, loc));

    g_object_unref(file);
}

void
model_source_file_type_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* fileLoc = (GString*)g_hash_table_lookup(table, "FileLoc");
    ModelSourceFile* file = model_source_file_new(fileLoc);

    gint expectedType = *((gint*)g_hash_table_lookup(table, "FileType"));
    gint type = model_source_file_get_file_type(file);

    assert_int_equal(expectedType, type);

    g_object_unref(file);
}

void
model_source_file_equility_test(void** state)
{
    GHashTable* table = (GHashTable*)(*state);

    GString* fileLoc1 = (GString*)g_hash_table_lookup(table, "FirstFileLoc");
    ModelSourceFile* file1 = model_source_file_new(fileLoc1);

    GString* fileLoc2 = (GString*)g_hash_table_lookup(table, "SecondFileLoc");
    ModelSourceFile* file2 = model_source_file_new(fileLoc2);

    gboolean isEqual = *((gboolean*)g_hash_table_lookup(table, "IsEqual"));

    gboolean given = model_source_file_equals(file1, file2);

    assert_true(isEqual == given);

    g_object_unref(file1);
    g_object_unref(file2);
}