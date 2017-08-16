#include "Building.h"
int Building::s_Building = 0;
	Building::Building()
	{
	++s_Building;
	m_pLeft = 0;
	m_pRight = 0;
	m_Service = false;
	}

int Building::GetTotal()
{
	return s_Building;
}

int Building::GetIncome()
{
	return m_Income;
}

void Building::SetLeft(Building* Left)
{
	m_pLeft = Left;
}

void Building::SetRight(Building* Right)
{	
	m_pRight = Right;
}
Building* Building::GetLeft()
{
	return m_pLeft;
}
Building* Building::GetRight()
{
	return m_pRight;
}

void Building::SetService()//service changes for each building - maybe should be virtual?
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

bool Building::GetService()
{
	return m_Service;
}

std::string Building::GetType()
{
	return m_Type;
}