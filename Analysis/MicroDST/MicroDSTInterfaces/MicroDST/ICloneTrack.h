// $Id: ICloneTrack.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
#ifndef MICRODST_ICLONETRACK_H 
#define MICRODST_ICLONETRACK_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class Track;
}

static const InterfaceID IID_ICloneTrack ( "ICloneTrack", 1, 0 );

/** @class ICloneTrack ICloneTrack.h MicroDST/ICloneTrack.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class ICloneTrack : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneTrack; }

  virtual LHCb::Track* operator() (const LHCb::Track* track) = 0;

protected:

private:

};
#endif // MICRODST_ICLONETRACK_H
