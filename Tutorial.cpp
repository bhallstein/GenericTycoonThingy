#include "Tutorial.h"
#include <iostream>

Tutorial::Tutorial()
{
}

Tutorial::~Tutorial(void)
{
}

void Tutorial::Info()
{
	std::cout << "\n\tWelcome to Man Flesh Tycoon, the management game centered around\n\t\tthe commoditization of human flesh!\n";

	bool TutOver = false;

	do
	{
		std::cout << "\nPlease select a category for more information.\n";
		std::cout << "0 - End Tutorial \n1 - Constructing \n2 - Barbers \n3 - Pie Shops\n";
	
		int x;
		std::cin >> x;
		switch(x)
		{
		case 0:
			TutOver = true;
			break;
		case 1:
			std::cout << "\nEnter build followed by the structure name - barber or pieshop to set up a business.\n\n";
			break;
		case 2:
			std::cout << "\nBarbers can be set to either kill or shave. When shaving, they earn some money, when killing";
			std::cout << "they produce flesh but no money.\n\n";
			break;
		case 3:
			std::cout << "\nPie Shops produce a small amount of money by default, but when you produce flesh from a barber";
			std::cout << "they begin to sell pies, which earn you larger amounts of money.\n\n";
		default:
			std::cout << "\nError: please enter one of the available numbers.\n\n";
		}
	}
	while(!TutOver);
	
	
}