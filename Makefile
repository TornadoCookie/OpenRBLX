# Generated using Helium v2.2.0 (https://github.com/tornadocookie/he)

PLATFORM?=linux64-debug
DISTDIR?=build

.PHONY: all

RAYLIB_NAME=raylib5.5-$(PLATFORM)

ifeq ($(PLATFORM), linux64-debug)
EXEC_EXTENSION=-debug
LIB_EXTENSION=-debug.so
LIB_EXTENSION_STATIC=(null)
CC=gcc
RAYLIB_DLL=-lraylib
CFLAGS+=-g
CFLAGS+=-D DEBUG
CFLAGS+=-D EXEC_EXTENSION=\"-debug\"
CFLAGS+=-D LIB_EXTENSION=\"-debug.so\"
LDFLAGS+=-rdynamic
endif

ifeq ($(PLATFORM), win64)
EXEC_EXTENSION=.exe
LIB_EXTENSION=.dll
LIB_EXTENSION_STATIC=(null)
CC=x86_64-w64-mingw32-gcc
CXX=x86_64-w64-mingw32-g++
RAYLIB_DLL=-lraylibdll
CFLAGS+=-O2
CFLAGS+=-D RELEASE
CFLAGS+=-D EXEC_EXTENSION=\".exe\"
CFLAGS+=-D LIB_EXTENSION=\".dll\"
LDFLAGS+=-lws2_32
LDFLAGS+=-static-libstdc++
LDFLAGS+=-static-libgcc
endif

PROGRAMS=studio player
LIBRARIES=

curl_NAME=libcurl-$(PLATFORM)
CFLAGS+=-Ilib/$(curl_NAME)/include
LDFLAGS+=-Llib/$(curl_NAME)/lib
LDFLAGS+=-lcurl
LDFLAGS+=-Wl,-rpath,lib/$(curl_NAME)/lib


all: $(DISTDIR) $(DISTDIR)/src/../lib/luau/VM/src $(DISTDIR)/src/../lib/luau/Compiler/src $(DISTDIR)/src/../lib/luau/Ast/src $(DISTDIR)/src $(DISTDIR)/src/luau $(DISTDIR)/src/../lib/cJSON/src $(DISTDIR)/src/filetypes $(DISTDIR)/src/../lib/xml/src $(DISTDIR)/src/../lib/lz4/src $(DISTDIR)/src/../lib/zstd/src $(DISTDIR)/src/../studio/classes $(DISTDIR)/src/../studio $(DISTDIR)/src/../player $(foreach prog, $(PROGRAMS), $(DISTDIR)/$(prog)$(EXEC_EXTENSION)) $(foreach lib, $(LIBRARIES), $(DISTDIR)/$(lib)$(LIB_EXTENSION) $(DISTDIR)/$(lib)$(LIB_EXTENSION_STATIC)) deps

ifneq ($(DISTDIR), .)
deps:
	mkdir -p $(DISTDIR)/lib
	if [ -d lib/$(curl_NAME) ] && [ ! -d $(DISTDIR)/lib/$(curl_NAME) ]; then cp -r lib/$(curl_NAME) $(DISTDIR)/lib; fi
	if [ -d lib/$(RAYLIB_NAME) ] && [ ! -d $(DISTDIR)/lib/$(RAYLIB_NAME) ]; then cp -r lib/$(RAYLIB_NAME) $(DISTDIR)/lib; fi
else
deps:
endif

$(DISTDIR)/src/../lib/luau/VM/src:
	mkdir -p $@

$(DISTDIR)/src/../lib/luau/Compiler/src:
	mkdir -p $@

$(DISTDIR)/src/../lib/luau/Ast/src:
	mkdir -p $@

$(DISTDIR)/src:
	mkdir -p $@

$(DISTDIR)/src/luau:
	mkdir -p $@

$(DISTDIR)/src/../lib/cJSON/src:
	mkdir -p $@

$(DISTDIR)/src/filetypes:
	mkdir -p $@

$(DISTDIR)/src/../lib/xml/src:
	mkdir -p $@

$(DISTDIR)/src/../lib/lz4/src:
	mkdir -p $@

$(DISTDIR)/src/../lib/zstd/src:
	mkdir -p $@

$(DISTDIR)/src/../studio/classes:
	mkdir -p $@

$(DISTDIR)/src/../studio:
	mkdir -p $@

$(DISTDIR)/src/../player:
	mkdir -p $@

$(DISTDIR):
	mkdir -p $@

