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

#define ALLOW_DRAG true
#define DISALLOW_DRAG false

class Button;
class Callback;
class ResponderMap;

class UIView : public View {
public:
	UIView(W *, JenniferAniston &, ResponderMap *, bool _allowDrag);
	~UIView();
	
	void processMouseEvent(Event *);
	void subscribeToButtons(Callback *);
	
protected:
	ResponderMap *rm;
	bool allowDrag, dragloop;
	intcoord drag_initial;
	
	std::vector<Button*> buttons;
	std::vector<Callback*> subscribers;
};

#endif
