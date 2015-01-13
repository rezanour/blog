#pragma once

#include "RigidBodyPair.h"

class RigidBody;
class DebugRenderer;

// The physics world is the container for the physics simulation.
class PhysicsWorld
{
public:
    // Construct the world with a gravity vector and the maximum
    // number of iterations the solver is allowed to use.
    PhysicsWorld(const Vector2& gravity, int maxIterations);

    void AddBody(RigidBody* body);
    void RemoveBody(RigidBody* body);

    // Step the simulation forward by dt seconds
    void Update(float dt);

    void Draw(DebugRenderer* renderer);

private:
    void UpdatePairs();

    Vector2 _gravity;
    int _maxIterations;
    std::vector<RigidBody*> _bodies;
    std::map<PairKey, RigidBodyPair> _pairs;
};
