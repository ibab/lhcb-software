// $Id: SelectJPsiMuMu.h,v 1.2 2002-03-28 18:56:51 gcorti Exp $
#ifndef SELECTJPSIMUMU_H 
#define SELECTJPSIMUMU_H 1

// Include files
// from STL
//#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IHistogram1D;
class IPhysDesktop;
class IMassVertexFitter;
class IGeomDispCalculator;
class IVertexFitter;
class IParticleFilter;
class IParticleStuffer;

/** @class SelectJPsiMuMu SelectJPsiMuMu.h
 *  
 *  Algorithm to select JPsi -> mu mu
 *  @author Sandra Amato
 *  @date   11/12/2001
 */
class SelectJPsiMuMu : public Algorithm {
public:
  /// Standard constructor
  SelectJPsiMuMu( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SelectJPsiMuMu( ); ///< Destructor
 
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 

private:

  int         m_nEvents;     ///< N events processed

  double		m_JPsiMassWin;   ///< Mass window for the two Mu Combination
  double		m_JPsiZWin;     ///< Z vertex window for the two Mu Combination
  double    m_chi2UncVtxCut;///< Maximum chi2 of Unconstrained Vtx fit
  
  double    m_ZDistanceCut; ///< Minimum Value of z JPsi Vtx - z prim Vtx
  
  double    m_ZVertexCut; ///< Maximum z of JPsi Vtx 
  
  double    m_JPsiMassCut; ///< JPsi Mass Window
  
  double    m_chi2ConVtxCut; ///<Maximum chi2 of mass constrained vertex fit
 
  bool			m_SelJPsi;
  
  int		m_JPsiCount;

  IPhysDesktop* m_pDesktop;        ///< Reference to desktop tool
  IMassVertexFitter* m_pLagFit; ///< Reference to Mass Constrained Vertex Fitter
  IVertexFitter* m_pVertexFit;    ///< Reference to unconstrained
  IGeomDispCalculator* m_pGeomDispCalc;  ///< Reference to geometrical calc. 
  IParticleFilter* m_pFilter;  ///< Reference to ParticleFilter. 
  IParticleStuffer* m_pStuffer;  ///< Reference to ParticleStuffer. 


  
  bool                  m_produceHistogram;    // flag for histo production

  // Histograms ( used if m_produceHistogram = 1 (true) )
  IHistogram1D*         m_hChi2UncFit;
  IHistogram1D*         m_hSum4p;
  IHistogram1D*         m_hZDistance;
  IHistogram1D*         m_hZVertex;
  IHistogram1D*         m_hJPsiMass;
  IHistogram1D*         m_hChi2ConFit;



  // Retrieve SdtHep ID for muons
  long   m_muPlusID;
  long   m_muMinusID;
  long   m_jpsiID;
  double m_jpsiMass;
  

};
#endif // SELECTJPSIMUMU_H
