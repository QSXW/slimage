#include <stdio.h>
#include <slimage.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    Frame frame;
    if (!argv[1])
    {   
        argv[1] = "D:\\Code\\C\\slmedia/data/images/jpeg/lol1.jpg";
    }
    if (!(frame = ImageRead(argv[1]))) 
    {
        fprintf(stderr, "usage: %s [The absolute path for image]\n", argv[0]);
        return 0;
    }

    ImageShow(frame);
    slFrameDeallocator(frame);
    
    exit(EXIT_SUCCESS);
}
