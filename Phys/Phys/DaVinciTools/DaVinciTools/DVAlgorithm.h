#ifndef DAVINCITOOLS_DVALGORITHM_H
#define DAVINCITOOLS_DVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"

// from DaVinciTools
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IParticleFilter.h"

/** @class DVAlgorithm DVAlgorithm.h DaVinciTools/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 */
class DVAlgorithm : public Algorithm {
public:
  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm ( name , pSvcLocator )
      , m_pDesktop(0)
      , m_pLagFit(0)
      , m_pVertexFit(0)
      , m_pGeomDispCalc(0)
      , m_pStuffer(0)
      , m_pFilter(0)
      , m_ppSvc(0)
      , m_setFilterCalled(false)
      , m_toolsLoaded(false)
      , m_countFilterWrite(0)
      , m_countFilterPassed(0)
  {  
    
    declareProperty("VertexFitter", m_typeVertexFit="UnconstVertexFitter");
    declareProperty("MassVertexFitter", 
                    m_typeLagFit="LagrangeMassVertexFitter");
    declareProperty("DecayDescriptor", m_decayDescriptor="not specified");
    declareProperty("AvoidSelResult", m_avoidSelResult = false);
    declareProperty("PrintSelResult", m_printSelResult = true);

  };

  virtual ~DVAlgorithm( ){ }; ///< Destructor

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools();

  /// Method to release all tools
  StatusCode releaseTools();
  
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

  /// Overridden from Gaudi Algo to produce a warning if not called by user
  StatusCode setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  StatusCode sysInitialize ();  

  // Overridden from Gaudi Algorithm
  StatusCode sysFinalize ();  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (std::string outputLocationString);  

  /// Variable needed to ensure that calling getTools by user 
  /// (which is now not needed anymore) will give only one
  /// message warning, not one per event. To be removed
  /// from code when calling by user not tolerated anymore
  bool m_loadToolsWarned;
 
protected:

private:

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
  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Have Tools been already loaded?
  bool m_toolsLoaded;
  /// Count number of times selection filter is used 
  /// (= number of times alg is called)
  int m_countFilterWrite ;
  /// Number of passing events
  int m_countFilterPassed ;
  
  
};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------


#endif // DAVINCITOOLS_DVALGORITHM_H
