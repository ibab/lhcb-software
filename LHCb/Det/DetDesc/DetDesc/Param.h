// $Id: Param.h,v 1.9 2008-02-22 12:12:12 marcocle Exp $
#ifndef DETDESC_PARAM_H 
#define DETDESC_PARAM_H 1

// Include files
#include <string>
#include <vector>
#include <map>

#include <sstream>

#include "DetDesc/BasicParam.h"
#include "GaudiKernel/GaudiException.h"

/** @class Param Param.h DetDesc/Param.h
 *  
 *  Class implementing a polimorfic parameter using templates
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-02-22
 */
template <class T>
class Param: public BasicParam {
public:
  typedef T value_type;
  
  /// Standard constructor
  Param( ) = default;

  /// Constructor by value
  Param(value_type val): m_val(std::move(val)) {}

  virtual ~Param() = default; ///< Destructor

  /// Setter (with automatic conversion)
  template <class T1>
  inline void set(T1 val) { m_val = std::move(val); }

  /// Getter
  inline value_type &get() { return m_val; }

  /// Getter (const version)
  inline const value_type &get() const { return m_val; }

  /// String representation for printout
  inline virtual std::string toStr() const;
  
  /// String representation for printout
  virtual std::string toXMLStr(const std::string& name,
                               const std::string& comment = "",
                               int precision = 16) const;
  
  /// TypeId of the datum
  virtual const std::type_info &type() const { return typeid(T); }

  /// Create a copy of this object on the heap (generic copy)
  virtual BasicParam * new_copy() const {
    return new Param<T>(m_val);
  }

private:
  /// Void pointer to the datum (used by BasicParam)
  virtual void *_get_ptr() { return &m_val;}
  /// Void pointer to the datum (used by BasicParam), const version
  virtual const void *_get_ptr() const { return &m_val;}
  /// Where the datum is stored
  value_type m_val;

};

#include "GaudiKernel/SerializeSTL.h"
/// String representation for printout
template<class T>
inline std::string Param<T>::toStr() const {
  using namespace GaudiUtils;
  std::ostringstream o;
  o << m_val;
  return o.str();
}

template<> inline std::string Param<double>::toStr() const {
  std::ostringstream o;
  o.precision(16);
  o << m_val;
  return o.str();
}

/// Default implementation. Only a limited number of types are allowed in 
/// the XML representation of a ParamValidDataObject
template<class T>
std::string Param<T>::toXMLStr(const std::string&, const std::string&, int) const {
  throw GaudiException(std::string("Cannot convert parameter of type ") + typeid(m_val).name() + " to XML",
                       "Param<T>::toXMLStr()", StatusCode::FAILURE );
  return {};
}

// === Forward declaration of version of toXMLStr for the supported types ===

template<> std::string Param<int>::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<double>::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::string>::toXMLStr(const std::string& name, const std::string& comment, int precision) const;

template<> std::string Param<std::vector<int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::vector<double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::vector<std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;

template<> std::string Param<std::map<std::string,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::map<std::string,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::map<std::string,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;

template<> std::string Param<std::map<int,int> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::map<int,double> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;
template<> std::string Param<std::map<int,std::string> >::toXMLStr(const std::string& name, const std::string& comment, int precision) const;

#endif // DETDESC_PARAM_H
