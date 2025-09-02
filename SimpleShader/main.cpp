//============================================================================
//	Johns Hopkins University Whiting School of Engineering
//	605.667 Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    SimpleShader/main.cpp
//	Purpose: OpenGL shader program to draw a set of "wide lines"
//
//============================================================================

#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"

#include "SimpleShader/line_node.hpp"
#include "SimpleShader/line_shader_node.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace cg
{

// Simple logging function, should be defined in the cg namespace
void logmsg(const char *message, ...)
{
    // Open file if not already opened
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("SimpleShader.log", "w"); }

    va_list arg;
    va_start(arg, message);
    vfprintf(lfile, message, arg);
    putc('\n', lfile);
    fflush(lfile);
    va_end(arg);
}

} // namespace cg

// Root of the scene graph
std::shared_ptr<cg::SceneNode> g_scene_root;

// Scene state
cg::SceneState g_scene_state;

// SDL Objects
SDL_Window       *g_sdl_window = nullptr;
SDL_GLContext     g_gl_context;
constexpr int32_t DRAWS_PER_SECOND = 30;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * Reshape function. Load a 2-D orthographic projection matrix using the
 * window width and height so we can directly take window coordinates and
 * send to OpenGL. Note that this function MUST be called at start-up.
 * @param  width  Window width
 * @param  height Window height
 */
void reshape(int32_t width, int32_t height)
{
    // Set a simple 4x4 matrix (use an array until we develop the Matrix4x4 class)
    g_scene_state.ortho[0] = 2.0f / static_cast<float>(width);
    g_scene_state.ortho[1] = 0.0f;
    g_scene_state.ortho[2] = 0.0f;
    g_scene_state.ortho[3] = 0.0f;
    g_scene_state.ortho[4] = 0.0f;
    g_scene_state.ortho[5] = -2.0f / static_cast<float>(height);
    g_scene_state.ortho[6] = 0.0f;
    g_scene_state.ortho[7] = 0.0f;
    g_scene_state.ortho[8] = 0.0f;
    g_scene_state.ortho[9] = 0.0f;
    g_scene_state.ortho[10] = 1.0f;
    g_scene_state.ortho[11] = 0.0f;
    g_scene_state.ortho[12] = -1.0f;
    g_scene_state.ortho[13] = 1.0f;
    g_scene_state.ortho[14] = 0.0f;
    g_scene_state.ortho[15] = 1.0f;

    // Update the viewport
    glViewport(0, 0, width, height);
}

/**
 * Display function
 */
void display()
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT);

    g_scene_root->draw(g_scene_state);
    cg::check_error("After Draw");

    // Swap buffers
    SDL_GL_SwapWindow(g_sdl_window);
}

/**
 * Handle Events function.
 */
bool handle_events()
{
    SDL_Event e;
    bool      cont_program = true;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: cont_program = false; break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                reshape(e.window.data1, e.window.data2);
                break;

            case SDL_EVENT_QUIT: cont_program = false; break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN: break;

            case SDL_EVENT_KEY_DOWN:
                switch(e.key.key)
                {
                    case SDLK_ESCAPE: cont_program = false; break;
                    case SDLK_SPACE: break;
                    case SDLK_M: break;
                    default: break;
                }
                break;
            default: break;
        }
    }
    return cont_program;
}

/**
 * Create the scene.
 */
