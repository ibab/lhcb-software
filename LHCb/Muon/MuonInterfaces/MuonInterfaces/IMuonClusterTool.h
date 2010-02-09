// $Id: IMuonClusterTool.h,v 1.1.1.1 2010-02-09 12:58:35 asatta Exp $
#ifndef MUONTOOLS_IMUONCLUSTERTOOL_H 
#define MUONTOOLS_IMUONCLUSTERTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IMuonClusterTool ( "IMuonClusterTool", 1, 0 );

/** @class IMuonClusterTool IMuonClusterTool.h MuonTools/IMuonClusterTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2010-01-15
 */
class IMuonClusterTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonClusterTool; }
  virtual StatusCode doCluster(const std::string Input,const std::string 
Output)=0;


protected:

private:

};
#endif // MUONTOOLS_IMUONCLUSTERTOOL_H
