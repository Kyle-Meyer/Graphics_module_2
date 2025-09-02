#include "SimpleShader/line_node.hpp"

namespace cg
{

LineNode::LineNode(std::vector<Point2> &pts, const Color4 &color, int32_t position_loc)
{
    // Copy the color and set the number of vertices
    color_ = color;
    vertex_count_ = static_cast<uint32_t>(pts.size());

    // Create vertex buffer object
    glGenBuffers(1, &vbo_);

    // Add the points to the buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER, vertex_count_ * sizeof(Point2), (GLvoid *)&pts.front(), GL_STATIC_DRAW);

    // Allocate a VAO, enable it and set the vertex attribute arrays and pointers
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}

LineNode::~LineNode()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void LineNode::draw(SceneState &scene_state)
{
    // Set the color
    glUniform4f(scene_state.color_loc, color_.r, color_.g, color_.b, color_.a);

    // Bind the VAO and draw the lines
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINE_STRIP, 0, vertex_count_);
    glBindVertexArray(0);
}

} // namespace cg
