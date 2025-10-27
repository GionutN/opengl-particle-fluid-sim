#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class SPHSolver
{
public:
	struct MoleculeProperties
	{
		glm::vec3 Position;
		glm::vec3 PredictedPosition;
		glm::vec3 Velocity;
		float Density;
		float NearDensity;
		float Pressure;
		float NearPressure;
	};

	struct SpatialLookupStruct
	{
		uint32_t Index; // the position in the MoleculesData properties vector
		uint32_t Hash;  // associated hash code
	}; 
	
	struct MoleculesData
	{
		float Scale;
		float h;  // influence radius
		float Mass;
		float Ro0;  // fluid density at rest, measured in kg/m^3
		float Viscosity;
		std::vector<SPHSolver::MoleculeProperties> Properties;

		std::vector<SPHSolver::SpatialLookupStruct> SpatialLookup;  // the array of neighbours
		std::vector<uint32_t> StartIndices;		// the start positions of each hash code
		std::vector<glm::ivec3> Offsets;        // the offsets that form the 3x3 grid around the molecule
	};

public:
	static void Init();
	static void Update(float dt, const glm::vec3& bounds);
	static void ResetMolecules();

	static glm::ivec3 GetGridPosition(const glm::vec3& pos);
	static uint32_t GetHashCodeFromGrid(const glm::ivec3& gridPos);
	static void CheckNeighbours();

	static float Kernel(float distance, float radius);
	static float KernelDerivative(float distance, float radius);
	static float ViscosityKernelLaplacian(float distance, float radius);
	static float NearDensityKernel(float distance, float radius);
	static float NearDensityKernelDerivative(float distance, float radius);

	static void SolveCollisions(MoleculeProperties& props, float scale, const glm::vec3& bounds);

	static std::vector<SPHSolver::MoleculeProperties>& GetProperties();


private:
	SPHSolver() = default;

};