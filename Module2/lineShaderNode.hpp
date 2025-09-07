#ifndef __SCENE_LINE_SHADER_NODE_HPP__
#define __SCENE_LINE_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"

namespace cg
{

class LineShaderNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     */
    LineShaderNode();

    /**
     * Destructor.
     */
    virtual ~LineShaderNode();

    /**
     * Draw the shader node and its children. Sets up shader state for line drawing.
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
};

} // namespace cg

#endif //end of __SCENE_LINE_SHADER_NODE_HPP__
