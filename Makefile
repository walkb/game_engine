SRC_DIR = src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

LUA_DIR = ThirdParty/lua-5.4.6
LUA_SRCS := $(wildcard $(LUA_DIR)/*.c)

BOX_DIR = ThirdParty/box2d/src
BOX_SRCS := $(wildcard $(BOX_DIR)/*/*.cpp)

SRCS += $(LUA_SRCS)

SRCS += $(BOX_SRCS)


main:
	clang++ -O3 $(SRCS) -o game_engine_linux -std=c++17 -I$(LUA_DIR) -I"./ThirdParty/box2d/src" -I"./ThirdParty/box2d/include" -I"./ThirdParty/glm-0.9.9.8" -I"./ThirdParty" -I"./ThirdParty/rapidjson-1.1.0/include" -I"./ThirdParty/SDL2" -lSDL2 -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf -llua5.4