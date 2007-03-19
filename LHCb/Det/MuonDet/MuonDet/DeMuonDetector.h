// $Id: DeMuonDetector.h,v 1.20 2007-03-19 08:53:51 cattanem Exp $
// ============================================================================
#ifndef MUONDET_DEMUONDETECTOR_H
#define MUONDET_DEMUONDETECTOR_H 1

//Det Desc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidBox.h"   // Not needed here but used by many algorithms

//Gaudi
#include "GaudiKernel/IDataProviderSvc.h"
#include "Kernel/Packer.h"

//Muon Detector
#include "MuonDet/MuonChamberLayout.h"
#include "MuonDet/MuonFrontEndID.h"

#include "MuonDet/PackMCMuonHit.h"  
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"


/** @class DeMuonDetector DeMuonDetector.h MuonDet/DeMuonDetector.h
 *  
 *  Detector element class for the muon system
 *
 *  @author Alessia Satta
 *  @author Alessio Sarti
 *  @date   18/10/2005
 */

#define partition 20
#define maxReadoutType 2
#define numberChamber 1380   
#define numberGap 4
#define nFE 2
#define nLogMap 2
static const CLID CLID_DEMuonDetector = 11009;  

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


  StatusCode Hit2ChamberNumber(Gaudi::XYZPoint myPoint, 
			       int station, 
			       int & chamberNumber, int& regNum) const;
  
  StatusCode Hit2GapNumber(Gaudi::XYZPoint myPoint, 
			   int station, int & gapNumber,
			   int & chamberNumber, int& regNum) const ;
  
  StatusCode Pos2ChamberNumber(const double x,
                               const double y,
                               const double z,
                               int & chamberNumber, int& regNum) const;

  StatusCode Pos2GapNumber(const double x,
			   const double y,
			   const double z, int & gapNumber,
			   int & chamberNumber, int& regNum) const;

  StatusCode Pos2StChamberNumber(const double x,
				 const double y,
				 int station ,
				 int & chamberNumber, int& regNum) const;

  StatusCode Pos2StGapNumber(const double x,
			     const double y,
			     int station , int & gapNumber,
			     int & chamberNumber, int& regNum) const;
  
  StatusCode Pos2ChamberTile(const double x,
                             const double y,
                             const double z,
                             LHCb::MuonTileID& tile) const;

  StatusCode Pos2StChamberPointer(const double x,
                                const double y,
                                int station ,
                                DeMuonChamber* & chamberPointer) const;
  
  StatusCode Pos2ChamberPointer(const double x,
                                const double y,
                                const double z,
                                DeMuonChamber* & chamberPointer) const;

  StatusCode Chamber2Tile(int  chaNum, int station, int region, 
                          LHCb::MuonTileID& tile) const ;


  const int sensitiveVolumeID(const Gaudi::XYZPoint &  myPoint) const;
  

  unsigned int gapID( int sensDetID);
  
  unsigned int chamberID( int sensDetID);
  
  unsigned int regionID( int sensDetID );
  
  unsigned int stationID( int sensDetID );
  
  unsigned int quadrantID( int sensDetID );
  

  //Returns the list of physical channels for a given chamber  
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > 
  listOfPhysChannels(Gaudi::XYZPoint my_entry, Gaudi::XYZPoint my_exit, 
                     int region, int chamber);
  StatusCode getPCCenter(MuonFrontEndID fe,int chamber,int station, 
                         int region,double& xcenter, 
                         double& ycenter, double & zcenter);
  //Returns the station index starting from the z position
  int getStation(const double z) const;
  
  //Returns a detector element identified by chmb, reg, num
  IDetectorElement* ReturnADetElement(int lsta, int lreg, int lchm);
  
  // Return the chamber pointer from m_ChmbPtr
  DeMuonChamber* getChmbPtr(const int station, const int region,
			    const int chmb) const;

  //Fills the vector of chamber pointers  
  void fillChmbPtr();

  //Fills various geometry related info
  void fillGeoInfo();
  void fillGeoArray();
  StatusCode Tile2XYZ(LHCb::MuonTileID tile, 
		      double & x, double & dx,
		      double & y, double & dy, 
		      double & z, double & dz);  
  void CountDetEls();
 
  int stations();
  int regions();
  int regions(int stations);
  float getStationZ(const int station);

  
  int gapsInRegion(const int station, const int region){
    return m_gapPerRegion[station*4+region];    
  };
  int gapsPerFE(const int station, const int region){
    return m_gapPerFE[station*4+region];
  };
  int readoutInRegion(const int station, const int region){
    return m_readoutNumber[station*4+region];
  };
  int mapInRegion(const int station, const int region){
    return m_LogMapPerRegion[station*4+region];
  };
  double areaChamber(const int station, const int region){
    return m_areaChamber[station*4+region];
  };
  unsigned int getPhChannelNX(const int readout, const int station, 
                              const int region){
    return  m_phChannelNX[readout][station*4+region];
  };
  unsigned int getPhChannelNY(const int readout,const int station, 
                              const int region){
    return  m_phChannelNY[readout][station*4+region];
  };
  float getPadSizeX(const int station, 
                              const int region){
    return m_padSizeX[station*4+region];
  };

  float getPadSizeY(const int station, 
                              const int region){
    return m_padSizeY[station*4+region];
  };
  unsigned int getReadoutType(const int ireadout,const int station, 
                              const int region){    
    return  m_readoutType[ireadout][station*4+region];
  };
  unsigned int chamberInRegion(const int station, const int region){
    return  m_chamberPerRegion[station*4+region];
  };
  unsigned int getLogMapInRegion(const int station, const int region){
    return  m_LogMapPerRegion[station*4+region];
  };
  unsigned int getLogMapRType(const int ireadout, const int station, 
                              const int region){
    return m_LogMapRType[ireadout][station*4+region];
  };
  unsigned int getLogMapMergex( const int ireadout,const int station, 
                                const int region){
    return m_LogMapMergex[ireadout][station*4+region];
  };
  unsigned int getLogMapMergey( const int ireadout,const int station, 
                                const int region){ 
    return m_LogMapMergey[ireadout][station*4+region];
  };
  unsigned int getLayoutX( const int ireadout,const int station, 
                           const int region){
    return m_layoutX[ireadout][station*4+region];
  };
  unsigned int getLayoutY( const int ireadout,const int station, 
                           const int region){  
    return m_layoutY[ireadout][station*4+region];
  };
  float getSensAreaX( const int station, 
                           const int region){  
    return m_sensitiveAreaX[station*4+region];
  };
  float getSensAreaY( const int station, 
                           const int region){  
    return m_sensitiveAreaY[station*4+region];
  };
  float getSensAreaZ( const int station, 
                           const int region){  
    return m_sensitiveAreaZ[station*4+region];
  };

  float getInnerX(const int station){
    return m_stationBox[station][0];
  };
  float getInnerY(const int station){
    return m_stationBox[station][1];
  };
  float getOuterX(const int station){
    return m_stationBox[station][2];
  };
  float getOuterY(const int station){
    return m_stationBox[station][3];
  };

  unsigned int getCardiacORX(const int par, const unsigned int readoutType){
    return m_phCardiacORNX[readoutType][par];
  };
  unsigned int getCardiacORY(const int par, const unsigned int readoutType){
    return m_phCardiacORNY[readoutType][par];
  };
