// $Id: $
#ifndef INSHARED_H 
#define INSHARED_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"


static const InterfaceID IID_INShared ( "INShared", 1, 0 );

/** @class INShared INShared.h
 *  
 *
 *  @author Xabier Cid Vidal
 *  @date   2011-03-17
 */
class INShared : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_INShared; }
  virtual StatusCode getNShared(const LHCb::Track& muTrack, int& nshared)=0;
  virtual void fillNShared()=0;

protected:

private:

};
#endif // INSHARED_H
