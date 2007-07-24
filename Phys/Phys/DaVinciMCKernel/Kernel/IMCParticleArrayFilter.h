// $Id: IMCParticleArrayFilter.h,v 1.2 2007-07-24 12:38:25 jpalac Exp $
#ifndef KERNEL_IMCPARTICLEARRAYFILTER_H 
#define KERNEL_IMCPARTICLEARRAYFILTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/MCParticle.h"

static const InterfaceID IID_IMCParticleArrayFilter ( "IMCParticleArrayFilter", 1, 0 );

/** @class IMCParticleArrayFilter IMCParticleArrayFilter.h Kernel/IMCParticleArrayFilter.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2007-07-24
 */
class IMCParticleArrayFilter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCParticleArrayFilter; }

  /// Filter and put results into new array
  virtual StatusCode filter( const LHCb::MCParticle::ConstVector&, 
                             LHCb::MCParticle::ConstVector& ) const = 0;
  /// Filter and remove elements that do not pass filter from array
  virtual StatusCode filter( LHCb::MCParticle::ConstVector& ) const = 0;



protected:

private:

};
#endif // KERNEL_IMCPARTICLEARRAYFILTER_H
