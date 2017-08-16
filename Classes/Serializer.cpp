#include "Serializer.hpp"

/* _serialize fn implementations */

std::string _serialize(int &x)    { return _numericalSerialize(x); }
std::string _serialize(float &x)  { return _numericalSerialize(x); }
std::string _serialize(double &x) { return _numericalSerialize(x); }
std::string _serialize(bool &b)   { return _numericalSerialize(b); }
std::string _serialize(std::string &s) {
	std::stringstream ss;
	ss << "\"";
	for (int i=0; i < s.length(); i++)
		if (s[i] == '"') ss << "\\\"";
		else ss << s[i];
	ss << "\"";
	return ss.str();
}
std::string _serialize(W::position &p) {
	std::stringstream ss;
	ss << "{ "
	   << "x=" << p.x << "," << "y=" << p.y << ","
	   << "a=" << p.a << "," << "b=" << p.b
	   << " }";
	return ss.str();
}
std::string _serialize(W::size &s) {
	std::stringstream ss;
	ss << "{ width=" << s.width << ", height=" << s.height << " }";
	return ss.str();
}
std::string _serialize(W::rect &r) {
	std::stringstream ss;
	ss << "{\n"
	   << "position = " << _serialize(r.pos) << ",\n"
	   << "size = " << _serialize(r.sz)
	   << "\n}";
	return ss.str();
}
std::string _serialize(const UID &uid) {
	std::stringstream ss;
	ss << uid.id;
	return ss.str();
}
std::string _serialize(const UnitMode::T &m) {
	using namespace UnitMode;
	if (m == IDLE) return "\"IDLE\"";
	else if (m == VOYAGING) return "\"VOYAGING\"";
	else if (m == ANIMATING) return "\"ANIMATING\"";
	return "\"UNKNOWN\"";
}


/* _deserialize fn implementations */

void _deserialize(LuaObj &o, int &x)   { x = o.number_value; }
void _deserialize(LuaObj &o, float &x) { x = o.number_value; }
void _deserialize(LuaObj &o, bool &b)  { b = o.bool_value; }
void _deserialize(LuaObj &o, std::string &s) { s = o.str_value; }
void _deserialize(LuaObj &o, W::position &p) {
	p.x = o["x"].number_value;
	p.y = o["y"].number_value;
	p.a = o["a"].number_value;
	p.b = o["b"].number_value;
}
void _deserialize(LuaObj &o, W::size &s) {
	s.width = o["width"].number_value;
	s.height = o["height"].number_value;
}
void _deserialize(LuaObj &o, W::rect &r) {
	_deserialize(o["position"], r.pos);
	_deserialize(o["size"], r.sz);
}
void _deserialize(LuaObj &o, std::vector<int> &v) {
	v.clear();
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isNumber()) ; // error
		else v.push_back(d.number_value);
	}
}
void _deserialize(LuaObj &o, std::vector<float> &v) {
	v.clear();
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isNumber()) ; // error
		else v.push_back(d.number_value);
	}
}
void _deserialize(LuaObj &o, std::vector<bool> &v) {
	v.clear();
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isBool()) ; // error
		else v.push_back(d.bool_value);
	}
}
void _deserialize(LuaObj &o, std::vector<std::string> &v) {
	v.clear();
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isString()) ; // error
		else v.push_back(d.str_value);
	}
}
void _deserialize(LuaObj &o, std::vector<W::position> &v) {
	v.clear();
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		W::position p;
		_deserialize(it->second, p);
		v.push_back(p);
	}
}
void _deserialize(LuaObj &o, UID &uid) {
	uid = UID(o.number_value);
	UIDManager::translateLoadedUID(uid);
}
void _deserialize(LuaObj &o, UnitMode::T &m) {
	using namespace UnitMode;
	std::string &s = o.str_value;
	if (s == "VOYAGING") m = VOYAGING;
	else if (s == "ANIMATING") m = ANIMATING;
	else m = IDLE;
}


/* Reindenting function for lua */

void reindentLuaString(std::string &s) {
	std::string t;
	int indentLevel = 0;
	bool insideString = false;
	for (std::string::iterator it = s.begin(); it != s.end(); ++it) {
		bool last = (it+1 == s.end());
		bool first = (it == s.begin());
		char c = *it, cp, cn;
		if (!first) cp = *(it-1);
		if (!last) cn = *(it+1);
		
		if (!insideString) {
			if (c == '"') insideString = true;
		}
		if (insideString) {
			if (c == '"' && cp != '\\') insideString = false;
		}
		
		if (c == '{' && !insideString) ++indentLevel;
		if (!last && cn == '}' && !insideString) --indentLevel;
		
		t += c;
		if (c == '\n' && !insideString) {
			for (int i=0; i < indentLevel; ++i) t += "\t";
		}
	}
	s = t;
}