#ifndef __MODULE2_LINE_NODE_HPP__
#define __MODULE2_LINE_NODE_HPP__

#include "scene/scene_node.hpp"
#include <vector>

namespace cg
{

class LineNode : public SceneNode
{
  public:
    /**
     * Constructor.
     */
    LineNode();

    /**
     * Destructor.
     */
    virtual ~LineNode();

    /**
     * Initialize OpenGL resources (VAO, VBO)
     * @return  Returns true if successful
     */
    bool initialize();

    /**
     * Draw the line geometry
     * @param  scene_state  Current scene state
     */
    virtual void draw(SceneState &scene_state) override;

    /**
     * Add a new vertex to the line
     * @param  x  X coordinate in NDC space
     * @param  y  Y coordinate in NDC space
     */
    void add_vertex(float x, float y);

    /**
     * Clear all vertices
     */
    void clear_vertices();

    /**
     * Get the number of vertices
     * @return  Returns the number of vertices
     */
    size_t vertex_count() const;

    /**
     * Update the VBO with current vertex data
     */
    void update_vbo();

    /**
     * Set the line width
     * @param width  Line width value
     */
    void set_line_width(float width);

    /**
     * Get the current line width
     * @return  Current line width
     */
    float get_line_width() const;

  private:
    std::vector<float> vertices_;  // Vertex data (x,y pairs)
    GLuint vao_;                   // Vertex Array Object
    GLuint vbo_;                   // Vertex Buffer Object
    bool initialized_;             // Whether OpenGL resources are initialized
    bool vbo_dirty_;              // Whether VBO needs updating
    float line_width_;            // Current line width
};

} // namespace cg

#endif
