// $Id: EvtGenDecay.h,v 1.9 2009-03-16 16:53:26 robbep Exp $
#ifndef GENERATORS_EVTGENDECAY_H 
#define GENERATORS_EVTGENDECAY_H 1

// Avoid contamination with WINDOWS symbols
#ifdef WIN32
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#endif

// Include files
// from STL
#include <string>

// from boost
#include "boost/filesystem/path.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector4DTypes.h"

// from MCInterfaces
#include "MCInterfaces/IDecayTool.h"

// from Generators
#include "IEvtGenTool.h"

// from EvtGen
#include "EvtGenBase/EvtId.hh"

// forward declarations
namespace LHCb {
  class ParticleID ;
}

class EvtGen ;
class EvtParticle ;
class EvtRandomEngine ;

/** @class EvtGenDecay EvtGenDecay.h "EvtGenDecay.h"
 *  
 *  Tool to interface to EvtGen generator. Concrete implementation of 
 *  a IDecayTool.
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

  /** Initialize method.
   *  In initialization:
   *  -# Create a temporary evt.pdl file to transfer particle properties
   *     from the particle property service to EvtGen.
   *  -# Read the main DECAY.DEC decay file and the user signal decay file.
   *  -# Manages Pythia and PHOTOS print-out according to message level.
   *  -# Initializes Pythia for EvtGen.
   */
  virtual StatusCode initialize() ;

  /// Finalize method
  virtual StatusCode finalize() ;

  /// Implements IDecayTool::generateDecay
  virtual StatusCode generateDecay( HepMC::GenParticle * theMother ) const ;

  /// Implements IDecayTool::generateSignalDecay
  virtual StatusCode generateSignalDecay( HepMC::GenParticle * theMother , 
                                          bool & flip ) const ;
  
  /// Implements IDecayTool::generateDecayWithLimit
  virtual StatusCode generateDecayWithLimit( HepMC::GenParticle * theMother,
                                             const int targetId ) const ;

  /// Implements IDecayTool::enableFlip
  virtual void enableFlip() const ; 

  /// Implements IDecayTool::disableFlip
  virtual void disableFlip() const ;
  
  /// Implements IDecayTool::isKnownToDecayTool
  virtual bool isKnownToDecayTool( const int pdgId ) const ;

  /// Implements IDecayTool::getSignalBr
  virtual double getSignalBr( ) const ;
  
  /// Implements IDecayTool::checkSignalPresence
  virtual bool checkSignalPresence( ) const ;

  /// Implements IDecayTool::setSignal
  virtual void setSignal( const int ) ;
  
 protected:
  /** Make a HepMCTree tree from an EvtGen particle stopping at the PDG
   *  Id targetId
   *
   *  @param theParticle pointer to the EvtGen particle to put in HepMC format
   *  @param theMother pointer to the HepMC particle associated to theParticle
   *  @param theOrigin position of the first particle in the decay tree 
   *                (the root of the decay tree)
   *  @param targetId int pdgId of the last particle to generate (not used if
   *         negative)
   */
  StatusCode makeHepMC( EvtParticle * theParticle ,
                        HepMC::GenParticle * theMother ,
                        const Gaudi::LorentzVector & theOrigin ,
                        int targetId = -999 )  const ;

  /** Check if HepMC Particle is valid for decay by EvtGen and to fill a
   *  HepMC event (checks if it is not already decayed and ensures that
   *  it has a defined production vertex) 
   *  @param[in] thePart HepMC::GenParticle to analyze
   *  @return StatusCode::SUCCESS if no problem
   *  @return StatusCode::ERROR if particle has already an end vertex
   */
  void checkParticle( const HepMC::GenParticle * theParticle ) const ;

  /** Call EvtGen to decay a particle
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

  /// Return the id of the alias corresponding to the pdg code pdgId
  virtual const EvtId getSignalAlias( int pdgId ) const ;
 
  EvtGen * m_gen ; ///< EvtGen engine

  EvtRandomEngine * m_randomEngine ; ///< Random Engine to use in EvtGeni

  IEvtGenTool * m_evtgentool ;

private:

  /** Create a temporary evt.pdl file filled with Gaudi Particle Svc
   *  properties to update EvtGen particle properties
   *
   *  @param tempFileName name of the temporary created file
   */
  StatusCode createTemporaryEvtFile( const boost::filesystem::path & tempFileName ) 
    const ;
      
  /** Get 2J+1 spin for particles not supported in LHCbKernel/ParticleID
   *
   *  @param theId ParticleID of the particle for which to calculate 2J+1
   *  @return 2J+1 of the particle
   */
  int getParticleSpin( const LHCb::ParticleID & theId ) const ;

  /// returns branching fraction of the given Id
  double branching( const EvtId & id ) const ;

  /// check if id exists in generic decay table
  bool checkGeneric( const EvtId & id ) const ;
 
  std::string m_decayFile ; ///< Generic decay file name (set by options)

  std::string m_userDecay ; ///< User decay file name (set by options)

  EvtId m_signalId ; /// EvtGen Id of signal ID

  /// Do not erase temporary evt.pdl particle property file (set by options)
  bool  m_keepTempEvtFile ;

  /// Minimum value for ctau. Below ctau is set to 0.
  double m_minctau ;

  /// Minimum value for ctau. Above ctau is set to 0.
  double m_maxctau ; 

  /// Minimum value for width. Below it is set to 0.
  double m_minwidth ;

  /// Unit for TAULA output
  int m_taulaUnit ;

  /// Unit for PHOTOS output
  int m_photosUnit ;

  /// Flag for polarized Lambda_b production (set by options)
  bool m_generatePolLambdab ;

  /// Flag for polarized charmonium production (set by options)
  bool m_generatePolCharmonium ;
  
  /// Real part of spin matrix for helicity 0 for charmonium polarization
  double m_realHelZero ;

  /// Imaginary part of spin matrix for helicity 0 for charmonium polarization
  double m_imHelZero ;
  
  /// Real part of spin matrix for helicity 1 for charmonium polarization
  double m_realHelOne ;

  /// Imaginary part of spin matrix for helicity 1 for charmonium polarization
  double m_imHelOne ;
};

//#include "EvtGenBase/EvtIncoherentMixing.hh"

//=============================================================================
// Enable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenDecay::enableFlip() const { 
//  TODO: look how to implement in EvtGen
//  EvtIncoherentMixing::enableFlip() ; 
}

//=============================================================================
// Disable the possibility to flip the flavour for CP decay modes in EvtGen
//=============================================================================
inline void EvtGenDecay::disableFlip() const { 
// TODO: look how to implement in EvtGen
//  EvtIncoherentMixing::disableFlip() ; 
}

#include "GaudiKernel/RndmGenerators.h"

// from EvtGen
#include "EvtGenBase/EvtRandomEngine.hh"

// forward declaration
class IRndmGenSvc ;
/** @class EvtGenGaudiRandomEngine
 *  interface to GaudiRandomEngine random engine which will be used inside
 *  EvtGen.
 */
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
