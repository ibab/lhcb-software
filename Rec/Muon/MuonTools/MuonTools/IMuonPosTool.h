// $Id: IMuonPosTool.h,v 1.2 2006-09-25 15:53:25 asatta Exp $
#ifndef MUONTOOLS_IMUONPOSTOOL_H 
#define MUONTOOLS_IMUONPOSTOOL_H 1

// Include files
#include "Kernel/MuonTileID.h"

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"

#include "MuonDet/DeMuonDetector.h"

//forward definitions

/** @class IMuonPosTool IMuonPosTool.h MuonTools/IMuonPosTool.h
 *  
 *  Interface for the tools to convert MuonTileID to coordinates 
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
static const InterfaceID IID_IMuonPosTool( "IMuonPosTool" , 4 , 0 );

class IMuonPosTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonPosTool; 
  }
  
  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   *  this ignores gaps: these can never be read out independently
   */
  virtual StatusCode calcTilePos(const LHCb::MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz) = 0;

protected:

private:

};
#endif // MUONTOOLS_IMUONTILEXYZTOOL_H
