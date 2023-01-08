CXX=g++
CPPFLAGS=-Wall -g -O3 -std=c++2a
INC=-I./src/include -I./src/include/asio
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
	$(CXX) $(CPPFLAGS) $(INC) -c $< -o $@

.PHONY: clean
clean :
	rm -rf $(BUILDDIR)/*
