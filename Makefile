SRCS = \
	main.cpp

# compiler flags
CXX = clang++
CINCLUDE = -I"./$(SRC_DIR)" -I"./$(TOP_OBJ_DIR)"
CFLAGS = -std=c++11
CWARN = -Wall -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Weffc++ -Wextra -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunreachable-code -pedantic
CWARNGCC = -Wlogical-op -Wnoexcept -Wstrict-null-sentinel 
COFF = -Wno-unused -Wno-unused-parameter -Wno-reorder
CERR = -Werror
CXXFLAGS = $(CFLAGS) $(CWARN) $(CERR) $(CINCLUDE)
LDLIBS = -lSDLmain -lSDL -lpng
LDLIBS += -lGL -lGLU
ISPC = ispc
ISPCFLAGS = -O2 --target=avx-x2 --arch=x86-64

# src and executable
SRC_DIR = src
#SRCS = $(wildcard **/*.cpp)
TARGET = main
# .o files
TOP_OBJ_DIR = bin
OBJ_DIR = $(TOP_OBJ_DIR)/$(SUB_OBJ_DIR)
OBJS = $(SRCS:.cpp=.o)
OBJS_PATH = $(addprefix $(OBJ_DIR)/,$(OBJS))
# .d files
DEPS = $(OBJS:.o=.d)
DEPS_PATH = $(addprefix $(OBJ_DIR)/,$(DEPS))

# default to release; other option is debug
ifeq ($(MODE),)
    MODE = release
endif

ifeq ($(MODE), release)
    SUB_OBJ_DIR = release
    CXXFLAGS += -O3
else ifeq ($(MODE), debug)
    SUB_OBJ_DIR = debug
    CXXFLAGS += -g -O0
endif

# targets
.PHONY: all clean fmt 

all: $(TARGET)

$(TARGET): $(OBJS_PATH)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MM -MP -MT $(@:.d=.o) -o $@ $<

# don't need to mkdir for object files since d files already exist
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.ispc
	$(ISPC) $(ISPCFLAGS) $< -o $@

$(SRC_DIR)/%.h: $(SRC_DIR)/%.ispc
	$(ISPC) $(ISPCFLAGS) $< -h $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS_PATH)
endif

clean:
	rm -rf $(TOP_OBJ_DIR) $(TARGET)

fmt:
	astyle --recursive --style=allman "*.cpp"
	astyle --recursive --style=allman "*.hpp"
