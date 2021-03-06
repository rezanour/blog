#include "Precomp.h"
#include "RigidBody.h"
#include "RigidBodyPair.h"
#include "Shape.h"

static bool CollideCircleCircle(RigidBody* body1, RigidBody* body2, ContactInfo& contact);
static bool CollideCircleBox(RigidBody* body1, RigidBody* body2, ContactInfo& contact);
static bool CollideBoxBox(RigidBody* body1, RigidBody* body2, ContactInfo& contact);

bool Collide(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const Shape* shape1 = body1->GetShape();
    const Shape* shape2 = body2->GetShape();

    if (shape1->Type() == ShapeType::Circle && shape2->Type() == ShapeType::Circle)
    {
        return CollideCircleCircle(body1, body2, contact);
    }
    else if (shape1->Type() == ShapeType::Box && shape2->Type() == ShapeType::Box)
    {
        return CollideBoxBox(body1, body2, contact);
    }
    else if (shape1->Type() == ShapeType::Circle && shape2->Type() == ShapeType::Box)
    {
        return CollideCircleBox(body1, body2, contact);
    }
    else if (shape1->Type() == ShapeType::Box && shape2->Type() == ShapeType::Circle)
    {
        if (CollideCircleBox(body2, body1, contact))
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

bool CollideCircleBox(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const CircleShape* shape1 = (const CircleShape*)body1->GetShape();
    const BoxShape* shape2 = (const BoxShape*)body2->GetShape();

    // Transform the circle to local space of the box (box then becomes aabb)
    Matrix2 rotB = Matrix2(body2->Rotation());
    Matrix2 invRotB = rotB.Transposed();

    Vector2 toCircle = body1->Position() - body2->Position();
    Vector2 localToCircle = invRotB * toCircle;

    Vector2 halfWidths = 0.5f * shape2->Size();

    // If the center of the sphere is outside of the aabb
    if (localToCircle.x < -halfWidths.x || localToCircle.x > halfWidths.x ||
        localToCircle.y < -halfWidths.y || localToCircle.y > halfWidths.y)
    {
        // Find closest point on box to the center of the circle.
        Vector2 closestPt = Vector2(
            localToCircle.x >= 0 ? min(halfWidths.x, localToCircle.x) : max(-halfWidths.x, localToCircle.x),
            localToCircle.y >= 0 ? min(halfWidths.y, localToCircle.y) : max(-halfWidths.y, localToCircle.y));

        Vector2 toClosest = closestPt - localToCircle;
        float d2 = toClosest.LengthSq();
        float r2 = shape1->Radius() * shape1->Radius();
        if (d2 > r2)
        {
            return false;
        }

        contact.distance = sqrtf(d2) - shape1->Radius();
        contact.normal = -(rotB * toClosest).Normalized();
        contact.worldPosition = body1->Position() - contact.normal * shape1->Radius();
        return true;
    }
    else
    {
        // Otherwise, find side we're closest to & use that
        float dists[] =
        {
            localToCircle.x - (-halfWidths.x),
            halfWidths.x - localToCircle.x,
            localToCircle.y - (-halfWidths.y),
            halfWidths.y - localToCircle.y
        };

        int iMin = 0;
        for (int i = 1; i < _countof(dists); ++i)
        {
            if (dists[i] < dists[iMin])
            {
                iMin = i;
            }
        }

        contact.distance = -(dists[iMin] + shape1->Radius());
        switch (iMin)
        {
        case 0: contact.normal = Vector2(-1, 0); break;
        case 1: contact.normal = Vector2(1, 0); break;
        case 2: contact.normal = Vector2(0, -1); break;
        case 3: contact.normal = Vector2(0, 1); break;
        default: assert(false); break;
        }

        contact.normal = (rotB * contact.normal).Normalized();
        contact.worldPosition = body1->Position() - contact.normal * shape1->Radius();
        return true;
    }
}

static Vector2 SupportMapping(const BoxShape* box, const Vector2& localDir)
{
    Vector2 half = 0.5f * box->Size();
    return Vector2(
        localDir.x >= 0 ? half.x : -half.x,
        localDir.y >= 0 ? half.y : -half.y
        );
}

bool CollideBoxBox(RigidBody* body1, RigidBody* body2, ContactInfo& contact)
{
    const BoxShape* shape1 = (const BoxShape*)body1->GetShape();
    const BoxShape* shape2 = (const BoxShape*)body2->GetShape();

    Matrix2 rot1(body1->Rotation());
    Matrix2 rot2(body2->Rotation());
    Matrix2 invRot1(rot1.Transposed());
    Matrix2 invRot2(rot2.Transposed());
    Vector2 toBody2 = body2->Position() - body1->Position();

    float minPen = FLT_MAX;
    Vector2 normal, pointOn1;

    Vector2 dirs1[] =
    {
        rot1.col1,
        rot1.col2,
    };

    float bounds1[] =
    {
        shape1->Size().x * 0.5f,
        shape1->Size().y * 0.5f,
    };

    for (int i = 0; i < _countof(dirs1); ++i)
    {
        Vector2 v = dirs1[i].Normalized();
        Vector2 pt = toBody2 + rot2 * SupportMapping(shape2, invRot2 * -v);
        float d = Dot(pt, v);
        float pen = bounds1[i] - d;
        if (pen < 0)
        {
            return false;
        }
        if (pen < minPen)
        {
            minPen = pen;
            normal = -v;
            pointOn1 = pt + normal * pen;
        }

        // Other way
        pt = toBody2 + rot2 * SupportMapping(shape2, invRot2 * v);
        d = Dot(pt, -v);
        pen = bounds1[i] - d;
        if (pen < 0)
        {
            return false;
        }
        if (pen < minPen)
        {
            minPen = pen;
            normal = v;
            pointOn1 = pt + normal * pen;
        }
    }

    Vector2 dirs2[] =
    {
        rot2.col1,
        rot2.col2,
    };

    float bounds2[] =
    {
        shape2->Size().x * 0.5f,
        shape2->Size().y * 0.5f,
    };

    for (int i = 0; i < _countof(dirs2); ++i)
    {
        Vector2 v = dirs2[i].Normalized();
        Vector2 pt = -toBody2 + rot1 * SupportMapping(shape1, invRot1 * -v);
        float d = Dot(pt, v);
        float pen = bounds2[i] - d;
        if (pen < 0)
        {
            return false;
        }
        if (pen < minPen)
        {
            minPen = pen;
            normal = v;
            pointOn1 = pt + toBody2;
        }

        // Other way
        pt = -toBody2 + rot1 * SupportMapping(shape1, invRot1 * v);
        d = Dot(pt, -v);
        pen = bounds2[i] - d;
        if (pen < 0)
        {
            return false;
        }
        if (pen < minPen)
        {
            minPen = pen;
            normal = -v;
            pointOn1 = pt + toBody2;
        }
    }

    contact.normal = normal;
    contact.distance = -minPen;
    contact.worldPosition = body1->Position() + pointOn1;
    return true;
}
