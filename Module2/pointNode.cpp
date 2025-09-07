#include "pointNode.hpp"
#include <iostream>

namespace cg
{

PointNode::PointNode() 
    : vao_(0), vbo_(0), initialized_(false), vbo_dirty_(false)
{
    node_type_ = SceneNodeType::GEOMETRY;
}

PointNode::~PointNode()
{
    if (initialized_)
    {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
    }
}

bool PointNode::initialize()
{
    if (initialized_) return true;

    // Generate VAO and VBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Set up vertex attribute pointers
    // Position attribute (location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized_ = true;
    return true;
}

void PointNode::draw(SceneState &scene_state)
{
    if (!initialized_) return;
    if (vertices_.empty()) return;

    // Update VBO if needed
    if (vbo_dirty_)
    {
        update_vbo();
        vbo_dirty_ = false;
    }

    // Bind VAO and draw
    glBindVertexArray(vao_);
    
    // Draw all vertices as points
    glDrawArrays(GL_POINTS, 0, vertices_.size() / 2);
    
    glBindVertexArray(0);

    // Draw children
    SceneNode::draw(scene_state);
}

void PointNode::add_vertex(float x, float y)
{
    vertices_.push_back(x);
    vertices_.push_back(y);
    vbo_dirty_ = true;
}

void PointNode::clear_vertices()
{
    vertices_.clear();
    vbo_dirty_ = true;
}

size_t PointNode::vertex_count() const
{
    return vertices_.size() / 2;
}

void PointNode::update_vbo()
{
    if (!initialized_ || vertices_.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 
                 vertices_.size() * sizeof(float), 
                 vertices_.data(), 
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace cg
