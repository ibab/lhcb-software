// $Id: IDecayTreeFitter.h,v 1.1 2009-05-07 13:02:10 wouter Exp $
#ifndef TRACKINTERFACES_IDECAYTREEFITTER_H 
#define TRACKINTERFACES_IDECAYTREEFITTER_H 1

// Include files
// -------------
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

namespace LHCb {
  class Particle;
};

static const InterfaceID IID_IDecayTreeFitter ( "IDecayTreeFitter", 1, 0 );

/** @class IDecayTreeFitter IDecayTreeFitter.h TrackInterfaces/IDecayTreeFitter.h
 *  
 *
 */

class IDecayTreeFitter : virtual public IAlgTool 
{
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDecayTreeFitter; }

  virtual StatusCode fit( const LHCb::Particle& bc ) const ;
  
  virtual double chiSquare() const ;

  // interface to beta
  virtual BbrDoubleErr decayLength(const LHCb::Particle& cand) const ;
  virtual BbrDoubleErr lifeTime(const LHCb::Particle& cand) const ;
  virtual BtaFitParams btaFitParams(const LHCb::Particle& cand) const ;
  
}

#endif
