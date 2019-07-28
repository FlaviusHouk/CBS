## 0.0.4
* Added new options for build command: publish, which build binaries with further distributing (via tarball or with some package) in mind and rebuild option which forces project to rebuild even if all files are up to date;
* Object files and output binaries are checked for update. If there was no changes in source files, object file won't be compiled again;
* Added version field for project definition to make backward compatibility available;
* Error in external tools are now handled and it might be a reason to stop build or other process that requires invoking external tools;
* Provided config file for cbs. According to Freedesktop recomendation it is located in ~/config/cbs folder. It contains default build definitions (currently Debug and Release);
* Bug fixes and refactoring (look the page with this version).

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