CFLAGS+=-Isrc
CFLAGS+=-Iinclude
CFLAGS+=-Ilib/xml/src
CFLAGS+=-Ilib/xml/include
CFLAGS+=-Ilib/lz4/src
CFLAGS+=-Ilib/lz4/include
CFLAGS+=-Ilib/cJSON/src
CFLAGS+=-Ilib/cJSON/include
CFLAGS+=-Ilib/mini_gzip/src
CFLAGS+=-Ilib/mini_gzip/include
CFLAGS+=-Ilib/zstd/src
CFLAGS+=-Ilib/zstd/include
CFLAGS+=-D PLATFORM=\"$(PLATFORM)\"
CFLAGS+=-Iinclude
CFLAGS+=-Ilib/xml/include
CFLAGS+=-Wno-incompatible-pointer-types
CFLAGS+=-Ilib/luau/VM/include
CFLAGS+=-Ilib/luau/Compiler/include
CFLAGS+=-Ilib/luau/Common/include
CFLAGS+=-Ilib/luau/Ast/include
CFLAGS+=-Ilib/luau/VM/src
CXXFLAGS+=-DLUA_API='extern
CXXFLAGS+="C"'
CXXFLAGS+=-DLUACODE_API='extern
CXXFLAGS+="C"'

LDFLAGS+=-lcurl

CFLAGS+=-Ilib/$(RAYLIB_NAME)/include

LDFLAGS+=-lm
LDFLAGS+=-Llib/$(RAYLIB_NAME)/lib
LDFLAGS+=$(RAYLIB_DLL)
LDFLAGS+=-Wl,-rpath,lib/$(RAYLIB_NAME)/lib

LDFLAGS+=-lstdc++
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lapi.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/laux.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lbaselib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lbitlib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lbuffer.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lbuflib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lbuiltins.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lcorolib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ldblib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ldebug.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ldo.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lfunc.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lgc.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lgcdebug.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/linit.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lmathlib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lmem.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lnumprint.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lobject.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/loslib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lperf.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lstate.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lstring.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lstrlib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ltable.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ltablib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ltm.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/ludata.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lutf8lib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lveclib.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lvmexecute.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lvmload.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/VM/src/lvmutils.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/BuiltinFolding.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/Builtins.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/BytecodeBuilder.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/Compiler.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/ConstantFolding.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/CostModel.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/lcode.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/TableShape.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/Types.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Compiler/src/ValueTracking.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Allocator.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Ast.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Confusables.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Cst.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Lexer.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Location.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/Parser.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/StringUtils.o
luau_CXX_SOURCES+=$(DISTDIR)/src/../lib/luau/Ast/src/TimeTrace.o
luau_SOURCES+=$(DISTDIR)/src/scriptruntime.o
luau_SOURCES+=$(DISTDIR)/src/luau/disassembler.o
luau_SOURCES+=$(DISTDIR)/src/luau/cframe.o
luau_SOURCES+=$(DISTDIR)/src/luau/color3.o
luau_SOURCES+=$(DISTDIR)/src/luau/dockwidgetpluginguiinfo.o
luau_SOURCES+=$(DISTDIR)/src/luau/enum.o
luau_SOURCES+=$(DISTDIR)/src/luau/event.o
luau_SOURCES+=$(DISTDIR)/src/luau/globals.o
luau_SOURCES+=$(DISTDIR)/src/luau/instance.o
luau_SOURCES+=$(DISTDIR)/src/luau/rect.o
luau_SOURCES+=$(DISTDIR)/src/luau/serialize.o
luau_SOURCES+=$(DISTDIR)/src/luau/state.o
luau_SOURCES+=$(DISTDIR)/src/luau/tweeninfo.o
luau_SOURCES+=$(DISTDIR)/src/luau/udim2.o
luau_SOURCES+=$(DISTDIR)/src/luau/udim.o
luau_SOURCES+=$(DISTDIR)/src/luau/vector2.o
luau_SOURCES+=$(DISTDIR)/src/luau/vector3.o
luau_SOURCES+=$(DISTDIR)/src/luau/numbersequence.o
luau_SOURCES+=$(DISTDIR)/src/luau/font.o
luau_SOURCES+=$(DISTDIR)/src/luau/path2dcontrolpoint.o
luau_SOURCES+=$(DISTDIR)/src/luau/numberrange.o
luau_SOURCES+=$(DISTDIR)/src/luau/colorsequence.o
luau_SOURCES+=$(DISTDIR)/src/luau/brickcolor.o

