SRC_DIR := .
LSRC_DIR := ./Lua
OBJ_DIR := ./obj
LOBJ_DIR := ./lobj
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
C_FILES := $(wildcard $(SRC_DIR)/Lua/*.c)
OBJ_FILES :=  $(patsubst $(LSRC_DIR)/%.c,$(LOBJ_DIR)/%.o,$(C_FILES)) $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_FILES))
INCLUDES := -I../cereal/include
LDFLAGS := -lpthread
CPPFLAGS := -O3 $(INCLUDES) -funroll-loops -pipe -march=native -mtune=native -fgraphite-identity -floop-nest-optimize -fdevirtualize-at-ltrans -fipa-pta -fno-semantic-interposition -fuse-linker-plugin -falign-functions=32 -fPIC -flto
CFLAGS := -O3 -pipe -funroll-loops -march=native -mtune=native -fgraphite-identity -floop-nest-optimize -fdevirtualize-at-ltrans -fipa-pta -fno-semantic-interposition -fuse-linker-plugin -falign-functions=32 -fPIC -flto
CXXFLAGS := -O3

dbframe: $(OBJ_FILES)
	g++ -o $@ $(LOBJ_DIR)/*.o $(OBJ_DIR)/*.o $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(LOBJ_DIR)/%.o: $(LSRC_DIR)/%.c
	gcc $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

