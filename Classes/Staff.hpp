/* Staff.hpp - Staff inherits from Unit and represents a single Staff member of any type
 *
 */


#ifndef STAFF_H
#define STAFF_H

#include "Unit.hpp"

class NavMap;

class Staff : public Unit
{
public:
	Staff(NavMap *navmap, int _x, int _y) : Unit(navmap, _x, _y) { }
	~Staff() { }

};

#endif