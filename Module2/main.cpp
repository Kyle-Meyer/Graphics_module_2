#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Global variables for rendering
std::vector<float> clickPoints; // Store x,y pairs
GLuint shaderProgram, VAO, VBO;

std::string readShaderFile(const std::string& filePath) 
{
    std::ifstream file;
    std::stringstream buffer;
    
    // Ensure ifstream objects can throw exceptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try 
    {
        file.open(filePath);
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }
    catch (std::ifstream::failure& e) 
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::cout << "Failed to read: " << filePath << std::endl;
        return "";
    }
}

GLuint compileShader(GLenum type, const char* source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  if(!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "shader compilation failed: " << infoLog << std::endl;
  }

  return shader;
}

GLuint createShaderProgram()
{
  std::string vertexCode = readShaderFile("vertex_shader.glsl");
  std::string fragmentCode = readShaderFile("fragment_shader.glsl");

  if(vertexCode.empty() || fragmentCode.empty())
  {
    std::cout << "Failed to load both shader files" << std::endl;
    return 0;
  }

  //compile the shaders
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

  //create the program 
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  //check the linking
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "Program linking failed: " << infoLog << std::endl;
  }
  
  //clean up time 
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

bool handleKeys(SDL_Event event)
{
  // Initialize line width and point size
  float lineWidth = 1.0f;
  float pointSize = 2.0f;

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
    lineWidth = (float)digit;
    pointSize = lineWidth * 2.0f;
    
    // Set OpenGL line width and point size
    glLineWidth(lineWidth);
    glPointSize(pointSize);
    
    std::cout << "Line width set to: " << lineWidth << ", point size set to: " << pointSize << std::endl;
  }
  
  return true;
}


int main()
{
  //Step 1
   if(SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    std::cout << "SDL init failed bro" << std::endl;
    return -1;
  }

  //first set the color widths, if we want true color then we want 0-255, or 8 bit
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //now make it double buffer 
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  //set the openGL version
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   
   int width = 800;
   int height = 600;
  //create the window
   SDL_Window* window = SDL_CreateWindow("Kyle Meyer(cool)", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (!window) 
  {
    std::cout << "Window creation failed: %s\n" << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  //create the context 
  SDL_GLContext context = SDL_GL_CreateContext(window);
  if(!context)
  {
    std::cout << "failed to bind context!" << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  //not sure if vsync is needed, but the docs recommended it 
  SDL_GL_SetSwapInterval(1) ;

  //create the shader 
  shaderProgram = createShaderProgram();

  //create the VAO and VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  //set vertex attribute pointers
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  //unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //main run loop
  bool running = true;
  while(running)
  {
    SDL_Event event;
    //event loop listener
    while (SDL_PollEvent(&event)) 
    {
      switch(event.type)
      {
         case SDL_EVENT_QUIT:
            running = false;
            break;

         case SDL_EVENT_WINDOW_RESIZED:
            SDL_GetWindowSize(window, &width, &height);
            glViewport(0,0, width, height);
            break;

          case SDL_EVENT_KEY_DOWN:
            running = handleKeys(event);
            break;

          case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) 
            {
              float mouseX = event.button.x;
              float mouseY = event.button.y;
        
              // Convert screen coordinates to normalized device coordinates (-1 to 1)
              float ndcX = (mouseX / width) * 2.0f - 1.0f;
              float ndcY = 1.0f - (mouseY / height) * 2.0f; // Flip Y axis
        
              clickPoints.push_back(ndcX);
              clickPoints.push_back(ndcY);
        
              std::cout << "Click at: " << ndcX << ", " << ndcY << std::endl;
            }
            break;
      }

   
    }

    //clear the back buffer for re-use
    glClearColor(0.6f, 0.3f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    if(!clickPoints.empty())
    {
      //update the VBO with click points 
      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, clickPoints.size() * sizeof(float),
                   clickPoints.data(), GL_DYNAMIC_DRAW);

      //draw 
      glDrawArrays(GL_POINTS, 0, clickPoints.size()/2);

      glBindVertexArray(0);
    }

    glUseProgram(0);

    //swapem 
    SDL_GL_SwapWindow(window);
  }
 

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  //clean up after yourself
  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
