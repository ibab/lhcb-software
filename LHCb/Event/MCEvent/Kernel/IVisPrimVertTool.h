// $Id: IVisPrimVertTool.h,v 1.1 2007-01-12 09:49:08 cattanem Exp $
#ifndef KERNEL_IVISPRIMVERTTOOL_H 
#define KERNEL_IVISPRIMVERTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class MCVertex ;
  class MCParticle ;
}

static const InterfaceID IID_IVisPrimVertTool("IVisPrimVertTool", 1 , 0); 

/** @class IVisPrimVertTool IVisPrimVertTool.h 
 *
 *  Tells if a primary MCVertex is visible and a little more
 *  The recommended way to use it is 
 *  
 *  @author Christian Jacoby (IPHE, UNIL, Lausanne)
 *  @date   2002-12-17
 */
class IVisPrimVertTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVisPrimVertTool; }
  virtual long countVertices() const      = 0; ///< count number of visible PVs
  virtual bool isVisible     ( const LHCb::MCVertex* primvtx )const  = 0; ///< is visible ?
  virtual bool isB           ( const LHCb::MCVertex* primvtx )const  = 0; ///< has a B?
  virtual long countVisTracks( const LHCb::MCVertex* primvtx )const  = 0; ///< count visible tracks
};
#endif // KERNEL_IVISPRIMVERTTOOL_H
