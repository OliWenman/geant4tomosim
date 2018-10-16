#include <cstdlib>
#include <iostream>

using namespace std;

int main() 
{	
	int TotalTests;
	cout << std::endl << "Please enter the number of speed tests you would like to do ";
	cin >> TotalTests;
	cout << endl << "Launching Speed Test" << endl;

	for (int nTest = 1; nTest < TotalTests; nTest++)
	{
		cout << endl << "############################################################" << endl;
		cout << endl << "                       Time Test " << nTest << endl;
		cout << endl << "############################################################" << endl;
   		system("./X-Ray_Simulation");
	}
	
	cout << endl << "############################################################" << endl;
	cout << endl << "                     Time Test complete! " << endl;
	cout << endl << "############################################################" << endl;

   	return 0;
}


