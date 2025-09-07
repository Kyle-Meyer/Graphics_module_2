#include "lineNode.hpp"
#include <iostream>

namespace cg
{

LineNode::LineNode() 
    : vao_(0), vbo_(0), initialized_(false), vbo_dirty_(false)
{
    node_type_ = SceneNodeType::GEOMETRY;
}

LineNode::~LineNode()
{
    if (initialized_)
    {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
    }
}

bool LineNode::initialize()
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

void LineNode::draw(SceneState &scene_state)
{
    if (!initialized_) return;
    if (vertices_.empty()) return;

    // Update VBO if needed
    if (vbo_dirty_)
    {
        update_vbo();
        vbo_dirty_ = false;
    }

    // Set line width
    glLineWidth(scene_state.line_width);

    // Bind VAO and draw
    glBindVertexArray(vao_);
    
    // Draw lines as LINE_STRIP to connect all points
    if (vertices_.size() >= 4) // Need at least 2 vertices (4 floats)
    {
        glDrawArrays(GL_LINE_STRIP, 0, vertices_.size() / 2);
    }
    
    glBindVertexArray(0);

    // Draw children
    SceneNode::draw(scene_state);
}

void LineNode::add_vertex(float x, float y)
{
    vertices_.push_back(x);
    vertices_.push_back(y);
    vbo_dirty_ = true;
}

void LineNode::clear_vertices()
{
    vertices_.clear();
    vbo_dirty_ = true;
}

size_t LineNode::vertex_count() const
{
    return vertices_.size() / 2;
}

void LineNode::update_vbo()
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
