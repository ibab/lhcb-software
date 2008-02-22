// $Id: Param.cpp,v 1.1 2008-02-22 12:12:12 marcocle Exp $

#include "DetDesc/Param.h"

namespace ParamXMLUtils {

  template <class T1, class T2, class COMP, class ALLOC>
  inline std::ostream& operator << ( std::ostream& s,
                                     const std::map<T1,T2,COMP,ALLOC>& m ) {
    return GaudiUtils::operator<<(s,m);
  }
  
  template <class T, class ALLOC>
  inline std::ostream& operator<< ( std::ostream& s, const std::vector<T,ALLOC>& v )
  {
    for(typename std::vector<T,ALLOC>::const_iterator i=v.begin(); i!=v.end(); ++i) {
      if ( i != v.begin()) s << ' ';
      s << (*i);
    }
    return s;
  }
}

// =================================================================

#define toXMLStr_basic_impl(Tag,Type) \
  std::ostringstream o; \
  o.precision(precision); \
  o << "<" Tag " name=\"" << name << "\" type=\"" Type "\""; \
  if ( ! comment.empty() ) { o << " comment=\"" << comment << "\""; } \
  o << ">" << m_val << "</" Tag ">"; \
  return o.str()

#define toXMLStr_impl(Type) toXMLStr_basic_impl("param",Type)

template<> std::string Param<int>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_impl("int");
}
template<> std::string Param<double>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_impl("double");
}
template<> std::string Param<std::string>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_impl("other");
}

// =================================================================

#define toXMLStr_vect_impl(Type) \
  using namespace ParamXMLUtils; \
  toXMLStr_basic_impl("paramVector",Type)

template<> std::string Param<std::vector<int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_vect_impl("int");
}
template<> std::string Param<std::vector<double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_vect_impl("double");
}
template<> std::string Param<std::vector<std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_vect_impl("std::string");
}

// =================================================================

#define toXMLStr_map_impl(Key,Type) \
  std::ostringstream o; \
  o.precision(precision); \
  o << "<map name=\"" << name << "\" keytype=\"" Key "\" valuetype=\"" Type "\""; \
  if ( ! comment.empty() ) { o << " comment=\"" << comment << "\""; } \
  o << ">"; \
  for ( value_type::const_iterator i = m_val.begin(); \
        i != m_val.end(); ++i ) { \
    o << "<item key=\"" << i->first << "\" value=\"" << i->second << "\"/>"; \
  } \
  o << "</map>"; \
  return o.str()


template<> std::string Param<std::map<std::string,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("string","int");
}
template<> std::string Param<std::map<std::string,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("string","double"); 
}
template<> std::string Param<std::map<std::string,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("string","string");
}

// =================================================================

template<> std::string Param<std::map<int,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("int","int");
}
template<> std::string Param<std::map<int,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("int","double");
}
template<> std::string Param<std::map<int,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  toXMLStr_map_impl("int","string");
}

// =================================================================
