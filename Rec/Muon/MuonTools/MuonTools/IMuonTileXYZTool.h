// $Id: IMuonTileXYZTool.h,v 1.1.1.1 2002-03-15 15:58:17 dhcroft Exp $
#ifndef MUONTOOLS_IMUONTILEXYZTOOL_H 
#define MUONTOOLS_IMUONTILEXYZTOOL_H 1

// Include files
#include "MuonKernel/MuonTileID.h"

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
/** @class IMuonTileXYZTool IMuonTileXYZTool.h MuonTools/IMuonTileXYZTool.h
 *  
 *  Interface for the tools to convert MuonTileID to coordinates 
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
static const InterfaceID IID_IMuonTileXYZTool( "IMuonTileXYZTool" , 1 , 0 );

class IMuonTileXYZTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonTileXYZTool; 
  }
  
  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   *  this ignores gaps: these can never be read out independently
   */
  virtual StatusCode calcTilePos(const MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz) = 0;

  /// Calculate the MuonTileID of the chamber containing x,y,z (in mm)
  virtual StatusCode calcChamberTile(const double& x, 
                                     const double& y, 
                                     const double& z, 
                                     MuonTileID& tile) = 0;
  
  /** Calculate the MuonTileID of the muon pad: gives correct layer
   *  containing x,y,z (in mm)
   */
  virtual StatusCode calcPadTile(const double& x, 
                                 const double& y, 
                                 const double& z,
                                 const MuonLayout& padLayout,
                                 MuonTileID& tile) = 0;
protected:

private:

};
#endif // MUONTOOLS_IMUONTILEXYZTOOL_H
