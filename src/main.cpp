
#include <cstring>
#include <iostream>
#include <string>
#include <stdexcept>

#include "game.hpp"

void pUsage()
{
    std::cout << "\nUsage:\n";
    std::cout << "  [-p prefix] : prefix of files to use/edit.\n";
    std::cout << "  [-i imageFile]  : level tile file.\n";
}

int main(int argc, char** argv) {

    std::string oFile;
    std::string imgFile;

    while(--argc > 0)
    {
        ++argv;
        if(!strncmp(*argv, "-p", 2) && argc > 1)
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

    if(oFile == "")
    {
        std::cout << "\nError: outputFile not specified!\n";
        pUsage();
        return 0;
    }
    else if(imgFile == "")
    {
        std::cout << "\nError: tilesheet file not specified!\n";
        pUsage();
        return 0;
    }

    std::cout << "Using sheet " << imgFile << "\nUsing output " << oFile << "\n";

    try {
        Game game(oFile, imgFile);
        game.run();
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << "\n";
    }
    return 0;
}

