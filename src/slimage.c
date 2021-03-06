#include "slimage.h"
#include "slshader.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

INT32 ImageFormat(const DWORD *formatID) 
{
    INT32 sign;
    const BYTE *type;
    
    type = (const BYTE *)formatID;
    sign = 0x0;
    switch (*type) 
    {
        case SLIMAGE_ID_BMP_BEG:
            ASSERT_IMAGE_ID(type, SLIMAGE_ID_BMP_END, sign, SLIMAGE_FORMAT_BMP);
            break;

        case SLIMAGE_ID_JPEG_BEG:
            ASSERT_IMAGE_ID(type, SLIMAGE_ID_JPEG_END, sign, SLIMAGE_FORMAT_JPEG);
            break;

        default:
            sign = SLIMAGE_FORMAT_UNRECOGNIZED;
            break;
    }

    return sign;
}

Frame ImageRead(const char *filename, const BYTE *sequence, size_t size)
{
    Frame   frame;
    Stream stream;
    INT32 format;

    frame = NULL;
    stream = (filename) ? NewStream(filename, READABLE | CACHED) : NewStreamFromeSequence(sequence, size);
    if ((stream))
    {
        format = ImageFormat((DWORD *)slStreamCurrentPosition(stream));
        switch (format)
        {
            case SLIMAGE_FORMAT_JPEG:
                frame = JpegRead(stream);
                break;
            
            case SLIMAGE_FORMAT_BMP:
                frame = BMPRead(stream);
                break;
                
            default:
                printf("Unable to recognize the format of the input image.");
                break;
            }
    }
    CloseStream(stream);
    return frame;
}

INT32 ImageWrite(const char *filePath, Frame frame, INT8 monochorome, INT32 format)
{
    FILE *fp;
    CharSequence *cs;
    DWORD channels;

    if ((fp = fopen(filePath, "wb")))
    {
        channels = frame->dims;
        if (monochorome) { frame->dims = 1; }
        if (!(cs = CharSequenceAllocator()))
        {
            return SLEXIT_FAILURE;
        }
        switch (format)
        {
        case SLIMAGE_FORMAT_BMP:
            BMPEncoder(frame, &cs);
            break;

        default:
            break;
        }
        frame->dims = channels;
        fwrite(cs->data, cs->size, 1, fp);
        fclose(fp);
    }
    
    return 0;
}

Frame ImageResize(Frame src, float widthRatio, float heightRatio, INT32 width, INT32 height, INT32 method)
{
    Frame dst;
    INT32 (*interplotatefuncptr)(Frame, Frame, float, float);

    dst = NULL;
    if (width && height)
    {
        widthRatio  = (float)width / (float)src->cols;
        heightRatio = (float)height / (float)src->row;
    }
    if (widthRatio == 1 && heightRatio == 1)
    {
        // Copy src frame to dst frame and return;
    }
    else
    {
        slAssert(
            dst = slFrameAllocator((INT32)floor(((double)(src->cols) * (double)widthRatio)), (INT32)floor(((double)(src->row) * (double)heightRatio)), src->dims, src->dtype, NULL),
            SLEXCEPTION_NULLPOINTER_REFERENCE,
            NULL
        );
        switch(method)
        {
        case SLIMAGE_NEAREST:
            interplotatefuncptr = slNearestInterpolateRGB;
            break;
        default:
            interplotatefuncptr = slBicubicConvolutionInterpolateRGB;
            break;
        }
        interplotatefuncptr(src, dst, widthRatio, heightRatio);
    }
    
    return dst;
}

INT32 ImageShow(Frame frame)
{
    GLFWwindow* window;
    GLFWmonitor **monitor;
    const GLFWvidmode *mode;
    Frame RGBFrame;
    GLuint programID;
    unsigned int texture;
    int count, width, height;

    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f,  -1.0f, 0.0f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
        -1.0f,  -1.0f, 0.0f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
        -1.0f,   1.0f, 0.0f,   1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    
    count = 1;
    monitor = glfwGetMonitors(&count);
    mode = glfwGetVideoMode(monitor[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    width  = frame->cols;
    height = frame->row;

    if (width > mode->width && slMax(width, height) == width)
    {
        width = mode->width - 120;
        height = (int)((float)height * ((float)width / (float)frame->cols));
    }
    else if (height > mode->height && slMax(width, height) == height)
    {
        height = mode->height - 120;
        width =  (int)((float)width * ((float)height / (float)frame->row));
    }

    RGBFrame = slDefaultToRGBA(frame);

    window = glfwCreateWindow(width, height, "Imagination", NULL, NULL);
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        exit(EXIT_FAILURE);
    }

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture 
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RGBFrame->cols, RGBFrame->row, 0, GL_RGBA, GL_UNSIGNED_BYTE, RGBFrame->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    slFrameDeallocator(RGBFrame);
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders(NULL, NULL);

    glUniform1i(glGetUniformLocation(programID, "imageTexture"), 0);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glUseProgram(programID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
