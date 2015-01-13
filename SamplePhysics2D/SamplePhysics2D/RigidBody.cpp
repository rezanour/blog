#include "Precomp.h"
#include "RigidBody.h"
#include "Shape.h"

RigidBody::RigidBody(Shape* shape, float mass)
    : _shape(shape)
    , _rotation(0.0f)
    , _angularVelocity(0.0f)
    , _torque(0.0f)
    , _mass(mass)
{
    assert(shape);

    if (mass < FLT_MAX)
    {
        _invMass = 1.0f / mass;
        _I = _shape->ComputeI(mass);
        _invI = 1.0f / _I;
    }
    else
    {
        // approx infinate mass (immovable object)
        _invMass = 0.0f;
        _I = FLT_MAX;
        _invI = 0.0f;
    }
}

RigidBody::~RigidBody()
{
    delete _shape;
    _shape = nullptr;
}
