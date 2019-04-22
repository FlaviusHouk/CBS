#include "ModelTestRunner.h"

#include "stdio.h"
#include "stdarg.h"
#include "stddef.h"
#include "setjmp.h"
#include "stdint.h"

#include "cmocka.h"
#include "gmodule.h"

typedef char** (*TestDiscoverer)(void);
typedef void (*test)(void** state);

typedef struct _ModelTestRunner
{
    GObject parent_object;


} ModelTestRunner;

G_DEFINE_TYPE(ModelTestRunner, model_test_runner, G_TYPE_OBJECT);

static void
model_test_runner_dispose(GObject* obj)
{
    G_OBJECT_CLASS(model_test_runner_parent_class)->dispose(obj);
}

static void
model_test_runner_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_test_runner_parent_class)->finalize(obj);
}

static void
model_test_runner_class_init(ModelTestRunnerClass* class)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(class);

    objectClass->dispose = model_test_runner_dispose;
    objectClass->finalize = model_test_runner_finalize;
}

static void
model_test_runner_init(ModelTestRunner* this)
{

}

ModelTestRunner*
model_test_runner_new(void)
{
    ModelTestRunner* this = g_object_new(MODEL_TYPE_TEST_RUNNER, NULL);
    
    return this; 
}

gint
model_test_runner_execute_tests(ModelTestRunner* this, GString* loc)
{
    g_assert(this);

    gboolean isSupported = g_module_supported();

    if(isSupported)
    {
        GModule* tests = g_module_open(loc->str, G_MODULE_BIND_LAZY);

        if(tests)
        {
            TestDiscoverer discover;
            if(g_module_symbol(tests, "get_available_tests", (gpointer *)&discover))
            {
                char** toExecute = discover();

                int testCount = g_strv_length(toExecute);
                struct CMUnitTest* cMockaTests = malloc(sizeof(struct CMUnitTest) * testCount); 

                test toRun;
                for(int i = 0; i<testCount; i++)
                {
                    if(g_module_symbol(tests, toExecute[i], (gpointer*)&toRun))
                    {
                        struct CMUnitTest unitTest = { toExecute[i], toRun, NULL, NULL, NULL };
                        cMockaTests[i] = unitTest;
                    }
                }

                _cmocka_run_group_tests("Tests", cMockaTests, testCount, NULL, NULL);
            }
        }

        g_module_close(tests);
    }
}