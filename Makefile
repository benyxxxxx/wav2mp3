
CC := gcc
CXX := g++
LINK.o = $(LINK.cc)

LAME_LIB_DIR := /usr/local/lib
LAME_INC_DIR := /usr/local/include/lame

CPPFLAGS := -g -I$(LAME_INC_DIR) -MMD -std=c++11
CFLAGS   := -W -Wall -g  -MMD
LAME_LIB := $(LAME_LIB_DIR)/libmp3lame.a
LDLIBS   := $(LAME_LIB) -pthread -W -Wall 

SRC_C       := thpool.c 
SRC_CPP     := myglob.cpp lame_wrapper.cpp wav2mp3.cpp 
SRC         := $(SRC_C) $(SRC_CPP)
OBJ         := $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o)
APP         := wav2mp3
DEP         := $(OBJ:.o=.d)

all: $(APP)

$(APP) : $(OBJ)

-include $(DEP)

clean :
	$(RM) $(OBJ) $(APP) $(DEP)




