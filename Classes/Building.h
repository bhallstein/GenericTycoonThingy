 #pragma once
#include "Address.h"

class Building	//base class for other structure types
{
public:
	Building();

	int GetTotal();

	int GetIncome();

	std::string m_Address;

	void SetLeft(Building* Left);
	void SetRight(Building* Right); 

	Building* GetLeft();
	Building* GetRight();

	void SetService();
	bool GetService();

	std::string GetType();

protected:
	int m_Income;

	Building* m_pLeft;
	Building* m_pRight;

	static int s_Building;

	bool m_Service;

	std::string m_Type;
};
