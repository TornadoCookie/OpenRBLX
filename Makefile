# Generated using Helium v2.1.1 (https://github.com/tornadocookie/he)

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


$(DISTDIR)/src/../lib/luau/VM/src $(DISTDIR)/src/../lib/luau/Compiler/src $(DISTDIR)/src/../lib/luau/Ast/src $(DISTDIR)/src $(DISTDIR)/src/luau $(DISTDIR)/src/../lib/cJSON/src $(DISTDIR)/src/filetypes $(DISTDIR)/src/../lib/xml/src $(DISTDIR)/src/../lib/lz4/src $(DISTDIR)/src/../lib/zstd/src $(DISTDIR)/src/../studio/classes $(DISTDIR)/src/../studio $(DISTDIR)/src/../player:
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

LDFLAGS+=-lcurl

CFLAGS+=-Ilib/$(RAYLIB_NAME)/include

LDFLAGS+=-lm
LDFLAGS+=-Llib/$(RAYLIB_NAME)/lib
LDFLAGS+=$(RAYLIB_DLL)
LDFLAGS+=-Wl,-rpath,lib/$(RAYLIB_NAME)/lib

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
	$(CXX) -o $@ $^ $(LDFLAGS)

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
	$(CXX) -o $@ $^ $(LDFLAGS)

$(DISTDIR)/%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@

$(DISTDIR)/%.o: %.cpp
	$(CXX) -c $^ $(CFLAGS) -o $@

