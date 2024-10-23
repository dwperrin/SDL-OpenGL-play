#include <SDL.h>
#include <GL/glew.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

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

    /* initialize viewing values */
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    /* select clearing (background) color */
    glClearColor(0.0, 0.0, 0.0, 0.0);

    return 1;
}

int GL_Render()
{
    /* clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT);
    /* draw white polygon (rectangle) with corners at
     * (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
     */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glEnd();
    /* don’t wait!
     * start processing buffered OpenGL routines
     */
    glFlush();
    return 1;
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // SDL_GL_CONTEXT_PROFILE_CORE using compatibility profile to make it work

    // Turn on double buffering with a 24bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Declare window variables
    int width = 1280;
    int height = 1024;
    bool fullscreen = false;

    // Create our window
    Window = SDL_CreateWindow("Hello", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (fullscreen * SDL_WINDOW_FULLSCREEN));

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
