// $Id: IMuonGeometryTool.h,v 1.2 2002-08-05 12:52:24 dhcroft Exp $
#ifndef MUONTOOLS_IMUONGEOMETRYTOOL_H 
#define MUONTOOLS_IMUONGEOMETRYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IMuonGeometryTool IMuonGeometryTool.h MuonTools/IMuonGeometryTool.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   29/04/2002
 */
static const InterfaceID IID_IMuonGeometryTool( "IMuonGeometryTool" , 2 , 0 );

class IMuonGeometryTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonGeometryTool;
  }

  /// return the number of stations
  virtual StatusCode nStation(int &NStation) = 0;
  /// return the number of stations
  virtual StatusCode nRegion(int &NRegion) = 0;

  /// Return the box for a station
  virtual StatusCode getStationBox(const int &station,
                                   double &deltaX,
                                   double &deltaY,
                                   double &z, double &deltaZ) = 0 ;

  /// Return the box for a region
  virtual StatusCode getRegionBox(const int &station,
                                  const int &region,
                                  double &outerDeltaX,
                                  double &innerDeltaX,
                                  double &outerDeltaY,
                                  double &innerDeltaY,
                                  double &z, double &deltaZ) = 0 ;

  /// Return pad size in a region
  virtual StatusCode getPadSize(const int &station, const int &region,
                                double &sizeX, double &sizeY) = 0;  

  /// return the number of horizonal logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanHorizGridX(const int &station, const int &region) = 0;
  /// return the number of horizonal logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanHorizGridY(const int &station, const int &region) = 0;

  /// return the number of vertical logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanVertGridX(const int &station, const int &region) = 0; 
  /// return the number of vertical logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanVertGridY(const int &station, const int &region) = 0;

  /// return the number of pads in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int padGridX(const int &station, const int &region) = 0;
  /// return the number of pads in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int padGridY(const int &station, const int &region) = 0;

protected:

private:

};
#endif // MUONTOOLS_IMUONGEOMETRYTOOL_H