clean:
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lapi.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/laux.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lbaselib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lbitlib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lbuffer.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lbuflib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lbuiltins.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lcorolib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ldblib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ldebug.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ldo.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lfunc.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lgc.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lgcdebug.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/linit.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lmathlib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lmem.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lnumprint.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lobject.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/loslib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lperf.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lstate.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lstring.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lstrlib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ltable.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ltablib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ltm.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/ludata.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lutf8lib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lveclib.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lvmexecute.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lvmload.o
	rm -f $(DISTDIR)/src/../lib/luau/VM/src/lvmutils.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/BuiltinFolding.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/Builtins.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/BytecodeBuilder.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/Compiler.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/ConstantFolding.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/CostModel.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/lcode.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/TableShape.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/Types.o
	rm -f $(DISTDIR)/src/../lib/luau/Compiler/src/ValueTracking.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Allocator.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Ast.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Confusables.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Cst.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Lexer.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Location.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/Parser.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/StringUtils.o
	rm -f $(DISTDIR)/src/../lib/luau/Ast/src/TimeTrace.o
	rm -f $(DISTDIR)/src/scriptruntime.o
	rm -f $(DISTDIR)/src/luau/disassembler.o
	rm -f $(DISTDIR)/src/luau/cframe.o
	rm -f $(DISTDIR)/src/luau/color3.o
	rm -f $(DISTDIR)/src/luau/dockwidgetpluginguiinfo.o
	rm -f $(DISTDIR)/src/luau/enum.o
	rm -f $(DISTDIR)/src/luau/event.o
	rm -f $(DISTDIR)/src/luau/globals.o
	rm -f $(DISTDIR)/src/luau/instance.o
	rm -f $(DISTDIR)/src/luau/rect.o
	rm -f $(DISTDIR)/src/luau/serialize.o
	rm -f $(DISTDIR)/src/luau/state.o
	rm -f $(DISTDIR)/src/luau/tweeninfo.o
	rm -f $(DISTDIR)/src/luau/udim2.o
	rm -f $(DISTDIR)/src/luau/udim.o
	rm -f $(DISTDIR)/src/luau/vector2.o
	rm -f $(DISTDIR)/src/luau/vector3.o
	rm -f $(DISTDIR)/src/datamodel.o
	rm -f $(DISTDIR)/src/instance.o
	rm -f $(DISTDIR)/src/rbxscriptsignal.o
	rm -f $(DISTDIR)/src/workspace.o
	rm -f $(DISTDIR)/src/serviceprovider.o
	rm -f $(DISTDIR)/src/rootinstance.o
	rm -f $(DISTDIR)/src/model.o
	rm -f $(DISTDIR)/src/pvinstance.o
	rm -f $(DISTDIR)/src/trusspart.o
	rm -f $(DISTDIR)/src/basepart.o
	rm -f $(DISTDIR)/src/part.o
	rm -f $(DISTDIR)/src/formfactorpart.o
	rm -f $(DISTDIR)/src/camera.o
	rm -f $(DISTDIR)/src/brickcolor.o
	rm -f $(DISTDIR)/src/meshcontentprovider.o
	rm -f $(DISTDIR)/src/cacheablecontentprovider.o
	rm -f $(DISTDIR)/src/physicalcharacter.o
	rm -f $(DISTDIR)/src/lighting.o
	rm -f $(DISTDIR)/src/wedgepart.o
	rm -f $(DISTDIR)/src/valuebase.o
	rm -f $(DISTDIR)/src/vector3value.o
	rm -f $(DISTDIR)/src/cylindermesh.o
	rm -f $(DISTDIR)/src/bevelmesh.o
	rm -f $(DISTDIR)/src/datamodelmesh.o
	rm -f $(DISTDIR)/src/runservice.o
	rm -f $(DISTDIR)/src/serverscriptservice.o
	rm -f $(DISTDIR)/src/luasourcecontainer.o
	rm -f $(DISTDIR)/src/basescript.o
	rm -f $(DISTDIR)/src/script.o
	rm -f $(DISTDIR)/src/specialmesh.o
	rm -f $(DISTDIR)/src/filemesh.o
	rm -f $(DISTDIR)/src/decal.o
	rm -f $(DISTDIR)/src/faceinstance.o
	rm -f $(DISTDIR)/src/../lib/cJSON/src/cJSON.o
	rm -f $(DISTDIR)/src/httpservice.o
	rm -f $(DISTDIR)/src/numbervalue.o
	rm -f $(DISTDIR)/src/folder.o
	rm -f $(DISTDIR)/src/trianglemeshpart.o
	rm -f $(DISTDIR)/src/meshpart.o
	rm -f $(DISTDIR)/src/texturecontentprovider.o
	rm -f $(DISTDIR)/src/players.o
	rm -f $(DISTDIR)/src/player.o
	rm -f $(DISTDIR)/src/stringvalue.o
	rm -f $(DISTDIR)/src/spawnlocation.o
	rm -f $(DISTDIR)/src/modulescript.o
	rm -f $(DISTDIR)/src/localscript.o
	rm -f $(DISTDIR)/src/starterplayerscripts.o
	rm -f $(DISTDIR)/src/starterplayer.o
	rm -f $(DISTDIR)/src/playerscripts.o
	rm -f $(DISTDIR)/src/accessory.o
	rm -f $(DISTDIR)/src/accoutrement.o
	rm -f $(DISTDIR)/src/startercharacterscripts.o
	rm -f $(DISTDIR)/src/testservice.o
	rm -f $(DISTDIR)/src/localizationtable.o
	rm -f $(DISTDIR)/src/plugin.o
	rm -f $(DISTDIR)/src/baseplayergui.o
	rm -f $(DISTDIR)/src/coregui.o
	rm -f $(DISTDIR)/src/collectionservice.o
	rm -f $(DISTDIR)/src/textservice.o
	rm -f $(DISTDIR)/src/guiservice.o
	rm -f $(DISTDIR)/src/rbxanalyticsservice.o
	rm -f $(DISTDIR)/src/contentprovider.o
	rm -f $(DISTDIR)/src/stylebase.o
	rm -f $(DISTDIR)/src/stylerule.o
	rm -f $(DISTDIR)/src/stylingservice.o
	rm -f $(DISTDIR)/src/tweenservice.o
	rm -f $(DISTDIR)/src/genericsettings.o
	rm -f $(DISTDIR)/src/globalsettings.o
	rm -f $(DISTDIR)/src/studiowidgetsservice.o
	rm -f $(DISTDIR)/src/startpageservice.o
	rm -f $(DISTDIR)/src/studiouserservice.o
	rm -f $(DISTDIR)/src/messagebusservice.o
	rm -f $(DISTDIR)/src/guibase.o
	rm -f $(DISTDIR)/src/guibase2d.o
	rm -f $(DISTDIR)/src/guiobject.o
	rm -f $(DISTDIR)/src/frame.o
	rm -f $(DISTDIR)/src/studio.o
	rm -f $(DISTDIR)/src/standalonepluginscripts.o
	rm -f $(DISTDIR)/src/boolvalue.o
	rm -f $(DISTDIR)/src/studiotheme.o
	rm -f $(DISTDIR)/src/stylesheet.o
	rm -f $(DISTDIR)/src/filetypes/rbxlx.o
	rm -f $(DISTDIR)/src/filetypes/rbxmx.o
	rm -f $(DISTDIR)/src/../lib/xml/src/xml.o
	rm -f $(DISTDIR)/src/filetypes/rbxm.o
	rm -f $(DISTDIR)/src/../lib/lz4/src/lz4.o
	rm -f $(DISTDIR)/src/../lib/zstd/src/zstd_decompress.o
	rm -f $(DISTDIR)/src/filetypes/rbxs.o
	rm -f $(DISTDIR)/src/../studio/classes/studioservice.o
	rm -f $(DISTDIR)/src/../studio/classes/corepackages.o
	rm -f $(DISTDIR)/src/../studio/studio.o
	rm -f $(DISTDIR)/studio$(EXEC_EXTENSION)
	rm -f $(DISTDIR)/src/../player/player.o
	rm -f $(DISTDIR)/player$(EXEC_EXTENSION)

all_dist:
	DISTDIR=$(DISTDIR)/dist/linux64-debug PLATFORM=linux64-debug $(MAKE)
	DISTDIR=$(DISTDIR)/dist/win64 PLATFORM=win64 $(MAKE)
