# C Build System

This is a tool for automation of build procedure for C projects. It aims to be lightweight and fast. With minimum of third party dependencies (mainly GLib) it tries to provide full support for different kinds of projects with a single XML-definition. It is inspired by MS dotnet projects and build tools. You can look at our [roadmap](https://github.com/FlaviusHouk/CBS/blob/master/Documentation/Roadmap.md) to find information about plans and future releases.

## Compiling

In case of compiling from scratch makefile should be edited and some setting for cbs executable should be provided. 
To link dynamic library rpath should be provided, so add following parameters in compiler call for Main build target(without quotes): "-Wl,-rpath=/path/to/folder/with/libcbs -L/path/to/folder/with/libcbs".
If you already have cbs binaries simple build command does all necessary work.

## Usage

For the 0.0.4 version there are such commands: 

### Create

Can be called by

```
cbs --create [-name] projName [[-templateName] [templateName]]
```

When project is created in a base folder, a few subfolders (src,include and scripts) are created. If base folder already contains any of such folders, they won't be overridden. All files in src folders automatically will be added to project as source files(with extension check). Project definition is stored in file called projName. It is XML file with UTF-8 encoding that might be edited in any text editor. templateName is a name of template to use for new project. It's optional parameter. There are four types of project. You can look at them [CBSTemplates repo](https://github.com/FlaviusHouk/CBSTempates). Also you can edit project definition manually according to [scheme](https://github.com/FlaviusHouk/CBS/blob/master/Documentation/Project.xcd).


### Add source files

```
cbs --addFile projLocation file1 file2 ... fileN
```

Relative and absolute paths are supported for either project location and source files. It is not necessary to hold all files in src folder. They just have to be accessible to read in build time. All files can be added to project, but only ones that have have .c extension is processed as file with C code. Non existing file cannot be added.

Currently there are four files types:

* Code ('*.c')
* Headers ('*.h')
* Markup ('*.gml')
* Resource (all others)

Header files might not be added to project. To use it you can specify just include folder. 

### Delete file

```
cbs --deleteFile projLocation file1 file2 ... fileN
```

Pay attention: when you deleting file from the project, it still stays in the folder.

### Add include folder

```
cbs --addInclude projLocation fold1 fold2 ... foldN
```

Works in the same way as add file command.

### Delete include folder

```
cbs --deleteInclude projLocation folder1 folder2 ...  folderN
```

Works in the same way as delete file command.

### Add dependency

```
cbs --addDependency [-projLoc] projLocation [-depType] depType [-depRepr] depName
```

There are four types of dependencies that are supported:

* Library installed in system (type 0)
    
    For this type depName is the library name that installed in the system. To find information about it pkg-config is used. So a valid name for it should be specified.

* Dynamic library (type 1)

    For this type depName is the path to dynamic library, including fileName.

* Static library (type 2)

    Same as for Dynamic library

* Other CBS project (type 3)

    Here depName is the path to another project definition. All these dependencies will be build before(after building object files) "current" project.

### Delete dependency

```
cbs --deleteDependency [-projLoc] projLocation [-depName] depName
```

Deletes the first dependency with the same depName that was found. 

### Build

```
cbs --build [-projLoc] projName [-publish] [-rebuild] [[-config] [buildConfigName]]
```

When build starts, two additional folders will be created if they do not exist: obj and bin. First one contains object file for each processed source file and the second one contains compiled binary file - build artifact. Source files will not be compiled if there is up to date object file. If rebuild flag is set, there will be no checks and every source file will be compiled.

While executing build procedure all compiler output and build commands for each file are printed in console you're using. All compile and link errors are printed as well.

gcc compiler is used for build. 

In this version there are two default build definitions: Debug and Release. Both will build elf executable with c11 language standard. Debug config uses -g option to produce debug info and Release uses -O1. Currently there is no opportunity to add build build definition with cli interface and you have to edit "BuildConfigurations" section in cpd file. You can edit cbs config file to change this parameters.

If publish flag is set rpath won't be included in binary. All dependencies, except for system one, will be copied (override if exists) to binary folder of project that building.

### Test

```
cbs --test projName
```

Initializes CMocka unit test engine, build unit test project (project specified in UnitTestLocation field of project definition), it's dependencies will run tests. Unit test library has to provide special interface to be used in unit testing with cbs. See more [Testing](https://github.com/FlaviusHouk/CBS/blob/master/Documentation/Testing.md).

### Version

```
cbs --version
```

Prints current version of build system.

### Help

```
cbs --help
```

Prints information about usage of the build system.

All commands support straightforvard and named parameters pass.