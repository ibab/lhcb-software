// $Id: MuonTileIDXYZ.h,v 1.1.1.1 2002-03-15 15:58:17 dhcroft Exp $
#ifndef MUONTILEIDXYZ_H 
#define MUONTILEIDXYZ_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from MuonKernel
#include "MuonKernel/MuonTileID.h"

// local
#include "MuonTools/IMuonTileXYZTool.h"

/** @class MuonTileIDXYZ MuonTileIDXYZ.h
 *  
 *  Convert an MuonTileID into an xyz position in the detector (with size)
 *  No abstract interface as I do not want to make more than one of these...
 *
 *  @author David Hutchcroft
 *  @date   07/03/2002
 */
class MuonTileIDXYZ : public AlgTool, 
                      virtual public IMuonTileXYZTool {
public:
  /// Standard constructor
  MuonTileIDXYZ( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonTileIDXYZ( ) ; ///< Destructor

  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   * this ignores gaps: these can never be read out independently
   */
  StatusCode calcTilePos(const MuonTileID& tile, 
                         double& x, double& deltax,
                         double& y, double& deltay,
                         double& z, double& deltaz);

  /// Calculate the MuonTileID of the chamber containing x,y,z (in mm)
  StatusCode calcChamberTile(const double& x, 
                             const double& y, 
                             const double& z, 
                             MuonTileID& tile);

  /** Calculate the MuonTileID of the muon pad
   *  layer returned is 0 if 0th layer or not in a specific gap
   *  otherwise it is the gap number
   *  containing x,y,z (in mm)
   *  @param   x           x of pad (mm) 
   *  @param   y           y of pad (mm) 
   *  @param   z           z of pad (mm) 
   *  @param   padLayout   layout of pad (if matches readout 2
   *  @param   x   x of pad (mm) 
   */
  StatusCode calcPadTile(const double& x, 
                         const double& y, 
                         const double& z,
                         const MuonLayout& padLayout,
                         MuonTileID& tile);
private:

  /// get position of chamber from a tile
  StatusCode getXYZChamberTile(const MuonTileID& tile, 
                               double& x, double& deltax,
                               double& y, double& deltay,
                               double& z, double& deltaz);

  /// get position of a "named" chamber 
  StatusCode getXYZChamber(const int& station,
                           const int& region,
                           const int& chamberNum,
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);

  // fill the array with the position and size of the stations
  StatusCode fillStationExtent();

  /// get the chamber number (counts from 1) for the tile
  int getChamberNumber(const MuonTileID& tile);

  /// get postion of logical channel (may be multiple chambers)
  StatusCode getXYZLogical(const MuonTileID& tile, 
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);
  
  /// get xyz of specific pad
  StatusCode getXYZPad(const MuonTileID& tile, 
                       double& x, double& deltax,
                       double& y, double& deltay,
                       double& z, double& deltaz);

  /// locate the chamber tile given the station, region and quater  
  StatusCode locateChamberFromXYZ( const int& station,
                                   const int& region,
                                   const int& quarter,
                                   const double& x,
                                   const double& y,
                                   const double& z,
                                   MuonTileID& tile);


  // store x,y,z,dx,dy,dz of chambers locally (Cached) to avoid multiple
  // TDS lookups (the other choice is to cache the smartpointers)
  class muonExtent_ {
  public:
    double x;
    double y;
    double z;
    double dx;
    double dy;
    double dz;
  } ;
  // only split by region to save a little space 
  //(about 2000 copies of muonExtent)
  muonExtent_ chamR1[5][12];
  muonExtent_ chamR2[5][24];
  muonExtent_ chamR3[5][48];
  muonExtent_ chamR4[5][192];

  muonExtent_ stationExtent[5];

  // store chamber layout locally
  MuonSystemLayout * m_chamberSystemLayout;

private:
  IDataProviderSvc* m_DDS;

};
#endif // MUONTILEIDXYZ_H
