#pragma once

enum class ShapeType
{
    Circle = 0,
    Plane           // Only use for static infinite planes. Not simulated
};

class Shape
{
public:
    ShapeType Type() const { return _type; }

    // Compute moment of inertia for the shape, given mass.
    // http://en.wikipedia.org/wiki/List_of_moments_of_inertia contains
    // a list of formulas for common shapes.
    virtual float ComputeI(float mass) const = 0;

protected:
    // Force this to only be a base class by making ctor protected
    Shape(ShapeType type) : _type(type) {}

private:
    ShapeType _type;

    // Prevent copy
    Shape(const Shape&);
    Shape& operator= (const Shape&);
};

class CircleShape : public Shape
{
public:
    CircleShape() : Shape(ShapeType::Circle), _radius(1.0f) {}
    CircleShape(float radius) : Shape(ShapeType::Circle), _radius(radius) {}

    const float Radius() const { return _radius; }
    float& Radius() { return _radius; }

    // Shape
    float ComputeI(float mass) const override;

private:
    float _radius;
};

class PlaneShape : public Shape
{
public:
    PlaneShape() : Shape(ShapeType::Plane), _normal(0.0f, 1.0f), _distance(0.0f) {}
    PlaneShape(const Vector2& normal, float distance)
        : Shape(ShapeType::Plane), _normal(normal), _distance(distance)
    {}

    const Vector2& Normal() const { return _normal; }
    Vector2& Normal() { return _normal; }

    const float Distance() const { return _distance; }
    float& Distance() { return _distance; }

    // Shape
    float ComputeI(float mass) const override;

private:
    Vector2 _normal;
    float _distance;
};