instance_SOURCES+=$(DISTDIR)/src/datamodel.o
instance_SOURCES+=$(DISTDIR)/src/instance.o
instance_SOURCES+=$(DISTDIR)/src/rbxscriptsignal.o
instance_SOURCES+=$(DISTDIR)/src/workspace.o
instance_SOURCES+=$(DISTDIR)/src/serviceprovider.o
instance_SOURCES+=$(DISTDIR)/src/rootinstance.o
instance_SOURCES+=$(DISTDIR)/src/model.o
instance_SOURCES+=$(DISTDIR)/src/pvinstance.o
instance_SOURCES+=$(DISTDIR)/src/trusspart.o
instance_SOURCES+=$(DISTDIR)/src/basepart.o
instance_SOURCES+=$(DISTDIR)/src/part.o
instance_SOURCES+=$(DISTDIR)/src/formfactorpart.o
instance_SOURCES+=$(DISTDIR)/src/camera.o
instance_SOURCES+=$(DISTDIR)/src/brickcolor.o
instance_SOURCES+=$(DISTDIR)/src/meshcontentprovider.o
instance_SOURCES+=$(DISTDIR)/src/cacheablecontentprovider.o
instance_SOURCES+=$(DISTDIR)/src/physicalcharacter.o
instance_SOURCES+=$(DISTDIR)/src/lighting.o
instance_SOURCES+=$(DISTDIR)/src/wedgepart.o
instance_SOURCES+=$(DISTDIR)/src/valuebase.o
instance_SOURCES+=$(DISTDIR)/src/vector3value.o
instance_SOURCES+=$(DISTDIR)/src/cylindermesh.o
instance_SOURCES+=$(DISTDIR)/src/bevelmesh.o
instance_SOURCES+=$(DISTDIR)/src/datamodelmesh.o
instance_SOURCES+=$(DISTDIR)/src/runservice.o
instance_SOURCES+=$(DISTDIR)/src/serverscriptservice.o
instance_SOURCES+=$(DISTDIR)/src/luasourcecontainer.o
instance_SOURCES+=$(DISTDIR)/src/basescript.o
instance_SOURCES+=$(DISTDIR)/src/script.o
instance_SOURCES+=$(DISTDIR)/src/specialmesh.o
instance_SOURCES+=$(DISTDIR)/src/filemesh.o
instance_SOURCES+=$(DISTDIR)/src/decal.o
instance_SOURCES+=$(DISTDIR)/src/faceinstance.o
instance_SOURCES+=$(DISTDIR)/src/../lib/cJSON/src/cJSON.o
instance_SOURCES+=$(DISTDIR)/src/httpservice.o
instance_SOURCES+=$(DISTDIR)/src/numbervalue.o
instance_SOURCES+=$(DISTDIR)/src/folder.o
instance_SOURCES+=$(DISTDIR)/src/trianglemeshpart.o
instance_SOURCES+=$(DISTDIR)/src/meshpart.o
instance_SOURCES+=$(DISTDIR)/src/texturecontentprovider.o
instance_SOURCES+=$(DISTDIR)/src/players.o
instance_SOURCES+=$(DISTDIR)/src/player.o
instance_SOURCES+=$(DISTDIR)/src/stringvalue.o
instance_SOURCES+=$(DISTDIR)/src/spawnlocation.o
instance_SOURCES+=$(DISTDIR)/src/modulescript.o
instance_SOURCES+=$(DISTDIR)/src/localscript.o
instance_SOURCES+=$(DISTDIR)/src/starterplayerscripts.o
instance_SOURCES+=$(DISTDIR)/src/starterplayer.o
instance_SOURCES+=$(DISTDIR)/src/playerscripts.o
instance_SOURCES+=$(DISTDIR)/src/accessory.o
instance_SOURCES+=$(DISTDIR)/src/accoutrement.o
instance_SOURCES+=$(DISTDIR)/src/startercharacterscripts.o
instance_SOURCES+=$(DISTDIR)/src/testservice.o
instance_SOURCES+=$(DISTDIR)/src/localizationtable.o
instance_SOURCES+=$(DISTDIR)/src/plugin.o
instance_SOURCES+=$(DISTDIR)/src/baseplayergui.o
instance_SOURCES+=$(DISTDIR)/src/coregui.o
instance_SOURCES+=$(DISTDIR)/src/collectionservice.o
instance_SOURCES+=$(DISTDIR)/src/textservice.o
instance_SOURCES+=$(DISTDIR)/src/guiservice.o
instance_SOURCES+=$(DISTDIR)/src/rbxanalyticsservice.o
instance_SOURCES+=$(DISTDIR)/src/contentprovider.o
instance_SOURCES+=$(DISTDIR)/src/stylebase.o
instance_SOURCES+=$(DISTDIR)/src/stylerule.o
instance_SOURCES+=$(DISTDIR)/src/stylingservice.o
instance_SOURCES+=$(DISTDIR)/src/tweenservice.o
instance_SOURCES+=$(DISTDIR)/src/genericsettings.o
instance_SOURCES+=$(DISTDIR)/src/globalsettings.o
instance_SOURCES+=$(DISTDIR)/src/studiowidgetsservice.o
instance_SOURCES+=$(DISTDIR)/src/startpageservice.o
instance_SOURCES+=$(DISTDIR)/src/studiouserservice.o
instance_SOURCES+=$(DISTDIR)/src/messagebusservice.o
instance_SOURCES+=$(DISTDIR)/src/guibase.o
instance_SOURCES+=$(DISTDIR)/src/guibase2d.o
instance_SOURCES+=$(DISTDIR)/src/guiobject.o
instance_SOURCES+=$(DISTDIR)/src/frame.o
instance_SOURCES+=$(DISTDIR)/src/studio.o
instance_SOURCES+=$(DISTDIR)/src/standalonepluginscripts.o
instance_SOURCES+=$(DISTDIR)/src/boolvalue.o
instance_SOURCES+=$(DISTDIR)/src/studiotheme.o
instance_SOURCES+=$(DISTDIR)/src/stylesheet.o
instance_SOURCES+=$(DISTDIR)/src/mouse.o
instance_SOURCES+=$(DISTDIR)/src/pluginmouse.o
instance_SOURCES+=$(DISTDIR)/src/humanoid.o
instance_SOURCES+=$(DISTDIR)/src/localizationservice.o
instance_SOURCES+=$(DISTDIR)/src/userinputservice.o
instance_SOURCES+=$(DISTDIR)/src/translator.o
instance_SOURCES+=$(DISTDIR)/src/renderer.o
instance_CXX_SOURCES+=$(luau_CXX_SOURCES)
instance_SOURCES+=$(luau_SOURCES)

