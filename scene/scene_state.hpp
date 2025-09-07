//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    scene_state.hpp
//	Purpose: Class used to propogate state during traversal of the scene graph.
//
//============================================================================

#ifndef __SCENE_SCENE_STATE_HPP__
#define __SCENE_SCENE_STATE_HPP__

#include "scene/graphics.hpp"

#include <array>

namespace cg
{

/**
 * Scene state structure. Used to store OpenGL state - shader locations,
 * matrices, etc.
 */
struct SceneState
{
    // Vertex attribute locations
    GLint position_loc; // Vertex position attribute location

    // Uniform locations
    GLint ortho_matrix_loc; // Orthographic projection location (2-D)
    GLint color_loc;        // Constant color
    GLint is_drawing_points_loc;    // Boolean uniform for point vs line drawing
    GLint point_size_loc;           // Point size uniform location

    float point_size;    // Current point size
    float line_width;    // Current line width

    // Current matrices
    std::array<float, 16> ortho; // Orthographic projection matrix (2-D)
};

} // namespace cg

#endif
