#include "lineShaderNode.hpp"
#include <iostream>

namespace cg
{

LineShaderNode::LineShaderNode() 
    : position_loc_(-1), is_drawing_points_loc_(-1)
{
    node_type_ = SceneNodeType::SHADER;
}

LineShaderNode::~LineShaderNode() {}

void LineShaderNode::draw(SceneState &scene_state)
{
    // Use the shader program
    shader_program_.use();
    
    // Set uniform to indicate we're drawing lines (not points)
    glUniform1i(is_drawing_points_loc_, 0);
    
    // Update scene state with current locations
    scene_state.position_loc = position_loc_;
    scene_state.is_drawing_points_loc = is_drawing_points_loc_;
    
    // Draw children (geometry nodes)
    SceneNode::draw(scene_state);
    
    // Unbind shader program
    glUseProgram(0);
}

bool LineShaderNode::get_locations()
{
    // Get attribute locations
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "aPos");
    if (position_loc_ < 0) 
    {
        std::cout << "Failed to get position attribute location\n";
        return false;
    }
    
    // Get uniform locations
    is_drawing_points_loc_ = glGetUniformLocation(shader_program_.get_program(), "isDrawingPoints");
    if (is_drawing_points_loc_ < 0) 
    {
        std::cout << "Failed to get isDrawingPoints uniform location\n";
        return false;
    }
    
    return true;
}

} // namespace cg
