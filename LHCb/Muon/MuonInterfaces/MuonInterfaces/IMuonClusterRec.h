// $Id: IMuonClusterRec.h,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#ifndef MUONTRACKREC_IMUONCLUSTERREC_H 
#define MUONTRACKREC_IMUONCLUSTERREC_H 1

// Include files
// from STL
#include <string>
#include <vector>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class MuonHit;
class MuonLogPad;
class DeMuonDetector;
class IMuonFastPosTool;

static const InterfaceID IID_IMuonClusterRec ( "IMuonClusterRec", 1, 0 );

/** @class IMuonClusterRec IMuonClusterRec.h MuonTrackRec/IMuonClusterRec.h
 *  
 *  Interface to clustering algorithm for standalone muon reconstruction
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class IMuonClusterRec : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonClusterRec; }

  virtual const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                                bool force=false) =0;

protected:  
  DeMuonDetector* m_muonDetector;
  IMuonFastPosTool* m_posTool;

  std::string m_posToolName;
  
  std::vector<MuonHit*> m_clusters;
  bool m_clustersDone;
  

};
#endif // MUONTRACKREC_IMUONCLUSTERREC_H
