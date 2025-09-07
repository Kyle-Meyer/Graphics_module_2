#ifndef __MODULE2_POINT_NODE_HPP__
#define __MODULE2_POINT_NODE_HPP__

#include "scene/scene_node.hpp"
#include <vector>

namespace cg
{

class PointNode : public SceneNode
{
  public:
    /**
     * Constructor.
     */
    PointNode();

    /**
     * Destructor.
     */
    virtual ~PointNode();

    /**
     * Initialize OpenGL resources (VAO, VBO)
     * @return  Returns true if successful
     */
    bool initialize();

    /**
     * Draw the point geometry
     * @param  scene_state  Current scene state
     */
    virtual void draw(SceneState &scene_state) override;

    /**
     * Add a new vertex to the point list
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

  private:
    std::vector<float> vertices_;  // Vertex data (x,y pairs)
    GLuint vao_;                   // Vertex Array Object
    GLuint vbo_;                   // Vertex Buffer Object
    bool initialized_;             // Whether OpenGL resources are initialized
    bool vbo_dirty_;              // Whether VBO needs updating
};

} // namespace cg

#endif
