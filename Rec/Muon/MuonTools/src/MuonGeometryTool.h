// $Id: MuonGeometryTool.h,v 1.2 2002-08-05 12:53:04 dhcroft Exp $
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

  /// return the number of stations
  virtual StatusCode nStation(int &NStation);
  /// return the number of stations
  virtual StatusCode nRegion(int &NRegion);

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

  /// return the number of horizonal logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanHorizGridX(const int &station, const int &region);
  /// return the number of horizonal logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanHorizGridY(const int &station, const int &region);

  /// return the number of vertical logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanVertGridX(const int &station, const int &region); 
  /// return the number of vertical logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int logChanVertGridY(const int &station, const int &region);

  /// return the number of pads in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int padGridX(const int &station, const int &region);
  /// return the number of pads in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  virtual int padGridY(const int &station, const int &region);

protected:

private:

  /// Fill the local arrays
  StatusCode fillLocals();
  
  /// fill system parameters
  StatusCode fillSystemNumbers();

  /// Number of stations
  int m_NStation;
  /// Number of regions
  int m_NRegion;

  /// Horizontal Logical channel number across 1/4 of a region in x
  std::vector<int> m_logHorizGridX;
  /// Horizontal Logical channel number across 1/4 of a region in y
  std::vector<int> m_logHorizGridY;

  /// Vertical Logical channel number across 1/4 of a region in x
  std::vector<int> m_logVertGridX;
  /// Vertical Logical channel number across 1/4 of a region in y
  std::vector<int> m_logVertGridY;

  /// Pad number across 1/4 of a region in x
  std::vector<int> m_padGridX;
  /// Pad number across 1/4 of a region in y
  std::vector<int> m_padGridY;

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

  std::vector<regionExtent_> m_regionExtent;
  inline regionExtent_ & regionExtent(int station, int region){
    return m_regionExtent[station*m_NRegion + region];
  }
  std::vector<padExtent_> m_padExtent;
  inline padExtent_ & padExtent(int station, int region){
    return m_padExtent[station*m_NRegion + region];
  }

private:
  IMuonTileXYZTool *m_tileTool;
  IDataProviderSvc* m_DDS;
};
#endif // MUONGEOMETRYTOOL_H
