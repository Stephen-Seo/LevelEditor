
#include <cstring>
#include <iostream>
#include <string>

#include "game.hpp"

void pUsage()
{
    std::cout << "\nUsage:\n";
    std::cout << "  [-o outputFile] : output level file." <<
        "If exists, the program will edit it.\n";
    std::cout << "  [-i imageFile]  : level tile file.\n";
}

int main(int argc, char** argv) {

    std::string oFile;
    std::string imgFile;

    while(--argc > 0)
    {
        argv++;
        if(!strncmp(*argv, "-o", 2) && argc > 1)
        {
            oFile = argv[1];
            --argc;
            ++argv;
        }
        else if(!strncmp(*argv, "-i", 2) && argc > 1)
        {
            imgFile = argv[1];
            --argc;
            ++argv;
        }
        else
        {
            pUsage();
            return 0;
        }
    }

    if(oFile == "" || imgFile == "")
    {
        std::cout << "\nError: outputFile and/or imageFile not specified!\n";
        pUsage();
        return 0;
    }

    Game game(oFile, imgFile);
    game.run();
    return 0;
}

