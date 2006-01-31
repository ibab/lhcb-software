// $Id: G4HepMCToMCTruth.h,v 1.1.1.1 2006-01-31 11:03:51 gcorti Exp $
#ifndef G4HEPMCTOMCTRUTH_H 
#define G4HEPMCTOMCTRUTH_H 1
 
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gauss
#include "GaussTools/MCTruthManager.h"

/** @class G4HepMCToMCTruth G4HepMCToMCTruth.h 
 *
 *  Algorithm to retrieve HepMC as filled during G4 tracking and fill
 *  corresponding LHCb MCParticles/MCVertices
 *
 *  @author  Witold Pokorski, adapted by G.Corti
 *  @date    23/09/2005
 */
class G4HepMCToMCTruth: public GaudiAlgorithm {
public:
  
  /// Standard constructor
  G4HepMCToMCTruth( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~G4HepMCToMCTruth    ( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization  
  
private:
  
  void convert( HepMC::GenParticle* genpart, LHCb::MCVertex* prodvertex );
  
  
private:

  std::string m_particles;
  std::string m_vertices;
  
  LHCb::MCParticles* partcont;
  LHCb::MCVertices* vtxcont;

  // map of barcodes to pointers of particles
  std::map<int, LHCb::MCParticle*> particlemap;

  /// Reference to the particle property service
  IParticlePropertySvc* m_ppSvc;

};

#endif  // G4HEPMCTOMCTRUTH_H

