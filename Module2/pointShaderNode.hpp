#ifndef __MODULE2_POINT_SHADER_NODE_HPP__
#define __MODULE2_POINT_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"

namespace cg
{

class PointShaderNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     */
    PointShaderNode();

    /**
     * Destructor.
     */
    virtual ~PointShaderNode();

    /**
     * Draw the shader node and its children. Sets up shader state for point drawing.
     * @param  scene_state  Current scene state
     */
    virtual void draw(SceneState &scene_state) override;

    /**
     * Get uniform and attribute locations from the shader program
     * @return  Returns true if successful
     */
    virtual bool get_locations() override;

  private:
    GLint position_loc_;            // Vertex position attribute location
    GLint is_drawing_points_loc_;   // Uniform for distinguishing points vs lines
    GLint point_size_loc_;          // Uniform for point size
};

} // namespace cg

#endif
