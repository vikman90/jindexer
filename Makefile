# December 7th, 2020

TARGET = bin/jindexer
INDEX = var/index.db

INCLUDES = -Iinclude
CXXFLAGS = -Wall -Wextra -pipe -O2 $(INCLUDES)
LDFLAGS = -pipe -O2
LIBS = -lcjson -lsqlite3

SQLITE = sqlite3

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.hpp)
OBJECTS = $(SOURCES:.cpp=.o)
SCHEMA = etc/schema.sql

.PHONY: all clean clean-index prepare

all: $(TARGET) $(INDEX)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(HEADERS)

$(INDEX): $(SCHEMA)
	$(SQLITE) $@ < $<

clean: clean-index
	$(RM) $(TARGET) $(OBJECTS)

clean-index:
	$(RM) $(INDEX)

prepare:
	apt-get update
	apt-get install -y g++ make libsqlite3-dev libcjson-dev
