#include <GL/glew.h>
#include <SDL.h>
#ifdef _WIN32
#include <Windows.h>
#endif

// Debug.cpp
extern void GLDebug_Init();
// Shaders.cpp
extern bool GL_LoadShader(GLuint &uiShader, GLenum ShaderType, const GLchar *p_cShader);
extern bool GL_LoadShaders(GLuint &uiShader, GLuint uiVertexShader, GLuint uiFragmentShader);
extern bool GL_LoadShaderFile(GLuint &uiShader, GLenum ShaderType, const char *p_cFileName, int iFileID);

// Declare window variables
int g_iWindowWidth = 1280;
int g_iWindowHeight = 1024;
bool g_bWindowFullscreen = true;
// Declare OpenGL variables
GLuint g_uiVAO;
GLuint g_uiVBO;
GLuint g_uiMainProgram;

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

#ifdef _DEBUG
    // Initialise debug call-back
    GLDebug_Init();
#endif

#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    // Set up initial GL attributes
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    //// Create vertex shader
    // GLuint uiVertexShader;
    // if(!GL_LoadShader(uiVertexShader, GL_VERTEX_SHADER, p_cVertexShaderSource))
    //     return false;

    // Create vertex shader
    GLuint uiVertexShader;
    if (!GL_LoadShaderFile(uiVertexShader, GL_VERTEX_SHADER, "tut1Vert.glsl", 100))
        return false;

    // Create fragment shader
    GLuint uiFragmentShader;
    if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "tut1Frag.glsl", 200))
        return false;

    // // Create vertex shader
    // GLuint uiVertexShader;
    // if (!GL_LoadShader(uiVertexShader, GL_VERTEX_SHADER, p_cVertexShaderSource))
    //     return false;

    // // Create fragment shader
    // GLuint uiFragmentShader;
    // if (!GL_LoadShader(uiFragmentShader, GL_FRAGMENT_SHADER, p_cFragmentShaderSource))
    //     return false;

    // Create program
    if (!GL_LoadShaders(g_uiMainProgram, uiVertexShader, uiFragmentShader))
        return false;

    // Clean up unneeded shaders
    glDeleteShader(uiVertexShader);
    glDeleteShader(uiFragmentShader);

    // Create a Vertex Array Object
    glGenVertexArrays(1, &g_uiVAO);
    glBindVertexArray(g_uiVAO);

    // Create VBO data
    // Triangle pointing up
    // {
    //     -0.5f, -0.5f,
    //     0.5f, -0.5f,
    //     0.0f, 0.5f};
    // Triangle pointing down
    GLfloat fVertexData[] =
        {0.0f, -0.5f,
         0.5f, 0.5f,
         -0.5f, 0.5f};

    // Create Vertex Buffer Object
    glGenBuffers(1, &g_uiVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fVertexData), fVertexData, GL_STATIC_DRAW);

    // Specify location of data within buffer
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Specify program to use
    glUseProgram(g_uiMainProgram);

    return true;
}

int GL_Render()
{
    // Clear the render output and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specify VAO to use
    glBindVertexArray(g_uiVAO);

    // Draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return true;
}

void GL_Quit()
{
    // Release the shader program
    glDeleteProgram(g_uiMainProgram);

    // Delete VBO and VAO
    glDeleteBuffers(1, &g_uiVBO);
    glDeleteVertexArrays(1, &g_uiVAO);
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
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Use OpenGL 4.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // SDL_GL_CONTEXT_PROFILE_CORE using compatibility profile to make it work

    // Turn on double buffering with a 24bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Get desktop resolution
    SDL_DisplayMode CurrentDisplay;
    g_bWindowFullscreen &= (SDL_GetCurrentDisplayMode(0, &CurrentDisplay) == 0);

    // Update screen resolution
    g_iWindowWidth = (g_bWindowFullscreen) ? CurrentDisplay.w : g_iWindowWidth;
    g_iWindowHeight = (g_bWindowFullscreen) ? CurrentDisplay.h : g_iWindowHeight;

    // Create a SDL window
    Window = SDL_CreateWindow("AGT Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              g_iWindowWidth, g_iWindowHeight,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (g_bWindowFullscreen * SDL_WINDOW_FULLSCREEN));
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
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Can't set current OpenGL ES context: %s\n", SDL_GetError());
        return 1;
        ;
    }

    // Enable VSync
    SDL_GL_SetSwapInterval(-1);

    // Initialize OpenGL
    if (GL_Init(g_iWindowWidth, g_iWindowHeight))
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
