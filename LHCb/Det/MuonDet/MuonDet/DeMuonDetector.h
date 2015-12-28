// ============================================================================
#ifndef MUONDET_DEMUONDETECTOR_H
#define MUONDET_DEMUONDETECTOR_H 1

//DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidBox.h"

//LHCbKernel
#include "Kernel/Packer.h"

//Muon Detector
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonFrontEndID.h"

#include "MuonDet/PackMCMuonHit.h"  
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "MuonDet/MuonDAQHelper.h"

#include "MuonDet/MuonNamespace.h"

// Forward declarations
class MuonChamberLayout;
class IDataProviderSvc;


/** @class DeMuonDetector DeMuonDetector.h MuonDet/DeMuonDetector.h
 *  
 *  Detector element class for the muon system
 *
 *  @author Alessia Satta
 *  @author Alessio Sarti
 *  @date   18/10/2005
 */

static const CLID CLID_DEMuonDetector = 11009;  



class DeMuonDetector: public DetectorElement {

public:
  /// Constructor
  DeMuonDetector();

  /// Destructor
  ~DeMuonDetector() override;

  /// object identification
  const CLID& clID () const override;

  /// object identification
  static const CLID& classID () { return CLID_DEMuonDetector; }

  //Initialize of Detector Element
  StatusCode initialize() override;


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


  int sensitiveVolumeID(const Gaudi::XYZPoint &  myPoint) const;
  

  unsigned int gapID( int sensDetID);
  
  unsigned int chamberID( int sensDetID);
  
  unsigned int regionID( int sensDetID );
  
  unsigned int stationID( int sensDetID );
  
  unsigned int quadrantID( int sensDetID );
  
  bool isM1defined();

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
  //IDetectorElement* ReturnADetElement(int lsta, int lreg, int lchm);
  
  // Return the chamber pointer from m_ChmbPtr
  DeMuonChamber* getChmbPtr(const int station, const int region,
			    const int chmb) const;

  //const DeMuonChamber* getChmbPtr(const int station, const int region,
//			    const int chmb) const;



  //Fills the vector of chamber pointers  
  //void fillChmbPtr();

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
  double getStationZ(const int station);

  
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

  double getInnerX(const int station){
    return m_stationBox[station][0];
  };
  double getInnerY(const int station){
    return m_stationBox[station][1];
  };
  double getOuterX(const int station){
    return m_stationBox[station][2];
  };
  double getOuterY(const int station){
    return m_stationBox[station][3];
  };

  unsigned int getCardiacORX(const int par, const unsigned int readoutType){
    return m_phCardiacORNX[readoutType][par];
  };
  unsigned int getCardiacORY(const int par, const unsigned int readoutType){
    return m_phCardiacORNY[readoutType][par];
  };

  //Returns the station (as DetectorElement pointer) identified by MuonTileID
  DetectorElement* Tile2Station(const LHCb::MuonTileID aTile);
  
  // Return the chambers (as vector of DeMuonChamber pointer) from the MuonTileID
  std::vector<DeMuonChamber*> Tile2Chamber(const LHCb::MuonTileID aTile);

  //Returns the station (as DetectorElement pointer) from a Hit 
  DetectorElement* Hit2Station(const Gaudi::XYZPoint myPoint);
  
  //Returns the chamber (as DetectorElement pointer) from a Hit 
  DeMuonChamber* Hit2Chamber(const Gaudi::XYZPoint myPoint);

  inline MuonDAQHelper* getDAQInfo(){
    return &(m_daqHelper);
  };

private:

  // Useful constants
  static const unsigned int partition = 20;
  static const unsigned int maxReadoutType = 2;

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( UNLIKELY(!m_msgStream) ) m_msgStream.reset( new MsgStream(msgSvc(),name()) );
    return *m_msgStream;
  }

  //Answer the question: is "station" a filter?
  inline bool testForFilter(const IDetectorElement *station) const;

private:

  mutable std::unique_ptr<MsgStream> m_msgStream;

  //My data provider
  IDataProviderSvc* m_detSvc = nullptr;

  //My vector of Chamber pointers
  std::vector<DeMuonChamber*> m_ChmbPtr;

  //Chamber Layout
  std::unique_ptr<MuonChamberLayout> m_chamberLayout;


  //How many stations and regions
  int m_stations = 0;
  int m_regions = 0;
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
  //  double m_stationZ[5];
  std::vector<double> m_stationZ;
  MuonDAQHelper m_daqHelper;
  int   m_hitNotInGap = 0;

  bool m_isM1defined = true;
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline int DeMuonDetector::getStation(const double z) const
{
  //station index starting from z position (in mm)
  int idX = 0; double s_size(400.);
//  double s_off[5] = {12100,15200,16400,17600,18800};
  double offset = /* DDDB cords - s_off = */ 70; //Shouldn't be zero?

  int nsta = m_stationZ.size();
  for(idX = 0; idX<nsta; idX++) {
    //    if(fabs(z-m_stationZ[idX]+offset)<s_size) break;
    if(fabs(z-m_stationZ.at(idX)+offset)<s_size) break;
  }


  //If M1 is not defined (post upgrade) shift the index back
//  if (m_isM1defined == false) 
//    idX--;

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

inline double DeMuonDetector::getStationZ(const int station){
  //    return m_stationZ[station];
  return m_stationZ.at(station);
}


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

inline bool DeMuonDetector::isM1defined()
{
    return m_isM1defined;
}


// Answer the question, is "station" a muon filter?
inline bool DeMuonDetector::testForFilter(const IDetectorElement *station) const
{
    std::string stationName = station->name();

    //Check if the "station" isn't actually a filter
    if (stationName.find("/MF") != stationName.npos)
      return true;

    return false;
}

#endif    // MUONDET_DEMUONDETECTOR_H
