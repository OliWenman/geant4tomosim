#include "Simulation.hh"

int main()
{
	Simulation Sim;

	Sim.Initialize();
	Sim.RunSimulation();
	Sim.KillSimulation();
  	
   	return 0;
}

