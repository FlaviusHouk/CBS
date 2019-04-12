$INCLUDE = pkg-config --cflags glib-2.0
$LIBS = pkg-config --libs glib-2.0
$cc = gcc

Main: main.o CLI.o helper.o CLICommandInfo.o libcbs.so
	cc -g  main.o helper.o CLI.o CLICommandInfo.o -Wl,-rpath=/home/Flavius/C/CBS/CBS -L/home/Flavius/C/CBS/CBS -lcbs $(shell pkg-config --libs glib-2.0 gobject-2.0 libxml-2.0) -o cbs.elf

main.o:
	cc -g -c main.c -o main.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

CLI.o:
	cc -g -c  CLI.c -o CLI.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

CLICommandInfo.o:
	cc -g -c  CLICommandInfo.c -o CLICommandInfo.o $(shell pkg-config --cflags glib-2.0 gobject-2.0)

libcbs.so: helper.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o ModelProjectConfiguration.o
	cc -g -shared -o libcbs.so helper.o ModelProject.o ModelSourceFile.o ModelProjectDependency.o ModelProjectManager.o ModelProjectConfiguration.o

helper.o:
	cc -g -fpic -c Helper.c -o helper.o $(shell pkg-config --cflags glib-2.0 gobject-2.0)

ModelProject.o:
	cc -g -fpic -c ModelProject.c -o ModelProject.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelSourceFile.o:
	cc -g -fpic -c ModelSourceFile.c -o ModelSourceFile.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelProjectDependency.o:
	cc -g -fpic -c ModelProjectDependency.c -o ModelProjectDependency.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelProjectManager.o:
	cc -g -fpic -c ModelProjectManager.c -o ModelProjectManager.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)

ModelProjectConfiguration.o:
	cc -g -fpic -c ModelProjectConfiguration.c -o ModelProjectConfiguration.o $(shell pkg-config --cflags glib-2.0 gobject-2.0 libxml-2.0)