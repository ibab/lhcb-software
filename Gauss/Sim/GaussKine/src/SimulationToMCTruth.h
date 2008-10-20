// $Id: SimulationToMCTruth.h,v 1.1 2008-10-20 08:08:54 robbep Exp $
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
class IParticlePropertySvc ;
namespace HepMC {
  class GenParticle ;
};
namespace LHCb {
  class MCHeader ;
};

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

  /** accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  IGiGaSvc* gigaSvc() const { 
    return m_gigaSvc; 
  }

  /** accessor to kinematics  conversion service 
   *  @return pointer to kinematics conversion service 
   */
  IGiGaKineCnvSvc*      kineSvc   () const { 
    return m_gigaKineCnvSvc;
  } 
  
  /// Convert a HepMC particle from Geant4 to a MC Particle
  void convert( const HepMC::GenParticle* genpart, LHCb::MCVertex* prodvertex );
  
  /// Determines the MCVertex event type
  LHCb::MCVertex::MCVertexType vertexType( int id );
  
  /// Find the primary vertex
  LHCb::MCVertex * findPrimaryVertex( const Gaudi::XYZPoint & vertex ) const ; 

  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service

  std::string m_particles ;    ///< Name of MCParticles location
  std::string m_vertices  ;    ///< Name of MCVertices location
  
  LHCb::MCParticles * m_particleContainer ; ///< MCParticles container
  LHCb::MCVertices  * m_vertexContainer   ; ///< MCVertices container
  LHCb::MCHeader    * m_mcHeader          ; ///< MCHeader container

  /// Reference to the particle property service
  IParticlePropertySvc * m_ppSvc;

  /// Set to hold keys of treated MCParticles when rebuilding decay tree
  std::set< LHCb::MCParticle::key_type >  m_treatedParticles ;
};

#endif  // SIMULATIONTOMCTRUTH_H

