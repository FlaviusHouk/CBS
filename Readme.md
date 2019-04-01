# C Build System

This is a tool for automation of build procedure for C projects. It aims to be lightweight and fast. With the minimum of third party dependencies it tries to provide full support for different kinds of projects with a single XML-definition. It is inspired by MS dotnet projects and build tools.

## Compiling

Before compiling makefile should be edited and some setting for cbs executable should be provided. 

To compile current version of build system its better to use build script build.sh. It deletes previously compiled binaries and uses automake for build. In future it will be replaced by CBS.

## Usage

For the 0.0.2 version there is a few supported basic commands. 

### Create

Can be called by

```
cbs --create projName
```

When project folder initialized a few folders (src, include and scripts) are created. If folder already containing such folder, they will not be overridden. All files in src folders automatically will be added to project as source files. Project definition is stored in file called projName. It is XML file with UTF-8 encoding that might be edited in any text editor.


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
cbs --deleteFile projLocation file
```

Currently only single file deletion is supported. Pay attention: when you deleting file from the project, it still stays in the folder.

### Add include folder

```
cbs --addInclude projLocation fold1 fold2 ... foldN
```

Works in the same way as add file command.

### Delete include folder

```
cbs --deleteInclude projLocation folder
```

Works in the same way as delete file command.

### Add dependency

```
cbs --addDependency projLocation depType depName
```

There is four types of dependencies that are supported:

* Library installed in system (type 0)
    
    For this type depName is the library name that installed in the system. To find information about it pkg-config is used. So a valid name for it should be specified.

* Dynamic library (type 1)

    For this type depName is the path to dynamic library, including fileName.

* Static library (type 2)

    Same as for Dynamic library

* Other CBS project (type 3)

    Here depName is the path to another project definition. All these dependencies will be build before "current" project.

### Delete dependency

```
cbs --deleteDependency projLocation depName
```

Deletes the first dependency with the same depName that was found. 

### Build

```
cbs --build projName buildConfigName
```

When build is started two additional folders will be created if they does not exist: obj and bin. First one contains object file for each processed source file and the second one contains compiled binary file - build artifact. 

While build is executed all compiler output and build commands for each file are printed in console you're using. All compile and link errors are printed as well.

By default gcc compiler is used. 

In this version there are two default build definitions: Debug and Release. Both will build elf executable with c11 language standard. Debug config uses -g option to produce debug info and Release uses -O1. Currently there is no opportunity to add build build definition with cli interface and you have to edit "BuildConfigurations" section in cpd file.