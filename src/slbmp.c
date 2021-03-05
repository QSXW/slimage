#include <slbmp.h>

Frame BMPRead(const Stream stream)
{
    Frame frame;
    BMP *bmp;
    
    bmp = (BMP *)stream->_mbuf;
    
    if (bmp->bitsPerPixel &&  bmp->bitsPerPixel != 0x18)
    {
        printf("\033[0;31m Unsupported BMP file format.\033[0m\n");
        // Write Error
    }
    BMPDecoder(bmp, &frame);

    return frame;
}

INT32 BMPDecoder(const struct _SLBMP *bmp, Frame *frame) 
{
    BYTE  *redChannel;
    BYTE  *greenChannel;
    BYTE  *blueChannel;
    DWORD  channelSize;

    BYTE  *colour;
    INT32  complement;
    DWORD width;
    DWORD height;
    size_t x, y;

    width = bmp->width;
    height = bmp->height;
    complement = width % 4;
    channelSize = width * height;
    slAssert(
        (*frame = slFrameAllocator(width, height, 3, SLFRAME_DTYPE_BYTE, NULL)),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        -1
    );
    redChannel = (*frame)->data + channelSize - width;
    greenChannel = redChannel + channelSize;
    blueChannel = greenChannel + channelSize;
    
    colour = (((BYTE *)bmp) + bmp->offset);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            blueChannel[x]  = *colour++;
            greenChannel[x] = *colour++;
            redChannel[x]   = *colour++;
        }
        blueChannel -= x;
        greenChannel -= x;
        redChannel -= x;
        colour += complement;
    }

    return 0;
}

INT32 BMPEncoder(Frame frame, CharSequence **charSequence)
{
    BMP *bmp;
    size_t dataSize, complement, fileSize;
    BYTE *redChannel, *greenChannel, *blueChannel, *colour;
    INT32 x, y, width, height;

    complement = frame->cols % 4;
    dataSize = (frame->cols + complement) * frame->row * 3;
    fileSize = sizeof(BMP) + dataSize;
    if ((bmp = (BMP *)malloc(fileSize)))
    {
        width = frame->cols;
        height = frame->row;

        memset(bmp, 0x0, fileSize);
        bmp->identifer            = 0x4d42;
        bmp->fileSize             = fileSize;
        bmp->offset               = sizeof(BMP);
        bmp->informationSize      = 0x28;
        bmp->width                = width;
        bmp->height               = height;
        bmp->planes               = 0x1;
        bmp->bitsPerPixel         = 0x18;
        bmp->imageSize            = dataSize;

        if (frame->dims == 1)
        {
            redChannel = greenChannel = blueChannel = frame->data + frame->size - width;
        }
        else if (frame->dims == 3)
        {
            redChannel   = frame->data + frame->size - width;
            greenChannel = redChannel + frame->size;
            blueChannel  = greenChannel + frame->size;
        }
        else
        {
            // #Incorrect RGB frame   
        }
        colour = bmp->data;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                *colour++ = blueChannel[x];
                *colour++ = greenChannel[x];
                *colour++ = redChannel[x];
            }
            blueChannel  -= x;
            greenChannel -= x;
            redChannel   -= x;
            colour += complement;
        }
        if (!(*charSequence))
        {
            *charSequence = CharSequenceAllocator();
        }
        CharSequencePushStack(charSequence, bmp, fileSize);
    }
    else
    {
        // #1114
    }

    return 0;
}

