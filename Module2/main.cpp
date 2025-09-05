#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>
#include <stdio.h>
#include <iostream>


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

      }

   
    }

    //clear the back buffer for re-use
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //rest of rendering code later 

    //swapem 
    SDL_GL_SwapWindow(window);
  }
  
  //clean up after yourself
  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
