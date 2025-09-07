#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>
#include <stdio.h>
#include <iostream>
#include <memory>

// Include scene graph classes
#include "scene/scene_node.hpp"
#include "scene/scene_state.hpp"
#include "lineShaderNode.hpp"
#include "pointShaderNode.hpp"
#include "lineNode.hpp"
#include "pointNode.hpp"

using namespace cg;

// Global scene graph components
std::shared_ptr<SceneNode> root_node;
std::shared_ptr<PointShaderNode> point_shader;
std::shared_ptr<LineShaderNode> line_shader;
std::shared_ptr<PointNode> point_geometry;
std::shared_ptr<LineNode> line_geometry;
SceneState scene_state;

float g_lineWidth = 3.0f;
float g_pointSize = 6.0f;

bool initializeSceneGraph()
{
    // Initialize scene state
    scene_state.point_size = g_pointSize;
    scene_state.line_width = g_lineWidth;

    // Create root node
    root_node = std::make_shared<SceneNode>();
    root_node->set_name("Root");

    // Create shader nodes
    point_shader = std::make_shared<PointShaderNode>();
    point_shader->set_name("Point Shader");
    
    line_shader = std::make_shared<LineShaderNode>();
    line_shader->set_name("Line Shader");

    // Load shaders from files
    if (!point_shader->create("vertex_shader.glsl", "fragment_shader.glsl"))
    {
        std::cout << "Failed to create point shader\n";
        return false;
    }
    
    if (!line_shader->create("vertex_shader.glsl", "fragment_shader.glsl"))
    {
        std::cout << "Failed to create line shader\n";
        return false;
    }

    // Get shader locations
    if (!point_shader->get_locations() || !line_shader->get_locations())
    {
        std::cout << "Failed to get shader locations\n";
        return false;
    }

    // Create geometry nodes
    point_geometry = std::make_shared<PointNode>();
    point_geometry->set_name("Point Geometry");
    if (!point_geometry->initialize())
    {
        std::cout << "Failed to initialize point geometry\n";
        return false;
    }

    line_geometry = std::make_shared<LineNode>();
    line_geometry->set_name("Line Geometry");
    if (!line_geometry->initialize())
    {
        std::cout << "Failed to initialize line geometry\n";
        return false;
    }

    // Build scene graph
    // Root -> Point Shader -> Point Geometry
    //      -> Line Shader -> Line Geometry
    root_node->add_child(point_shader);
    point_shader->add_child(point_geometry);
    
    root_node->add_child(line_shader);
    line_shader->add_child(line_geometry);

    return true;
}

bool handleKeys(SDL_Event event)
{
    SDL_Keycode key = event.key.key;
              
    if (key == SDLK_ESCAPE) 
    {
        std::cout << "ESC pressed - Quitting application" << std::endl;
        return false;
    }
    else if (key >= SDLK_1 && key <= SDLK_9) 
    {
        // Convert keycode to digit (1-9)
        int digit = key - SDLK_0;

        g_lineWidth = (float)digit;
        g_pointSize = g_lineWidth * 2.0f;
        
        // Update scene state
        scene_state.line_width = g_lineWidth;
        scene_state.point_size = g_pointSize;
        
        std::cout << "Line width set to: " << g_lineWidth << ", point size set to: " << g_pointSize << std::endl;
    }
    
    return true;
}

int main()
{
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        std::cout << "SDL init failed" << std::endl;
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
   
    int width = 800;
    int height = 600;
    SDL_Window* window = SDL_CreateWindow("Module2 - Scene Graph Example", width, height, 
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) 
    {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if(!context)
    {
        std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_SetSwapInterval(1);

    // Initialize scene graph
    if (!initializeSceneGraph())
    {
        std::cout << "Failed to initialize scene graph" << std::endl;
        return -1;
    }

    // Print scene graph structure
    std::cout << "Scene Graph Structure:" << std::endl;
    root_node->print_graph();

    // Main loop
    bool running = true;
    while(running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window, &width, &height);
                    glViewport(0, 0, width, height);
                    break;

                case SDL_EVENT_KEY_DOWN:
                    running = handleKeys(event);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) 
                    {
                        float mouseX = event.button.x;
                        float mouseY = event.button.y;
                
                        // Convert screen coordinates to NDC
                        float ndcX = (mouseX / width) * 2.0f - 1.0f;
                        float ndcY = 1.0f - (mouseY / height) * 2.0f;
                
                        // Add vertex to both point and line geometry
                        point_geometry->add_vertex(ndcX, ndcY);
                        line_geometry->add_vertex(ndcX, ndcY);
                
                        std::cout << "Click at: " << ndcX << ", " << ndcY << std::endl;
                        std::cout << "Points: " << point_geometry->vertex_count() 
                                  << ", Lines: " << line_geometry->vertex_count() << std::endl;
                    }
                    break;
            }
        }

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the scene graph
        root_node->draw(scene_state);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    root_node.reset();
    point_shader.reset();
    line_shader.reset();
    point_geometry.reset();
    line_geometry.reset();

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
