#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "slimage.h"
#define DEBUG

int main(int argc, char **argv)
{
    Frame frame;
    if (!argv[1])
    {   
        argv[1] = "../data/images/jpeg/lol1.jpg";
    }
    if (!(frame = ImageRead(argv[1], NULL, 0))) 
    {
        fprintf(stderr, "usage: %s [The absolute path for image]\n", argv[0]);
        return 0;
    }
    ImageShow(frame);
    slFrameDeallocator(frame);
    
    exit(EXIT_SUCCESS);
}
