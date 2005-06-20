// $Id: GenBaseAlg.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// --------------------------------------------------
//
// Initial code by M. Shapiro
//
// LHCb version: W. Pokorski
//


#ifndef GENERATORS_GENBASEALG_H
#define GENERATORS_GENBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

#include "HepMC/GenEvent.h"

class HepMCEvent ;
class HardInfo ;
class HepRandomEngine ;

/** @class GenBaseAlg GenBaseAlg.h Algorithms/GenBaseAlg.h
 *  
 *  Base class to use to interface generators
 *
 *  @author
 *  @date   2003-10-10
 */
class GenBaseAlg : public GaudiAlgorithm {
public:
  typedef std::vector< HepMC::GenParticle * >           GenParticles     ;
  typedef std::vector< HepMCEvent * >                   EventVector      ;
  typedef std::vector< HardInfo *>                      HardVector       ;
  typedef HepMC::GenEvent::vertex_const_iterator        HepMCVIterator   ;
  typedef HepMC::GenEvent::particle_const_iterator      HepMCPIterator   ;
  typedef std::vector< int >                            PIDs             ;
  
  /// Standard constructor
  GenBaseAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~GenBaseAlg();

  /** Initialize method (implemented in GenBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode initialize();

  /** Execute method (implemented in GenBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode execute();

  /** Finalize method (implemented in GenBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode finalize();

  /** Initialization of the generator (to be overriden in specific
   *  implementation code)
   *  @return Status Code
   */
  virtual StatusCode genInitialize() { return StatusCode::SUCCESS ; }

  /** Call to the generator (to be overriden in specific implementation code)
   *  @return integer to describe event type 
   */
  virtual int callGenerator() = 0 ;

  /** Finalization of the generator (to be overriden in specific implementation
   *  code)
   *  @return Status Code
   */
  virtual StatusCode genFinalize() { return StatusCode::SUCCESS ; }

  /** Convert event in internal generator format to HepMC::GenEvent format
   *  @return Status Code
   *  @param[out] evt the converted HepMC::GenEvent event
   */
  virtual StatusCode fillEvt( HepMC::GenEvent* /* evt */ ) = 0 ;
  
  /** Apply generator level selection criteria
   *  @retval true if event pass selection criteria
   *  @param[in] evt the HepMC::GenEvent to analyze
   */
  virtual bool selectEvt( HepMC::GenEvent* evt) ;

  /** Optional pre-initialization sequence (does nothing if not overriden)
   *  @retval Status Code
   */
  virtual StatusCode preInitialize() { return StatusCode::SUCCESS ; }

  /** Optional pre-finalization sequence (does nothing it not overriden)
   *  @retval Status Code
   */
  virtual StatusCode preFinalize() { return StatusCode::SUCCESS ; }
  
  /// Turn off Pythia fragmentation
  virtual void turnOffPythiaFragmentation ( ) { ; }

  /// Turn on Pythia fragmentation
  virtual void turnOnPythiaFragmentation ( ) { ; }
  
  /** Counts the number of b quarks in the event
   *  @retval the number of b quarks
   */
  virtual int  isBBbarEvent( ) { return 0 ; }

  /// Save Parton event 
  virtual void savePartonEvent( ) { ; }

  /// Hadronize event
  virtual void hadronize( ) { ; }

  /// Convert to HEPEVT Common block
  virtual int  convertToStdHep( ) { return 0 ; }

  /// Load parton event
  virtual void loadPartonEvent( ) { ; }
 
  /// function to retrieve the hard scatter parameters
  virtual void hardProcessInfo(HardInfo* /* hardInfo */ ) { ; }
  
protected:
	/// Event Type
  int m_evnType;

  /// List of hadrons to produce
  PIDs m_hadrons ;

  /// PDG id of signal B hadron
  int m_bhadron;

  /// Maximum angle (in rad) of the angular acceptance cut
  double m_thetamax;

  /// Minimum p of the signal particle
  double m_pmin;

  /// Mode for pile-up
  int m_mode;

  /// Has the B hadron already been found ?
  bool m_bfound;

  /// Flag for signal job
  bool m_signal ;

  /// Forced fragmentation is requested
  bool m_forcedFragmentation ;

  /// Flag for inclusif B production
  bool m_binclus;

  /// Flag for repeated hadronization
  bool m_repHad ;

  /// Mean number of pile-up interactions to generate per event
  double m_meanInteractions;

  /// Luminosity
  double m_luminos;

  /// Fill duration
  double m_filldur;

  /// Beam decay time
  double m_dectime;

  /// Beam crossing rate
  double m_crossrate;

  /// Total cross section
  double m_totxsect;

  /// Gaussian distribution generator
  Rndm::Numbers m_gaussDist;

  /// Number of z-inverted events
  int m_numbOfZminusZEvt;

  /// Number of B hadron z-inverted events 
  int m_numbOfZminusZEvtB ;

  /// Number of Bbar hadron z-inverted events 
  int m_numbOfZminusZEvtBbar ;

  /// Number of generated interactions containing at least one b quark
  int m_numberOfBInteractions ;

  /// Number of accepted interactions containing at least one b quark
  int m_numberOfAcceptedBInteractions ;

  /// Number of accepted interactions containing at least three b quarks
  int m_numberOfAcceptedDoubleBInteractions ;

  /// number of flip accepted events
  int m_numberOfFlipAccepted ;  

  /// Number of Flip accepted B
  int m_numberOfFlipAcceptedB ;

  /// Number of Flip accepted Bbar
  int m_numberOfFlipAcceptedBbar ;

  /// Number of particles presented to the angular cut 
  int m_numberBPresentedAngularCut ;

  /// Number of B presented to the angular cut 
  int m_numberBPresentedAngularCutB ;

  /// Number of Bbar presented to the angular cut 
  int m_numberBPresentedAngularCutBbar ;

  /// Number of particles accepted by the angular cut
  int m_numberBAcceptedAngularCut ;

  /// Number of B accepted by the angular cut
  int m_numberBAcceptedAngularCutB ;

  /// Number of Bbar accepted by the angular cut
  int m_numberBAcceptedAngularCutBbar ;

  /// Number of particles before Flip test
  int m_numberOfBBeforeFlip ;

  /// Number of B before Flip test
  int m_numberOfBBeforeFlipB ;

  /// Number of Bbar before Flip test
  int m_numberOfBBeforeFlipBbar ;

  /// Number of events with at least one b quark
  int m_bCounter ;

  /// Number of events with at least 3 b quarks
  int m_fourbCounter ;

  /// flag to control repetition of hadronization
  bool m_skipEvent ;
  
private:
  /// Peak luminosity
  double m_rlpeak;

  /// Number of bunch crossings
  int m_numbOfBunchX ;

  /// Number of non empty events
  int m_numbOfEvents ;

  /// Number of accepted events
  int m_numbOfAcceptedEvt ;

  /// Number of interactions generated
  int m_numbOfInter ;

  /// Number of interactions in the accepted events
  int m_numbOfAcceptedInt ;

  /// Number of times the repeated hadronization is executed
  int m_numberOfRepetitions ;

  /// Number of times the hadronization repetitions are greater than the limit
  int m_numberOfExcessHad ;

  /// Maximum number of try for repeated hadronization
  int m_maxNumberOfRep ;  
};   

#endif  // GENERATORS_GENBASEALG_H

