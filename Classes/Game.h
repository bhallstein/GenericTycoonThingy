#pragma once
#include "Builder.h"
#include "Tutorial.h"
#include "Level.h"
#include <string.h>

class Game
{
public:
	Game(void);
	~Game(void);
	
	int Run();

	int m_AddressNumb;
	int m_BarberNumb;
	int M_PieshopNumb;
	void BuildingReport(Builder& bill);

	bool Action(char x[256], Builder& bill);

	void GatherIncome(Builder& bill);

	int m_Money;
private:
	
	int m_Days;
};
