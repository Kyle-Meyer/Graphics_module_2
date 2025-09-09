//============================================================================
//	Johns Hopkins University Whiting School of Engineering
//	605.667  Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    Module2/main.cpp
//	Purpose: OpenGL shader program to draw points or lines based on mouse
//           clicks.
//
//============================================================================

#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"
#include "shader_support/glsl_shader.hpp"

// Include your custom node classes
#include "lineShaderNode.hpp"
#include "pointShaderNode.hpp"
#include "lineNode.hpp"
#include "pointNode.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>

namespace cg
{

// Simple logging function, should be defined in the cg namespace
void logmsg(const char *message, ...)
{
    // Open file if not already opened
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("Module2.log", "w"); }

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

// Your custom nodes (cast from g_scene_root children when needed)
std::shared_ptr<cg::PointShaderNode> g_point_shader;
std::shared_ptr<cg::LineShaderNode> g_line_shader;
std::shared_ptr<cg::PointNode> g_point_geometry;
std::shared_ptr<cg::LineNode> g_line_geometry;

// SDL Objects
SDL_Window* g_window = nullptr;
SDL_GLContext g_context;
int32_t g_window_width = 800;
int32_t g_window_height = 600;

// Global size variables
float g_lineWidth = 3.0f;
float g_pointSize = 6.0f;

constexpr int32_t DRAWS_PER_SECOND = 30;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

// Sleep function to help run a reasonable timer
void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * Reshape callback. Load a 2-D orthographic projection matrix using the
 * window width and height so we can directly take window coordinates and
 * send to OpenGL. Note that this callback will be called when the window
 * is opened.
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
    
    // Store window dimensions
    g_window_width = width;
    g_window_height = height;
}

/**
 * Display function
 */
void display()
{
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the scene graph
    if (g_scene_root) {
        g_scene_root->draw(g_scene_state);
    }

    // Swap buffers
    SDL_GL_SwapWindow(g_window);
}

/**
 * Window event handler.
 */
bool handle_window_event(const SDL_Event &event)
{
    bool cont_program = true;

    if (event.type == SDL_EVENT_WINDOW_RESIZED || 
        event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
        SDL_GetWindowSize(g_window, &g_window_width, &g_window_height);
        reshape(g_window_width, g_window_height);
    }

    return cont_program;
}

/**
 * Mouse event handler.
 */
bool handle_mouse_event(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && 
        event.button.button == SDL_BUTTON_LEFT) {
        
        float mouseX = event.button.x;
        float mouseY = event.button.y;

        // Convert screen coordinates to NDC
        float ndcX = (mouseX / g_window_width) * 2.0f - 1.0f;
        float ndcY = 1.0f - (mouseY / g_window_height) * 2.0f;

        // Add vertex to both point and line geometry
        if (g_point_geometry && g_line_geometry) {
            g_point_geometry->add_vertex(ndcX, ndcY);
            g_line_geometry->add_vertex(ndcX, ndcY);

            std::cout << "Click at: " << ndcX << ", " << ndcY << std::endl;
            std::cout << "Points: " << g_point_geometry->vertex_count() 
                      << ", Lines: " << g_line_geometry->vertex_count() << std::endl;
        }
    }

    return true;
}

/**
 * Keyboard event handler.
 */