// unsigned int getReadoutType(int station, int region,MuonLayout lay);  
// MuonODEBoard* getODEContainingTile(LHCb::MuonTileID digit);  
// MuonL1Board* getL1ContainingTile(LHCb::MuonTileID digit);

private:

  //My data provider
  IDataProviderSvc* m_detSvc;

  //My vector of Chamber pointers
  std::vector<DeMuonChamber*> m_ChmbPtr;

  //Chamber Layout
  MuonChamberLayout *m_chamberLayout;

  //Maximum number of chambers allowed per region
  int MaxRegions[4];

  //How many stations and regions
  int m_stations;
  int m_regions;
  int m_regsperSta[5];

  //geometry info
  int m_readoutNumber[partition];
  unsigned int m_chamberPerRegion[partition];
  unsigned int m_gapPerRegion[partition];
  unsigned int m_LogMapPerRegion[partition]; 
  unsigned int m_readoutType[maxReadoutType][partition];
  unsigned int m_LogMapRType[maxReadoutType][partition];
  unsigned int m_LogMapMergex[maxReadoutType][partition];
  unsigned int m_LogMapMergey[maxReadoutType][partition];
  unsigned int m_phChannelNX[maxReadoutType][partition];
  unsigned int m_phChannelNY[maxReadoutType][partition];
  unsigned int m_phCardiacORNX[maxReadoutType][partition];
  unsigned int m_phCardiacORNY[maxReadoutType][partition];

  float m_padSizeX[partition];
  float m_padSizeY[partition];
  int m_gapPerFE[partition];
  int m_layoutX[maxReadoutType][partition];
  int m_layoutY[maxReadoutType][partition];
  double  m_areaChamber[partition]; 
  float m_sensitiveAreaX[partition];
  float m_sensitiveAreaY[partition];
  float m_sensitiveAreaZ[partition];  
  double m_stationBox[5][4];
  double m_stationZ[5];
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline int DeMuonDetector::getStation(const double z) const
{
  //station index starting from z position (in mm)
  int idX = 0; double s_size(400.);
  double s_off[5] = {12100,15200,16400,17600,18800};
  for(idX = 0; idX<5; idX++) {
    if(fabs(z-s_off[idX])<s_size) break;
  }
  return idX;
}

inline int DeMuonDetector::stations()
{
  //Number of stations 
  return m_stations;
}

inline int DeMuonDetector::regions() 
{
  //Number of all regions
  return m_regions;
}

inline float DeMuonDetector::getStationZ(const int station){
    return m_stationZ[station];
  };


inline int DeMuonDetector::regions(int stations)
{
  //Number of regions in each station
  return m_regsperSta[stations];
}

inline unsigned int DeMuonDetector::gapID(int sensDetID)
{
  
  return LHCb::Packer::getBit(sensDetID,
                        PackMCMuonHit::maskGapID,PackMCMuonHit::shiftGapID);
  
}


inline unsigned int DeMuonDetector::chamberID(int sensDetID)
{
  
  return LHCb::Packer::getBit(sensDetID,
                        PackMCMuonHit::maskChamberID,
                        PackMCMuonHit::shiftChamberID);
  
}


inline unsigned int DeMuonDetector::regionID(int sensDetID)
{
  
  return LHCb::Packer::getBit(sensDetID,
                                    PackMCMuonHit::maskRegionID,
                                    PackMCMuonHit::shiftRegionID);
  
}


inline unsigned int DeMuonDetector::stationID(int sensDetID)
{
  
  return LHCb::Packer::getBit(sensDetID,
                                    PackMCMuonHit::maskStationID,
                                    PackMCMuonHit::shiftStationID);
  
}


inline unsigned int DeMuonDetector::quadrantID(int sensDetID)
{
  
  return LHCb::Packer::getBit(sensDetID,
                                    PackMCMuonHit::maskQuadrantID,
                                    PackMCMuonHit::shiftQuadrantID);
  
}




#endif    // MUONDET_DEMUONDETECTOR_H
