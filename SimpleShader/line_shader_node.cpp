#include "SimpleShader/line_shader_node.hpp"

namespace cg
{

bool LineShaderNode::get_locations()
{
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_position");
    if(position_loc_ < 0)
    {
        std::cerr << "Error getting vertex position location\n";
        return false;
    }

    ortho_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "ortho");
    if(ortho_matrix_loc_ < 0)
    {
        std::cerr << "Error getting ortho matrix location\n";
        return false;
    }

    color_loc_ = glGetUniformLocation(shader_program_.get_program(), "color");
    if(color_loc_ < 0)
    {
        std::cerr << "Error getting color location\n";
        return false;
    }
    return true;
}

void LineShaderNode::draw(SceneState &scene_state)
{
    // Enable this program
    shader_program_.use();

    // Set scene state locations to ones needed for this program
    scene_state.position_loc = position_loc_;
    scene_state.ortho_matrix_loc = ortho_matrix_loc_;
    scene_state.color_loc = color_loc_;

    // Set the matrix
    glUniformMatrix4fv(ortho_matrix_loc_, 1, GL_FALSE, scene_state.ortho.data());

    // Draw all children
    SceneNode::draw(scene_state);
}

int32_t LineShaderNode::get_position_loc() const { return position_loc_; }

} // namespace cg
