/*
 *  ButtonReceiver.hpp - reciever for a button action
 *
 */

#ifndef BUTTONRECEIVER_H
#define BUTTONRECEIVER_H

class Button;

class ButtonReceiver {
public:
	virtual void buttonClick(Button *) = 0;
	
};

#endif
