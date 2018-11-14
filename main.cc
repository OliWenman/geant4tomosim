#include "Simulation.hh"
#include <iostream>

void Ref(int &n)
{	++n;}

int main()
{
	Simulation Sim;

	Sim.Initialize();
	Sim.RunSimulation();
	Sim.KillSimulation();	

   	return 0;
}

