#include "SPHSolver.h"

#include "Renderer.h"
#include "Random.h"
#include "CollisionSolver.h"

#include <iostream>
#include <algorithm>
#include <thread>

static SPHSolver::MoleculesData Mdata;

void SPHSolver::ResetMolecules()
{
	// get the position and scale of the starting box
	glm::mat4 boxData = Renderer::Scene::GetStartingBoxData();
	glm::vec3 boxPos = boxData[0];
	glm::vec3 scale = boxData[1];
	// compute the coordinates of the top-left corner of the box
	// used to randomly set the molecule's position within bounds
	glm::vec3 topLeft;
	topLeft.x = boxPos.x - scale.x * 0.5f;
	topLeft.y = boxPos.y + scale.y * 0.5f;
	for (int32_t i = 0; i < Renderer::Scene::NumMolecules; i++) {
		Mdata.Properties[i].Velocity = glm::vec3(0.0f);
		Mdata.Properties[i].Position.x = Random::GetFloat(topLeft.x, topLeft.x + scale.x);
		Mdata.Properties[i].Position.y = Random::GetFloat(topLeft.y - scale.y, topLeft.y);
		//Mdata.Properties[i].Position.z = Random::GetFloat(topLeft.y - scale.y, topLeft.y);
		Mdata.Properties[i].Position.z = 0.0f;
	}
}

glm::ivec3 SPHSolver::GetGridPosition(const glm::vec3& pos)
{
	// snap the real position to the grid
	glm::ivec3 result;
	result.x = (int)(std::floorf(pos.x / Mdata.h));
	result.y = (int)(std::floorf(pos.y / Mdata.h));
	//result.z = (int)(std::floorf(pos.z / Mdata.h));
	result.z = 0.0f;
	return result;
}

uint32_t SPHSolver::GetHashCodeFromGrid(const glm::ivec3& gridPos)
{
	uint32_t hashCode = (uint32_t)((gridPos.x * 73856093) ^ (gridPos.y * 19349663) ^ (gridPos.z * 8349279)) % (Renderer::Scene::NumMolecules);
	return hashCode;
}

void SPHSolver::CheckNeighbours()
{
	// add the all the molecules' hash and index in an array
	for (uint32_t i = 0; i < Renderer::Scene::NumMolecules; i++) {
		Mdata.SpatialLookup[i].Hash = SPHSolver::GetHashCodeFromGrid(SPHSolver::GetGridPosition(Mdata.Properties[i].PredictedPosition));
		Mdata.SpatialLookup[i].Index = i;
		Mdata.StartIndices[i] = UINT32_MAX;
	}

	// sort the array based on the hash value
	std::sort(Mdata.SpatialLookup.begin(), Mdata.SpatialLookup.end(), 
		[](const SPHSolver::SpatialLookupStruct& a, const SPHSolver::SpatialLookupStruct& b) {
			return a.Hash < b.Hash;
		});

	// swap the ordering in Mdata::properties to match the ordering in the spatial lookup
	// for better cache hit rate
	std::vector<SPHSolver::MoleculeProperties> propertiesCopy = Mdata.Properties;
	for (uint32_t i = 0; i < Renderer::Scene::NumMolecules; i++) {
		Mdata.Properties[i] = propertiesCopy[Mdata.SpatialLookup[i].Index];
		Mdata.SpatialLookup[i].Index = i;
	}

	// fill in the start indices of each hash code
	Mdata.StartIndices[Mdata.SpatialLookup[0].Hash] = 0;
	for (uint32_t i = 1; i < Renderer::Scene::NumMolecules; i++) {
		if (Mdata.SpatialLookup[i].Hash != Mdata.SpatialLookup[i - 1].Hash) {
			Mdata.StartIndices[Mdata.SpatialLookup[i].Hash] = i;
		}
	}
}

// spiky kernel function
float SPHSolver::Kernel(float distance, float radius)
{
	if (distance < 0.0f || distance > radius) {
		return 0.0f;
	}

	float scale = 4.774648f / std::powf(radius, 6.0f);  // 15/(pi * h^6)
	float difference = radius - distance;

	return scale * difference * difference * difference;
}

float SPHSolver::KernelDerivative(float distance, float radius)
{
	if (distance < 0.0f || distance > radius) {
		return 0.0f;
	}

	float scale = 4.774648f / std::powf(radius, 6.0f);  // 15/(pi * h^6)
	float difference = radius - distance;

	return -3.0f * scale * difference * difference;
}

float SPHSolver::ViscosityKernelLaplacian(float distance, float radius)
{
	if (distance < 0.0f || distance > radius) {
		return 0.0f;
	}

	float radius3 = std::powf(radius, 3.0f);
	float scale = 2.387324f / radius3;  // 15/(2pi * h^3)
	float sum = -3.0f * distance / radius3 + 2.0f / (radius * radius) + radius / (distance * distance * distance);
	return scale * sum;
}

float SPHSolver::NearDensityKernel(float distance, float radius)
{
	if (distance < 0.0f || distance > radius) {
		return 0.0f;
	}

	float scale = 6.684507f / std::powf(radius, 7.0f);  // 15/(pi * h^6)
	float difference = radius - distance;

	return scale * difference * difference * difference * difference;
}

