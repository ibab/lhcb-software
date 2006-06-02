// $Id: Param.h,v 1.5 2006-06-02 06:59:01 cattanem Exp $
#ifndef DETDESC_PARAM_H 
#define DETDESC_PARAM_H 1

// Include files
#include <string>
#include <sstream>

#include <DetDesc/BasicParam.h>

#include <vector>
/// Needed to write vectors to a string
template <class _CharT, class _Traits, class T>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>&os,
           const std::vector<T> &v){
  // print a "white space" separated list of elements

  if (v.empty()) return os; // print nothing for empty vectors

  typename std::vector<T>::const_iterator i;

  // check if the contained type is a string
  // (funny: type_info::operator== returns an int!)
  int is_string = (typeid(T) == typeid(std::string));

  // the first element does not need the space in front
  i = v.begin();
  os << *i;
  ++i;

  // loop over other elements
  for ( ; i !=v.end(); ++i ){
    os << " ";
    if (is_string) os << '"'; // enclose strings in quotes
    os << *i;
    if (is_string) os << '"'; // enclose strings in quotes
  }
  return os;
}

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
  /// Standard constructor
  Param( ): m_val() {}

  /// Constructor by value
  Param(const T &val): m_val(val) {}

  virtual ~Param(){} ///< Destructor

  /// Setter (with automatic conversion)
  template <class T1>
  inline void set(const T1 &val) { m_val = val; }

  /// Getter
  inline T &get() { return m_val; }

  /// Getter (const version)
  inline const T &get() const { return m_val; }

  /// String representation for printout
  inline virtual std::string toStr() {
    std::ostringstream o;
    o << m_val;
    return o.str();
  }
  
  /// TypeId of the datum
  virtual const std::type_info &type() const { return typeid(T); }

  /// Create a copy of this object on the heap (generic copy)
  virtual BasicParam * new_copy() const {
    return new Param<T>(m_val);
  }

private:
  /// Void pointer to the datum (used by BasicParam)
  virtual void *_get_ptr() { return (void*)&m_val;}
  /// Void pointer to the datum (used by BasicParam), const version
  virtual const void *_get_ptr() const { return (void*)&m_val;}
  /// Where the datum is stored
  T m_val;

};

inline std::string Param<double>::toStr() {
  std::ostringstream o;
  o.precision(16);
  o << m_val;
  return o.str();
}

#endif // DETDESC_PARAM_H
