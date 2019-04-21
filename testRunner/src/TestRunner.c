#include "TestRunner.h"

#include "stdio.h"
#include "stdarg.h"
#include "stddef.h"
#include "setjmp.h"
#include "stdint.h"

#include "cmocka.h"

#include "gmodule.h"

typedef char** (*TestDiscoverer)(void);
typedef void (*test)(void** state);

gint
test_runner_execute_tests(GString* fileLoc)
{
    gboolean isSupported = g_module_supported();

    if(isSupported)
    {
        GModule* tests = g_module_open(fileLoc->str, G_MODULE_BIND_LAZY);

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