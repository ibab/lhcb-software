// $Id: MuonPosTool.h,v 1.1 2004-12-06 12:01:34 asatta Exp $
#ifndef MUONTILEIDXYZ_H 
#define MUONTILEIDXYZ_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// local
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/IMuonPosTool.h"
#include "MuonDet/MuonBasicGeometry.h"

/** @class MuonPosTool MuonPosTool.h
 *  
 *  Convert an MuonTileID into an xyz position in the detector (with size)
 *  No abstract interface as I do not want to make more than one of these...
 *
 *  @author David Hutchcroft
 *  @date   07/03/2002
 */
class MuonPosTool : public AlgTool, 
                      virtual public IMuonPosTool {
public:
  /// Standard constructor
  MuonPosTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonPosTool( ) ; ///< Destructor

  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   * this ignores gaps: these can never be read out independently
   */
  virtual StatusCode calcTilePos(const MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz);
  

  // Number of stations
  int m_stationNumber;
  // Number of regions
  int m_regionNumber;
  //Names of the station
  std::vector<std::string> m_stationNames;
  



  // size of pads
  std::vector<unsigned int> m_padGridX;
  std::vector<unsigned int> m_padGridY;



private:
  IDataProviderSvc* m_DDS;
  std::vector<float> m_xpos[5];
  std::vector<float> m_ypos[5];
  std::vector<float> m_zpos[5];
  IMuonTileXYZTool* m_tileTool ;
  
  bool m_debug;
};
#endif // MUONTILEIDXYZ_H
