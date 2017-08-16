#ifndef BUILDER_H
#define BUILDER_H

#include "Address.h"
#include "Structure.h"

class Builder
{
public:
	Builder();
	~Builder();

	void addAddress(std::string name);
	void removeAddress();

	void addBuilding(std::string b, int& cost);

	int m_struct;
	int m_total;

	bool m_action;
	bool returnAction();
	void actionFalse();

	Address* m_pFarLeft;
	Address* m_pFarRight;

	Structure* m_pBuildFarLeft;
	Structure* m_pBuildFarRight;

	std::vector<std::string> m_structData;
	std::vector<std::string> m_structName;
	std::vector<int> m_Prices;
	std::vector<int> m_Income;
	std::vector<std::string> m_Action1;//
	std::vector<std::string> m_Action2;//
	std::vector<std::string> m_Response1;//used in report
	std::vector<std::string> m_Response2;//
	std::vector<std::string> m_ActionVerb1;//
	std::vector<std::string> m_ActionVerb2;//
	std::vector<int> m_StructTotal;//
	void LoadStructures(std::vector<std::string> data);

	int m_money;
	void setMoney(int money);//to be rolled into LoadStruct
	int getMoney();

	void BuildingReport();
private:
	
};

#endif