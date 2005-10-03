// $Id: Signal.h,v 1.1 2005-10-03 10:26:10 robbep Exp $
#ifndef GENERATORS_SIGNAL_H 
#define GENERATORS_SIGNAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/ExternalGenerator.h" 

namespace HepMC {
  class GenParticle ;
}

/** @class Signal Signal.h 
 *  
 *  Base Tool for signal samples generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Signal : public ExternalGenerator {
public:
  typedef std::vector< HepMC::GenParticle * > GenParticles ;

  /// Standard constructor
  Signal( const std::string& type, 
          const std::string& name,
          const IInterface* parent);
  
  virtual ~Signal( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;  

  virtual void printCounters( ) const ;

protected:
  PIDs          m_pids             ;
  bool          m_isolateSignal    ;
  int           m_nEventsBeforeCut ;
  int           m_nEventsAfterCut  ;
  int           m_nInvertedEvents  ;
  double        m_signalMass       ;
  unsigned int  m_signalPID        ;
  bool          m_cpMixture        ;
  double        m_signalBr         ;
  Rndm::Numbers m_flatGenerator    ;

  StatusCode isolateSignal( const HepMC::GenParticle * theSignal ) const ;
  StatusCode fillHepMCEvent( HepMC::GenEvent * theEvent , 
                             HepMC::GenParticle * theNewParticle ,
                             const HepMC::GenParticle * theOldParticle ) 
    const ;
  
private:
  std::vector< int > m_pidVector   ;
};
#endif // GENERATORS_SIGNAL_H
