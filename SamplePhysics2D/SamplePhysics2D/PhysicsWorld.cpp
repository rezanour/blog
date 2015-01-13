#include "Precomp.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "RigidBodyPair.h"
#include "Shape.h"
#include "DebugRenderer.h"

PhysicsWorld::PhysicsWorld(const Vector2& gravity, int maxIterations)
    : _gravity(gravity)
    , _maxIterations(maxIterations)
{
}

void PhysicsWorld::AddBody(RigidBody* body)
{
    _bodies.push_back(body);
}

void PhysicsWorld::RemoveBody(RigidBody* body)
{
    for (auto it = std::begin(_bodies); it != std::end(_bodies); ++it)
    {
        if ((*it) == body)
        {
            _bodies.erase(it);
            return;
        }
    }
}

void PhysicsWorld::Update(float dt)
{
    float invDt = dt > 0.0f ? 1.0f / dt : 0.0f;

    // Determine overlapping bodies and update contact points
    UpdatePairs();

    // Integrate forces to obtain updated velocities
    for (auto& body : _bodies)
    {
        if (body->InvMass() == 0.0f)
            continue;

        body->LinearVelocity() += dt * (_gravity + body->InvMass() * body->Force());
        body->AngularVelocity() += dt * body->InvI() * body->Torque();
    }

    // Do all one time init for the pairs
    for (auto& pair : _pairs)
    {
        pair.second.PreSolve(invDt);
    }

    // Sequential Impulse (SI) loop. See Erin Catto's GDC slides for SI info
    for (int i = 0; i < _maxIterations; ++i)
    {
        for (auto& pair : _pairs)
        {
            pair.second.Solve();
        }
    }

    // Integrate new velocities to obtain final state vector (position, rotation).
    // Also clear out any forces in preparation for the next frame
    for (auto& body : _bodies)
    {
        if (body->InvMass() == 0.0f)
            continue;

        body->Position() += dt * body->LinearVelocity();
        body->Rotation() += dt * body->AngularVelocity();

        body->Force() = Vector2(0, 0);
        body->Torque() = 0.0f;
    }
}

void PhysicsWorld::Draw(DebugRenderer* renderer)
{
    for (auto& body : _bodies)
    {
        const Shape* shape = body->GetShape();

        switch (shape->Type())
        {
        case ShapeType::Circle:
            renderer->DrawCircle(body->Position(), ((CircleShape*)shape)->Radius(), body->Rotation());
            break;

        case ShapeType::Box:
            renderer->DrawBox(body->Position(), ((BoxShape*)shape)->Size(), body->Rotation());
            break;

        default:
            assert(false);
            break;
        }
    }

    for (auto& pair : _pairs)
    {
        if (pair.second.HasContact())
        {
            renderer->DrawPoint(pair.second.Contact().worldPosition);
        }
    }
}

void PhysicsWorld::UpdatePairs()
{
    // Ideally, we'd implement some sort of broadphase to reduce the number of pairs.
    // For now, we'll just brute force n^2
    for (int i = 0; i < (int)_bodies.size(); ++i)
    {
        RigidBody* body1 = _bodies[i];

        for (int j = i + 1; j < (int)_bodies.size(); ++j)
        {
            RigidBody* body2 = _bodies[j];

            // If both are completely immovable, nothing to do
            if (body1->InvMass() == 0.0f && body2->InvMass() == 0.0f)
            {
                continue;
            }

            // Define new pair structure
            RigidBodyPair pair(body1, body2);
            PairKey key(body1, body2);

            if (pair.HasContact())
            {
                // Update the existing pair, if one exists. If not, add one
                auto it = _pairs.find(key);
                if (it == std::end(_pairs))
                {
                    _pairs.insert(std::make_pair(key, pair));
                }
                else
                {
                    it->second = pair;
                }
            }
            else
            {
                _pairs.erase(key);
            }
        }
    }
}
