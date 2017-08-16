#ifndef ADDRESS_H
#define ADDRESS_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class Address
{
public:
	Address(const std::string& name = "");
	Address(const Address&);
	~Address();

	void SetLeft(Address* Left);
	void SetRight(Address* Right);
	Address* m_pLeft;
	Address* m_pRight;
	//Address* GetLeft();
	Address* GetRight();

	static int s_Total;	
	static int GetTotal();

	bool isBound;
	bool GetBound();
	std::string SetBound();
	void StopBound();

	std::string GetName();

protected:
	std::string m_Name;

private:
	
};

#endif