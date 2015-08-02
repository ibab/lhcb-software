// $Id: SimulationToMCTruth.h,v 1.5 2009-03-26 21:42:04 robbep Exp $
#ifndef SIMULATIONTOMCTRUTH_H 
#define SIMULATIONTOMCTRUTH_H 1
 
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Event
#include "Event/MCVertex.h"

// Forward declarations
class IGiGaSvc ;
class IGiGaKineCnvSvc ;
class IFlagSignalChain;
namespace HepMC {
  class GenParticle ;
  class GenVertex ;
}
namespace LHCb {
  class IParticlePropertySvc ;
  class MCHeader ;
}

/** @class SimulationToMCTruth SimulationToMCTruth.h 
 *
 *  Algorithm to retrieve HepMC as filled during G4 tracking and fill
 *  corresponding LHCb MCParticles/MCVertices, rebuilding links left 
 *  over from GenerationToSimulation
 *
 *  @author  Witold Pokorski, adapted by G.Corti and P. Robbe
 *  @date    06/10/2008
 */
class SimulationToMCTruth: public GaudiAlgorithm {
public:
  
  /// Standard constructor
  SimulationToMCTruth( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SimulationToMCTruth    ( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  
private:
  /// Delete a complete tree from event record
  void deleteParticle( LHCb::MCParticle * P ) ;
  
  /// Convert a HepMC particle from Geant4 to a MC Particle
  void convert( const HepMC::GenParticle* genpart, LHCb::MCVertex* prodvertex , 
                bool isPrimary );
                
  /// Check if a vertex is at the Geant4 volume boundary
  bool isEndOfWorldVertex( HepMC::GenVertex * ev ) ;
  
  /// Determines the MCVertex event type
  LHCb::MCVertex::MCVertexType vertexType( int id );
  
  /// Find the primary vertex
  LHCb::MCVertex * findPrimaryVertex( const Gaudi::XYZPoint & vertex ) const ; 

  // Data members
  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  bool               m_checkUnknown;     ///< Check for unknown PDG Ids
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service

  std::string m_particles ;    ///< Name of MCParticles location
  std::string m_vertices  ;    ///< Name of MCVertices location
  std::string m_mcHeaderLocation ; ///< Name of MCHeader Location
  
  LHCb::MCParticles * m_particleContainer ; ///< MCParticles container
  LHCb::MCVertices  * m_vertexContainer   ; ///< MCVertices container
  LHCb::MCHeader    * m_mcHeader          ; ///< MCHeader container

  /// Reference to the particle property service
  LHCb::IParticlePropertySvc * m_ppSvc;

  /// Reference to tool to propagate fromSignal flag
  IFlagSignalChain* m_setSignalFlagTool ;

  /// Set to hold keys of treated MCParticles when rebuilding decay tree
  std::set< LHCb::MCParticle::key_type >  m_treatedParticles ;
  
  /// PDG Id of the intermediate particles
  int m_intermediatePDG ;
};

#endif  // SIMULATIONTOMCTRUTH_H

