// Simple fragment shader that sets the pixel color to the color specified via 
// the color uniform variable

#version 410 core

layout (location = 0) out vec4 frag_color; // Output color to the frame buffer

uniform vec4 color;

void main() 
{
    frag_color = color;
}
