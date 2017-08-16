/* 
 * Client.hpp
 *
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "Unit.hpp"

class Client : public Unit
{
public:
	
	// Methods
	

	// Properties
	int Hairyness;
	int Hunger;
	int Tiredness;

protected:
	// Methods
	
	// Properties
	int HairyRate;
	int HungerRate;
	int TiredRate;

};

#endif
