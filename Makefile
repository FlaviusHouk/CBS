$INCLUDE = pkg-config --cflags glib-2.0
$LIBS = pkg-config --libs glib-2.0
$cc = gcc

Main: main.o CLI.o helper.o CLICommandInfo.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o
	cc -g  main.o helper.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o CLI.o CLICommandInfo.o $(shell pkg-config --libs glib-2.0 gobject-2.0 libxml-2.0) -o cbs.lef

main.o:
	cc -g -c main.c -o main.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

helper.o:
	cc -g -c Helper.c -o helper.o $(shell pkg-config --cflags glib-2.0 gobject-2.0)

CLI.o:
	cc -g -c  CLI.c -o CLI.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

CLICommandInfo.o:
	cc -g -c  CLICommandInfo.c -o CLICommandInfo.o $(shell pkg-config --cflags glib-2.0 gobject-2.0)

ModelProject.o:
	cc -g -c ModelProject.c -o ModelProject.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelSourceFile.o:
	cc -g -c ModelSourceFile.c -o ModelSourceFile.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelProjectDependency.o:
	cc -g -c ModelProjectDependency.c -o ModelProjectDependency.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelProjectManager.o:
	cc -g -c ModelProjectManager.c -o ModelProjectManager.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)