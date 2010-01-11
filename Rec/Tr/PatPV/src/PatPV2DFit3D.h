// $Id: PatPV2DFit3D.h,v 1.5 2010-01-11 10:10:48 witekma Exp $
#ifndef PATPV2DFIT3D_H
#define PATPV2DFIT3D_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "IPVSeeding.h"

#include "Event/Track.h"
#include "Event/RecVertex.h"

#include "TrackInterfaces/IPVOfflineTool.h"

/** @class PatPV2DFit3D PatPV2DFit3D.h
 *  PV reconstruction in 2D
 *
 *  @author Olivier Callot
 *  @date   2005-06-22
 */
class PatPV2DFit3D : public GaudiHistoAlg {
public:
  /// Standard constructor
  PatPV2DFit3D( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatPV2DFit3D( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  void addTracks(int n);
  StatusCode updateVelo();

private:
  // steering parameters for PV reconstruction
  unsigned int  m_maxNumPv;     // maximum number of PV
  int     m_maxIter;      // maximum number of iterations of PV fit
  int     m_minMult;      // min multiplicity of any PV vertex
  double  m_dzSeparHisto; // min dz between any PV vertex
  bool   m_saveSeedsAsPV; // Save seeds as PVs (for monitoring)
  std::vector<LHCb::Track*> m_sTracks;

  std::vector<double> m_phiOfSector;
  Gaudi::XYZVector m_boxOffsetLeft[2];
  Gaudi::XYZVector m_boxOffsetRight[2];

  double m_zLeft[2];
  double m_zRight[2];
  int     m_multiQualityPV;
  int     m_minBackQualityPV;
  double  m_dzQualityPV;

  LHCb::Tracks*  m_inputTracks;    ///< Pointer to the SpaceVelo tracks
  LHCb::RecVertices* m_outputVertices; ///< Pointer to the 3D vertices

  IPVSeeding* m_pvSeedTool;

  std::string m_inputTracksName;
  std::string m_outputVerticesName;

  IPVOfflineTool* m_pvsfit;    // PV fitting tool
};
#endif // PATPV2DFIT3D_H
