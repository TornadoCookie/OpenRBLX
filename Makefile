PLATFORM?=linux64-debug
DISTDIR?=.

.PHONY: all

RAYLIB_NAME=raylib5-$(PLATFORM)

ifeq ($(PLATFORM), linux64)
EXEC_EXTENSION=
LIB_EXTENSION=.so
CC=gcc
RAYLIB_DLL=-lraylib
CFLAGS+=-O2
CFLAGS+=-D RELEASE
CFLAGS+=-D EXEC_EXTENSION=\"\"
CFLAGS+=-D LIB_EXTENSION=\".so\"
endif

ifeq ($(PLATFORM), linux64-debug)
EXEC_EXTENSION=-debug
LIB_EXTENSION=-debug.so
CC=gcc
RAYLIB_DLL=-lraylib
CFLAGS+=-g
CFLAGS+=-D DEBUG
CFLAGS+=-D EXEC_EXTENSION=\"-debug\"
CFLAGS+=-D LIB_EXTENSION=\"-debug.so\"
endif

ifeq ($(PLATFORM), win64)
EXEC_EXTENSION=.exe
LIB_EXTENSION=.dll
CC=x86_64-w64-mingw32-gcc
RAYLIB_DLL=-lraylibdll
CFLAGS+=-O2
CFLAGS+=-D RELEASE
CFLAGS+=-D EXEC_EXTENSION=\".exe\"
CFLAGS+=-D LIB_EXTENSION=\".dll\"
endif

PROGRAMS=test_rbxmx test_rbxlx
LIBRARIES=

all: $(DISTDIR) $(foreach prog, $(PROGRAMS), $(DISTDIR)/$(prog)$(EXEC_EXTENSION)) $(foreach lib, $(LIBRARIES), $(DISTDIR)/$(lib)$(LIB_EXTENSION))

$(DISTDIR):
	mkdir -p $@

CFLAGS+=-Isrc
CFLAGS+=-Ilib/xml/src
CFLAGS+=-D PLATFORM=\"$(PLATFORM)\"
CFLAGS+=-Iinclude
CFLAGS+=-Ilib/xml/include
CFLAGS+=-Wno-incompatible-pointer-types

CFLAGS+=-Ilib/$(RAYLIB_NAME)/include
CFLAGS+=-Wl,-rpath,lib/$(RAYLIB_NAME)/lib

LDFLAGS+=-lm
LDFLAGS+=-Llib/$(RAYLIB_NAME)/lib
LDFLAGS+=$(RAYLIB_DLL)

test_rbxmx_SOURCES+=src/../tests/test_rbxmx.c
test_rbxmx_SOURCES+=src/filetypes/rbxmx.c
test_rbxmx_SOURCES+=src/datamodel.c
test_rbxmx_SOURCES+=src/instance.c
test_rbxmx_SOURCES+=src/../lib/xml/src/xml.c
test_rbxmx_SOURCES+=src/rbxscriptsignal.c
test_rbxmx_SOURCES+=src/workspace.c
test_rbxmx_SOURCES+=src/serviceprovider.c
test_rbxmx_SOURCES+=src/rootinstance.c
test_rbxmx_SOURCES+=src/model.c
test_rbxmx_SOURCES+=src/pvinstance.c
test_rbxmx_SOURCES+=src/trusspart.c
test_rbxmx_SOURCES+=src/basepart.c
test_rbxmx_SOURCES+=src/part.c
test_rbxmx_SOURCES+=src/formfactorpart.c
test_rbxmx_SOURCES+=src/camera.c
test_rbxmx_SOURCES+=src/brickcolor.c
test_rbxmx_SOURCES+=src/meshcontentprovider.c
test_rbxmx_SOURCES+=src/cacheablecontentprovider.c
test_rbxmx_SOURCES+=src/physicalcharacter.c
test_rbxmx_SOURCES+=src/lighting.c

$(DISTDIR)/test_rbxmx$(EXEC_EXTENSION): $(test_rbxmx_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test_rbxlx_SOURCES+=src/../tests/test_rbxlx.c
test_rbxlx_SOURCES+=src/filetypes/rbxlx.c
test_rbxlx_SOURCES+=src/filetypes/rbxmx.c
test_rbxlx_SOURCES+=src/datamodel.c
test_rbxlx_SOURCES+=src/instance.c
test_rbxlx_SOURCES+=src/../lib/xml/src/xml.c
test_rbxlx_SOURCES+=src/rbxscriptsignal.c
test_rbxlx_SOURCES+=src/workspace.c
test_rbxlx_SOURCES+=src/serviceprovider.c
test_rbxlx_SOURCES+=src/rootinstance.c
test_rbxlx_SOURCES+=src/model.c
test_rbxlx_SOURCES+=src/pvinstance.c
test_rbxlx_SOURCES+=src/trusspart.c
test_rbxlx_SOURCES+=src/basepart.c
test_rbxlx_SOURCES+=src/part.c
test_rbxlx_SOURCES+=src/formfactorpart.c
test_rbxlx_SOURCES+=src/camera.c
test_rbxlx_SOURCES+=src/brickcolor.c
test_rbxlx_SOURCES+=src/meshcontentprovider.c
test_rbxlx_SOURCES+=src/cacheablecontentprovider.c
test_rbxlx_SOURCES+=src/physicalcharacter.c
test_rbxlx_SOURCES+=src/lighting.c

$(DISTDIR)/test_rbxlx$(EXEC_EXTENSION): $(test_rbxlx_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(DISTDIR)/test_rbxmx$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/test_rbxlx$(EXEC_EXTENSION)
