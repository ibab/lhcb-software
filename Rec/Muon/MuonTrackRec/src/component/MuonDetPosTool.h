#ifndef COMPONENT_MUONDETPOSTOOL_H 
#define COMPONENT_MUONDETPOSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonFastPosTool.h"            // Interface
class DeMuonDetector;

/** @class MuonDetPosTool MuonDetPosTool.h component/MuonDetPosTool.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-03-17
 */
class MuonDetPosTool : public extends<GaudiTool, IMuonFastPosTool> {
public: 
  /// Standard constructor
  MuonDetPosTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  ~MuonDetPosTool( ) override; ///< Destructor
  StatusCode initialize() override;    
  StatusCode calcTilePos(const LHCb::MuonTileID& tile, 
                         double& x, double& deltax,
                         double& y, double& deltay,
                         double& z, double& deltaz) const override;
private:
  DeMuonDetector* m_muonDetector = nullptr;
};
#endif // COMPONENT_MUONDETPOSTOOL_H
