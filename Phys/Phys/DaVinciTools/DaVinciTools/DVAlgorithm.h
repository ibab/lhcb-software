#ifndef DAVINCITOOLS_DVALGORITHM_H
#define DAVINCITOOLS_DVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IHistogram1D;
class IPhysDesktop;
class IMassVertexFitter;
class IVertexFitter;
class IGeomDispCalculator;
class IParticleStuffer;
class IParticleFilter;

/** @class DVAlgorithm DVAlgorithm.h DaVinciTools/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 */
class DVAlgorithm : public Algorithm {
public:
  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization (DUMMY)
  virtual StatusCode execute   ();    ///< Algorithm execution (DUMMY)
  virtual StatusCode finalize  ();    ///< Algorithm finalization (DUMMY)
  
  /// Acessor for PhysDesktop Tool
  IPhysDesktop* desktop() const; 

  /// Acessor for Mass Constrained Vertex Fitter Tool
  IMassVertexFitter* massVertexFitter() const; 

  /// Acessor for Unconstrained Vertex Fitter Tool
  IVertexFitter* uncostVertexFitter() const;

  /// Acessor for Geometrical Displacement Calculation Tool
  IGeomDispCalculator* geomDispCalculator() const;

  /// Acessor for Particle Stuffer Tool
  IParticleStuffer* particleStuffer() const;

  /// Acessor for Particle Filter Tool
  IParticleFilter* particleFilter() const;
  
protected:

private:
  /// Reference to desktop tool
  mutable IPhysDesktop* m_pDesktop;        
  ///< Reference to Mass Constrained Vertex Fitter
  mutable IMassVertexFitter* m_pLagFit; 
  ///< Reference to unconstrained Vertex Fitter
  mutable IVertexFitter* m_pVertexFit;    
  ///< Reference to geometrical displacement Calculation.
  mutable IGeomDispCalculator* m_pGeomDispCalc;  
  ///< Reference to ParticleStuffer.
  mutable IParticleStuffer* m_pStuffer;  
  ///< Reference to ParticleFilter.
  mutable IParticleFilter* m_pFilter;  

  
};
#endif // DAVINCITOOLS_DVALGORITHM_H
