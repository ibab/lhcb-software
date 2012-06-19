// $Id: Data.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
//====================================================================
//  ActiveMQ
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ plugin 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/Data.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_DATA_H
#define ACTIVEMQ_DATA_H 1

// C++ include files
#include <string>

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ {

  /**@class Data Data.h ActiveMQ/Data.h
   *
   * @author M.Frank
   */
  class Data  {
  public:
    /// Data tag name
    std::string tag;
    /// Data value
    std::string value;
    /// Time stamp
    int         time;
    // Empty constructor
    Data() : time(0) {
    }
    /// Copy constructor
    Data(const Data& d) : tag(d.tag), value(d.value), time(d.time) {
    }
    /// Initializing constructor
    Data(const std::string& t, const std::string& v, int tm) : tag(t), value(v), time(tm) {
    }
    /// Assignment operator
    Data& operator=(const Data& d) {
      if ( this != &d ) {
        tag = d.tag;  
        value = d.value; 
        time = d.time;
      }
      return *this;
    }
  };
}      // End namespace ActiveMQ
#endif /* ACTIVEMQ_DATA_H */

