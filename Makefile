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

PROGRAMS=test_rbxmx test_rbxlx studio test_rbxm test_rbxl
LIBRARIES=

all: $(DISTDIR) $(foreach prog, $(PROGRAMS), $(DISTDIR)/$(prog)$(EXEC_EXTENSION)) $(foreach lib, $(LIBRARIES), $(DISTDIR)/$(lib)$(LIB_EXTENSION))

$(DISTDIR):
	mkdir -p $@

CFLAGS+=-Isrc
CFLAGS+=-Iinclude
CFLAGS+=-Ilib/xml/src
CFLAGS+=-Ilib/xml/include
CFLAGS+=-Ilib/lz4/src
CFLAGS+=-Ilib/lz4/include
CFLAGS+=-Ilib/minilua/src
CFLAGS+=-Ilib/minilua/include
CFLAGS+=-D PLATFORM=\"$(PLATFORM)\"
CFLAGS+=-Iinclude
CFLAGS+=-Ilib/xml/include
CFLAGS+=-Wno-incompatible-pointer-types
CFLAGS+=-rdynamic

CFLAGS+=-Ilib/$(RAYLIB_NAME)/include
CFLAGS+=-Wl,-rpath,lib/$(RAYLIB_NAME)/lib

LDFLAGS+=-lm
LDFLAGS+=-Llib/$(RAYLIB_NAME)/lib
LDFLAGS+=$(RAYLIB_DLL)

instance_SOURCES+=src/datamodel.c
instance_SOURCES+=src/instance.c
instance_SOURCES+=src/rbxscriptsignal.c
instance_SOURCES+=src/workspace.c
instance_SOURCES+=src/serviceprovider.c
instance_SOURCES+=src/rootinstance.c
instance_SOURCES+=src/model.c
instance_SOURCES+=src/pvinstance.c
instance_SOURCES+=src/trusspart.c
instance_SOURCES+=src/basepart.c
instance_SOURCES+=src/part.c
instance_SOURCES+=src/formfactorpart.c
instance_SOURCES+=src/camera.c
instance_SOURCES+=src/brickcolor.c
instance_SOURCES+=src/meshcontentprovider.c
instance_SOURCES+=src/cacheablecontentprovider.c
instance_SOURCES+=src/physicalcharacter.c
instance_SOURCES+=src/lighting.c
instance_SOURCES+=src/wedgepart.c
instance_SOURCES+=src/valuebase.c
instance_SOURCES+=src/vector3value.c
instance_SOURCES+=src/cylindermesh.c
instance_SOURCES+=src/bevelmesh.c
instance_SOURCES+=src/datamodelmesh.c
instance_SOURCES+=src/runservice.c
instance_SOURCES+=src/serverscriptservice.c
instance_SOURCES+=src/luasourcecontainer.c
instance_SOURCES+=src/basescript.c
instance_SOURCES+=src/script.c
instance_SOURCES+=src/specialmesh.c
instance_SOURCES+=src/filemesh.c

rbxmx_SOURCES+=src/filetypes/rbxlx.c
rbxmx_SOURCES+=src/filetypes/rbxmx.c
rbxmx_SOURCES+=src/../lib/xml/src/xml.c

rbxm_SOURCES+=src/filetypes/rbxm.c
rbxm_SOURCES+=src/../lib/lz4/src/lz4.c

test_rbxmx_SOURCES+=src/../tests/test_rbxmx.c
test_rbxmx_SOURCES+=$(instance_SOURCES)
test_rbxmx_SOURCES+=$(rbxmx_SOURCES)

$(DISTDIR)/test_rbxmx$(EXEC_EXTENSION): $(test_rbxmx_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test_rbxlx_SOURCES+=src/../tests/test_rbxlx.c
test_rbxlx_SOURCES+=$(instance_SOURCES)
test_rbxlx_SOURCES+=$(rbxmx_SOURCES)

$(DISTDIR)/test_rbxlx$(EXEC_EXTENSION): $(test_rbxlx_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

studio_SOURCES+=src/../studio/studio.c
studio_SOURCES+=$(instance_SOURCES)
studio_SOURCES+=$(rbxmx_SOURCES)
studio_SOURCES+=$(rbxm_SOURCES)

$(DISTDIR)/studio$(EXEC_EXTENSION): $(studio_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test_rbxm_SOURCES+=src/../tests/test_rbxm.c
test_rbxm_SOURCES+=$(instance_SOURCES)
test_rbxm_SOURCES+=$(rbxm_SOURCES)

$(DISTDIR)/test_rbxm$(EXEC_EXTENSION): $(test_rbxm_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test_rbxl_SOURCES+=src/../tests/test_rbxl.c
test_rbxl_SOURCES+=$(instance_SOURCES)
test_rbxl_SOURCES+=$(rbxm_SOURCES)

$(DISTDIR)/test_rbxl$(EXEC_EXTENSION): $(test_rbxl_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(DISTDIR)/test_rbxmx$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/test_rbxlx$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/studio$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/test_rbxm$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/test_rbxl$(EXEC_EXTENSION)
