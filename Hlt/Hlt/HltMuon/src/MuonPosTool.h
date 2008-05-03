// $Id: MuonPosTool.h,v 1.3 2008-05-03 15:26:06 graven Exp $
#ifndef MUONTILEIDXYZ_H 
#define MUONTILEIDXYZ_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"

// local
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/IMuonPosTool.h"

/** @class MuonPosTool MuonPosTool.h
 *  
 *  Convert an MuonTileID into an xyz position in the detector (with size)
 *  No abstract interface as I do not want to make more than one of these...
 *
 *  @author David Hutchcroft
 *  @date   07/03/2002
 */
class MuonPosTool : public GaudiTool, 
                    virtual public IMuonPosTool {
public:
  /// Standard constructor
  MuonPosTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonPosTool( ){} ; ///< Destructor

  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   * this ignores gaps: these can never be read out independently
   */
  virtual StatusCode calcTilePos(const LHCb::MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz);
  
  StatusCode initialize();
  
private:

  // Number of stations
  int m_stationNumber;
  // Number of regions
  int m_regionNumber;
  //Names of the station
  std::vector<std::string> m_stationNames;

  // size of pads
  std::vector<unsigned int> m_padGridX;
  std::vector<unsigned int> m_padGridY;
  std::vector<float> m_padSizeX;
  std::vector<float> m_padSizeY;
//  IDataProviderSvc* m_DDS;
  std::vector<Gaudi::XYZPoint> m_pos[5];
  DeMuonDetector* m_muonDetector;  
  bool m_debug;
};
#endif // MUONTILEIDXYZ_H
