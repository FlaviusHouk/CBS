# C Build System

This is a tool for automatization of build procedure for C projects. It aims to be lightweight and fast. With the minimum of third party dependencies it tries to provide full support for different kinds of projects with a single XML-definition. It is inspired by dotnet projects and build tools.

## Compiling

To compile current version of build system its bettter to use build script build.sh. It is delete previously compiled binaries and uses make for build. In future it will be replaced by CBS.

## Usage

For the 0.0.1 version there is a few supported basic commands. 

### Create

Can be called by

```
cbs --create projName
```

When project folder initialized a few folders (src, include and scripts) are created. If folder already containing such folder, they will not be overriden. All files in src folders automatically will be added to project as source files. Project definition is stored in file called projName. It is XML file with UTF-8 encoding that might be edited in any text editor.


### Add source files

```
cbs --addFile projLocation file1 file2 ... fileN
```

Relative and absolute pathes are supproted for either project location and source files. It is not necessary to hold all files in src folder. They just have to be accessible to read in build time. All files can be added to project, but only ones that have have .c extension is processed as file with C code. Non existing file cannot be added.

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

Currently only single file deletion is supprorted. Pay attention: when you deletinig file from the project, it still stays in the folder.

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
* Dynamic library (type 1)
* Static library (type 2)
* Other CBS project (type 3)

