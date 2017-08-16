/*
 * UIView.hpp - a UI view
 *
 * A View that has a set of buttons.
 * Uses the Callback system to let things subscribe to actions.
 * 
 */

#ifndef UIVIEW_H
#define UIVIEW_H

#include <string>
#include <vector>
#include <map>

#include "types.hpp"
#include "View.hpp"

class Button;
class Callback;

class UIView : public View {
public:
	UIView(W *, JenniferAniston &);
	~UIView();
	
	void processMouseEvent(Event *);
	void subscribe(const char *, Callback *);
	
protected:
	std::vector<Button *> buttons;
	std::map<std::string, Callback *> subscriptions;
};

#endif
