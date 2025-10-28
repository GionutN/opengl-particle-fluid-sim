	Overview
	This project simulates a fluid using the Lagrangian approach, where molecules store the data needed at each step in the Smoothed Particle Hydrodynamics (SPH) solver.

	Requirements
	Requires Visual Studio 2022 and an OpenGL 3.3 compatible GPU.

	Project description
	The code in this project was inspired by Sebastian Lague's Fluid Simulator (https://github.com/SebLague/Fluid-Sim), where I attempt a 2D version of the simulation.

	Third-Party Libraries
	The following third-party libraries are used:
- GLFW - 3.4
	- Repo: https://github.com/glfw/glfw
	- License: zlib/libpng
	- Purpose: window/context creation and input.

- GLEW - 2.2.0
	- Repo: https://github.com/nigels-com/glew
	- License: BSD-Style
	- Purpose: OpenGL extension loading.

- GLM - 1.0.1
	- Repo: https://github.com/g-truc/glm
	- License: MIT
	- Purpose: header-only math (vec/mat/quaternion).

- ImGui
	- Repo: https://github.com/ocornut/imgui
	- License: MIT
	- Purpose:  bloat-free graphical user interface library for C++.

	Building the project
	To get the project up and running, download the zip archive, open the .sln file and compile the solution.

	Architecture overview
	The backbone of this project is the Application class, which handles the initialization of OpenGL and GLFW during construction.
	The only accessible method is the Run method, which encapsulates the game loop along with its four main steps:
	- BeginFrame - clears the color buffer
	- UpdateFrame - handles the logic for ImGui and runs the SPH solver
	- DrawFrame - renders all the particles and their container
	- EndFrame - swaps buffers and checks for window closure
	The class also owns the meshes and shaders used.
	Besides OpenGL, the class also initializes the Renderer and the SPH solver.

	The Renderer is divided into two parts, a UI and a scene renderer, each one implemented as a singleton.
	The UI manages input for starting, pausing/resuming and resetting the simulation. It also allows modification of various fluid parameters, and displays telemetry data, such as how many quads are rendered per frame, the number of molecules and the frames per second.
	The Renderer::Scene issues the actual draw calls for each mesh, using the transforms computed by the SPH solver to place them correctly.

	The SPH solver is the core of the project. Here is implemented the main simulation logic. After the user sets the properties, the simulation starts inside the Update method. Here are the main steps that take place:
	- external forces (gravity, wind, etc.) are applied to each molecule. The main integration method is predictor-corrector, so at each step the "next-step" position is used inside the computations.
	- the movement of the particles is determined by the difference in pressure across the fluid, and for the pressure to be computed, density is needed.
	- once pressure differences are determined, the solver converts this into actual forces that will be applied to each molecule, viscosity dampening is added, and finally the velocity and current positions is computed.

	All of these steps can be parallelized. The solver spawns CPU threads for pressure computations, then a barrier is used to ensure all the molecules have updated pressures. Then for turning pressure differences into forces, the threads all run in parallel again.
	Although this improves performance, another optimization further reduces computation. Since the neighbouring particles that are closer to the current one have a higher influence than the ones further away, there is a lot of computing power wasted on negligeable forces. A solutions is to split the entire space in a grid, and assign to each of the cells has a hash code, and so only the molecules that are in cells with the same hash code are used.
	After these optimizations, 2048 molecules can be processed 7 times per frame with 6 threads.

	Features
	- for a better visualization, the molecules change their color based on their speed, making vortices easy to observe.
	- a simple UI for configuring the initial distribution of molecules
	- ability to move, scale and rotate the container for direct interaction with the fluid

	Controls
	Pressing the C key brings up the container and fluid properties window, and the T key brings up the telemetry window.

	Planned Features
	The main goals for future development are:
	- moving computation to compute shaders to handle more particles
	- extending to 3D simulations
	- implementing obstacles
	- adding realistic water shaders
	- simulating spray and foam

	Bibliography
	https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_09_ParticleBasedFluids.pdf
	https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
	"Incompressible fluid simulation and advanced surface handling with SPH" by Barbara Solenthaler


	License
	This project is provided for educational purposes. All third-party libraries retain their original licenses.