void create_scene()
{
    g_scene_root = std::make_shared<cg::SceneNode>();

    // Create shader and get uniform locations
    auto line_shader = std::make_shared<cg::LineShaderNode>();

    if(!line_shader->create("SimpleShader/lines.vert", "SimpleShader/lines.frag") ||
       !line_shader->get_locations())
    {
        exit(-1);
    }

    // Create a line node using a set of vertices that spell 'Computer'
    std::vector<cg::Point2> pts_computer = {
        {240.0f, 150.0f}, {230.0f, 140.0f}, {210.0f, 140.0f}, {200.0f, 150.0f},
        {200.0f, 190.0f}, {210.0f, 200.0f}, {230.0f, 200.0f}, {240.0f, 190.0f}, // C
        {250.0f, 170.0f}, {250.0f, 190.0f}, {260.0f, 200.0f}, {270.0f, 190.0f},
        {270.0f, 170.0f}, {260.0f, 160.0f}, {250.0f, 170.0f}, {260.0f, 160.0f},
        {270.0f, 170.0f}, // o
        {275.0f, 160.0f}, {280.0f, 170.0f}, {280.0f, 200.0f}, {280.0f, 170.0f},
        {285.0f, 160.0f}, {290.0f, 170.0f}, {290.0f, 200.0f}, {290.0f, 170.0f},
        {295.0f, 160.0f}, {300.0f, 170.0f}, {300.0f, 200.0f}, {310.0f, 200.0f}, // m
        {310.0f, 230.0f}, {310.0f, 160.0f}, {325.0f, 160.0f}, {330.0f, 170.0f},
        {330.0f, 190.0f}, {325.0f, 200.0f}, {310.0f, 200.0f}, {330.0f, 200.0f}, // p
        {340.0f, 190.0f}, {340.0f, 160.0f}, {340.0f, 190.0f}, {350.0f, 200.0f},
        {360.0f, 190.0f}, {360.0f, 160.0f}, {360.0f, 190.0f}, {370.0f, 200.0f}, // u
        {380.0f, 190.0f}, {380.0f, 150.0f}, {370.0f, 150.0f}, {390.0f, 150.0f},
        {380.0f, 150.0f}, {380.0f, 140.0f}, {380.0f, 190.0f}, {390.0f, 200.0f}, // t
        {420.0f, 160.0f}, {400.0f, 160.0f}, {400.0f, 200.0f}, {430.0f, 200.0f}, // e
        {430.0f, 160.0f}, {450.0f, 160.0f}, {445.0f, 180.0f}, {450.0f, 200.0f},
        {460.0f, 200.0f} // r
    };

    auto lines_computer = std::make_shared<cg::LineNode>(
        pts_computer, cg::Color4(0.7f, 0.1f, 0.1f, 1.0f), line_shader->get_position_loc());
    line_shader->add_child(lines_computer);

    // Create a line node using a set of vertices that spell 'Graphics'
    std::vector<cg::Point2> pts_graphics = {
        {200.0f, 300.0f}, {230.0f, 260.0f}, {230.0f, 240.0f}, {220.0f, 240.0f},
        {220.0f, 260.0f}, {230.0f, 270.0f}, {240.0f, 270.0f}, {230.0f, 300.0f},
        {210.0f, 300.0f}, {200.0f, 280.0f}, {230.0f, 280.0f}, // G
        {250.0f, 300.0f}, {250.0f, 300.0f}, {250.0f, 260.0f}, {270.0f, 260.0f},
        {265.0f, 280.0f}, {270.0f, 300.0f}, {300.0f, 300.0f}, // r
        {300.0f, 260.0f}, {285.0f, 260.0f}, {280.0f, 270.0f}, {280.0f, 290.0f},
        {285.0f, 300.0f}, {310.0f, 300.0f}, // a
        {310.0f, 330.0f}, {310.0f, 260.0f}, {325.0f, 260.0f}, {330.0f, 270.0f},
        {330.0f, 290.0f}, {325.0f, 300.0f}, {310.0f, 300.0f}, {340.0f, 300.0f}, // p
        {340.0f, 240.0f}, {340.0f, 260.0f}, {355.0f, 260.0f}, {360.0f, 270.0f},
        {360.0f, 300.0f}, {370.0f, 300.0f}, // h
        {370.0f, 260.0f}, {370.0f, 300.0f}, // i
        {380.0f, 290.0f}, {380.0f, 270.0f}, {385.0f, 260.0f}, {395.0f, 260.0f},
        {400.0f, 270.0f}, {395.0f, 260.0f}, {385.0f, 260.0f}, {380.0f, 270.0f},
        {380.0f, 290.0f}, {385.0f, 300.0f}, {400.0f, 300.0f}, // c
        {420.0f, 260.0f}, {430.0f, 270.0f}, {430.0f, 290.0f}, {420.0f, 300.0f},
        {400.0f, 300.0f}, {440.0f, 300.0f} // s
    };

    auto lines_graphics = std::make_shared<cg::LineNode>(
        pts_graphics, cg::Color4(0.1f, 0.1f, 0.7f, 1.0f), line_shader->get_position_loc());

    line_shader->add_child(lines_graphics);

    // Create scene graph
    g_scene_root->add_child(line_shader);
    line_shader->add_child(lines_computer);
    line_shader->add_child(lines_graphics);
}

/**
 * Main
 */
int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);
    std::cout << "Keyboard Controls:\n";
    std::cout << "ESC - Exit program\n";

    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Initialize display mode and window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_PropertiesID props = SDL_CreateProperties();
    if(props == 0)
    {
        std::cout << "Error creating SDL Window Properties: " << SDL_GetError() << '\n';
        exit(1);
    }

    SDL_SetStringProperty(
        props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "SimpleShader by Brian Russin");
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 100);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 100);

    g_sdl_window = SDL_CreateWindowWithProperties(props);

    if(g_sdl_window == nullptr)
    {
        std::cout << "Error initializing SDL Window" << SDL_GetError() << '\n';
        exit(1);
    }

    g_gl_context = SDL_GL_CreateContext(g_sdl_window);

    if(g_gl_context == nullptr)
    {
        std::cout << "Error Getting OpenGL Context: " << SDL_GetError() << '\n';
        exit(1);
    }

    std::cout << "OpenGL  " << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

#if BUILD_WINDOWS
    int32_t glew_init_result = glewInit();
    if(GLEW_OK != glew_init_result)
    {
        std::cout << "GLEW Error: " << glewGetErrorString(glew_init_result) << '\n';
        exit(EXIT_FAILURE);
    }
#endif

    // Set OpenGL defaults
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    reshape(640, 480);

    // Create the scene
    create_scene();
    cg::check_error("create_scene");

    g_scene_root->print_graph();

    while(handle_events())
    {
        display();
        sleep(DRAW_INTERVAL_MILLIS);
    }

    SDL_GL_DestroyContext(g_gl_context);
    SDL_DestroyWindow(g_sdl_window);
    SDL_Quit();

    return 0;
}