float SPHSolver::NearDensityKernelDerivative(float distance, float radius)
{
	if (distance < 0.0f || distance > radius) {
		return 0.0f;
	}

	float scale = 6.684507f / std::powf(radius, 7.0f);  // 15/(pi * h^6)
	float difference = radius - distance;

	return -4.0f * scale * difference * difference * difference;
}

void SPHSolver::Init()
{
	Mdata.Properties = std::vector<SPHSolver::MoleculeProperties>(Renderer::Scene::NumMolecules);
	SPHSolver::ResetMolecules();

	Mdata.Ro0 = 30.0f;

	Mdata.SpatialLookup = std::vector<SpatialLookupStruct>(Mdata.Properties.size());
	Mdata.StartIndices = std::vector<uint32_t>(Mdata.Properties.size());

	Mdata.Offsets = std::vector<glm::ivec3>(27);
	Mdata.Offsets[0] = glm::ivec3(-1,  1, 0);
	Mdata.Offsets[1] = glm::ivec3( 0,  1, 0);
	Mdata.Offsets[2] = glm::ivec3( 1,  1, 0);
	Mdata.Offsets[3] = glm::ivec3(-1,  0, 0);
	Mdata.Offsets[4] = glm::ivec3( 0,  0, 0);
	Mdata.Offsets[5] = glm::ivec3( 1,  0, 0);
	Mdata.Offsets[6] = glm::ivec3(-1, -1, 0);
	Mdata.Offsets[7] = glm::ivec3( 0, -1, 0);
	Mdata.Offsets[8] = glm::ivec3( 1, -1, 0);

	Mdata.Offsets[9] = glm::ivec3(-1,  1, -1);
	Mdata.Offsets[10] = glm::ivec3( 0,  1, -1);
	Mdata.Offsets[11] = glm::ivec3( 1,  1, -1);
	Mdata.Offsets[12] = glm::ivec3(-1,  0, -1);
	Mdata.Offsets[13] = glm::ivec3( 0,  0, -1);
	Mdata.Offsets[14] = glm::ivec3( 1,  0, -1);
	Mdata.Offsets[15] = glm::ivec3(-1, -1, -1);
	Mdata.Offsets[16] = glm::ivec3( 0, -1, -1);
	Mdata.Offsets[17] = glm::ivec3( 1, -1, -1);

	Mdata.Offsets[18] = glm::ivec3(-1,  1, 1);
	Mdata.Offsets[19] = glm::ivec3( 0,  1, 1);
	Mdata.Offsets[20] = glm::ivec3( 1,  1, 1);
	Mdata.Offsets[21] = glm::ivec3(-1,  0, 1);
	Mdata.Offsets[22] = glm::ivec3( 0,  0, 1);
	Mdata.Offsets[23] = glm::ivec3( 1,  0, 1);
	Mdata.Offsets[24] = glm::ivec3(-1, -1, 1);
	Mdata.Offsets[25] = glm::ivec3( 0, -1, 1);
	Mdata.Offsets[26] = glm::ivec3( 1, -1, 1);
}

void SPHSolver::SolveCollisions(MoleculeProperties& props, float scale, const glm::vec3& bounds)
{
	float lowestvertexPos = props.Position.y - 0.5f * scale;
	float highestvertexPos = props.Position.y + 0.5f * scale;
	float rightmostvertexPos = props.Position.x + 0.5f * scale;
	float leftmostvertexPos = props.Position.x - 0.5f * scale;

	float dampness = 0.05f;
	if (highestvertexPos > -bounds.y) {
		props.Velocity.y = -props.Velocity.y * dampness;
		props.Position.y = -bounds.y - 0.5f * scale;
	}
	if (lowestvertexPos < bounds.y) {
		props.Velocity.y = -props.Velocity.y * dampness;
		props.Position.y = bounds.y + 0.5f * scale;
	}
	if (rightmostvertexPos > bounds.x) {
		props.Velocity.x = -props.Velocity.x * dampness;
		props.Position.x = bounds.x - 0.5f * scale;
	}
	if (leftmostvertexPos < -bounds.x) {
		props.Velocity.x = -props.Velocity.x * dampness;
		props.Position.x = -bounds.x + 0.5f * scale;
	}
}

