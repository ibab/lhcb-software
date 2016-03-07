#include "DetDesc/Param.h"

namespace {
  namespace detail {

  //TODO: use ostream_joiner from GaudiKernel
  struct IdentityOutputter {
    template <typename T>
    std::ostream& operator()(std::ostream& os, T&& t) const { return os << std::forward<T>(t); }
  };

  template <typename Stream, typename Iterator, typename Separator, typename OutputElement = IdentityOutputter>
  Stream& ostream_joiner(Stream& os, Iterator first, Iterator last, Separator sep, OutputElement output = OutputElement{}) {
    if (first!=last) { output(os,*first); ++first; }
    for (;first!=last;++first) { output(os << sep,*first); }
    return os;
  }

  template <typename Stream, typename Container, typename Separator, typename OutputElement = IdentityOutputter>
  Stream& ostream_joiner(Stream& os, const Container& c, Separator sep, OutputElement output = OutputElement{}) {
    return ostream_joiner( os, std::begin(c), std::end(c), sep, output );
  }
  }

  template <class T, class ALLOC>
  inline std::ostream& operator<< ( std::ostream& s, const std::vector<T,ALLOC>& v )
  {
    return detail::ostream_joiner(s, v, ' ' );
  }

  template <typename T> struct my_traits;
  template<> struct my_traits<int> { static constexpr const char *tag = "param"; static constexpr const char* type="int" ;};
  template<> struct my_traits<double> { static constexpr const char* tag = "param"; static constexpr const char* type = "double";};
  template<> struct my_traits<std::string> { static constexpr const char* tag = "param"; static constexpr const char* type ="other";};
  template<> struct my_traits<std::vector<int>> { static constexpr const char* tag="paramVector"; static constexpr const char* type="int";};
  template<> struct my_traits<std::vector<double>> { static constexpr const char* tag= "paramVector"; static constexpr const char* type="double";};
  template<> struct my_traits<std::vector<std::string>> { static constexpr const char* tag = "paramVector"; static constexpr const char* type="std::string";};
  template <typename T> struct my_map_traits;
  template <> struct my_map_traits<double>      { static constexpr const char* str="double";};
  template <> struct my_map_traits<int>         { static constexpr const char* str="int";};
  template <> struct my_map_traits<std::string> { static constexpr const char* str="string";};

  constexpr struct stream_key_value_pair_t {
      template <typename Key, typename Value> std::ostream& operator()(std::ostream& os, const std::pair<Key,Value>& p) {
              return os << "<item key=\"" << p.first << "\" value=\"" << p.second << "\"/>";
      }
  } stream_key_value_pair{};

  template <typename T>
  std::string toXMLStr_impl(const std::string& name, const std::string& comment, int precision,
                       const T& val)
  {
    std::ostringstream o;
    o.precision(precision);
    o << "<" << my_traits<T>::tag << " name=\"" << name << "\" type=\"" << my_traits<T>::type << "\"";
    if ( ! comment.empty() ) { o << " comment=\"" << comment << "\""; }
    o << ">" << val << "</" << my_traits<T>::tag << ">";
    return o.str();
  }

  template <typename Key, typename Value>
  std::string toXMLStr_impl(const std::string& name, const std::string& comment, int precision,
                       const std::map<Key,Value>& value) {
    std::ostringstream o;
    o.precision(precision);
    o << "<map name=\"" << name << "\" keytype=\"" << my_map_traits<Key>::str<< "\" valuetype=\"" << my_map_traits<Value>::str<< "\"";
    if ( ! comment.empty() ) { o << " comment=\"" << comment << "\""; }
    o << ">";
    detail::ostream_joiner( o, value, "" ,stream_key_value_pair );
    o << "</map>";
    return o.str();
  }
}

template<> std::string Param<int>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<double>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::string>::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::vector<int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::vector<double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::vector<std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<std::string,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<std::string,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<std::string,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<int,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<int,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
template<> std::string Param<std::map<int,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const {
  return toXMLStr_impl(name,comment,precision,m_val);
}
