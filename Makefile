CXX = g++ -W -Wall -Wextra -pedantic -std=c++0x
CFLAGS = -c -g
INCLUDE = -I/home/stephen/Downloads/SFML-latestish/include
LINK = -L/home/stephen/Downloads/SFML-latestish/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
BINDIR = bin
SOURCES = \
        src/game.o \
        src/stateStack.o \
        src/state.o

all: $(BINDIR) $(BINDIR)/LevelEditor

test: tests/UnitTests

$(BINDIR):
		mkdir -p $(BINDIR)

$(BINDIR)/LevelEditor: $(SOURCES) src/main.o
		$(CXX) -o $(BINDIR)/LevelEditor $(SOURCES) src/main.o $(LINK) 

tests/UnitTests: $(SOURCES) src/unitTests.o
		$(CXX) -o tests/UnitTests $(SOURCES) src/UnitTests.o $(LINK)

clean:		src.clean tests.clean
		rm -f $(BINDIR)/LevelEditor

%.clean:
		rm -f $*/*.o

%.o:		%.cpp
		$(CXX) $(INCLUDE) $(CFLAGS) -o $*.o $*.cpp
