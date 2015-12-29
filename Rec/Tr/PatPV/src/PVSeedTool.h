// $Id: PVSeedTool.h,v 1.2 2008-06-11 19:28:25 witekma Exp $
#ifndef PATPV_PVSEEDTOOL_H
#define PATPV_PVSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "Event/Track.h"


// auxiliary class for searching of clusters of tracks
class vtxCluster {

public:
  double  z;            // z of the cluster
  double  sigsq;        // sigma**2 of the cluster
  double  sigsqmin;     // minimum sigma**2 of the tracks forming cluster
  int     ntracks;      // number of tracks in the cluster
  int     not_merged;   // flag for iterative merging

  vtxCluster() {
    z           = 0.0;
    sigsq       = 0.0;
    sigsqmin    = 0.0;
    ntracks     = 0;
    not_merged  = 0;
  }

};

// auxiliary class for merging procedure of tracks/clusters
class pair_to_merge {

public:
  vtxCluster* first;  // pointer to first cluster to be merged
  vtxCluster* second; // pointer to second cluster to be merged
  double chi2dist;    // a chi2dist = zdistance**2/(sigma1**2+sigma2**2)

  pair_to_merge() {
    first    = 0;
    second   = 0;
    chi2dist = 10.e+10;
  }

};

/** @class PVSeedTool PVSeedTool.h tmp/PVSeedTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2005-11-19
 */
class PVSeedTool : public GaudiTool, virtual public IPVSeeding {
public:

  /// Standard constructor
  PVSeedTool( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~PVSeedTool( ); ///< Destructor

  void getSeeds(std::vector<const LHCb::Track*>& inputTracks,
		const Gaudi::XYZPoint& beamspot,
		std::vector<Gaudi::XYZPoint>& seeds);

protected:

private:

  void findClusters(std::vector<vtxCluster>& vclus, std::vector<double>& zclusters);
  void errorForPVSeedFinding(double tx, double ty, double &sigzaq);

  double zCloseBeam(const LHCb::Track* track, const Gaudi::XYZPoint& beamspot);
  void print_clusters(std::vector<vtxCluster*>& pvclus);

  std::vector<pair_to_merge*> m_pvecp2m;
  std::vector<pair_to_merge>  m_vecp2m;
  int m_p2mstatic;

  double m_maxChi2Merge;
  double m_factorToIncreaseErrors;
									
  int    m_minClusterMult;
  double m_dzCloseTracksInCluster;
  int    m_minCloseTracksInCluster;
  int    m_highMult;
  double m_ratioSig2HighMult;
  double m_ratioSig2LowMult;

  double  m_x0MS;         // X0 (tunable) of MS to add for extrapolation of
                          // track parameters to PV
  double  m_scatCons;     // calculated from m_x0MS

};
#endif // PATPV_PVSEEDTOOL_H
