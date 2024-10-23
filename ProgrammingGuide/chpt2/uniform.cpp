#include <stdio.h>
#include <stdlib.h>

#include "vgl.h"
#include "LoadShaders.h"
#include <exception>

size_t TypeSize(GLenum type)
{
    size_t size;

#define CASE(Enum, Count, Type)      \
    case Enum:                       \
        size = Count * sizeof(Type); \
        break

    switch (type)
    {
        CASE(GL_FLOAT, 1, GLfloat);
        CASE(GL_FLOAT_VEC2, 2, GLfloat);
        CASE(GL_FLOAT_VEC3, 3, GLfloat);
        CASE(GL_FLOAT_VEC4, 4, GLfloat);
        CASE(GL_INT, 1, GLint);
        CASE(GL_INT_VEC2, 2, GLint);
        CASE(GL_INT_VEC3, 3, GLint);
        CASE(GL_INT_VEC4, 4, GLint);
        CASE(GL_UNSIGNED_INT, 1, GLuint);
        CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
        CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
        CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
        CASE(GL_BOOL, 1, GLboolean);
        CASE(GL_BOOL_VEC2, 2, GLboolean);
        CASE(GL_BOOL_VEC3, 3, GLboolean);
        CASE(GL_BOOL_VEC4, 4, GLboolean);
        CASE(GL_FLOAT_MAT2, 4, GLfloat);
        CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
        CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
        CASE(GL_FLOAT_MAT3, 9, GLfloat);
        CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
        CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
        CASE(GL_FLOAT_MAT4, 16, GLfloat);
        CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
        CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
#undef CASE
    default:
        fprintf(stderr, "Unknown type: 0x%x\n", type);
        exit(EXIT_FAILURE);
        break;
    }
    return size;
}

const GLuint NumVertices = 6;

void uniformLoadShaders()
{

    GLuint program;

    glClearColor(1, 0, 0, 1);

    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "uniform.vert"},
        {GL_FRAGMENT_SHADER, "uniform.frag"},
        {GL_NONE, NULL}};

    program = LoadShaders(shaders);
    glUseProgram(program);

    /* Initialize uniform values in uniform block "Uniforms" */
    GLuint uboIndex;
    GLint uboSize;
    GLuint ubo;
    unsigned char *buffer;

    // Find the uniform buffer index for "Uniforms", and
    // determine the block's sizes
    uboIndex = glGetUniformBlockIndex(program, "Uniforms");

    glGetActiveUniformBlockiv(program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    buffer = (unsigned char *)malloc(uboSize);

    // Create the buffer object and copy the data
    if (buffer == NULL)
    {
        fprintf(stderr, "Unable to allocate buffer\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        enum
        {
            Translation,
            Scale,
            Rotation,
            Enabled,
            NumUniforms
        };

        /* Values to be stored in the buffer object */
        GLfloat scale = 0.5f;
        GLfloat translation[] = {0.1f, 0.1f, 0.0f};
        GLfloat rotation[] = {90, 0.0f, 0.0f, 1.0f};
        GLboolean enabled = GL_TRUE;

        /* Sind we klnow the names of the uniforms
        ** in our block, make an array of those values */
        const char *names[NumUniforms] = {
            "translation",
            "scale",
            "rotation",
            "enabled"};

        /* Query the necessary attributes to determine
        ** where in the buffer we should write
        ** the values */
        GLuint indices[NumUniforms];
        GLint size[NumUniforms];
        GLint offset[NumUniforms];
        GLint type[NumUniforms];

        glGetUniformIndices(program, NumUniforms, (const GLchar **)names, indices);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_OFFSET, offset);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_SIZE, size);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_TYPE, type);

        try
        {
            /* Copy the uniform values into the buffer */
            memcpy(buffer + offset[Scale], &scale, size[Scale] * TypeSize(type[Scale]));
            memcpy(buffer + offset[Translation], &translation, size[Translation] * TypeSize(type[Translation]));
            memcpy(buffer + offset[Rotation], &rotation, size[Rotation] * TypeSize(type[Rotation]));
            memcpy(buffer + offset[Enabled], &enabled, size[Enabled] * TypeSize(type[Enabled]));
        }
        catch (const std::exception &e)
        {
            fprintf(stderr, "Error: %s\n", e.what());
        }

        /* Create the uniform buffer object, initilize its storage, and associate
        ** it with the shader program */
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer, GL_STATIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);
    }
}

int GL_Init(int width, int height)
{
    // Initialize GLEW
    glewExperimental = GL_TRUE; // Allow experimental extensions
    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize GLEW: %s\n", glewGetErrorString(GlewError));
        return false;
    }

    return true;
}

int GL_Render()
{
    static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, black);
    uniformLoadShaders();
    return true;
}

void GL_Quit()
{
}

#ifdef _WIN32
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int)
#else
int main(int argc, char **argv)
#endif
{
    // Declare SDL window and OpenGL context
    SDL_Window *Window = NULL;
    SDL_GLContext Context = NULL;
    int status = 0;

    // Initialize SDL. SDL_Init will return -1 if it fails.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error initializing SDL: %s", SDL_GetError());
        system("pause");
        // End the program
        return 1;
    }

    // Use OpenGL 4.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // SDL_GL_CONTEXT_PROFILE_CORE using compatibility profile to make it work

    // Turn on double buffering with a 24bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Declare window variables
    int width = 1280;
    int height = 1024;
    bool fullscreen = false;

    // Create our window
    Window = SDL_CreateWindow("Triangles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (fullscreen * SDL_WINDOW_FULLSCREEN));

    // Make sure creating the window succeeded
    if (Window == NULL)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a OpenGL Context
    Context = SDL_GL_CreateContext(Window);
    if (Context == NULL)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window);
        SDL_Quit();
        return 1;
    }

    status = SDL_GL_MakeCurrent(Window, Context);
    if (status < 0)
    {
        fprintf(stderr, "Can't set current OpenGL ES context: %s\n", SDL_GetError());
        exit(-1);
    }

    // Enable VSync
    // SDL_GL_SetSwapInterval(-1);

    // Initialize OpenGL
    if (GL_Init(width, height))
    {
        // Start the program message pump
        SDL_Event Event;
        bool bQuit = false;
        while (!bQuit)
        {
            // Poll SDL for buffered events
            while (SDL_PollEvent(&Event))
            {
                if (Event.type == SDL_QUIT)
                    bQuit = true;
                else if (Event.type == SDL_KEYDOWN)
                {
                    if (Event.key.keysym.sym == SDLK_ESCAPE)
                        bQuit = true;
                }
            }

            // Render the scene
            GL_Render();

            // Swap the back-buffer and present it
            SDL_GL_SwapWindow(Window);
        }

        // Delete any created GL resources
        GL_Quit();
    }

    // Delete the OpenGL context, SDL window and shutdown SDL
    SDL_GL_DeleteContext(Context);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    // End the program
    return 0;
}
