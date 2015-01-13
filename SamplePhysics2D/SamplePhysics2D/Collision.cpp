#include "Precomp.h"
#include "RigidBody.h"
#include "RigidBodyPair.h"
#include "Shape.h"

static bool CollideCircleCircle(RigidBody* body1, RigidBody* body2, ContactInfo& contact);
static bool CollideCirclePlane(RigidBody* body1, RigidBody* body2, ContactInfo& contact);

bool Collide(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const Shape* shape1 = body1->GetShape();
    const Shape* shape2 = body2->GetShape();

    if (shape1->Type() == ShapeType::Circle && shape2->Type() == ShapeType::Circle)
    {
        return CollideCircleCircle(body1, body2, contact);
    }
    else if (shape1->Type() == ShapeType::Circle && shape2->Type() == ShapeType::Plane)
    {
        return CollideCirclePlane(body1, body2, contact);
    }
    else if (shape1->Type() == ShapeType::Plane && shape2->Type() == ShapeType::Circle)
    {
        if (CollideCirclePlane(body2, body1, contact))
        {
            contact.worldPosition = contact.worldPosition + contact.normal * -contact.distance;
            contact.normal = -contact.normal;
            return true;
        }
    }

    return false;
}

bool CollideCircleCircle(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const CircleShape* shape1 = (const CircleShape*)body1->GetShape();
    const CircleShape* shape2 = (const CircleShape*)body2->GetShape();

    float r = shape1->Radius() + shape2->Radius();
    float r2 = r * r;

    Vector2 toBody1 = body1->Position() - body2->Position();
    float d2 = toBody1.LengthSq();

    if (d2 < r2)
    {
        contact.distance = sqrtf(d2) - r;
        contact.normal = toBody1.Normalized();
        contact.worldPosition = body1->Position() - contact.normal * shape1->Radius();
        return true;
    }

    return false;
}

bool CollideCirclePlane(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const CircleShape* shape1 = (const CircleShape*)body1->GetShape();
    const PlaneShape* shape2 = (const PlaneShape*)body2->GetShape();

    float d = Dot(body1->Position(), shape2->Normal()) - shape2->Distance();
    if (d < shape1->Radius())
    {
        contact.distance = shape1->Radius() - d;
        contact.normal = shape2->Normal();
        contact.worldPosition = body1->Position() - contact.normal * shape1->Radius();
        return true;
    }

    return false;
}
