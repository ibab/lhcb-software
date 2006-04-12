// $Id: G4HepMCToMCTruth.h,v 1.2 2006-04-12 19:02:49 gcorti Exp $
#ifndef G4HEPMCTOMCTRUTH_H 
#define G4HEPMCTOMCTRUTH_H 1
 
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gauss
#include "GaussTools/MCTruthManager.h"

// Forward declarations
class IGiGaSvc;
class IGiGaKineCnvSvc;

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
  
protected:

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

private:
  
  void convert( HepMC::GenParticle* genpart, LHCb::MCVertex* prodvertex );
  
  LHCb::MCVertex::MCVertexType vertexType( int id );

private:

  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service

  std::string m_particles;
  std::string m_vertices;
  
  LHCb::MCParticles* partcont;
  LHCb::MCVertices* vtxcont;

  /// Reference to the particle property service
  IParticlePropertySvc* m_ppSvc;

};

#endif  // G4HEPMCTOMCTRUTH_H

