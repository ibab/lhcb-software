// $Id: FlatPtRapidity.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_MOMENTUMRANGE_H
#define PARTICLEGUNS_MOMENTUMRANGE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class FlatPtRapidity FlatPtRapidity.h "FlatPtRapidity.h"
 *
 *  Particle gun with given momentum range
 *  
 *  @author Dan Johnson
 *  @date   2016-02-19
 */
class FlatPtRapidity : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  FlatPtRapidity( const std::string & type , const std::string& name, 
                 const IInterface * parent ) ;
  
  /// Destructor
  virtual ~FlatPtRapidity();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & momentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;

  /// Print counters
  virtual void printCounters( ) { ; } ;
                                 
 private:  
  double m_minPt;       ///< Minimum pT (Set by options)  
  double m_minRapidity; ///< Minimum rapidity (Set by options)

  double m_maxPt;       ///< Maximum pT (Set by options)
  double m_maxRapidity; ///< Maximum rapidity (Set by options)

  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int>         m_pdgCodes;

  /// Names of particles to generate
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};

#endif // PARTICLEGUNS_MOMENTUMRANGE_H
