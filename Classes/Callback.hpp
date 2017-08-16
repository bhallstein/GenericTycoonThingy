/*
 * Callback.hpp - a class for holding a callback to an instance method
 * 
 * To create a callback: Callback c(&X::method, x_instance);
 * 
 */

#ifndef CALLBACK_H
#define CALLBACK_H

#include "types.hpp"

class CallbackBase {
public:
	virtual void call(Event *) = 0;
};

template <class T>
class MFCallback : public CallbackBase {
	typedef void (T::*mftype)(Event *);
public:
	MFCallback(mftype _f, T *_o) : f(_f), o(_o) { }
	void call(Event *ev) {
		(o->*f)(ev);
	}
private:
	mftype f;
	T *o;
};

class Callback {
public:
	template <class T>
	Callback(void (T::*_f)(Event *), T *_o) : s(new MFCallback<T>(_f, _o)) { }
	void call(Event *ev) {
		s->call(ev);
	}
private:
	CallbackBase *s;
};

#endif
