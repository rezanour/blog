#include "Precomp.h"
#include "RigidBodyPair.h"
#include "RigidBody.h"

RigidBodyPair::RigidBodyPair(RigidBody* body1, RigidBody* body2)
{
    // Always store the body with the lower address as the first
    if (body1 <= body2)
    {
        _body1 = body1;
        _body2 = body2;
    }
    else
    {
        _body1 = body2;
        _body2 = body1;
    }

    _hasContact = Collide(_body1, _body2, _contact);
}

void RigidBodyPair::PreSolve(float invDt)
{
    static const float Slop = 0.01f;
    static const float BiasFactor = 0.2f;

    // Vectors from each object's center to the contact point
    _contact.r1 = _contact.worldPosition - _body1->Position();
    _contact.r2 = _contact.worldPosition - _body2->Position();

    // Find how much of each r is along contact normal
    float rn1 = Dot(_contact.r1, _contact.normal);
    float rn2 = Dot(_contact.r2, _contact.normal);

    // To compute effective inverseMass along contact normal,
    // start with the linear masses.
    float kNormal = _body1->InvMass() + _body2->InvMass();

    // Then, to account for rotational moment of inertia, we need
    // to apply the square of the amount of r perpendicular to the normal.
    // See http://chrishecker.com/images/e/e7/Gdmphys3.pdf for a good discussion
    // of how we arrive at this. We can either find the perp & dot it with the
    // normal directly (as Chris does in the pdf linked), or we can use
    // pythagorean theorem and subtract the edge rn^2 from hypotenus r^2 we
    // already have to find the value as below.
    kNormal +=
        _body1->InvI() * (Dot(_contact.r1, _contact.r1) - rn1 * rn1) +
        _body2->InvI() * (Dot(_contact.r2, _contact.r2) - rn2 * rn2);

    // The impulse computation actually needs the inverse of these values, so invert here
    _contact.massNormal = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

    // The bias is an additional boost to the impulse to compensate for already penetrating
    // objects to resolve the penetration in addition to solving velocity.
    _contact.positionBias = -BiasFactor * invDt * min(0.0f, _contact.distance + Slop);
}

void RigidBodyPair::Solve()
{
    // Relative velocity at contact
    Vector2 relVel = 
        _body1->LinearVelocity() + Cross(_body1->AngularVelocity(), _contact.r1) -
        _body2->LinearVelocity() - Cross(_body2->AngularVelocity(), _contact.r2);

    // Compute impulse along normal, using the mass normal we prebuilt and
    // the amount of relative velocity along the normal
    float velNormal = Dot(relVel, _contact.normal);
    float deltaImpulseNormal = _contact.massNormal * (-velNormal + _contact.positionBias);

    // Clamp the accum. impulse so we don't apply negative impulse
    float accumImpulseNormal = _contact.impulseNormal;
    _contact.impulseNormal = max(accumImpulseNormal + deltaImpulseNormal, 0.0f);
    deltaImpulseNormal = _contact.impulseNormal - accumImpulseNormal;

    // Put impulse in vector form and apply to each object
    Vector2 impulseNormal = deltaImpulseNormal * _contact.normal;

    // For linear, we apply directly
    _body1->LinearVelocity() += _body1->InvMass() * impulseNormal;
    // For angular, we convert to resulting angular component
    // by crossing with the vector from center to the point of contact
    _body1->AngularVelocity() += _body1->InvI() * Cross(_contact.r1, impulseNormal);

    _body2->LinearVelocity() -= _body2->InvMass() * impulseNormal;
    _body2->AngularVelocity() -= _body2->InvI() * Cross(_contact.r2, impulseNormal);
}
