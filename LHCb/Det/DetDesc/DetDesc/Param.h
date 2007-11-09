// $Id: Param.h,v 1.8 2007-11-09 17:10:07 marcocle Exp $
#ifndef DETDESC_PARAM_H 
#define DETDESC_PARAM_H 1

// Include files
#include <string>
#include <sstream>

#include "DetDesc/BasicParam.h"

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
  inline virtual std::string toStr();
  
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

#include "GaudiKernel/SerializeSTL.h"
/// String representation for printout
template<class T>
inline std::string Param<T>::toStr() {
  using namespace GaudiUtils;
  std::ostringstream o;
  o << m_val;
  return o.str();
}
  
template<> inline std::string Param<double>::toStr() {
  std::ostringstream o;
  o.precision(16);
  o << m_val;
  return o.str();
}

#endif // DETDESC_PARAM_H
