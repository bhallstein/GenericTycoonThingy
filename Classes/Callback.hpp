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
	virtual void call() = 0;
};

template <class T>
class MFCallback : public CallbackBase {
	typedef void (T::*mftype)();
public:
	MFCallback(mftype _f, T *_o) : f(_f), o(_o) { }
	void call() {
		(o->*f)();
	}
private:
	mftype f;
	T *o;
};

class Callback {
public:
	template <class T>
	Callback(void (T::*_f)(), T *_o) : s(new MFCallback<T>(_f, _o)) { }
	void call() {
		s->call();
	}
private:
	CallbackBase *s;
};

#endif
