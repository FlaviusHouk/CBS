# Testing approaches

## Unit tests

Staring from CBS 0.0.3 there is an opportuinty to run unit tests. Every project have field where CBS project (path to it) with unit tests might be specified. 

Project with test should have three "public" (not static) functions:

* ```char** get_available_tests(void)``` <br/>
Returns NULL-terminated array of tests. For each tests shoulb be defined function with the same name. Test name is used to invoke it.
* ```void** get_states_for_test(void* key)``` <br/>
Return NULL-terminated array of test data sets. It will be passed to corresponding test. Key - is the unique identifier of test, for example name. 
* ```void clean_up(void)```<br/>
Used to free allocated resources.

Every test method should have signature <br/>
```void test_name (void** state)```. 

## Integration tests
TBD

## Manual testing.
If you find some bug please create an issue in the repo.