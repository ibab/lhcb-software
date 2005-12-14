// $Id: Inclusive.h,v 1.4 2005-12-14 21:53:15 robbep Exp $
#ifndef GENERATORS_INCLUSIVE_H 
#define GENERATORS_INCLUSIVE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/ExternalGenerator.h" 

// forward declarations

namespace HepMC {
  class GenParticle ;
}

/** @class Inclusive Inclusive.h "Inclusive.h" 
 *  
 *  Tool for inclusive samples generation. The particles to 
 *  request are specified by their PDG Id in a list. Concrete
 *  implementation of a ISampleGenerationTool using the 
 *  ExternalGenerator base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Inclusive : public ExternalGenerator {
 public:
  /// Standard constructor
  Inclusive( const std::string& type, const std::string& name,
             const IInterface* parent);
  
  virtual ~Inclusive( ); ///< Destructor

  /** Initialize method 
   *  Find the lightest particle among the requested particles.
   *  When generating events, all particles heavier than this mass
   *  will be decayed with the IDecayTool before the event is analyzed
   *  to see if it contains particles of the inclusive list.
   */
  virtual StatusCode initialize( ) ;

  /** Generate a set of interactions.
   *  Implements ISampleGenerationTool::generate.
   *  -# Generate interactions using the IProductionTool
   *  -# Check if the interaction has a particle specified in
   *     the PID list
   *  -# Apply the IGenCutTool on the interaction containing
   *     a particle of the list.
   *  -# Parity-flip the event (z -> -z, pz -> -pz) if all particles
   *     of the inclusive list in the event have pz < 0.
   */
  virtual bool generate( const unsigned int nPileUp ,
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  /** Print generation counters.
   *  Implements ISampleGenerationTool::printCounters.
   *  Prints (after generator level cuts):
   *  -# Fractions of B0/B+/B0s/Bc/b-Baryons (and idem for D) in 
   *     selected events.
   *  -# Fractions of B/Bstar/Bdoublestar in selected events.
   */
  virtual void printCounters( ) const ;

 protected:

  /** Update counters for flavour fractions.
   *  @param[in] theEvent  The event where to count.
   */
  void updateHadronCounters( const HepMC::GenEvent * theEvent ) ;

  /** Update counters for excited states.
   *  @param[in] theEvent  The event where to count.
   */
  void updateExcitedStatesCounters( const HepMC::GenEvent * theEvent ) ;

 private:
  /// Mass of lightest hadron in list of particles to produce
  double        m_lightestMass     ; 

  /// PDG Id of lightest hadron in list of particles to produce
  unsigned int  m_lightestPID      ;

  /// Counter of events before applying generator level cut
  unsigned int  m_nEventsBeforeCut ;

  /// Counter of events after applying generator level cut
  unsigned int  m_nEventsAfterCut  ;

  /// Counter of parity-flipped events (z -> -z, pz -> -pz)
  unsigned int  m_nInvertedEvents  ;

  /// Ordered set of PDG Id of particles to produce (set by options)
  PIDs          m_pids             ;

  /// Vector to obtain list of PDG Ids from job options
  std::vector< int > m_pidVector   ;

  unsigned int  m_nB0 ; ///< Counter of B0 (after generator level cut)

  unsigned int m_nB0bar ; ///< Counter of B0bar (after cut)

  unsigned int  m_nBp ; ///< Counter of B+ (after cut)

  unsigned int m_nBm ; ///< Counter of B- (after cut)

  unsigned int  m_nBs0 ; ///< Counter of Bs0 (after cut)

  unsigned int m_nBs0bar ; ///< Counter of Bs0bar (after cut)

  unsigned int  m_nbBaryon ; ///< Counter of b-baryons (after cut)

  unsigned int m_nantibBaryon ; ///< Counter of anti-b-baryons (after cut)

  unsigned int  m_nBcp ; ///< Counter of Bc+ (after cut)

  unsigned int m_nBcm  ; ///< Counter of Bc- (after cut)

  unsigned int  m_nbb ; ///< Counter of (bb) (after cut)
  
  unsigned int  m_nD0 ; ///< Counter of D0 (after cut) 

  unsigned int m_nD0bar ; ///< Counter of D0bar (after cut) 

  unsigned int  m_nDp ; ///< Counter of D+ (after cut) 

  unsigned int m_nDm ; ///< Counter of D- (after cut)

  unsigned int  m_nDsp ; ///< Counter of Ds+ (after cut)

  unsigned int m_nDsm ; ///< Counter of Ds- (after cut)

  unsigned int m_ncBaryon ; ///< Counter of charm baryons (after cut)

  unsigned int m_nanticBaryon ; ///< Counter of anti-charm baryons (after cut)

  unsigned int m_ncc ; ///< Counter of (cc) (after cut)

  /// Counter of B (Spin 0, Angular momentum 0)
  unsigned int  m_n0starB ;

  /// Counter of B* (Spin 1, Angular momentum 0)
  unsigned int m_n1starB ;

  /// Counter of B** (Angular momentum 1)
  unsigned int m_n2starB ;

  /// Counter of D (spin 0, Angular momentum 0)
  unsigned int  m_n0starC ;

  /// Counter of D* (Spin 1, Angular momentum 0)
  unsigned int m_n1starC ;

  /// Counter of D** (Angular momentum 1)
  unsigned int m_n2starC ;
};
#endif // GENERATORS_INCLUSIVE_H
