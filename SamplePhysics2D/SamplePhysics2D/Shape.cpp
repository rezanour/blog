#include "Precomp.h"
#include "Shape.h"

float CircleShape::ComputeI(float mass) const
{
    return mass * (_radius * _radius) / 4.0f;
}

float PlaneShape::ComputeI(float mass) const
{
    assert(mass == FLT_MAX);
    return FLT_MAX;
}