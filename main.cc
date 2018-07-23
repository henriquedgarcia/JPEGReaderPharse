/**
* Let's Build a JPEG Decoder: Segment lister
* Entry point [main.cpp]
* Imran Nazar, Jan 2013
*/

#include "jpeg.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <file.jpg>\n", argv[0]);
        return 1;
    }

    std::string in = std::string(argv[1]);

    JPEG j(in);
    return 0;
}

