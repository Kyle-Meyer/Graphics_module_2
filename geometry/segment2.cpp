#include "geometry/segment2.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

LineSegment2::LineSegment2() : a{0.0f, 0.0f}, b{0.0f, 0.0f} {}

LineSegment2::LineSegment2(const Point2 &p1, const Point2 &p2) : a(p1), b(p2) {}

Segment2PointDistanceResult LineSegment2::distance(const Point2 &p) const
{
    // Construct vectors v (ab) and w (ap)
    Vector2 v(a, b);
    Vector2 w(a, p);

    // Numerator of the component of w onto v. If <= 0 then a
    // is the closest point. By separat_ing into the numerator
    // and denominator of the component we avoid a division unless
    // it is necessary.
    float n = w.dot(v);
    if(n <= 0.0f) { return {w.norm(), a}; }

    // Get the denominator of the component.  If the component >= 1
    // (d <= n) then point b is the closest point
    float d = v.dot(v);
    if(d <= n) { return {Vector2(b, p).norm(), b}; }

    // Closest point is along the segment. The point is the projection of
    // w onto v.
    Point2 closest = a + v * (n / d);
    return {Vector2(closest, p).norm(), closest};
}

Segment2IntersectionResult LineSegment2::intersect(const LineSegment2 &segment) const
{
    // Construct vectors
    Vector2 v = b - a;
    Vector2 w = segment.b - segment.a;

    // Set 2D perpendicular vector to w
    Vector2 wp(w.y, -w.x);

    // Check if denominator will be 0 (lines are parallel)
    float wp_dot_v = wp.dot(v);
    if(wp_dot_v == 0.0f) { return {false, Point2()}; }

    // Solve for the parameter t. Early exit if no intersect.
    Vector2 c = segment.a - a;
    float   t = wp.dot(c) / wp_dot_v;
    if(t < 0.0f || t > 1.0f) { return {false, Point2()}; }

    // Solve for the parameter u
    Vector2 vp(v.y, -v.x);
    float   u = vp.dot(c) / wp_dot_v;
    if(u < 0.0f || u > 1.0f) { return {false, Point2()}; }

    // An intersect occurs. Return 'true' and the intersect point.
    return {true, a + v * t};
}

Segment2ClipResult LineSegment2::clip_to_polygon(const std::vector<Point2> &poly) const
{
    // Defined Later...
    return {true, LineSegment2()};
}

Segment2ClipResult LineSegment2::clip_to_rectangle(const CRectangle &r) const
{
    // Defined Later...
    return {true, LineSegment2()};
}

} // namespace cg
