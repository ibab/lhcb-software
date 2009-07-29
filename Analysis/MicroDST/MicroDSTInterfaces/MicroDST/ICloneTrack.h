// $Id: ICloneTrack.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
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

/** @class ICloneTrack ICloneTrack.h MicroDST/ICloneTrack.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneTrack : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneTrack, 2, 0 );
  
  virtual LHCb::Track* operator() (const LHCb::Track* track) = 0;

};
#endif // MICRODST_ICLONETRACK_H
