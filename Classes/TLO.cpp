#include "TLO.hpp"

TLO::TLO(W::EventHandler *_eh) :
	eh(_eh), destroyed(false), hover(false), rotation(0)
{
	// oh hai tlo
}

TLO::~TLO()
{
	// bye now
}