void SPHSolver::Update(float dt, const glm::vec3& bounds)
{
	//dt = 0.0016666666f;
	// make sure to update all that can be changed through the UI
	Mdata.Scale = Renderer::Scene::GetMoleculeScale();
	Mdata.h = Renderer::Scene::GetInfluenceRadius();
	Mdata.Viscosity = Renderer::Scene::GetViscosityStrength();
	//Mdata.Mass = Mdata.h * Mdata.h * Mdata.h * Mdata.Ro0;
	Mdata.Mass = 1.0f;

	// apply all the external forces and predict the position
	for (uint32_t i = 0; i < Renderer::Scene::NumMolecules; i++) {
		SPHSolver::MoleculeProperties& props = Mdata.Properties[i];
		props.Velocity.y += -9.81f * dt;
		props.PredictedPosition = props.Position + props.Velocity * dt;
	}

	SPHSolver::CheckNeighbours();

	// compute the density and pressure
	const uint32_t poolSize = std::thread::hardware_concurrency() / 2;
	std::vector<std::thread> threadPool(poolSize);

	for (uint32_t z = 0; z < poolSize; z++) {
		threadPool[z] = std::thread([z, poolSize]() {
			for (uint32_t i = z; i < Renderer::Scene::NumMolecules; i += poolSize) {
				SPHSolver::MoleculeProperties& props = Mdata.Properties[i];
				props.Density = 0.0f;
				props.NearDensity = 0.0f;
				glm::ivec3 gridPos = SPHSolver::GetGridPosition(props.PredictedPosition);

				// go through the 3x3x3 grid
				for (uint32_t k = 0; k < 9; k++) {
					glm::ivec3 neighbourGrid = gridPos + Mdata.Offsets[k];
					uint32_t code = SPHSolver::GetHashCodeFromGrid(neighbourGrid);
					uint32_t startIndex = Mdata.StartIndices[code];

					// compute the density
					for (uint32_t j = startIndex; j < Renderer::Scene::NumMolecules; j++) {
						// stop if looking in another cell
						if (Mdata.SpatialLookup[j].Hash != code) {
							break;
						}
						// a particle should not influence itself
						if (Mdata.SpatialLookup[j].Index == i) {
							continue;
						}

						const SPHSolver::MoleculeProperties& other = Mdata.Properties[Mdata.SpatialLookup[j].Index];
						float distance = glm::length(props.PredictedPosition - other.PredictedPosition);
						float influence = SPHSolver::Kernel(distance, Mdata.h);
						props.Density += Mdata.Mass * influence;

						props.NearDensity += Mdata.Mass * SPHSolver::NearDensityKernel(distance, Mdata.h);
					}
				}
				props.Pressure = 15.0f * (props.Density - Mdata.Ro0);
				props.NearPressure = 2.0f * props.NearDensity;
			}
		});
	}
	for (auto& t : threadPool) {
		t.join();
	}

	// compute the final total force
	for (uint32_t z = 0; z < poolSize; z++) {
		threadPool[z] = std::thread([z, poolSize, dt, bounds]() {
			for (uint32_t i = z; i < Renderer::Scene::NumMolecules; i += poolSize) {
				SPHSolver::MoleculeProperties& props = Mdata.Properties[i];
				glm::ivec3 gridPos = SPHSolver::GetGridPosition(props.PredictedPosition);
				glm::vec3 totalForce = glm::vec3(0.0f);
				// go through the 3x3x3 grid
				for (uint32_t k = 0; k < 9; k++) {
					glm::ivec3 neighbourGrid = gridPos + Mdata.Offsets[k];
					uint32_t code = SPHSolver::GetHashCodeFromGrid(neighbourGrid);
					uint32_t startIndex = Mdata.StartIndices[code];

					// compute the pressure force
					for (uint32_t j = startIndex; j < Renderer::Scene::NumMolecules; j++) {
						// stop if looking in another cell
						if (Mdata.SpatialLookup[j].Hash != code) {
							break;
						}
						// a particle should not influence itself
						if (Mdata.SpatialLookup[j].Index == i) {
							continue;
						}

						const SPHSolver::MoleculeProperties& other = Mdata.Properties[Mdata.SpatialLookup[j].Index];
						if (other.Density < 0.01f || props.Density < 0.01f || other.NearDensity < 0.01f) {
							continue;
						}
						glm::vec3 difference = props.PredictedPosition - other.PredictedPosition;
						float length = glm::length(difference);
						// if the length is too small, ignore
						if (length < 0.00001f) {
							continue;
						}
						difference = glm::normalize(difference);
						float aux = (props.Pressure + other.Pressure) / (2.0f * other.Density);
						float slope = SPHSolver::KernelDerivative(length, Mdata.h);
						totalForce += -aux * slope * Mdata.Mass * difference;

						aux = (props.NearPressure + other.NearPressure) / (2.0f * other.NearDensity);
						slope = SPHSolver::NearDensityKernelDerivative(length, Mdata.h);
						totalForce += Mdata.Mass * aux * -slope * difference;

						// apply viscosity
						difference = other.Velocity - props.Velocity;
						length = glm::length(difference);
						// if the length is too small, get a new random direction
						if (length < 0.00001f) {
							continue;
						}
						difference = glm::normalize(difference);
						float laplacian = SPHSolver::ViscosityKernelLaplacian(length, Mdata.h);
						totalForce += Mdata.Viscosity * Mdata.Mass / other.Density * difference;
					}
				}

				props.Velocity += dt / Mdata.Mass * totalForce;
				props.Position += dt * props.Velocity;

				//SPHSolver::SolveCollisions(props, Mdata.Scale, bounds);
				CollisionSolver::ContainerCollision(props, Mdata.Scale);
			}
		});
		
	}
	for (auto& t : threadPool) {
		t.join();
	}
}

std::vector<SPHSolver::MoleculeProperties>& SPHSolver::GetProperties()
{
	return Mdata.Properties;
}
