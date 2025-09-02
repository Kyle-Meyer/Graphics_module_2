//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    line_node.hpp
//	Purpose: Simple geometry node that draws a connected line.
//
//============================================================================

#ifndef __SIMPLESHADER_LINE_NODE_HPP__
#define __SIMPLESHADER_LINE_NODE_HPP__

#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"

#include <vector>

namespace cg
{

/**
 * Simple scene graph geometry node to store and draw lines.
 */
class LineNode : public GeometryNode
{
  public:
    /**
     * Constructor.
     */
    LineNode(std::vector<Point2> &pts, const Color4 &color, int32_t position_loc);

    /**
     * Destructor
     */
    virtual ~LineNode();

    /**
     * Draw the lines
     * @param  scene_state  Current scene state.
     */
    void draw(SceneState &scene_state) override;

  protected:
    uint32_t vertex_count_; // Number of vertices
    GLuint   vbo_;          // VBO for positions
    GLuint   vao_;          // Vertex Array Object
    Color4   color_;        // Color to draw the lines
};

} // namespace cg

#endif
