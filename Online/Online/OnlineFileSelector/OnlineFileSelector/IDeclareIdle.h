#ifndef ICHALKIA_IDECLAREIDLE_H 
#define ICHALKIA_IDECLAREIDLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IDeclareIdle ( "IDeclareIdle", 1, 0 );

/** @class IDeclareIdle IDeclareIdle.h ichalkia/IDeclareIdle.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-24
 */
class IDeclareIdle : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDeclareIdle; }

  virtual StatusCode idle() = 0;
  virtual StatusCode dataReady() = 0;
  
};
#endif // ICHALKIA_IDECLAREIDLE_H
