// $Id: MuonGeometryTool.h,v 1.1 2002-05-10 12:47:09 dhcroft Exp $
#ifndef MUONGEOMETRYTOOL_H 
#define MUONGEOMETRYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// MuonTool interfaces
#include "MuonTools/IMuonGeometryTool.h"

// forward reference to the base tool
class IMuonTileXYZTool;


/** @class MuonGeometryTool MuonGeometryTool.h
 *  
 *  Get basic muon system geometry parameters
 *
 *  @author David Hutchcroft
 *  @date   29/04/2002
 */
class MuonGeometryTool : public AlgTool,
                         public virtual IMuonGeometryTool {
public:
  /// Standard constructor
  MuonGeometryTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~MuonGeometryTool() {}; ///< Destructor

  /// Return the box for a station
  virtual StatusCode getStationBox(const int &station,
                                   double &deltaX,
                                   double &deltaY,
                                   double &z,double &deltaZ);

  /// Return the box for a region
  virtual StatusCode getRegionBox(const int &station,
                                  const int &region,
                                  double &outerDeltaX,
                                  double &innerDeltaX,
                                  double &outerDeltaY,
                                  double &innerDeltaY,
                                  double &z, double &deltaZ);

  /// Return pad size in a region
  virtual StatusCode getPadSize(const int &station, const int &region,
                                double &sizeX, double &sizeY);  


protected:

private:

  /// Fill the local arrays
  StatusCode fillLocals();

  class regionExtent_ {
  public:
    double outerDeltaX;
    double innerDeltaX;
    double outerDeltaY;
    double innerDeltaY;
    double z;
    double deltaZ;
  };

  class padExtent_ {
  public:
    double sizeX;
    double sizeY;
  };

  regionExtent_ regionExtent[5][4];
  padExtent_ padExtent[5][4];

private:
  IMuonTileXYZTool *m_tileTool;
};
#endif // MUONGEOMETRYTOOL_H
