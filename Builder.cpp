#include "Builder.h"

Builder::Builder()
{
	m_pFarLeft = 0;
	m_pBuildFarLeft = 0;
	m_struct = 0;
	m_total = 0;
	m_action = false;
	m_money = 0;
}

Builder::~Builder()
{
}

void Builder::addAddress(std::string name)//always adds to the right - code re: the right in place for future
{
	//temp naming system
	Address* pNewAddress = new Address(name);

	if(m_pFarLeft == 0)
	{
		m_pFarLeft = pNewAddress;
	}
	else
	{
		Address* pIter = m_pFarRight;
		pIter->SetRight(pNewAddress);
	}
	m_pFarRight = pNewAddress;
	m_struct = pNewAddress->GetTotal();
}

void Builder::removeAddress()
{
	if(m_pFarLeft == 0)
	{
		std::cout << "\nNo more structures remaining, cannot remove structure.";
	}
	else
	{
		Address* pTemp = m_pFarLeft;
		m_pFarLeft = m_pFarLeft->GetRight();
		delete pTemp;
	}
}

void Builder::addBuilding(std::string b, int& cost)
{
	if((m_pFarLeft == 0) || (m_struct <= m_total)) 
	{
		std::cout << "\nThere are no available structures, cannot build a " << b << std::endl;
	}
	else
	{
		int (*pf)(int)=tolower; 
		transform(b.begin(), b.end(), b.begin(), pf); //change to lower case
	
		bool d = false;	//bool for when a building has sucessfully been produced
		Address* pTarget = m_pFarLeft;

		while(d != true)
		{
			bool x;
			x = pTarget->GetBound();
			if(!x)		//if struct is not bound (i.e. bool is false) then a new building can be made
			{
				for(int i = 0; i < m_structName.size(); i++)
				{
					if(b == m_structName[i])
					{
						int z = m_money - m_Prices[i];
						if(z>= 0)
						{
							m_money -= m_Prices[i];
							Structure* pNewStructure = new Structure(pTarget, m_structName[i], m_Income[i]);
							if(m_pBuildFarLeft == 0)
							{
								m_pBuildFarLeft = pNewStructure;
							}
							else
							{
								Structure* pIter = m_pBuildFarRight;
								pIter->SetRight(pNewStructure);
							}
							m_pBuildFarRight = pNewStructure;

							//action to add to total barbers... maybe vector //m_barb = pNewBarber->GetTotal();
							m_action = true;
							std::cout << "\nA " << m_structName[i] << " has been built, costing ";
							std::cout << m_Prices[i] << " pounds.\n";
							d = true;
							m_total = pNewStructure->GetTotal();
							m_StructTotal[i] += 1;
						}
						else
						{
							std::cout << "\nNot enough money. Mine more money.\n";
							d = true;
						}
					}		
				}
				if(!d)
				{
					std::cout << "\nError! Cannot build a " << b << ".\n\n";
				d = true;
				}
										
			}
			if(!d)
			{
 				pTarget = pTarget->GetRight();
			}
		}	
	}
}

bool Builder::returnAction()
{
	return m_action;
}

void Builder::actionFalse()//needs replacing to a setaction system, with m_action made private
{
	m_action = false;
}

void Builder::setMoney(int money)
{
	m_money = money;
}

int Builder::getMoney()
{
	return m_money;
}

void Builder::LoadStructures(std::vector<std::string> data)//all structures loaded in
{
	int x = data.size();
	x = x/9;
	int z = 0;
	for(int i = 0; i < x; i++)
	{
	m_structName.push_back(data[(0 + z)]);
	int y = atoi(data[(1 + z)].c_str());
	m_Prices.push_back(y);
	y = atoi(data[(2 + z)].c_str());
	m_Income.push_back(y);
	m_Action1.push_back(data[(3 + z)]);
	m_Response1.push_back(data[(4 + z)]);
	m_Action2.push_back(data[(5 + z)]);
	m_Response2.push_back(data[(6 + z)]);
	m_ActionVerb1.push_back(data[(7 + z)]);
	m_ActionVerb2.push_back(data[(8 + z)]);
	z += 9;
	m_StructTotal.push_back(0);
	}
}

void Builder::BuildingReport()
{
	int x = m_structName.size();
	if(m_pBuildFarLeft != 0)
	{
		std::cout << "\nYou currently have:\n";
		for(int i = 0; i < x; i++)
		{
			if(m_StructTotal[i] > 0)
			{
				std::cout << m_StructTotal[i] << " " << m_structName[i];
				if(m_StructTotal[i] > 1)
					std::cout << "s ";
				else
					std::cout << " ";
				
				int y = 0;
				Structure* moo = m_pBuildFarLeft;
				while(moo != 0)
				{
					bool z = moo->GetService();
					if(z)
					y++;
					
					moo = moo->GetRight();
				}
				if(y == 0)
				{
					if(m_StructTotal[i] == 1)
						std::cout << "who is " << m_ActionVerb1[i] << std::endl;
					else
						std::cout << "who are all " << m_ActionVerb1[i] << std::endl;
				}
				else if(y == m_StructTotal[i])
				{
					if(m_StructTotal[i] == 1)
						std::cout << "who is " << m_ActionVerb2[i] << std::endl;
					else
						std::cout << "who are all " << m_ActionVerb2[i] << std::endl;
				}
				else
				{
					std:: cout << "of whom " << y;
				if(m_StructTotal[i] - y == 1)
					std::cout << " is ";
				else
					std::cout << " are ";
				std::cout << m_ActionVerb1[i] << ", and " << (m_StructTotal[i] - y);
				if(m_StructTotal[i] - y == 1)
					std::cout << " is ";
				else
					std::cout << " are ";
				std::cout << m_ActionVerb2[i] << std::endl;
				}
			}
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "\nThere are no structures to report on!\n";
	}
}