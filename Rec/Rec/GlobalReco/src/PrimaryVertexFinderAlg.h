// $Id: PrimaryVertexFinderAlg.h,v 1.1 2002-07-11 16:24:20 gcorti Exp $
#ifndef PRIMARYVERTEXFINDERALG_H 
#define PRIMARYVERTEXFINDERALG_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

///< forward declarations
//class IHistogram1D;
//class IHistogram2D;
class IPrimaryVertexFitterTool;
//class IParticlePropertySvc;
//class L1Track;

/** @class PrimaryVertexFinderAlg PrimaryVertexFinderAlg.h
 *  
 *
 *  @author Krzysztof Ciba
 *  @date   03/04/2002
 */
class PrimaryVertexFinderAlg : public Algorithm {
public:
  /// Standard constructor
  PrimaryVertexFinderAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrimaryVertexFinderAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: ///< private members

  std::string m_inputContainer; ///< paths in TES
  std::string m_outputContainer;

  std::vector <MyTrack> m_tr;
  std::vector <MyVertex> m_vtx;

  IPrimaryVertexFitterTool* m_pvft;
   
  ////////////////////////////////////////////////////////////////////////
  double m_chi2; ///< chi2 (norm)
  double m_chi2min; ///< minimal value of chi squared (norm)
  double m_chi2cut; ///< how many chi2 is accepted? (norm)
  double m_zcut; ///< abs window in z (mm)

  /////////////////////////////////////////////////////////////////////////
  // all requested histograms here
  IHistogram1D* m_Zclose;        ///< Z closest to beam for all  tracks
  IHistogram1D* m_TrPerEv;       ///< Number of tracks found per event
  IHistogram2D* m_TrVsVertZ;     ///< Number of tracks vs vertex Z
  IHistogram1D* m_TrInVert;      ///< Number of tracks in the vertex fit
  IHistogram1D* m_PerOfTrInVert; ///< Percentage of tracks in the vertex fit
  IHistogram1D* m_VertErrX;      ///< Reconstructed vertex error in X
  IHistogram1D* m_VertErrY;      ///< Reconstructed vertex error in Y
  IHistogram1D* m_VertErrZ;      ///< Reconstructed vertex error in Z
  IHistogram1D* m_pullX; ///< pullX = (x_rec - x_mc) / errX ans so on...
  IHistogram1D* m_pullY;
  IHistogram1D* m_pullZ;
  IHistogram1D* m_RECchi2;
  IHistogram1D* m_DiffMC_REC_X; //< x_rec - x_mc and so on...
  IHistogram1D* m_DiffMC_REC_Y;
  IHistogram1D* m_DiffMC_REC_Z;
  IHistogram1D* m_multiMCVtx; ///< number of PV in MC data
  IHistogram1D* m_multiRECVtx; ///< number of PV REC
  IHistogram1D* m_RECVtxChiSq; ///< PV rec chi2 
  IHistogram1D* m_Zweight; ///<
  IHistogram2D* m_XYZRECvsChi2;
  

  ///////////////////////////////////////////////////////////////////////
  int m_NumberOfTracks; ///< number of tracks in VELO
  int m_NumberOfTracksUsed; ///< number of tracks used to fit vertex
  int m_minNumberOfTracks; ///< minimal tracks to find PV
  int m_maxIteration; ///< how many iterations allowed 
  //////////////////////////////////////////////////////////////////////

};
#endif // PRIMARYVERTEXFINDERALG_H
