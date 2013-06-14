// $Id: ICloneTrack.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONETRACK_H 
#define MICRODST_ICLONETRACK_H 1
// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class Track;
}

/** @class ICloneTrack MicroDST/ICloneTrack.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneTrack : virtual public MicroDST::ICloner<LHCb::Track> 
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneTrack, 2, 0 );

  /// Destructor
  virtual ~ICloneTrack() { }

};

#endif // MICRODST_ICLONETRACK_H
