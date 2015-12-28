#ifndef MUONCLUSTERTOOL_H 
#define MUONCLUSTERTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "Event/MuonCoord.h"
#include "Event/MuonCluster.h"
#include "MuonInterfaces/IMuonClusterTool.h"            // Interface


/** @class MuonClusterTool MuonClusterTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2010-01-15
 */
class MuonClusterTool : public extends<GaudiTool, IMuonClusterTool> {
public: 
  /// Standard constructor
  MuonClusterTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  ~MuonClusterTool( ) override; ///< Destructor
  StatusCode initialize() override;	
  StatusCode doCluster(const std::string& Input,const std::string& Output) override;

private:
  StatusCode initClusterVector(LHCb::MuonCoords* coords);
  StatusCode mergeCluster();
  StatusCode mergeStation(int i);
  void ClearMemory();
  StatusCode SaveOutput(const std::string& output);
  
  bool detectCluster(LHCb::MuonCluster* one,LHCb::MuonCluster* two);
  bool isIncluded(int station,LHCb::MuonCluster* cluster)	;  
  std::vector<std::pair<LHCb::MuonCluster *,int> > m_inputclust[5];
  std::vector<LHCb::MuonCluster * > m_finalclust[5];
  DeMuonDetector* m_muonDetector;  
};
#endif // MUONCLUSTERTOOL_H
