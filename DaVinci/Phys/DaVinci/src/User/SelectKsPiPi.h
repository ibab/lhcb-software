// $Id: SelectKsPiPi.h,v 1.2 2002-03-28 18:57:15 gcorti Exp $
#ifndef SELECTKSPIPI_H 
#define SELECTKSPIPI_H 1

// Include files
// from STL
//#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IHistogram1D;
class IPhysDesktop;
class IMassVertexFitter;
class IVertexFitter;
class IGeomDispCalculator;
class IParticleFilter;
class IParticleStuffer;

/** @class SelectKsPiPi SelectKsPiPi.h
 *  
 *
 *  @Claudia Nunes
 *  @date   11/12/2001
 */
class SelectKsPiPi : public Algorithm {
public:
  /// Standard constructor
  SelectKsPiPi( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SelectKsPiPi( ); ///< Destructor
 
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 

private:

  int         m_nEvents;     ///< N events processed

  double		m_ksmasswinloo;         // Mass window for the two Mu Combination
  double		m_kszwin;         // Z vertex window for the two Mu Combination
  double                m_chi2UncVtxCut;
  
  double                m_ZDistanceCut;
  
  double                m_ZVertexCut;
  
  double                m_ksmasswinti;
  
  double                m_chi2ConVtxCut;
  
  double                m_ptCut;
  double                m_impParamCut;

  bool			m_SelKs;
  
  int   		m_KsCount;
  double                m_ksMass; 
  long        m_ksID;
  
  
  	
  IPhysDesktop* m_pDesktop;        ///< Reference to desktop tool
  IMassVertexFitter* m_pLagFit; ///< Reference to Mass Constrained Vertex Fitter
  IVertexFitter* m_pVertexFit;    ///< Reference to unconstrained
  IGeomDispCalculator* m_pGeomDispCalc;  ///< Reference to geometrical calc. 
  IParticleFilter* m_pFilter;  ///< Reference to ParticleFilter. 
  IParticleStuffer* m_pStuffer;  ///< Reference to ParticleStuffer. 

  
  bool                  m_produceHistogram;    // flag for histo production
  double                m_Sum4pCut;             // Mass cut

  // Histograms ( used if m_produceHistogram = 1 (true) )
  IHistogram1D*         m_hChi2UncFit;
  IHistogram1D*         m_hSum4p;
  IHistogram1D*         m_hZDistance;
  IHistogram1D*         m_hZVertex;
  IHistogram1D*         m_hKsMass;
  IHistogram1D*         m_hChi2ConFit;
  IHistogram1D*         m_hzorg;
  IHistogram1D*         m_hztrans;
  IHistogram1D*         m_hImpParam;
  IHistogram1D*         m_hPt;



  // Retrieve SdtHep ID for pions
  long   m_PiPlusID;
  long   m_PiMinusID;
  double m_KsMass;
  

};
#endif // SELECTKSPIPI_H






