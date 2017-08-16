/*
 *  Returny.hpp - Mister Returny
 *
 */

#ifndef RETURNY_H
#define RETURNY_H

#include <string>

class Returny {
public:
	enum returny_type { empty_returny, payload_returny, killer_returny };
	enum returny_code {
		no_action
	};
	
	Returny(returny_type _type = empty_returny, returny_code _code = no_action, std::string _payload = "") :
		type(_type), code(_code), payload(_payload)
	{
		
	}
	
 	returny_type type;
	returny_code code;
	std::string payload;
	
};

#endif