rbxmx_SOURCES+=$(DISTDIR)/src/filetypes/rbxlx.o
rbxmx_SOURCES+=$(DISTDIR)/src/filetypes/rbxmx.o
rbxmx_SOURCES+=$(DISTDIR)/src/../lib/xml/src/xml.o

rbxm_SOURCES+=$(DISTDIR)/src/filetypes/rbxm.o
rbxm_SOURCES+=$(DISTDIR)/src/../lib/lz4/src/lz4.o
rbxm_SOURCES+=$(DISTDIR)/src/../lib/zstd/src/zstd_decompress.o

rbxs_SOURCES+=$(DISTDIR)/src/filetypes/rbxs.o

studioclasses_SOURCES+=$(DISTDIR)/src/../studio/classes/studioservice.o
studioclasses_SOURCES+=$(DISTDIR)/src/../studio/classes/corepackages.o
studioclasses_SOURCES+=$(DISTDIR)/src/../studio/classes/changehistoryservice.o

studio_SOURCES+=$(DISTDIR)/src/../studio/studio.o
studio_CXX_SOURCES+=$(instance_CXX_SOURCES)
studio_SOURCES+=$(instance_SOURCES)
studio_CXX_SOURCES+=$(rbxmx_CXX_SOURCES)
studio_SOURCES+=$(rbxmx_SOURCES)
studio_CXX_SOURCES+=$(rbxm_CXX_SOURCES)
studio_SOURCES+=$(rbxm_SOURCES)
studio_CXX_SOURCES+=$(studioclasses_CXX_SOURCES)
studio_SOURCES+=$(studioclasses_SOURCES)

$(DISTDIR)/studio$(EXEC_EXTENSION): $(studio_SOURCES) $(studio_CXX_SOURCES)
	$(CC) -o $@ $^ $(LDFLAGS)

player_SOURCES+=$(DISTDIR)/src/../player/player.o
player_CXX_SOURCES+=$(instance_CXX_SOURCES)
player_SOURCES+=$(instance_SOURCES)
player_CXX_SOURCES+=$(rbxmx_CXX_SOURCES)
player_SOURCES+=$(rbxmx_SOURCES)
player_CXX_SOURCES+=$(rbxm_CXX_SOURCES)
player_SOURCES+=$(rbxm_SOURCES)
player_CXX_SOURCES+=$(rbxs_CXX_SOURCES)
player_SOURCES+=$(rbxs_SOURCES)

$(DISTDIR)/player$(EXEC_EXTENSION): $(player_SOURCES) $(player_CXX_SOURCES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DISTDIR)/%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@

$(DISTDIR)/%.o: %.cpp
	$(CXX) -c $^ $(CFLAGS) $(CXXFLAGS) -o $@

clean:
	rm -rf $(DISTDIR)/*

all_dist:
	DISTDIR=$(DISTDIR)/dist/linux64-debug PLATFORM=linux64-debug $(MAKE)
	DISTDIR=$(DISTDIR)/dist/win64 PLATFORM=win64 $(MAKE)
