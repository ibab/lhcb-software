// $Id: DeMuonDetector.h,v 1.5 2005-11-09 17:27:55 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef MUONDET_DEMUONDETECTOR_H
#define MUONDET_DEMUONDETECTOR_H 1

//Det Desc
#include "DetDesc/DetectorElement.h"

//Gaudi
#include "GaudiKernel/IDataProviderSvc.h"

//Muon Kernel
#include "MuonKernel/MuonTileID.h"

//Muon Detector
#include "MuonDet/MuonChamberLayout.h"
#include "MuonDet/MuonFrontEndID.h"

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


  StatusCode Hit2ChamberNumber(HepPoint3D myPoint, 
			       int station, 
			       int & chamberNumber, int& regNum);
  
  StatusCode Hit2GapNumber(HepPoint3D myPoint, 
			   int station, int & gapNumber,
			   int & chamberNumber, int& regNum);
  
  StatusCode Pos2ChamberNumber(const double x,
                               const double y,
                               const double z,
                               int & chamberNumber, int& regNum);

  StatusCode Pos2GapNumber(const double x,
			   const double y,
			   const double z, int & gapNumber,
			   int & chamberNumber, int& regNum);

  StatusCode Pos2StChamberNumber(const double x,
				 const double y,
				 int station ,
				 int & chamberNumber, int& regNum);

  StatusCode Pos2StGapNumber(const double x,
			     const double y,
			     int station , int & gapNumber,
			     int & chamberNumber, int& regNum);
  
  StatusCode Pos2ChamberTile(const double x,
                             const double y,
                             const double z,
                             MuonTileID& tile);

  StatusCode Pos2StChamberPointer(const double x,
                                const double y,
                                int station ,
                                DeMuonChamber* & chamberPointer);
  
  StatusCode Pos2ChamberPointer(const double x,
                                const double y,
                                const double z,
                                DeMuonChamber* & chamberPointer);

  //Returns the list of physical channels for a given chamber  
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > listOfPhysChannels(HepPoint3D my_entry, HepPoint3D my_exit, int region, int chamber);

  //Returns the station index starting from the z position
  int getStation(const double z);
  
  //Returns a detector element identified by chmb, reg, num
  IDetectorElement* ReturnADetElement(int lsta, int lreg, int lchm);
  
  // Return the chamber pointer from m_ChmbPtr
  DeMuonChamber* getChmbPtr(const int station, const int region,
			    const int chmb);

  //Fills the vector of chamber pointers  
  void fillChmbPtr();
  
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

  // sotto e' preso da muongeometrytool da cui si puo' prendere l 
  // inizializzazione

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

  //My vector of Chamber pointers
  std::vector<DeMuonChamber*> m_ChmbPtr;

  //Chamber Layout
  MuonChamberLayout m_chamberLayout;

  //Maximum number of chambers allowed per region
  int MaxRegions[4];

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline int DeMuonDetector::getStation(const double z)
{
  //station index starting from z position (in mm)
  int idX = 0; double s_size(400.);
  double s_off[5] = {12100,15200,16400,17600,18800};
  for(idX = 0; idX<5; idX++) {
    if(fabs(z-s_off[idX])<s_size) break;
  }
  return idX;
}

#endif    // MUONDET_DEMUONDETECTOR_H
