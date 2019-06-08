## 0.0.3
* Moving to sefl-building. From version 0.0.3 CBS can build itself. Makefiles is preserved to
make possible build CBS from scratch;
* Provided project templates, that might be used to created new project. 
* CMocka unit test framework is integrated into build system. It is possible to create and run
tests for projects;
* All Model types is now supporting GObject properties. So it is possible to write tools or
GUI using these types;
* Command line interface is now supporting optinal parameters. Named parameters are also supported;
* Project definition now contains ProjectData area where some project data might be stored (Key-Value
pairs). They used to generate config.h file with macro definitions to use in code.
* Small bug fixes and improvements.

## 0.0.2
* Build config type provided. Now it is possible to use such build parameters
as C Standard, Optimization level and so on. Also provided default build config for project 
and some predefined configs for system;
* Provided opportunity to run pre and post build scripts;
* CBS dependencies are building when main project is building;
* All cli commands have messeges with execution status;
* Project structure was imroved. There is no backward compatibility bitween projects created
with 0.0.1 version and 0.0.2 version. To use old projects some changes in XML structure should be
performed;
* Small bug fixed and improvements.