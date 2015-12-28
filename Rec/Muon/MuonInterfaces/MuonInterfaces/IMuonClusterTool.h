// $Id: IMuonClusterTool.h,v 1.1.1.1 2010-02-09 12:58:35 asatta Exp $
#ifndef MUONTOOLS_IMUONCLUSTERTOOL_H 
#define MUONTOOLS_IMUONCLUSTERTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"


/** @class IMuonClusterTool IMuonClusterTool.h MuonTools/IMuonClusterTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2010-01-15
 */
class IMuonClusterTool : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID( IMuonClusterTool, 2, 0 );
  virtual StatusCode doCluster(const std::string& Input,
                               const std::string& Output) =0;

};
#endif // MUONTOOLS_IMUONCLUSTERTOOL_H
