// $Id: DimInfoCounter.h,v 1.1 2007-06-22 16:37:57 psomogyi Exp $
#ifndef DIMINFOCOUNTER_H 
#define DIMINFOCOUNTER_H 1

#include "dic.hxx"
#include <string>


// Include files

/** @class DimInfoCounter DimInfoCounter.h
 *  
 *
 *  @author Ulrich Kerzel
 *  @date   2006-08-07
 */
class DimInfoCounter : public DimInfo {

  
public:

  // ---------------------------------------------------------------------
  //                    PUBLIC
  // ---------------------------------------------------------------------

  enum CounterType {NotFound,
                    Integer,
                    Float,
                    Double,
                    Character
  };


  DimInfoCounter(std::string serviceName, 
                 int         refreshTime,
                 int         verbosity = 0 );    ///< Standard constructor
  virtual ~DimInfoCounter( );                    ///< Destructor

  bool        serviceOK();  
  bool        serviceUpdated();
  void        ResetServiceUpdated();  // resetset m_serviceUpdated to false again
  float       getFloatValue();
  double      getDoubleValue();  
  int         getIntValue();
  
  CounterType getType();  
  
  // ---------------------------------------------------------------------
  //                    PRIVATE
  // ---------------------------------------------------------------------

private:
  void        infoHandler();   // overloaded from DimInfo
                               // called whenever server updates information

  CounterType m_counterType;  
  bool        m_serviceOK;
  bool        m_serviceUpdated;  // change to true once call-back is received and values 
                                 // may have changed
  int         m_intValue;
  float       m_floatValue;
  double      m_doubleValue;  
  int         m_verbosity;
  

}; // class DimInfoCounter
#endif // DIMINFOCOUNTER_H
