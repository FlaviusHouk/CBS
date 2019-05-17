LIB_INCLUDE = $(shell pkg-config --cflags glib-2.0 gmodule-2.0 gobject-2.0 libxml-2.0)
LIB_LINK = $(shell pkg-config --libs glib-2.0 gmodule-2.0 gobject-2.0 libxml-2.0 cmocka)

$cc = gcc

Main: main.o CLI.o helper.o CLICommandInfo.o CLIAddDepCommand.o CLIAddFileCommand.o CLIAddIncludeCommand.o CLIBuildCommand.o CLICreateCommand.o CLIDeleteDepCommand.o CLIDeleteFileCommand.o CLIDeleteIncludeCommand.o CLIHelpCommand.o CLITestCommand.o libcbs.so
	cc -g  main.o helper.o CLI.o CLICommandInfo.o CLIAddDepCommand.o CLIAddFileCommand.o CLIAddIncludeCommand.o CLIBuildCommand.o CLICreateCommand.o CLIDeleteDepCommand.o CLIDeleteFileCommand.o CLIDeleteIncludeCommand.o CLIHelpCommand.o CLITestCommand.o -Wl,/home/Flavius/C/CBS/CBS/old_build/libcbs.so -L/home/Flavius/C/CBS/CBS/old_build/ $(LIB_LINK) -o cbs

main.o:
	cc -g -c main.c -o main.o $(LIB_INCLUDE)

CLI.o:
	cc -g -c  CLI.c -o CLI.o $(LIB_INCLUDE)

CLICommandInfo.o:
	cc -g -c  CLICommandInfo.c -o CLICommandInfo.o $(LIB_INCLUDE)

CLIAddDepCommand.o:
	cc -g -c CLIAddDepCommand.c -o CLIAddDepCommand.o $(LIB_INCLUDE)

CLIAddFileCommand.o:
	cc -g -c CLIAddFileCommand.c -o CLIAddFileCommand.o $(LIB_INCLUDE)

CLIAddIncludeCommand.o:
	cc -g -c CLIAddIncludeCommand.c -o CLIAddIncludeCommand.o $(LIB_INCLUDE)

CLIBuildCommand.o:
	cc -g -c CLIBuildCommand.c -o CLIBuildCommand.o $(LIB_INCLUDE)

CLICreateCommand.o:
	cc -g -c CLICreateCommand.c -o CLICreateCommand.o $(LIB_INCLUDE)

CLIDeleteDepCommand.o:
	cc -g -c CLIDeleteDepCommand.c -o CLIDeleteDepCommand.o $(LIB_INCLUDE)

CLIDeleteFileCommand.o:
	cc -g -c CLIDeleteFileCommand.c -o CLIDeleteFileCommand.o $(LIB_INCLUDE)

CLIDeleteIncludeCommand.o:
	cc -g -c CLIDeleteIncludeCommand.c -o CLIDeleteIncludeCommand.o $(LIB_INCLUDE)

CLIHelpCommand.o:
	cc -g -c CLIHelpCommand.c -o CLIHelpCommand.o $(LIB_INCLUDE)

CLITestCommand.o:
	cc -g -c CLITestCommand.c -o CLITestCommand.o $(LIB_INCLUDE)

libcbs.so: helper.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o ModelProjectConfiguration.o ModelTestRunner.o
	cc -g -shared -o libcbs.so helper.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o ModelProjectConfiguration.o ModelTestRunner.o $(LIB_LINK)

helper.o:
	cc -g -fpic -c Helper.c -o helper.o $(LIB_INCLUDE)

ModelProject.o:
	cc -g -fpic -c ModelProject.c -o ModelProject.o $(LIB_INCLUDE)

ModelSourceFile.o:
	cc -g -fpic -c ModelSourceFile.c -o ModelSourceFile.o $(LIB_INCLUDE)

ModelProjectDependency.o:
	cc -g -fpic -c ModelProjectDependency.c -o ModelProjectDependency.o $(LIB_INCLUDE)

ModelProjectManager.o:
	cc -g -fpic -c ModelProjectManager.c -o ModelProjectManager.o $(LIB_INCLUDE)

ModelProjectConfiguration.o:
	cc -g -fpic -c ModelProjectConfiguration.c -o ModelProjectConfiguration.o $(LIB_INCLUDE)

ModelTestRunner.o:
	cc -g -fpic -c ModelTestRunner.c -o ModelTestRunner.o $(LIB_INCLUDE)