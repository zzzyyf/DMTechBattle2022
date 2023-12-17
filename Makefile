CXX=g++
CPPFLAGS=-Wall -g -std=c++2a

DBGFLAGS=-O3 -DNDEBUG

INC=-Isrc/asio -Isrc
LIBS=-pthread	# shouldn't use -lpthread

SRCDIR=./src
SRCS=$(wildcard $(SRCDIR)/*.cpp)

BUILDDIR=./obj
OBJS=$(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

TOOLS_DIR=$(SRCDIR)/tools
TOOLS_SRCS=$(wildcard $(TOOLS_DIR)/*.cpp)
TOOLS_OBJ_DIR=$(BUILDDIR)/tools
TOOLS_OBJS=$(patsubst $(TOOLS_DIR)/%.cpp, $(TOOLS_OBJ_DIR)/%.o, $(TOOLS_SRCS))
TOOLS_TARGETS=$(patsubst $(TOOLS_DIR)/%.cpp, $(TOOLS_DIR)/%.exe, $(TOOLS_SRCS))

TARGET=benchmark.exe

.PHONY: build
build : $(OBJS) $(TOOLS_TARGETS)
	$(CXX) $(LIBS) -o $(TARGET) $(OBJS)

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CPPFLAGS) $(DBGFLAGS) $(INC) -c $< -o $@

# tools
$(TOOLS_DIR)/%.exe : $(TOOLS_OBJ_DIR)/%.o
	$(CXX) $(LIBS) -o $@ $<

$(TOOLS_OBJ_DIR)/%.o : $(TOOLS_DIR)/%.cpp
	@mkdir -p $(TOOLS_OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(DBGFLAGS) $(INC) -c $< -o $@

.PHONY: clean
clean :
	rm -rf $(BUILDDIR)/*
	rm -r $(TARGET)
	rm -r $(TOOLS_TARGETS)
