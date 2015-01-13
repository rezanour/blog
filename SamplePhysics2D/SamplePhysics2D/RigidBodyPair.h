#pragma once

class RigidBody;

// In order to use the std::map efficiently to look up pairs of objects,
// we define the following key type, and implement a less than operator for it.
struct PairKey
{
    PairKey(RigidBody* body1, RigidBody* body2)
    {
        // Always store the body with the lower address as the first
        if (body1 <= body2)
        {
            this->body1 = body1;
            this->body2 = body2;
        }
        else
        {
            this->body1 = body2;
            this->body2 = body1;
        }
    }

    RigidBody* body1;
    RigidBody* body2;
};

inline bool operator< (const PairKey& lhs, const PairKey& rhs)
{
    return (lhs.body1 < rhs.body1) ||
        (lhs.body1 == rhs.body1 && lhs.body2 < rhs.body2);
}

// Defines a single contact point between two bodies, along with some
// support data about the contact that is built up and cached here.
struct ContactInfo
{
    ContactInfo() { memset(this, 0, sizeof(ContactInfo)); }

    Vector2 worldPosition;  // world position of the contact point
    Vector2 normal;         // normal (pointing away from body2)
    Vector2 r1, r2;         // relative position of contact for each object
    float   distance;       // distance between the two objects at the contact. Negative for overlap
    float   impulseNormal;  // Accumulated impulse along the normal
    float   impulseBias;    // Accumulated impulse along normal for position bias
    float   massNormal;     // Effective combined mass along the normal
    float   positionBias;   // Bias factor to make up for penetration
};

// Tests body1 and body2 for collision, and if one exists, the contact info is filled in
// and the function returns true. Otherwise, returns false.
bool Collide(RigidBody* body1, RigidBody* body2, ContactInfo& contact);

// For each pair of objects potentially interacting (colliding) with each other
// we need a RigidBodyPair object. This can be expanded later to be more
// of a manifold, iteratively building up a collection of contact points.
class RigidBodyPair
{
public:
    RigidBodyPair(RigidBody* body1, RigidBody* body2);

    const RigidBody* Body1() const { return _body1; }
    RigidBody*& Body1() { return _body1; }

    const RigidBody* Body2() const { return _body2; }
    RigidBody*& Body2() { return _body2; }

    bool HasContact() const { return _hasContact; }

    const ContactInfo& Contact() const { return _contact; }
    ContactInfo& Contact() { return _contact; }

    // Prior to beginning solver iterations, set up some one time info
    void PreSolve(float invDt);

    // Solve a single iteration. Computes and applies impulses
    void Solve();

private:
    RigidBody* _body1;
    RigidBody* _body2;
    ContactInfo _contact;
    bool _hasContact;
};
