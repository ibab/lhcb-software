// $Id: ChargedParticleMakerBase.h,v 1.3 2009-12-08 12:55:07 pkoppenb Exp $
#ifndef CHARGEDPARTICLEMAKERBASE_H 
#define CHARGEDPARTICLEMAKERBASE_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "ParticleMakerBase.h"
#include "Kernel/IParticle2State.h"
#include "TrackInterfaces/ITrackSelector.h"

/** @class ChargedParticleMakerBase ChargedParticleMakerBase.h
 *
 *  Base class for all Particle Maker Algorithms
 *
 *  @author P. Koppenburg
 *  @date   2009-04-21
 */

class ChargedParticleMakerBase : public ParticleMakerBase
{

public:

  /// Standard constructor
  ChargedParticleMakerBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedParticleMakerBase( ); ///< Destructor

  virtual StatusCode initialize() ; ///< Finalize

protected:

  // The method that each implementation should implement
  //  virtual StatusCode makeParticles(LHCb::Particle::Vector& ) = 0 ;
  ///  Particle to state convertion tool
  const IParticle2State* p2s()const{return m_p2s;}
  ///  Track selector tool
  const ITrackSelector * trSel()const{return m_trSel;}
  /// set particle properties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;
  /// Select the appropriate state 
  const LHCb::State* usedState( const LHCb::Track* track)const ;

private:

  /// Particle to state convertion tool
  const IParticle2State* m_p2s ;
  /// Track selector tool
  const ITrackSelector * m_trSel;

};
#endif // CHARGEDPARTICLEMAKERBASE
