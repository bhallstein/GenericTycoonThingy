/*
 * Serializer.hpp
 *
 * - A Serializer is a templated class created to wrap properties of a Serializable or one of its subclasses.
 * - For serialization, it uses helper functions, _serializez(x), to create a representation of the property
 *   it wraps which can be written out to a lua file.
 * - For deserialization, it calls a _deserialize fn, which sets the value of the property from a given LuaObj
 * - Generally, all that is needed to add support for a type is to implement a _serialize & _deserialize function for 
 *   that type.
 * 
 */

#ifndef Serializer_h
#define Serializer_h

#include "LuaObj.h"
#include "W.h"
#include "Serializable.hpp"
#include "UIDManager.hpp"
#include "Controller.hpp"

/*
 * Serializer functions:
 *  - used by a Serializer to create a lua-writable representation of a Serializable property
 *
 */

template <typename T>
std::string _numericalSerialize(T &x) {
  std::stringstream ss;
  ss.precision(8);
  ss << std::boolalpha << x;
  return ss.str();
}
std::string _serialize(int &x);
std::string _serialize(float &x);
std::string _serialize(double &x);
std::string _serialize(bool &b);
std::string _serialize(std::string &s);
std::string _serialize(W::v2i &p);
std::string _serialize(W::v2f &s);
std::string _serialize(W::fRect &r);
std::string _serialize(const UID &uid);
std::string _serialize(const UnitMode::T &);
template <typename T>
std::string _serialize(const std::vector<T> &x) {
  std::string s = "{ ";
  for (typename std::vector<T>::const_iterator it = x.begin(); it < x.end(); ++it) {
    T v = *it;
    s += _serialize(v) + (it == x.end() - 1 ? "" : ", ");
  }
  s += " }";
  return s;
}
std::string _serialize(const std::map<UID,UID> &);


/*
 * Deserializer functions
 *  - used by a Serializer to set a property of a Serializable from a given LuaObj
 *
 */

void _deserialize(LuaObj &, int &);
void _deserialize(LuaObj &, float &);
void _deserialize(LuaObj &, bool &);
void _deserialize(LuaObj &, std::string &);
void _deserialize(LuaObj &, W::v2i &);
void _deserialize(LuaObj &, W::v2f &);
void _deserialize(LuaObj &, W::fRect &);
void _deserialize(LuaObj &, std::vector<int> &);
void _deserialize(LuaObj &, std::vector<float> &);
void _deserialize(LuaObj &, std::vector<bool> &);
void _deserialize(LuaObj &, std::vector<std::string> &);
void _deserialize(LuaObj &, std::vector<W::v2i> &);
void _deserialize(LuaObj &, std::vector<W::v2f> &);
void _deserialize(LuaObj &, UID &);
void _deserialize(LuaObj &, UnitMode::T &);
void _deserialize(LuaObj &, std::map<UID,UID> &);
void _deserialize(LuaObj &o, SeekTarget::Type &);


/* SerializerBase class */

class SerializerBase {
public:
  virtual std::string serialize(Serializable *) = 0;
  virtual void deserialize(LuaObj &, Serializable *) = 0;
};


/* Serializer template class */

template<class T, typename mtype>
class Serializer : public SerializerBase {
  typedef mtype T::*ptype;
  ptype ptr;
public:
  Serializer(ptype _ptr) : ptr(_ptr) { }

  std::string serialize(Serializable *_o) {
    T *o = (T*) _o;
    return _serialize(o->*ptr);
  }
  void deserialize(LuaObj &luaObj, Serializable *_o) {
    T *o = (T*) _o;
    _deserialize(luaObj, o->*ptr);
  }
};


/* makeSerializer type-inferring factory function */

template<class T, typename mtype>
SerializerBase* makeSerializer(mtype T::*ptr) {
  return new Serializer<T, mtype>(ptr);
}


/* Pretty indenting function for lua */

void reindentLuaString(std::string &s);

#endif
