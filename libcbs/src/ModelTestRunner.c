#include "ModelTestRunner.h"

#include "stdio.h"
#include "stdarg.h"
#include "stddef.h"
#include "setjmp.h"
#include "stdint.h"

#include "cmocka.h"
#include "gmodule.h"

typedef char** (*TestDiscoverer)(void);
typedef void** (*StateGenerator)(void* key);
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

static struct CMUnitTest*
model_test_runner_init_test(const char* name, 
                            CMUnitTestFunction testFunc,
                            CMFixtureFunction setup_func,
                            CMFixtureFunction teardown_func,
                            void* state)
{
    struct CMUnitTest* test = g_malloc(sizeof(struct CMUnitTest));

    test->name = name;
    test->test_func = testFunc;
    test->setup_func = setup_func;
    test->teardown_func = teardown_func;
    test->initial_state = state;

    return test;
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
            StateGenerator generator;
            if(g_module_symbol(tests, "get_available_tests", (gpointer *)&discover) &&
               g_module_symbol(tests, "get_states_for_test", (gpointer*)&generator))
            {
                char** toExecute = discover();

                int testCount = g_strv_length(toExecute);
                GPtrArray* cMockaTests = g_ptr_array_new(); 

                test toRun;
                for(int i = 0; i<testCount; i++)
                {
                    if(g_module_symbol(tests, toExecute[i], (gpointer*)&toRun))
                    {
                        void** testCases = generator(toExecute[i]);
                        int testCaseCount = g_strv_length((gchar**)testCases); //a little bit hacky way to determinate length of generic array
                        
                        if(testCaseCount != 0)
                        {
                            for(int j = 0; j<testCaseCount; j++)
                            {
                                g_ptr_array_add(cMockaTests, 
                                                model_test_runner_init_test(toExecute[i], 
                                                                            toRun, 
                                                                            NULL, 
                                                                            NULL, 
                                                                            testCases[j]));
                            }
                        }
                        else
                        {
                            g_ptr_array_add(cMockaTests, 
                                            model_test_runner_init_test(toExecute[i], 
                                                                        toRun,
                                                                        NULL,
                                                                        NULL,
                                                                        NULL));
                        }
                    }
                }

                struct CMUnitTest* testArray = g_malloc(sizeof(struct CMUnitTest) * cMockaTests->len);
                for(int i = 0; i<cMockaTests->len; i++)
                    testArray[i] = *((struct CMUnitTest*)g_ptr_array_index(cMockaTests, i));

                _cmocka_run_group_tests("Tests", 
                                        testArray, 
                                        cMockaTests->len, 
                                        NULL, 
                                        NULL);
            }
        }

        g_module_close(tests);
    }
}