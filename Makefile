CXX=g++
CPPFLAGS=-Wall -g -std=c++2a

DBGFLAGS=-O3 -DNDEBUG

INC=-Isrc/asio
LIBS=-pthread	# shouldn't use -lpthread

SRCDIR=./src
SRCS=$(wildcard $(SRCDIR)/*.cpp)

BUILDDIR=./obj
OBJS=$(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

TARGET=benchmark.exe

.PHONY: build
build : $(OBJS)
	$(CXX) $(LIBS) -o $(TARGET) $(OBJS)

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(DBGFLAGS) $(INC) -c $< -o $@

.PHONY: clean
clean :
	rm -rf $(BUILDDIR)/*
