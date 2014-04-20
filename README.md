Note: the program is meant to be started from the terminal/cmd/powershell

Another Note: the program is licensed under the MIT license. It can
be found in the src folder. the font used by this program is licensed
under the Apache License Version 2.0. That can be found in the resources
folder.

# Compiling

The program requires SFML 2.1 and uses some C++11 code.
If you have not installed SFML 2.1 to your system, you may alter the
Makefile using the -I and -L options so that your compiler can find
the required lib and include directories.

# Required Files

The level editor requires at least two files to function:
A tile sheet image (preferably png), and a key file.

The image may be named anything, but the program expects the keyfile (and
other generated files) to follow a specific syntax.
The files in question should be named:  
\[prefix\]\_key.txt  
\[prefix\]\_level\_layer0.txt  
\[prefix\]\_level\_layer1.txt  
\[prefix\]\_level\_layer2.txt  
\[prefix\]\_waypoint.txt  
\[prefix\]\_obstacles.txt  
\[prefix\]\_entities.txt  
\[prefix\]\_warps.txt  
\[prefix\]\_door.txt  
\[prefix\]\_doorkey.txt


(\[prefix\] is defined at runtime by the user.)

## Keyfile format

The keyfile is a text file.
The very first line of the keyfile must be an integer that defines the size
(width and height, so it expects them to be square) of the tiles in the tile
sheet.
The rest of the keyfile should have a char symbol in the same place as the
tile to match with in the tile sheet. One cannot assign a tile to a space,
as spaces are ignored. Blank spots in the tilesheet can be skipped using
spaces.

## Waypoint format

The waypoint text file has a symbol followed by adjacent waypoints
(represented by symbols) on that line. The program will parse this
adjacency list until a '#' is reached. Following the '#' is a literal
map of characters that define where the waypoints are. Spaces denote
empty space while symbols denote a specific waypoint.

## Entites format

The entity text file has a symbol representing an entity followed by symbols
that represent connected waypoints. This continues line per line until '#'
is reached. Following the '# is a literal map of characters that define where
the entities are. Spaces denote empty space while symbols denote entities.

## Level/Obstacles format

The contents of these text files is a literal map of characters that define
where the tiles/obstacles are. Spaces denote empty space while symbols denote
a specific tile/obstacle (in the obstacles file, obstacles are denoted with
'o').

# Usage

run the LevelEditor binary in terminal/cmd/powershell.

needs options:  
-p \[prefix\]  
-i \[tilesheet\]

\[prefix\] determines the prefix of the files to open.
The only file required to run is \[prefix\]\_key.txt.

EXAMPLE: If your key file is "mapTest_key.txt",
then your prefix should be "mapTest".

\[tilesheet\] determines the tilesheet to open.
It can be "tiles.png" for example.

# Actual Usage

First select the map window.
The title of the small window tells you what "mode" you are in.

Arrow keys move the view.  
Period and Comma (< and >) changes the mode.  
Enter saves the map.  
Left click places an object.  
Right click deletes the object.  
L (in waypoint mode) links a waypoint to another waypoint.  
L (in entity\_waypoint\_connect mode) links an entity to a waypoint.  
(one must select an entity first, then a waypoint)  
A/D (in entities mode) changes the symbol that is drawn.  
L (in warps mode) picks a warp or sets a location for the selected warp to go to.  
A/D (in warps mode) changes the symbol that is drawn.  
L (in key mode) picks a key or sets a door that the key unlocks.  
A/D (in door/key mode) changes the symbol that is drawn.


In layer0/layer1/layer2 mode, you place the tiles in the layer specified.  
In waypoint mode, you place waypoints.  
Hover over a waypoint, hit 'L', hover over another waypoint, hit 'L' again to set/unset waypoint adjacency.  
In obstacles mode, you place obstacles (determines what the player cannot walk through).  
In entities mode, you can place multiple entities (represented by symbols) into the world.  
In entity\_waypoint\_connect mode, you connect entities to waypoints. If connecting with an entity
that has multiple instances of it, the editor may snap the lines to one of the duplicate entities,
but that type of entity is connected to all the waypoints set to that type.  
In warps mode, one can create warps that have a designated location for the warp to go to.  
In door mode, one creates door entities.  
In key mode, one creates key entities and is able to link keys to doors.

When saving, the program will overwrite the following files:  
\[prefix\]\_level\_layer0.txt  
\[prefix\]\_level\_layer1.txt  
\[prefix\]\_level\_layer2.txt  
\[prefix\]\_waypoint.txt  
\[prefix\]\_obstacles.txt  
\[prefix\]\_entities.txt  
\[prefix\]\_warps.txt  
\[prefix\]\_door.txt  
\[prefix\]\_doorkey.txt


# Exiting the program

Close the map window. The program will then close both windows and exit.
