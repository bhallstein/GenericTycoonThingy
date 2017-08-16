#include "Address.hpp"
int Address::s_Total = 0;//counter for buildings
Address::Address(const std::string& name)
{
	++s_Total;
	m_Name = name;
	m_pLeft = 0;
	m_pRight = 0;
	isBound = false;
}

Address::Address(const Address&)
{
	++s_Total;
}

Address::~Address()
{
	--s_Total;
}

int Address::GetTotal()
{
	return s_Total;
}

void Address::SetLeft(Address* Left)
{
	m_pLeft = Left;
}

void Address::SetRight(Address* Right)
{	
	m_pRight = Right;
}
//Address* Address::GetLeft()
//{
//	return m_pLeft;
//}
Address* Address::GetRight()
{
	return m_pRight;
}

bool Address::GetBound()
{
	return isBound;
}

std::string Address::SetBound()
{
	isBound = true;
	return m_Name;
}

void Address::StopBound()
{
	isBound = false;
}

std::string Address::GetName()
{
	return m_Name;
}