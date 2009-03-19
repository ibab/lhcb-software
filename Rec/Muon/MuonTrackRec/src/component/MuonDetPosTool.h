// $Id: MuonDetPosTool.h,v 1.1.1.1 2009-03-19 14:38:47 ggiacomo Exp $
#ifndef COMPONENT_MUONDETPOSTOOL_H 
#define COMPONENT_MUONDETPOSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"            // Interface
class DeMuonDetector;

/** @class MuonDetPosTool MuonDetPosTool.h component/MuonDetPosTool.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-03-17
 */
class MuonDetPosTool : public GaudiTool, virtual public IMuonPosTool {
public: 
  /// Standard constructor
  MuonDetPosTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~MuonDetPosTool( ); ///< Destructor
  // from GaudiTool
  virtual StatusCode initialize();    
  virtual StatusCode finalize  ();    
  
  virtual StatusCode calcTilePos(const LHCb::MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz);
  

protected:

private:
  DeMuonDetector* m_muonDetector;

};
#endif // COMPONENT_MUONDETPOSTOOL_H
