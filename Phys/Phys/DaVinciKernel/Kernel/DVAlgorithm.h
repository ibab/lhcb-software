#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"

// from DaVinciTools
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IMassVertexFitter.h"
#include "Kernel/IVertexFitter.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IParticleStuffer.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IAlgorithm2ID.h"

/** @class DVAlgorithm DVAlgorithm.h Kernel/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 *  16/07/2004: P. Koppenburg: Make it a GaudiTupleAlg
 *  11/11/2004: P. Koppenburg: Adapt to next get<> and put<>. Merge with PreDV.
 */
class DVAlgorithm : public GaudiTupleAlg {
public:

  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVAlgorithm( ){ }; ///< Destructor
  
  /// Accessor for PhysDesktop Tool
  IPhysDesktop* desktop() const; 

  /// Accessor for Mass Constrained Vertex Fitter Tool
  IMassVertexFitter* massVertexFitter() const; 

  /// Accessor for Unconstrained Vertex Fitter Tool
  IVertexFitter* vertexFitter() const;

  /// Accessor for Geometrical Displacement Calculation Tool
  IGeomDispCalculator* geomDispCalculator() const;

  /// Accessor for Particle Stuffer Tool
  IParticleStuffer* particleStuffer() const;

  /// Accessor for Particle Filter Tool
  IParticleFilter* particleFilter() const;

  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* ppSvc() const;

  /// Accessor for CheckOverlap Tool
  ICheckOverlap* checkOverlap() const;

  /// Overridden from Gaudi Algo to produce a warning if not called by user
  StatusCode setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  StatusCode sysInitialize ();  

  // Overridden from Gaudi Algorithm
  StatusCode sysFinalize ();  

  // Overridden from Gaudi Algorithm
  std::string getDecayDescriptor(){return m_decayDescriptor;};  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (std::string outputLocationString);  

  /// get algorithm ID
  int getAlgorithmID();
 
  /// Accessor for Algorithm2ID Tool
  IAlgorithm2ID* algorithmID() const;

protected:

private:

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools() ;

  /// Method to create SelResult container
  StatusCode fillSelResult() ;

  /// Concrete type Name of MassConstrained vertex to use (Property)
  std::string m_typeVertexFit;
  /// Concrete type of VertexFitter to use (Property)
  std::string m_typeLagFit;
  /// Decay description (Property)
  std::string m_decayDescriptor;
  /// Avoid writing SelResult object in TES (Property)
  bool m_avoidSelResult;
  /// Avoid printing SelResult statistics 
  /// (cannot be switched off by OutputLevel)
  bool m_printSelResult;
  
  /// Reference to desktop tool
  mutable IPhysDesktop* m_pDesktop;        
  /// Reference to Mass Constrained Vertex Fitter
  mutable IMassVertexFitter* m_pLagFit; 
  /// Reference to unconstrained Vertex Fitter
  mutable IVertexFitter* m_pVertexFit;    
  /// Reference to geometrical displacement Calculation.
  mutable IGeomDispCalculator* m_pGeomDispCalc;  
  /// Reference to ParticleStuffer.
  mutable IParticleStuffer* m_pStuffer;  
  /// Reference to ParticleFilter.
  mutable IParticleFilter* m_pFilter;
  /// Reference to ParticlePropertySvc
  mutable IParticlePropertySvc* m_ppSvc;
  /// Reference to CheckOverlap
  mutable ICheckOverlap* m_checkOverlap;
  /// Reference to Algorithm2ID
  mutable IAlgorithm2ID* m_algorithm2IDTool;

  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Count number of times selection filter is used 
  /// (= number of times alg is called)
  int m_countFilterWrite ;
  /// Number of passing events
  int m_countFilterPassed ;
  
  /// Variable needed to ensure that calling getTools by user 
  /// (which is now not needed anymore) will give only one
  /// message warning, not one per event. To be removed
  /// from code when calling by user not tolerated anymore
  bool m_loadToolsWarned;

  /// - For GaudiAlgorithm -
  /// Algorithm ID
  int m_algorithmID ;
};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------


#endif // DAVINCIKERNEL_DVALGORITHM_H
