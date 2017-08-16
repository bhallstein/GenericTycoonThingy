#include "Game.h"
using namespace std;

Game::Game()
{
	m_Money = 100;
	m_Days = 0;
}

Game::~Game()
{
}

int Game::Run()
{
	Builder bill;
	Level level1(bill); //eventually need level manager to load certain levels

	int error = 0;
	bool GameOver = false;
	
	do
	{
		bill.actionFalse();
		bill.setMoney(m_Money);

		cout << "Day " << m_Days << endl;
		cout << "Money: " << m_Money << endl;
		cout << "Please Enter an action.\n";
		
		char Input[256];
		cin.getline ( Input, 256, '\n' );

		if(Input != "")
		{
			GameOver = Action(Input, bill);	
		}
		else
		{
			cout << "No command has been entered - please enter an action.\n\n";
		}
		
			
		bool action = bill.returnAction();
		if(action)
		{
			m_Money = bill.getMoney();
			GatherIncome(bill);
			m_Days++;
		}
	}
	while(!GameOver);

	return error;
}


bool Game::Action(char Input[256], Builder& bill)
{
		bool GameOver = false;
		Input = strcat(Input, " default");	//error c2440 cannot convert from char* to char[256]
		char *first = strtok(Input, " ");
		char *second = strtok(NULL, " ");
		if(second != 0)
		{
			string uno = static_cast<string>(first);
			string dos = static_cast<string>(second);
		
			if(uno == "build")
			{
				bill.addBuilding(dos, m_Money);
			}
			else if(uno == "help")
			{
				Tutorial tut;
				tut.Info();
				cout << "\nClosing Tutorial\n";
			}	
			else if(uno == "report")
			{
				bill.BuildingReport();
			}
			else if(uno == "quit")
			{
				cout << "\nEnding Game.";
				GameOver = true;
			}
			else if(uno == "skip")
			{
				cout << "No action being taken this day.\n\n";
				bill.m_action = true;
			}
			else
			{
				bool change = false;
				for(int i = 0; i < bill.m_structName.size(); i++)
				{
					if(uno == bill.m_structName[i])//match structname
					{
						if(dos == bill.m_Action1[i])//first action - sets action to true
						{					
							Structure* moo = bill.m_pBuildFarLeft;
							while(change == false && moo != 0)
							{
								if((moo->GetType() == bill.m_structName[i]) && (moo->GetService() == false))
								{
									moo->SetService();
									change = true;
									cout << endl << bill.m_structName[i] << " set to " << bill.m_Action1[i];
									cout << "\n\n";
								}
								moo = moo->GetRight();
							}
					}
					else if(dos == bill.m_Action2[i])//second action - sets action to false
						{
							Structure* moo = bill.m_pBuildFarLeft;
							while(change == false && moo != 0)
							{
								if((moo->GetType() == bill.m_structName[i]) && (moo->GetService() == true))
								{
									moo->SetService();
									change = true;
									cout << endl << bill.m_structName[i] << " set to " << bill.m_Action2[i];
									cout << "\n\n";
								}
								moo = moo->GetRight();
							}
						}
						else
						{
							cout << "\nError: Unrecognised action for " << bill.m_structName[i] <<"s!\n\n";
						}
					}
				}
				if(!change)
				{
					cout << "\nUnrecognised command. Please try again.\n\n";
					
				}
			}		
		}
		return GameOver;
}

void Game::GatherIncome(Builder& bill)
{
	Structure* moo = bill.m_pBuildFarLeft;
	while (moo != 0)
	{
		m_Money += moo->GetIncome();
		moo = moo->GetRight();
	}
}

