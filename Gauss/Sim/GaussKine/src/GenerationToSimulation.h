// $Id: GenerationToSimulation.h,v 1.5 2009-03-26 21:42:04 robbep Exp $
#ifndef GENERATIONTOSIMULATION_H 
#define GENERATIONTOSIMULATION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Vector4DTypes.h"

// From Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// From HepMC
#include "HepMC/SimpleVector.h"

// Forward declarations
class IGiGaSvc;
class G4PrimaryVertex ;
class G4PrimaryParticle ;
namespace HepMC {
  class GenEvent ;
  class GenParticle ;
  class GenVertex ;
};

/** @class GenerationToSimulation GenerationToSimulation.h
 *  Algorithm to transmit particles from the generation phase
 *  to the simulation phase
 *
 *  @author Gloria CORTI, Patrick ROBBE
 *  @date   2008-09-24
 */
class GenerationToSimulation : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GenerationToSimulation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenerationToSimulation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private: 
  /** accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  IGiGaSvc* gigaSvc() const { return m_gigaSvc; }
  
  /// Determine the primary vertex of the interaction
  Gaudi::LorentzVector primaryVertex( const HepMC::GenEvent * genEvent ) const ;
  
  /// Decide if a particle has to be kept or not
  bool keep( const HepMC::GenParticle * particle ) const ;
  
  /// Convert a GenParticle either into a MCParticle or G4PrimaryParticle
  void convert( HepMC::GenParticle *& particle , 
                G4PrimaryVertex * pvertexg4 ,
                LHCb::MCVertex * originVertex , G4PrimaryParticle * motherg4 ,
                LHCb::MCParticle * mothermcp ) ;
		
  /// decides if the particle should be transfered to Geant4 or only MCParticle
  unsigned char transferToGeant4( const HepMC::GenParticle * p ) const ;
  
  /// Create a G4PrimaryParticle from a HepMC Particle
  G4PrimaryParticle * makeG4Particle( HepMC::GenParticle *& particle ,
                                      LHCb::MCParticle * mcp ) const ;
                                      
  /// Create a MCParticle from a HepMC Particle
  LHCb::MCParticle * makeMCParticle( HepMC::GenParticle *& particle ,
                                     LHCb::MCVertex *& endVertex ) const ;

   /// Compute the lifetime of a particle
  double lifetime( const HepMC::FourVector mom , const HepMC::GenVertex * P , 
                   const HepMC::GenVertex * E  ) const ;
  
  /// Check if a particle has oscillated
  const HepMC::GenParticle * hasOscillated( const HepMC::GenParticle * P ) const ;
  
  /// Remove a primary particle from a primary vertex
  void removeFromPrimaryVertex( G4PrimaryVertex *& pvertexg4 , 
				const G4PrimaryParticle * particleToDelete ) const ;

  std::string m_gigaSvcName;         ///< Name of GiGa Service
  IGiGaSvc*   m_gigaSvc;             ///< Pointer to GiGa Service
  double      m_travelLimit ;        ///< Max value of distance in detector for Geant4
  bool        m_lookForUnknownParticles ; ///< Check if the particles are known to G4
  bool        m_skipGeant4 ;        ///< Do not transmit anything to Geant4 (usuful for fast MC) 
  bool        m_updateG4ParticleProperties ; ///< Update G4 properties for masses

  std::string m_generationLocation ; ///< Location in TES of input HepMC events
  std::string m_particlesLocation ;  ///< Location in TES of output MCParticles
  std::string m_verticesLocation ;   ///< Location in TES of output MCVertices
  std::string m_mcHeader         ;   ///< Location in TES of MCHeader
  
  LHCb::MCParticles * m_particleContainer ; ///< Container of MCParticles
  LHCb::MCVertices  * m_vertexContainer   ; ///< Container of MCVertices
  
  std::map< int , std::pair< bool , G4PrimaryParticle * > > m_g4ParticleMap ; ///< Map to know if a particle was already converted to G4
  std::map< int , bool > m_mcParticleMap ; ///< Map to know if a particle was already converted to MCParticle
  std::vector< G4PrimaryParticle * > m_particlesToDelete ; ///< Vector of particles to remove from G4PrimaryVertex
};
#endif // GENERATIONTOSIMULATION
