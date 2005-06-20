// $Id: EvtGenTool.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
#ifndef GENERATORS_EVTGENTOOL_H 
#define GENERATORS_EVTGENTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from GeneratorModules
#include "Generators/IEvtGenTool.h"

// from EvtGen
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtIncoherentMixing.hh"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "EvtGenBase/EvtParticle.hh"
#include "EvtGen/EvtGen.hh"
#include "Kernel/ParticleID.h"

#include "CLHEP/Vector/LorentzVector.h"

/** @class EvtGenTool EvtGenTool.h Algorithms/EvtGenTool.h
 *  
 *  Tool to contain EvtGen generator
 *
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */
class EvtGenTool : public GaudiTool, virtual public IEvtGenTool {
public:
  /// Standard constructor
  EvtGenTool( const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~EvtGenTool( ) ; ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() ;

  /// Finalize method
  virtual StatusCode finalize() ;

  /** Generate a decay tree from a given HepMC particle
   *  
   *  @return status code
   *  @param theEvent pointer to the current HepMC event
   *  @param theMother pointer to the MC Particle to decay with EvtGen
   */
  virtual StatusCode generateDecay( HepMC::GenEvent * theEvent,
                                    HepMC::GenParticle * theMother ) const ;

  /** Generate a decay tree from a given HepMC particle
   *  The decay chain will stop when it finds a particle
   *  of type targetId (which is the limit)
   *  
   *  @return status code
   *  @param theEvent pointer to the current HepMC event
   *  @param theMother pointer to the MC Particle to decay with EvtGen
   *  @param targetId integer type of the limit particle to generate
   */
  virtual StatusCode generateDecayWithLimit( HepMC::GenEvent * theEvent,
                                             HepMC::GenParticle * theMother,
                                             int targetId ) const ;

  /** Generate a decay tree at rest of a particle with given Id
   * 
   *  @return status code
   *  @param theEvent pointer to the current HepMC event
   *  @param theId Id of the particle to decay with EvtGen
   *  @param resultId PDG Id of the effectivly generated particle
   *     (it might be different of theId because of the flip)
   *  @param theParticle pointer to the Pythia particle (to know
   *      the mass in case of a resonance). If points to NULL,
   *      the PDG mass will be taken as default mass.
   */
  virtual StatusCode generateDecayAtRest( HepMC::GenEvent * theEvent ,
                                          const EvtId & theId ,
                                          int & resultId ,
                                          const HepMC::GenParticle 
                                          * theParticle ) const ;

  /// Return the event number
  virtual int getEventType() const { return m_evnType ; } 

  /// Return the BHadronsID list
  virtual const std::vector< int >& getBHadronsList() const 
  { return m_hadrons ; } 

  /// Return the signal b-hadron Id
  virtual const EvtId& getbId() const { return m_bId ; } 

  /// Return the signal bbar-hadron Id
  virtual const EvtId& getbbarId() const { return m_bbarId ; } 

  /// Return the id of the alias corresponding to the pdg code pdgId
  virtual const EvtId& getSignalAlias( int pdgId ) const ;

  /// Enable the possibility to flip the flavour for CP decay modes in EvtGen
  virtual void enableFlip() const ; 

  /// Disable the possibility to flip the flavour for CP decay modes in EvtGen
  virtual void disableFlip() const ; 

  /// obtain a random number from the common random engine
  virtual double randomFromEngine() const 
  { return m_randomEngine->random() ; } 
  
  /// true if decay to generate is the B signal decay tree
  virtual bool isSignalDecay( ) const { return m_signalDecay ; } 
  
  /// true if one must generate a B/Bbar mixture 
  virtual bool doCPMixture( ) const { return m_cpMixture ; } 

  /** true if the particle has a decay table defined in the generic
   *  or in the user decay file
   *
   *  @return boolean
   *  @param pdgId particle Id of the particle to test
   */
  virtual bool isKnownToEvtGen( int pdgId ) const ;

  /// true if forced fragmentation is requested in Pythia
  virtual bool forcedFragmentation( ) const { return m_forcedFragmentation ; }

  /** true if inclusive production requested 
   *  This is necessary to distinguish with signal B production
   *  when there is only one or two PDG Id specified in options
   *
   *  This is to be used with care. It has only a meaning when
   *  BHadronId = { N } or BHadronId = { N , -N }
   *  In this case, the program will by default say it is a signal
   *  production. To force it to be an inclusive production, the option
   *  has to be explicitly set.
   */
  virtual bool inclusiveProduction( ) const { return m_inclusiveProduction ; }

  /// True if repeated hadronization is requested
  virtual bool repeatedHadronization( ) const 
  { return m_repeatedHadronization ; }

  /** get the signal branching fraction in the generic decay file 
   *  including daughter decay branching fractions
   */
  virtual double getSignalBr( ) const ;
  
  /// check if the signal decay mode is also in the generic table
  virtual bool checkSignalPresence( ) const ;
  
protected:

private:
  /** Make a HepMCTree tree from an EvtGen particle stopping at the PDG
   *  Id targetId
   *
   *  @return status code
   *  @param theParticle pointer to the EvtGen particle to put in HepMC format
   *  @param theEvent pointer to the current HepMC event
   *  @param theMother pointer to the HepMC particle associated to theParticle
   *  @param theOrigin position of the first particle in the decay tree 
   *                (the root of the decay tree)
   *  @param targetId int pdgId of the last particle to generate (not used if
   *         negative)
   */
  StatusCode makeHepMC( EvtParticle * theParticle ,
                        HepMC::GenEvent * theEvent , 
                        HepMC::GenParticle * theMother ,
                        const HepLorentzVector & theOrigin ,
                        int targetId = -999 )  const ;

  /** Create a temporary evt.pdl file filled with Gaudi Particle Svc
   *  properties to update EvtGen particle properties
   *
   *  @return status code
   *  @param tempFileName name of the temporary created file
   */
  StatusCode createTemporaryEvtFile( const std::string& tempFileName ) const ;

  /** Check if HepMC Particle is valid for decay by EvtGen and to fill a
   *  HepMC event (checks if it is not already decayed and ensures that
   *  it has a defined production vertex) 
   *  @return StatusCode
   *  @param[in] thePart HepMC::GenParticle to analyze
   */
  StatusCode checkParticle( const HepMC::GenParticle * theParticle ) const ;
  
  /** Call EvtGen to decay a particle
   *  @return StatusCode
   *  @param[out] thePart the EvtParticle to produce
   *  @param[in]  theHepMCParticle the HepMC::GenParticle to decay with 
   *              EvtGen
   *  @param[in]  eid if different from EvtId(-1,-1), specify the EvtId of
   *              the particle to generate (in case of an EvtGen alias for
   *              example) 
   */
  StatusCode callEvtGen( EvtParticle * &thePart , 
                         const HepMC::GenParticle * theHepMCParticle ,
                         const EvtId & eid ) const ;  
  
  /** Get 2J+1 spin for particles not supported in LHCbKernel/ParticleID
   *
   *  @return integer
   *  @param theId ParticleID of the particle for which to calculate 2J+1
   */
  int getParticleSpin( const ParticleID& theId ) const ;

  /// returns branching fraction of the given Id
  double branching( const EvtId& id ) const ;

  /// check if id exists in generic decay table
  bool checkGeneric( const EvtId& id ) const ;

  // member variables
 
  /// EvtGen engine
  EvtGen * m_gen ;

  /// Random Engine
  EvtRandomEngine * m_randomEngine ;

  /// Generic decay file name
  std::string m_decayFile ;

  /// User decay file name
  std::string m_userDecay ;

  /// Event type number
  int m_evnType ;

  /// PDG codes of B hadrons to generate
  std::vector< int > m_hadrons ;

  /// EvtId of B and Bbar hadrons to generate
  EvtId m_bId , m_bbarId ;

  /// Do cpMixture
  bool  m_cpMixture ;

  /// Is a signal decay tree 
  bool  m_signalDecay ;

  /// Do not erase temporary evt.pdl particle property file
  bool  m_keepTempEvtFile ;

  /// Minimum and maximum values for ctau. Outside, ctau is set to 0.
  double m_minctau, m_maxctau ; 

  /// Minimum value for width. Below it is set to 0.
  double m_minwidth ;

  /// Forced fragmentation flag
  bool m_forcedFragmentation ;

  /// Inclusive production flag
  bool m_inclusiveProduction ;

  /// Flag for repeated hadronization
  bool m_repeatedHadronization ;

  /// Flag for polarized Lambda_b production
  bool m_generatePolLambdab ;
};

//=============================================================================
// Enable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenTool::enableFlip() const { 
  EvtIncoherentMixing::enableFlip() ; 
}

//=============================================================================
// Disable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenTool::disableFlip() const { 
  EvtIncoherentMixing::disableFlip() ; 
}

#include "GaudiKernel/RndmGenerators.h"

class IRndmGenSvc ;
// interface to GaudiRandomEngine
// random engine which will be used inside EvtGen
class EvtGaudiRandomEngine : public EvtRandomEngine {
public:
  /// Constructor
  EvtGaudiRandomEngine( IRndmGenSvc* i , StatusCode & sc ) ;
  /// Destructor
  virtual ~EvtGaudiRandomEngine ( ) ;
  /// return a random number from the Gaudi engine
  double random() ;
private:
  /// Gaudi random engine common to all Gauss algorithms
  Rndm::Numbers m_randomgaudi ;
} ;

//=============================================================================
// Return random number
//=============================================================================
inline double EvtGaudiRandomEngine::random() { return m_randomgaudi() ; }

#endif // GENERATORS_EVTGENTOOL_H
