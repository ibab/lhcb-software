// $Id: IMuonFastPosTool.h,v 1.1 2009-04-29 14:11:20 asatta Exp $
#ifndef MUONDET_IMUONFASTPOSTOOL_H 
#define MUONDET_IMUONFASTPOSTOOL_H 1

// Include files
#include "Kernel/MuonTileID.h"

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"

/** @class IMuonPosTool IMuonPosTool.h MuonTools/IMuonPosTool.h
 *  
 *  Interface for the tools to convert MuonTileID to coordinates 
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
static const InterfaceID IID_IMuonFastPosTool( "IMuonFastPosTool" , 4 , 0 
);

class IMuonFastPosTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonFastPosTool; 
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
#endif // MUONDET_IMUONFASTPOSTOOL_H
