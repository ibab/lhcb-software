// $Id: EvtGenDecay.h,v 1.1 2005-10-03 10:26:45 robbep Exp $
#ifndef GENERATORS_EVTGENDECAY_H 
#define GENERATORS_EVTGENDECAY_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from GeneratorModules
#include "Generators/IDecayTool.h"

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

/** @class EvtGenDecay EvtGenDecay.h Algorithms/EvtGenDecay.h
 *  
 *  Tool to contain EvtGen generator
 *
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */
class EvtGenDecay : public GaudiTool, virtual public IDecayTool {
public:
  /// Standard constructor
  EvtGenDecay( const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~EvtGenDecay( ) ; ///< Destructor

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

  virtual StatusCode generateSignalDecay( HepMC::GenEvent * theEvent ,
                                          HepMC::GenParticle * theMother , 
                                          bool & flip ) const ;
  

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

  /// Enable the possibility to flip the flavour for CP decay modes in EvtGen
  virtual void enableFlip() const ; 

  /// Disable the possibility to flip the flavour for CP decay modes in EvtGen
  virtual void disableFlip() const ;
  
  /** true if the particle has a decay table defined in the generic
   *  or in the user decay file
   *
   *  @return boolean
   *  @param pdgId particle Id of the particle to test
   */
  virtual bool isKnownToDecayTool( int pdgId ) const ;

  /** get the signal branching fraction in the generic decay file 
   *  including daughter decay branching fractions
   */
  virtual double getSignalBr( ) const ;
  
  /// check if the signal decay mode is also in the generic table
  virtual bool checkSignalPresence( ) const ;

  virtual void setSignal( const int ) ;
  
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
  void checkParticle( const HepMC::GenParticle * theParticle ) const ;
  
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

  /// Return the id of the alias corresponding to the pdg code pdgId
  virtual const EvtId getSignalAlias( int pdgId ) const ;

  // member variables
 
  /// EvtGen engine
  EvtGen * m_gen ;

  /// Random Engine
  EvtRandomEngine * m_randomEngine ;

  /// Generic decay file name
  std::string m_decayFile ;

  /// User decay file name
  std::string m_userDecay ;

  /// EvtId of signal ID
  EvtId m_signalId ;

  /// Do not erase temporary evt.pdl particle property file
  bool  m_keepTempEvtFile ;

  /// Minimum and maximum values for ctau. Outside, ctau is set to 0.
  double m_minctau, m_maxctau ; 

  /// Minimum value for width. Below it is set to 0.
  double m_minwidth ;

  /// Flag for polarized Lambda_b production
  bool m_generatePolLambdab ;
};

//=============================================================================
// Enable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenDecay::enableFlip() const { 
  EvtIncoherentMixing::enableFlip() ; 
}

//=============================================================================
// Disable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenDecay::disableFlip() const { 
  EvtIncoherentMixing::disableFlip() ; 
}

#include "GaudiKernel/RndmGenerators.h"

class IRndmGenSvc ;
// interface to GaudiRandomEngine
// random engine which will be used inside EvtGen
class EvtGenGaudiRandomEngine : public EvtRandomEngine {
public:
  /// Constructor
  EvtGenGaudiRandomEngine( IRndmGenSvc* i , StatusCode & sc ) ;
  /// Destructor
  virtual ~EvtGenGaudiRandomEngine ( ) ;
  /// return a random number from the Gaudi engine
  double random() ;
private:
  /// Gaudi random engine common to all Gauss algorithms
  Rndm::Numbers m_randomgaudi ;
} ;

//=============================================================================
// Return random number
//=============================================================================
inline double EvtGenGaudiRandomEngine::random() { return m_randomgaudi() ; }

#endif // GENERATORS_EVTGENDECAY_H
