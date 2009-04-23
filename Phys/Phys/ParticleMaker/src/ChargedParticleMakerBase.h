// $Id: ChargedParticleMakerBase.h,v 1.2 2009-04-23 14:49:25 pkoppenb Exp $
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

class ChargedParticleMakerBase : public ParticleMakerBase {
public:
  /// Standard constructor
  ChargedParticleMakerBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedParticleMakerBase( ); ///< Destructor

  virtual StatusCode initialize() ; ///< Finalize
  virtual StatusCode finalize(){return DVAlgorithm::finalize();} ; ///< Finalize

protected:
  // The method that each implementation should implement
  //  virtual StatusCode makeParticles(LHCb::Particle::Vector& ) = 0 ;
  ///  Particle to state convertion tool
  const IParticle2State* p2s()const{return m_p2s;}
  ///  Track selector tool
  const ITrackSelector * trSel()const{return m_trSel;}
  /// set particl eproperties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;
  

private:
  /// Particle to state convertion tool
  const IParticle2State* m_p2s ;
  /// Track selector tool
  const ITrackSelector * m_trSel;


};
#endif // CHARGEDPARTICLEMAKERBASE
