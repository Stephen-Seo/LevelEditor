
#include <cstring>
#include <iostream>
#include <string>
#include <stdexcept>

#include "game.hpp"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__WIN32__)
#include <Windows.h>
#endif

void pUsage()
{
    std::cout << "\nUsage:\n";
    std::cout << "  [-p prefix] : prefix of files to use/edit.\n";
    std::cout << "  [-i imageFile]  : level tile file.\n";
}

int main(int argc, char** argv) {

    #if defined(_WIN32)
    FreeConsole();
    #elif defined(__WIN32__)
    FreeConsole();
    #endif

    std::string oFile = "";
    std::string imgFile = "";

#if defined(__APPLE__)
#else
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
#endif

    if(oFile != "" && imgFile != "")
        std::cout << "Using sheet \"" << imgFile << "\"\nUsing output \"" << oFile << "\"\n";

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

