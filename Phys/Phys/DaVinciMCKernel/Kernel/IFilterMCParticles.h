// $Id: IFilterMCParticles.h,v 1.1 2007-07-24 12:08:44 jpalac Exp $
#ifndef KERNEL_IFILTERMCPARTICLES_H 
#define KERNEL_IFILTERMCPARTICLES_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/MCParticle.h"

static const InterfaceID IID_IFilterMCParticles ( "IFilterMCParticles", 1, 0 );

/** @class IFilterMCParticles IFilterMCParticles.h Kernel/IFilterMCParticles.h
 *  
 *  Select or reject an ensemble of particles.
 *
 *  @author Juan Palacios
 *  @date   2007-07-24
 */
class IFilterMCParticles : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFilterMCParticles; }

  /// Test if filter is satisfied on ensemble of MCParticles
  virtual bool isSatisfied( const LHCb::MCParticle::ConstVector& ) const = 0;
  /// Test if filter is satisfied on ensemble of MCParticles
  virtual bool operator()( const LHCb::MCParticle::ConstVector& ) const = 0;

protected:

private:

};
#endif // KERNEL_IFILTERMCPARTICLES_H