bool handle_key_event(const SDL_Event &event)
{
    bool cont_program = true;

    if (event.type == SDL_EVENT_KEY_DOWN) {
        SDL_Keycode key = event.key.key;
                  
        if (key == SDLK_ESCAPE) {
            std::cout << "ESC pressed - Quitting application" << std::endl;
            cont_program = false;
        }
        else if (key >= SDLK_1 && key <= SDLK_9) {
            // Convert keycode to digit (1-9)
            int digit = key - SDLK_0;

            g_lineWidth = (float)digit;
            g_pointSize = g_lineWidth * 2.0f;
            
            // Update the actual objects with new sizes
            if (g_point_shader && g_line_geometry) {
                g_point_shader->set_point_size(g_pointSize);
                g_line_geometry->set_line_width(g_lineWidth);
            }
            
            std::cout << "Line width set to: " << g_lineWidth 
                      << ", point size set to: " << g_pointSize << std::endl;
        }
    }

    return cont_program;
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
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: cont_program = false; break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: cont_program = handle_window_event(e); break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: cont_program = handle_mouse_event(e); break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: cont_program = handle_key_event(e); break;
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
    // Create root node
    g_scene_root = std::make_shared<cg::SceneNode>();
    g_scene_root->set_name("Root");

    // Create shader nodes
    g_point_shader = std::make_shared<cg::PointShaderNode>();
    g_point_shader->set_name("Point Shader");
    
    g_line_shader = std::make_shared<cg::LineShaderNode>();
    g_line_shader->set_name("Line Shader");

    // Load shaders from files
    if (!g_point_shader->create("vertex_shader.glsl", "fragment_shader.glsl")) {
        std::cout << "Failed to create point shader\n";
        return;
    }
    
    if (!g_line_shader->create("vertex_shader.glsl", "fragment_shader.glsl")) {
        std::cout << "Failed to create line shader\n";
        return;
    }

    // Get shader locations
    if (!g_point_shader->get_locations() || !g_line_shader->get_locations()) {
        std::cout << "Failed to get shader locations\n";
        return;
    }

    // Create geometry nodes
    g_point_geometry = std::make_shared<cg::PointNode>();
    g_point_geometry->set_name("Point Geometry");
    if (!g_point_geometry->initialize()) {
        std::cout << "Failed to initialize point geometry\n";
        return;
    }

    g_line_geometry = std::make_shared<cg::LineNode>();
    g_line_geometry->set_name("Line Geometry");
    if (!g_line_geometry->initialize()) {
        std::cout << "Failed to initialize line geometry\n";
        return;
    }

    // Build scene graph
    // Root -> Point Shader -> Point Geometry
    //      -> Line Shader -> Line Geometry
    g_scene_root->add_child(g_point_shader);
    g_point_shader->add_child(g_point_geometry);
    
    g_scene_root->add_child(g_line_shader);
    g_line_shader->add_child(g_line_geometry);

    // Set initial sizes
    g_point_shader->set_point_size(g_pointSize);
    g_line_geometry->set_line_width(g_lineWidth);

    // Print scene graph structure
    std::cout << "Scene Graph Structure:" << std::endl;
    g_scene_root->print_graph();
}

/**
 * Main - entry point for GetStarted GLUT application.
 */
int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);
    std::cout << "Keyboard Controls:\n";
    std::cout << "1-9 : Alter line width and point size\n";
    std::cout << "ESC - Exit program\n";

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Initialize display mode and window
    g_window = SDL_CreateWindow("Module2 - Scene Graph Example", 
                               g_window_width, g_window_height, 
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!g_window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Initialize OpenGL
    g_context = SDL_GL_CreateContext(g_window);
    if(!g_context) {
        std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_SetSwapInterval(1);

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

    float aliased_line_width_range[2];
    float point_size_range[2];

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, aliased_line_width_range);
    glGetFloatv(GL_POINT_SIZE_RANGE, point_size_range);

    std::cout << "GL_ALIASED_LINE_WIDTH_RANGE: " << aliased_line_width_range[0] << ", "
              << aliased_line_width_range[1] << '\n';
    std::cout << "GL_POINT_SIZE_RANGE: " << point_size_range[0] << ", " << point_size_range[1]
              << '\n';

#if defined(GL_POINT_SPRITE)
    // NOTE: Some windowing systems contain a bug that requires the following line of code to
    // execute BEFORE vertex shaders will populate 'gl_PointCoord'
    glEnable(GL_POINT_SPRITE);
#endif

    // Set initial viewport
    reshape(g_window_width, g_window_height);

    // Create the scene
    create_scene();
    cg::check_error("create_scene");

    // Main loop
    while(handle_events())
    {
        display();
        sleep(DRAW_INTERVAL_MILLIS);
    }

    // Destroy OpenGL Context, SDL Window and SDL
    g_scene_root.reset();
    g_point_shader.reset();
    g_line_shader.reset();
    g_point_geometry.reset();
    g_line_geometry.reset();

    SDL_GL_DestroyContext(g_context);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
