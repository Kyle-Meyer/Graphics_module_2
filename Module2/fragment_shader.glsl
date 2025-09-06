#version 330 core

uniform bool isDrawingPoints;

out vec4 FragColor;

void main() 
{
  if(isDrawingPoints)
  {
    // Get the coordinate within the point (ranges from 0.0 to 1.0)
    vec2 coord = gl_PointCoord;
    
    // Convert to centered coordinates (ranges from -0.5 to 0.5)
    vec2 centered = coord - vec2(0.5, 0.5);
    
    // Calculate distance from center
    float distance = length(centered);
    
    // If distance is greater than 0.5 (outside the circle), discard the fragment
    if (distance > 0.5) {
        discard;
    }
    
    //convert NDC coords to color coords (0 to 1)
    //remember these are ADDITIVE
    float redColor = 1.0 - coord.x; 
    float greenColor = coord.y;

    vec3 finalColor = vec3(redColor, greenColor, 0.0);
    FragColor = vec4(finalColor, 1.0);
  }
  else 
  {
    FragColor = vec4(0.0, 0.0, 1.0, 1.0); //solid blued
  }
}
