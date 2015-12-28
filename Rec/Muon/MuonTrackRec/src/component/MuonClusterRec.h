#ifndef COMPONENT_MUONCLUSTERREC_H
#define COMPONENT_MUONCLUSTERREC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonInterfaces/IMuonClusterRec.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/MuonTileID.h"

/** @class MuonClusterRec MuonClusterRec.h component/MuonClusterRec.h
 *
 *  clustering tool for the muon detector
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class MuonClusterRec : public extends<GaudiTool, IMuonClusterRec, IIncidentListener> {
public:
  /// Standard constructor
  MuonClusterRec( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

  ~MuonClusterRec( ) override; ///< Destructor
  StatusCode initialize() override;
  StatusCode finalize  () override;
  void handle ( const Incident& incident ) override;
  const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                        bool force=false) override;

private:
  unsigned int m_maxPadsPerStation;

  DeMuonDetector* m_muonDetector = nullptr;
  IMuonFastPosTool* m_posTool = nullptr;

  std::string m_posToolName;

  std::vector<MuonHit*> m_clusters;
  bool m_clustersDone = false;

  void clear();

};
#endif // COMPONENT_MUONCLUSTERREC_H
