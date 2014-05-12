CXX = g++ -W -Wall -Wextra -pedantic -std=c++0x
CFLAGS = -c
INCLUDE =
LINK = -lsfml-graphics -lsfml-window -lsfml-system
BINDIR = bin
SOURCES = \
        src/game.o \
        src/coordinateMap.o \
        src/stateStack.o \
        src/state.o \
        src/editState.o \
        src/openState.o \
        src/waypoint.o \
        src/parser.o \
        src/command.o \
        src/leCommands.o

all: $(BINDIR) $(BINDIR)/LevelEditor

$(BINDIR):
		mkdir -p $(BINDIR)

$(BINDIR)/LevelEditor: $(SOURCES) src/main.o
		$(CXX) -o $(BINDIR)/LevelEditor $(SOURCES) src/main.o $(LINK) 

tests/UnitTests: $(SOURCES) src/unitTests.o
		$(CXX) -o tests/UnitTests $(SOURCES) src/UnitTests.o $(LINK)

clean:		src.clean
		rm -f $(BINDIR)/LevelEditor

%.clean:
		rm -f $*/*.o

%.o:		%.cpp
		$(CXX) $(INCLUDE) $(CFLAGS) -o $*.o $*.cpp
