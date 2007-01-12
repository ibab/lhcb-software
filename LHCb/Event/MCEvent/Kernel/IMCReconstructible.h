// $Id: IMCReconstructible.h,v 1.1 2007-01-12 14:17:33 cattanem Exp $
#ifndef KERNEL_IMCEFFRECONSTRUCTIBLE_H 
#define KERNEL_IMCEFFRECONSTRUCTIBLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class MCParticle ;
}

static const InterfaceID IID_IMCReconstructible ( "IMCReconstructible", 1, 0 );

/** @class IMCReconstructible IMCReconstructible.h Kernel/IMCReconstructible.h
 *  
 *
 *  @author Christian Jacoby
 *  @date   2004-04-28
 */
class IMCReconstructible : virtual public IAlgTool {
public: 

  /// Reconstructability category
  enum RecblCategory { NotRec=0, Long=1, TTT=2, VTT=3, Neutral=4 };

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCReconstructible; }

  /// Get the reconstruction category for the given MCParticle
  virtual IMCReconstructible::RecblCategory reconstructible( const LHCb::MCParticle* mcPart ) = 0;

  /// Is the MCParticle in the detector acceptance?
  virtual bool accepted( const LHCb::MCParticle* part ) = 0;
 

protected:

private:

};
#endif // KERNEL_IMCEFFRECONSTRUCTIBLE_H
