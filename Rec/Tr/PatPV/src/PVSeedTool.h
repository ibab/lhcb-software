// $Id: PVSeedTool.h,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
#ifndef PATPV_PVSEEDTOOL_H 
#define PATPV_PVSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

static const InterfaceID IID_PVSeedTool ( "PVSeedTool", 1, 0 );


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
class PVSeedTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PVSeedTool; }

  /// Standard constructor
  PVSeedTool( const std::string& type, 
            const std::string& name,
            const IInterface* parent);

  virtual ~PVSeedTool( ); ///< Destructor

  void findClusters(std::vector<vtxCluster>& vclus, std::vector<double>& zclusters);
  void errorForPVSeedFinding(double tx, double ty, double &sigzaq);
  

protected:

private:
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
