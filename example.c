#include <stdio.h>
#include <slimage.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG

int main(int argc, char **argv)
{
    clock_t start, end;
    Frame frame;
    if (!argv[1])
    {   
        argv[1] = "D:\\Code\\C\\slmedia/data/images/jpeg/IMG_0135.JPG";
    }
    start = clock();
    if (!(frame = ImageRead(argv[1], NULL, 0))) 
    {
        fprintf(stderr, "usage: %s [The absolute path for image]\n", argv[0]);
        return 0;
    }
    end = clock();
    ImageShow(frame);
    slFrameDeallocator(frame);
    
    exit(EXIT_SUCCESS);
}
