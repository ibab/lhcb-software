// $Id: DeMuonDetector.h,v 1.1 2005-10-25 06:54:28 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef MUONDET_DEMUONDETECTOR_H
#define MUONDET_DEMUONDETECTOR_H 1

//Det Desc
#include "DetDesc/DetectorElement.h"

#include "MuonKernel/MuonTileID.h"

//Gaudi
#include "GaudiKernel/IDataProviderSvc.h"

//Muon Detector
#include "MuonDet/MuonChamberLayout.h"

/** @class DeMuonDetector DeMuonDetector.h MuonDet/DeMuonDetector.h
 *  
 *  Detector element class for the muon system
 *
 *  @author Alessia Satta
 *  @author Alessio Sarti
 *  @date   18/10/2005
 */

static const CLID& CLID_DEMuonDetector = 11009;  

class DeMuonDetector: public DetectorElement {

public:
  /// Constructor, empty
  DeMuonDetector();

  /// Destructor
  virtual  ~DeMuonDetector();

  /// object identification
  const CLID& clID () const ;

  /// object identification
  static const CLID& classID () { return CLID_DEMuonDetector; }

  //Initialize of Detector Element
  virtual StatusCode initialize();
 
  StatusCode Pos2ChamberNumber(const double x,
                               const double y,
                               const double z,
                               int & chamberNumber, int& regNum);

  StatusCode Pos2ChamberTile(const double x,
                             const double y,
                             const double z,
                             MuonTileID& tile);
  
  //Returns the station index starting from the z position
  int getStation(const double z);

  /*

  int stations();
  int regions();
  int regions(int stations);
  
  StatusCode TileID2Pos(const MuonTileID tile,
                        double& x, double& deltax,
                        double& y, double& deltay,
                        double& z, double& deltaz);
  
  StatusCode TileIDinChamber2Pos(const MuonTileID tile,
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz);

  StatusCode Pos2ChamberPointer(const double x,
                                const double y,
                                const double z,
                                DeMuonChamber* & chamberPointer);
  
  StatusCode Pos2ChamberNumber(const double x,
                               const double y,
                               const int station ,
                               int & chamberNumber);
  
  StatusCode Pos2ChamberPointer(const double x,
                                const double y,
                                const int station ,
                                DeMuonChamber* & chamberPointer);

  StatusCode ChamberNumber2Pointer(const int chamberNumber ,
                                   DeMuonChamber* & chamberPointer);
  
  
 
  
  
  // sotto e' preso da muongeometrytool da cui si puo' prendere l 
  // inizializzazione

  /// Return pad size in a region
  StatusCode getPadSize(const int station, const int region,
                                double &sizeX, double &sizeY);
  /// return the number of horizonal logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int logChanHorizGridX(const int station, const int region);
  /// return the number of horizonal logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int logChanHorizGridY(const int station, const int region);
  /// return the number of vertical logical channels in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int logChanVertGridX(const int station, const int region); 
  /// return the number of vertical logical channels in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int logChanVertGridY(const int station, const int region);
  /// return the number of pads in X across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int padGridX(const int &station, const int &region);
  /// return the number of pads in Y across a 
  /// 1/4 of the region (use for MuonTileID grid)
  int padGridY(const int &station, const int &region);
  */
private:

  //My data provider
  IDataProviderSvc* m_detSvc;

  MuonChamberLayout m_chamberLayout;

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline int DeMuonDetector::getStation(const double z)
{
  //station index starting from z position (in mm)
  int idX = 0; double s_size(200.);
  double s_off[5] = {12100,15200,16400,17600,18800};
  for(idX = 0; idX<5; idX++) {
    if(fabs(z-s_off[idX])<s_size) break;
  }
  return idX;
}

#endif    // MUONDET_DEMUONDETECTOR_H
