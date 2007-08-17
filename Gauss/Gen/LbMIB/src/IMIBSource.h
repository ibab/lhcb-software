// $Id: IMIBSource.h,v 1.1 2007-08-17 12:54:14 gcorti Exp $
#ifndef IMIBSOURCE_H 
#define IMIBSOURCE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// From Event
#include "Event/HepMCEvent.h"
 
static const InterfaceID IID_IMIBSource ( "IMIBSource", 1, 0 );

/** @class IMIBSource IMIBSource.h
 *  
 *
 *  @author Gloria Corti
 *  @date   2007-08-10
 */
class IMIBSource : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMIBSource; }


  // Generate one event, including empty ones
  virtual StatusCode generateEvent( LHCb::GenHeader* theHeader,
                                    LHCb::GenCollisions* theCollisions,
                                    LHCb::HepMCEvents* theEvents,
                                    int& numParts ) = 0;

protected:

private:

};
#endif // IMIBSOURCE_H
