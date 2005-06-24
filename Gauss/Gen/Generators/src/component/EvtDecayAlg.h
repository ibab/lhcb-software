// $Id: EvtDecayAlg.h,v 1.2 2005-06-24 16:32:47 gcorti Exp $
#ifndef GENERATORS_EVTDECAYALG_H
#define GENERATORS_EVTDECAYALG_H 1

// Include Files
#include "GaudiAlg/GaudiAlgorithm.h"

#include "HepMC/GenEvent.h"

#include "CLHEP/Vector/ThreeVector.h"
class IEvtGenTool ;

/** @class EvtDecayAlg EvtDecayAlg.h GeneratorModules/EvtDecayAlg.h
 *
 *  Algorithm to decay particles with EvtGen.
 *  Perform generic decays and boost signal decay to correct frame
 *  given by Pythia.
 *
 *  @author Witek Pokorski
 *  @author Patrick Robbe
 *  @date   01/09/2003
 */

class EvtDecayAlg : public GaudiAlgorithm {
public:  
  /// Type for vector of HepMC::GenParticle *
  typedef std::vector< HepMC::GenParticle * > GenParticles ;
  
  /// Default constructor
  EvtDecayAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Virtual destructor
  virtual ~EvtDecayAlg( ) ;
  
  /// Initialize method
  virtual StatusCode initialize();

  /// Execute method
  virtual StatusCode execute();

  /// Finalize method
  virtual StatusCode finalize();

private:
  bool m_isolateSignal ; ///< Flag to store signal decay in specific location
  IEvtGenTool * m_evtTool ;   ///< Interface to EvtGen
  std::string m_eventLoc;     ///< Location in TES for event   

  // Utility functions
  /** boost a decay tree to a given frame
   *
   *  @return status code
   *  @param theMother pointer to the HepMCParticle to substitute with 
   *         the boosted decay tree of theSignal 
   *  @param theSignal pointer to the HepMCParticle to boost with boost
   *         vector theVector
   *  @param theEvent current HepMC event
   *  @param theVector boost vector
   */
  StatusCode boostTree( HepMC::GenParticle * theMother , 
                        const HepMC::GenParticle * theSignal ,
                        HepMC::GenEvent * theEvent , 
                        const Hep3Vector & theVector ) const ;

  /** isolate B signal decay tree and put it in a specific location
   *
   *  @return status code
   *  @param theParticle particle to isolate. The function will also isolate
   *         all its daughters
   */
  StatusCode isolateSignal( const HepMC::GenParticle * theParticle ) const ;

  /** copy a HepMC decay tree into a new HepMCEvent
   *
   *  @return status code
   *  @param theEvent the new HepMCEvent
   *  @param theNewParticle the starting point of the new decay tree
   *  @param theOldParticle the HepMCParticle decay tree to copy in the new
   *         event
   */
  StatusCode fillHepMCEvent( HepMC::GenEvent * theEvent , 
                             HepMC::GenParticle * theNewParticle , 
                             const HepMC::GenParticle * theOldParticle ) 
    const ;
};

#endif // GENERATORMODULES_EVTDECAYALG_H
