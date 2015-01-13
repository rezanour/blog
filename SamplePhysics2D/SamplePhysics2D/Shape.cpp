#include "Precomp.h"
#include "Shape.h"

float CircleShape::ComputeI(float mass) const
{
    return mass * (_radius * _radius) / 4.0f;
}

float BoxShape::ComputeI(float mass) const
{
    return mass * (_size.x * _size.x + _size.y * _size.y) / 12.0f;
}
