#ifndef MUONTRACKREC_IMUONCLUSTERREC_H 
#define MUONTRACKREC_IMUONCLUSTERREC_H 1

// Include files
// from STL
#include <vector>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class MuonHit;
class MuonLogPad;
class DeMuonDetector;
class IMuonFastPosTool;

/** @class IMuonClusterRec IMuonClusterRec.h MuonTrackRec/IMuonClusterRec.h
 *  
 *  Interface to clustering algorithm for standalone muon reconstruction
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class IMuonClusterRec : public extend_interfaces<IAlgTool> {
public: 

  DeclareInterfaceID( IMuonClusterRec, 2, 0);

  virtual const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                                bool force=false) =0;


};
#endif // MUONTRACKREC_IMUONCLUSTERREC_H
