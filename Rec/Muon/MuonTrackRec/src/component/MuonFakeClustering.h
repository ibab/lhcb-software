// $Id: MuonFakeClustering.h,v 1.2 2010-02-10 19:20:17 ggiacomo Exp $
#ifndef COMPONENT_MUONFAKECLUSTERING_H
#define COMPONENT_MUONFAKECLUSTERING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonInterfaces/IMuonClusterRec.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"

/** @class MuonFakeClustering MuonFakeClustering.h component/MuonFakeClustering.h
 *
 *produce fake clusters by just copying the list of MuonLogPads to MuonHits
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class MuonFakeClustering : public extends<GaudiTool, IMuonClusterRec, IIncidentListener> {
public:
  /// Standard constructor
  MuonFakeClustering( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MuonFakeClustering( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  virtual void handle ( const Incident& incident );
  virtual const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                                bool force=false);


private:
  void clear();
  DeMuonDetector* m_muonDetector;
  IMuonFastPosTool* m_posTool;

  std::string m_posToolName;

  std::vector<MuonHit*> m_clusters;
  bool m_clustersDone;
};
#endif // COMPONENT_MUONFAKECLUSTERING_H
