// $Id: IMuonGeometryTool.h,v 1.1 2002-05-10 12:47:05 dhcroft Exp $
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
static const InterfaceID IID_IMuonGeometryTool( "IMuonGeometryTool" , 1 , 0 );

class IMuonGeometryTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonGeometryTool;
  }

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

protected:

private:

};
#endif // MUONTOOLS_IMUONGEOMETRYTOOL_H
