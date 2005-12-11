// $Id: Signal.h,v 1.5 2005-12-11 23:21:47 robbep Exp $
#ifndef GENERATORS_SIGNAL_H 
#define GENERATORS_SIGNAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/ExternalGenerator.h" 

// Forward declarations
namespace HepMC {
  class GenParticle ;
}

/** @class Signal Signal.h "Generators/Signal.h"
 *  
 *  Base class for signal samples generation tools. It contains utility 
 *  function that can be used when generating signal samples.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Signal : public ExternalGenerator {
 public:
  /// Vector of particles
  typedef std::vector< HepMC::GenParticle * > GenParticles ;

  /// Standard constructor
  Signal( const std::string& type, const std::string& name, 
          const IInterface* parent);
  
  virtual ~Signal( ); ///< Destructor
  
  /// Initialize
  virtual StatusCode initialize( ) ;  

  /// Print counters and cut efficiencies at the end of the job.
  virtual void printCounters( ) const ;

protected:
  /// PID of the signal particle for this job (set by job options)
  PIDs          m_pids             ;

  /** Flag to indicate if clean events have to be generated (set by job 
   *  options). Clean events are events where only the signal decay tree
   *  is kept.
   */
  bool          m_cleanEvents      ;

  /// Number of events before the generator level cut.
  unsigned int  m_nEventsBeforeCut ; 

  /** Number of events accepted by the generator level cut. This counter is
   *  used to compute the generator cut efficiency on inclusive events.
   */
  unsigned int  m_nEventsAfterCut  ;

  /** Number of signal particles (w/r to anti-particles) before the generator 
   *  level cut.
   */
  unsigned int  m_nParticlesBeforeCut ;

  /// Number of signal anti-particles before the generator level cut.
  unsigned int  m_nAntiParticlesBeforeCut ;

  /** Number of signal particles accepted by the generator level cut. This 
   *  counter is used to compute the generator level cut efficiency to compute
   *  signal yields.
   */
  unsigned int  m_nParticlesAfterCut ;

  /// Number of signal anti-particles accepted by the generator level cut.
  unsigned int  m_nAntiParticlesAfterCut ;

  /// Number of events parity-flipped. (z -> -z, pz -> -pz)
  unsigned int  m_nInvertedEvents  ;

  double        m_signalMass       ; ///< Mass of the signal particle

  unsigned int  m_signalPID        ; ///< PDG Id of the signal particles

  /// Flag to indicate that a mixture of signal particles and anti-particles.
  bool          m_cpMixture        ;

  /** Branching fraction of the signal forced decay mode in the main decay 
   *  table.
   */
  double        m_signalBr         ;

  Rndm::Numbers m_flatGenerator    ; ///< Flat random nunmber generator

  /** Isolate signal particle decay tree for clean events. The resulting
   *  event is stored in the container /Event/Gen/SignalDecayTree.
   */
  StatusCode isolateSignal( const HepMC::GenParticle * theSignal ) const ;

  /** Choose randomly a particle in a list of particles and revert the event 
   *  if this particle has pz < 0, and returns the choosen particle.
   */
  HepMC::GenParticle * chooseAndRevert( const ParticleVector & particleList ) ;

  /** Ensures correct multiplicity of signal particles with an accept/reject
   *  algorithm.
   *  @param[in] nSignal  Number of particles in the event with flavour 
   *                      corresponding to the flavour of the signal particles.
   *  @return true if the event has to kept.
   *  @return false if the event has to be rejected.
   */
  bool ensureMultiplicity( const unsigned int nSignal ) ;

  /** Update counters for efficiency calculations.
   *  @param[in]  particleList         List of particles to count.
   *  @param[out] particleCounter      Number of particles (w/r to 
   *                                   antiparticles) in particleList.
   *  @param[out] antiparticleCounter  Number of anti-particles in particleList
   *  @param[in]  onlyForwardParticles If true, count only particles and 
   *                                   anti-particles with pz>0.
   */
  void updateCounters( const ParticleVector & particleList , 
                       unsigned int & particleCounter , 
                       unsigned int & antiparticleCounter ,
                       bool onlyForwardParticles ) const ;
  
 private:
  /** Add to an existing event a new decay tree.
   *  @param[out] theNewParticle  Particle to update with the decay tree
   *                              of theOldParticle.
   *  @param[in]  theOldParticle  Particle containing the decay tree to add
   *                              to theNewParticle.
   */
  StatusCode fillHepMCEvent( HepMC::GenParticle * theNewParticle ,
                             const HepMC::GenParticle * theOldParticle ) 
    const ;

  /// Temporary ector to obtain PIDs from job options.
  std::vector< int > m_pidVector   ;
};
#endif // GENERATORS_SIGNAL_H
