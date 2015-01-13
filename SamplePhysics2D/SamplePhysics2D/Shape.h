#pragma once

enum class ShapeType
{
    Circle = 0,
    Box,
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

class BoxShape : public Shape
{
public:
    BoxShape() : Shape(ShapeType::Box), _size(1.0f, 1.0f) {}
    BoxShape(float w, float h) : Shape(ShapeType::Box), _size(w, h) {}

    const Vector2& Size() const { return _size; }
    Vector2& Size() { return _size; }

    // Shape
    float ComputeI(float mass) const override;

private:
    Vector2 _size;
};
