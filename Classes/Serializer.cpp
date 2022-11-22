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
std::string _serialize(W::v2i &p) {
  W::v2f pf = p;
  return _serialize(pf);
}
std::string _serialize(W::v2f &p) {
	std::stringstream ss;
	ss << "{ "
	   << "a=" << p.a << "," << "b=" << p.b
	   << " }";
	return ss.str();
}
std::string _serialize(W::fRect &r) {
	std::stringstream ss;
	ss << "{\n"
	   << "position = " << _serialize(r.position) << ",\n"
	   << "size = " << _serialize(r.size)
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
std::string _serialize(const std::map<UID,UID> &m) {
	std::stringstream ss;
	ss << "{ ";
	int i=0, n = (int) m.size();
	for (std::map<UID,UID>::const_iterator it = m.begin(); it != m.end(); ++it)
		ss << it->first.id << " = " << it->second.id << (++i == n ? "" : ", ");
	ss << " }";
	return ss.str();
}
std::string _serialize(const SeekTarget::Type &targ) {
	return SeekTarget::Targets[targ];
}

/* _deserialize fn implementations */

void _deserialize(LuaObj &o, int &x)   { x = o.number_value(); }
void _deserialize(LuaObj &o, float &x) { x = o.number_value(); }
void _deserialize(LuaObj &o, bool &b)  { b = o.bool_value(); }
void _deserialize(LuaObj &o, std::string &s) { s = o.str_value(); }
void _deserialize(LuaObj &o, W::v2i &p) {
  p.a = o["a"].number_value();
  p.b = o["b"].number_value();
}
void _deserialize(LuaObj &o, W::v2f &p) {
	p.a = o["a"].number_value();
	p.b = o["b"].number_value();
}
void _deserialize(LuaObj &o, W::fRect &r) {
	_deserialize(o["position"], r.position);
	_deserialize(o["size"], r.size);
}
void _deserialize(LuaObj &o, std::vector<int> &v) {
	v.clear();
  for (auto it : o.descendants()) {
    LuaObj &d = it.second;
		if (!d.isNumber()) ; // error
		else v.push_back(d.number_value());
	}
}
void _deserialize(LuaObj &o, std::vector<float> &v) {
	v.clear();
  for (auto it : o.descendants()) {
    LuaObj &d = it.second;
		if (!d.isNumber()) ; // error
		else v.push_back(d.number_value());
	}
}
void _deserialize(LuaObj &o, std::vector<bool> &v) {
	v.clear();
  for (auto it : o.descendants()) {
    LuaObj &d = it.second;
		if (!d.isBool()) ; // error
		else v.push_back(d.bool_value());
	}
}
void _deserialize(LuaObj &o, std::vector<std::string> &v) {
	v.clear();
  for (auto it : o.descendants()) {
		LuaObj &d = it.second;
		if (!d.isString()) ; // error
		else v.push_back(d.str_value());
	}
}
void _deserialize(LuaObj &o, std::vector<W::v2i> &v) {
  v.clear();
  for (auto it : o.descendants()) {
    W::v2i p;
    _deserialize(it.second, p);
    v.push_back(p);
  }
}
void _deserialize(LuaObj &o, std::vector<W::v2f> &v) {
	v.clear();
  for (auto it : o.descendants()) {
		W::v2f p;
		_deserialize(it.second, p);
		v.push_back(p);
	}
}
void _deserialize(LuaObj &o, UID &uid) {
	uid = UID(o.number_value());
	UIDManager::translateLoadedUID(uid);
}
void _deserialize(LuaObj &o, UnitMode::T &m) {
	using namespace UnitMode;
	std::string s = o.str_value();
	if (s == "VOYAGING") m = VOYAGING;
	else if (s == "ANIMATING") m = ANIMATING;
	else m = IDLE;
}
void _deserialize(LuaObj &o, std::map<UID,UID> &m) {
	m.clear();
  for (auto it : o.descendants()) {
		UID u1, u2;
		unsigned int x;
		std::stringstream(it.first) >> x;
		_deserialize(it.second, u2);		// TODO: check this
		m[UID(u1)] = UID(u2);				// Seems like an error
	}
}
void _deserialize(LuaObj &o, SeekTarget::Type &targ) {
	std::string o_type = o.str_value();
	int n_types = (int) SeekTarget::__N;
	for (int i=0; i < n_types; ++i) {
		SeekTarget::Type type = (SeekTarget::Type) i;
		if (SeekTarget::Targets[type] == o_type) {
			targ = type;
		}
	}
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
