#include "Core.h"
#include "Application.h"

int main()
{
	bool result;
	Application app("Particle-based fluid simulation", &result);
	if (result == true) {
		app.Run();
	}
	return 0;
}
