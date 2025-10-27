#pragma once

#include "SPHSolver.h"

class CollisionSolver
{
public:
	static void ContainerCollision(SPHSolver::MoleculeProperties& props, float moleculeScale);

private:
	CollisionSolver() = default;

};