#ifndef PARTICLEGUNS_OTCOSMIC_H
#define PARTICLEGUNS_OTCOSMIC_H

// Include files
// From Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Transform3DTypes.h"

// From ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

class SolidBox ;
namespace LHCb { class ParticleProperty ; }


/** @class OTCosmic OTCosmic.h "OTCosmic.h"
 * The output will be stored in the transient event store so it can be
 *    passed to the simulation.
 *  
 *  @author W. Seligman
 *  @date   2002-11-08
 */
class OTCosmic : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  OTCosmic( const std::string & type , const std::string& name , 
                     const IInterface * parent ) ;
  
  virtual ~OTCosmic(); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize();

  /// Generate particle
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , int & pdgId ) ;
                                 
  /// Print counters
  virtual void printCounters( ) ;
  
private:
  
  // event counter, used for event ID
  int m_events, m_generated, m_rejectedbyenergycut, m_rejectedbyscintacceptance;
  float m_emin, m_emax;
  float m_ctcut;
  float m_tmin, m_tmax;
  int m_printEvent, m_printMod;
  float m_thetamin, m_thetamax, m_phimin, m_phimax;

  /// Flat random number generator
  Rndm::Numbers m_flatgenerator ;
  std::auto_ptr<SolidBox> m_scintsolid ;
  Gaudi::Transform3D m_toptransform ;
  Gaudi::Transform3D m_bottransform ;
  const LHCb::ParticleProperty* m_muplus  ;
  const LHCb::ParticleProperty* m_muminus ;
};

#endif
