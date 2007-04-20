// $Id: IAlignSelTool.h,v 1.1 2007-04-20 09:46:50 jblouw Exp $
#ifndef ALIGNTRTOOLS_IALIGNSELTOOL_H 
#define ALIGNTRTOOLS_IALIGNSELTOOL_H 1

// Include files
// from STL
#include <string>

#include "Event/Track.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAlignSelTool ( "IAlignSelTool", 1, 0 );

/** @class IAlignSelTool IAlignSelTool.h
 *  
 *
 *  @author Louis Nicolas
 *  @date   2006-12-01
 */
class IAlignSelTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlignSelTool; }

  virtual bool selectTrack(LHCb::Track* aTrack) = 0;

protected:

private:

};
#endif // ALIGNTRTOOLS_IALIGNSELTOOL_H
