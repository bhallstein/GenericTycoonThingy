#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "Address.h"

class Structure
{
public:
	Structure(Address* c, std::string d, int e);
	//need destructor at some point

	int GetTotal();

	int GetIncome();

	std::string m_Address;

	void SetLeft(Structure* Left);
	void SetRight(Structure* Right); 

	Structure* GetLeft();
	Structure* GetRight();

	void SetService();
	bool GetService();

	std::string GetType();
protected:
	int m_Income;

	Structure* m_pLeft;
	Structure* m_pRight;

	static int s_Total;

	bool m_Service;

	std::string m_Type;
};

#endif