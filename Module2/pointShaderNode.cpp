#include "pointShaderNode.hpp"
#include <iostream>

namespace cg
{

PointShaderNode::PointShaderNode() 
    : position_loc_(-1), is_drawing_points_loc_(-1), point_size_loc_(-1)
{
    node_type_ = SceneNodeType::SHADER;
}

PointShaderNode::~PointShaderNode() {}

void PointShaderNode::draw(SceneState &scene_state)
{
    // Use the shader program
    shader_program_.use();
    
    // Set uniform to indicate we're drawing points
    glUniform1i(is_drawing_points_loc_, 1);
    
    // Set point size from scene state
    glUniform1f(point_size_loc_, scene_state.point_size);
    
    // Update scene state with current locations
    scene_state.position_loc = position_loc_;
    scene_state.is_drawing_points_loc = is_drawing_points_loc_;
    scene_state.point_size_loc = point_size_loc_;
    
    // Draw children (geometry nodes)
    SceneNode::draw(scene_state);
    
    // Unbind shader program
    glUseProgram(0);
}

bool PointShaderNode::get_locations()
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
    
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "pointSize");
    if (point_size_loc_ < 0) 
    {
        std::cout << "Failed to get pointSize uniform location\n";
        return false;
    }
    
    return true;
}

} // namespace cg
