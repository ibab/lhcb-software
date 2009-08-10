// $Id: compareSensor.h,v 1.1 2009-08-10 08:48:02 szumlat Exp $
#ifndef COMPARESENSOR_H 
#define COMPARESENSOR_H 1

// Include files

/** @class compareSensor compareSensor.h Kernel/compareSensor.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-05-22
 */

template <class T>
class compareSensor:
public std::unary_function<T, bool> {
public: 
  /// Standard constructor
  compareSensor(const int inSens): m_sensor ( inSens )
  { }
  virtual ~compareSensor( ){ }   ///< Destructor
  inline bool operator()(T& inObj)
  {
    return ( (!inObj) ? false : inObj->sensor()!=m_sensor );
  }

protected:

private:

  int m_sensor;    /// number of current sensor

};
#endif // COMPARESENSOR_H
