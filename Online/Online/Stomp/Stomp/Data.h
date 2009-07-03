// $Id: Data.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
//====================================================================
//  Stomp
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp plugin 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/Stomp/Data.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
#ifndef STOMP_DATA_H
#define STOMP_DATA_H 1

// C++ include files
#include <string>

/*
 *   Stomp namespace declaration
 */
namespace Stomp {
  /**@class Data Stomp.h Stomp/Stomp.h
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
}      // End namespace Stomp
#endif /* STOMP_DATA_H */

