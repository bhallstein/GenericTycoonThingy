#include "Structure.h"
int Structure::s_Total;
Structure::Structure(Address* c, std::string d, int e)	//address, type, income, 
{
	//push back in total vector?
	m_Address = c->GetName();
	c->SetBound();
	m_pLeft = 0;
	m_pRight = 0;
	m_Service = false;
	m_Income = e;//current manager system directly takes income already
	m_Type = d;
	++s_Total;
}

int Structure::GetTotal()
{
	return s_Total;
}

int Structure::GetIncome()
{
	return m_Income;
}

void Structure::SetLeft(Structure* Left)
{
	m_pLeft = Left;
}

void Structure::SetRight(Structure* Right)
{	
	m_pRight = Right;
}
Structure* Structure::GetLeft()
{
	return m_pLeft;
}
Structure* Structure::GetRight()
{
	return m_pRight;
}

void Structure::SetService()//service changes for each building - maybe should be virtual?
{
	if(m_Service)
	{
		m_Service = false;
	}
	else
	{
		m_Service = true;
	}
}

bool Structure::GetService()
{
	return m_Service;
}

std::string Structure::GetType()
{
	return m_Type;
}