#pragma once

class Shape;

// A rigid body is a dynamic object which can be affected by forces and constraints
class RigidBody
{
public:
    // The rigid body takes over the shape's lifetime.
    // When the rigid body is destroyed, it will delete the shape
    RigidBody(Shape* shape, float mass);
    ~RigidBody();

    const Shape* GetShape() const { return _shape; }

    const Vector2& Position() const { return _position; }
    Vector2& Position() { return _position; }

    const Vector2& LinearVelocity() const { return _linearVelocity; }
    Vector2& LinearVelocity() { return _linearVelocity; }

    const Vector2& Force() const { return _force; }
    Vector2& Force() { return _force; }

    const float Mass() const { return _mass; }
    const float InvMass() const { return _invMass; }

    const float Rotation() const { return _rotation; }
    float& Rotation() { return _rotation; }

    const float AngularVelocity() const { return _angularVelocity; }
    float& AngularVelocity() { return _angularVelocity; }

    const float Torque() const { return _torque; }
    float& Torque() { return _torque; }

    const float I() const { return _I; }
    const float InvI() const { return _invI; }

private:
    Shape* _shape;

    // Linear
    Vector2 _position;
    Vector2 _linearVelocity;
    Vector2 _force;
    float _mass, _invMass;

    // Angular
    float _rotation;
    float _angularVelocity;
    float _torque;
    float _I, _invI;
};
