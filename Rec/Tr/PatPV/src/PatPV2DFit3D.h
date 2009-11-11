// $Id: PatPV2DFit3D.h,v 1.1 2009-11-11 15:11:49 witekma Exp $
#ifndef PATPV2DFIT3D_H
#define PATPV2DFIT3D_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "IPVSeeding.h"

#include "Event/Track.h"
#include "Event/RecVertex.h"

#include "TrackInterfaces/IPVOfflineTool.h"

typedef struct {
  // local representation of 2D tracks parameters close to PV (x=y=0)
  // and some variables used internally during PV reconstruction
  LHCb::Track *pTr2d; // pointer to Track2D
  int    pvidx;      // index of PV the track belongs to
  double pvweight;   // track weight used during PV fit
  double chi2;       // chi2 contribution to vertex
  bool   backward;
  int    sector;
  double z0;         // z at r = 0
  double tr;         // slope
  double d2r0;       // error**2 for r0
  double d2tr;       // error**2 for slope
  double cosPhi;
  double sinPhi;
} myTrack;

typedef struct {
  // local representation of reconstructed vertex
  double x;
  double y;
  double z;
  double sigmax;
  double sigmay;
  double sigmaz;
  int    multi;
  double chi2;
} myVertex;

/** @class PatPV2DFit3D PatPV2DFit3D.h
 *  PV reconstruction in 2D
 *
 *  @author Olivier Callot
 *  @date   2005-06-22
 */
class PatPV2DFit3D : public GaudiAlgorithm {
public:
  /// Standard constructor
  PatPV2DFit3D( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatPV2DFit3D( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void storeDummyVertices(std::vector<double> &zseeds);
  void errorForPVSeedFinding(double tx, double ty, double &sigzaq);

private:
  // steering parameters for PV reconstruction
  unsigned int  m_maxNumPv;     // maximum number of PV
  int     m_maxIter;      // maximum number of iterations of PV fit
  int     m_minMult;      // min multiplicity of any PV vertex
  double  m_dzSeparHisto; // min dz between any PV vertex
  double  m_x0MS;         // X0 (tunable) of MS to add for extrapolation of
                          // track parameters to PV
  double  m_scatCons;     // calculated from m_x0MS
  bool   m_saveSeedsAsPV; // Save seeds as PVs (for monitoring)

  std::vector<double> m_phiOfSector;
  Gaudi::XYZVector m_boxOffsetLeft;
  Gaudi::XYZVector m_boxOffsetRight;
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
