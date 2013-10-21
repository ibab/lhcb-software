// $Id: MuonDAQHelper.h,v 1.8 2010-02-09 12:47:38 asatta Exp $
#ifndef MUONDET_MUONDAQHELPER_H 
#define MUONDET_MUONDAQHELPER_H 1
#include <string>
#include <vector>

#include "GaudiKernel/MsgStream.h"

// Include files
class IDataProviderSvc;
class IMessageSvc;

#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "Kernel/MuonTileID.h"
#include "MuonDet/MuonBasicGeometry.h"

/** @class MuonDAQHelper MuonDAQHelper.h MuonDet/MuonDAQHelper.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-12-05
 */

static const unsigned int MuonDAQHelper_maxTell1Number=14;
static const unsigned int MuonDAQHelper_maxODENumber=180;
static const unsigned int MuonDAQHelper_linkNumber=24;

class MuonDAQHelper {
public: 
  /// Standard constructor
  MuonDAQHelper( ); 

  virtual ~MuonDAQHelper( ); ///< Destructor
  void initSvc(IDataProviderSvc* detSvc , IMessageSvc * msgSvc );
  /// steering initialization
  StatusCode initDAQMaps();
  /// TELL1 sanity checks
  StatusCode initTELL1();
  /// ODE sanity checks
  StatusCode initODE();
  /// Initialize TELL1 maps
  StatusCode initMaps();
  /// Initialize ODE maps
  //  StatusCode initODEMaps(); // removed, useless  
  /// Initialize TELL1 pad maps
  StatusCode initLUTCrossing()  ;
  //  StatusCode initODENames(); // removed, useless  
  StatusCode initReverseMaps();
  StatusCode checkMapConsistency();
  
  std::vector<LHCb::MuonTileID> DoPadDC06(std::vector<LHCb::MuonTileID> digit,
                                                   MuonTSMap* TS);
  std::vector<LHCb::MuonTileID> DoPadV1(std::vector<LHCb::MuonTileID> digit,
                                        std::vector<LHCb::MuonTileID> 
                                          wrongdigit,MuonTSMap* TS);
  LHCb::MuonTileID getADDInLink(unsigned int Tell1_num, long link_num,long ch);
  LHCb::MuonTileID getADDInODE(long ODE_num, long ch);
  LHCb::MuonTileID getADDInODENoHole(long ODE_num, long ch);
  LHCb::MuonTileID getADDInTell1(unsigned int Tell1_num, long ch);
  LHCb::MuonTileID getPadInTell1DC06(unsigned int Tell1_num, long pad);
  LHCb::MuonTileID getPadInTell1V1(unsigned int Tell1_num, long pad);
   unsigned int getPPNumber(unsigned int Tell1_num,unsigned int ODE_num);
  inline unsigned int TotTellNumber(){
    return m_TotTell1;
  };
  inline unsigned int M1TellNumber(){
    return m_M1Tell1;
  };
  inline std::string Tell1Name(unsigned int i){
    return m_TELL1Name[i];
  };
  inline unsigned int ODEInTell1(unsigned int Tell1_num){
    return m_ODENumberInTell1[Tell1_num];
  }
  inline unsigned int getODENumberInTell1(unsigned int Tell1_num,unsigned int ODE_num){
    return (m_ODEInTell1[Tell1_num])[ODE_num];
  }

  //GP moved to MuonDAQHelper.cpp. Protect against empty elements
  unsigned int getODENumberInLink(unsigned int Tell1_num,unsigned int Link_num);

  unsigned int findODENumber(LHCb::MuonTileID digit);
  unsigned int findODEChNumber(LHCb::MuonTileID digit);
  StatusCode findHWNumber(LHCb::MuonTileID digit,
                          long& L1Number,long& link_number,
                          long& ODE_number,long& ode_ch);
  
  StatusCode findL0Number(LHCb::MuonTileID digit,
                          std::string& ECSName,long& link_number,
                          long& link_ch);
  
  std::string getODEECSName(int number);
  StatusCode findStrips(LHCb::MuonTileID pad,
                        LHCb::MuonTileID  strips[2]);
  


  
  LHCb::MuonTileID findTS(LHCb::MuonTileID digit);
  unsigned int findDigitInTS(std::string TSPath, LHCb::MuonTileID TSTile,
                             LHCb::MuonTileID digit,bool hole=true);  
  unsigned int findODENumber(std::string odePath);

  unsigned int findODEPosition(std::string L1Path, long odeNumber,bool hole=true);  
  std::string findODEPath(LHCb::MuonTileID TS);
  std::string findL1(LHCb::MuonTileID TS);
  unsigned int findTSPosition(std::string ODEPath, LHCb::MuonTileID TSTile);
  std::string findTSPath(std::string ODEPath, long TSPosition,int station); 
  long channelsInL1BeforeODE(std::string L1Path,long ODENumber,bool hole=true);	
  unsigned int DAQaddressInODE(LHCb::MuonTileID digitTile, long& L1Number, long& ODENumber,bool hole=true);
  unsigned int DAQaddressInL1(LHCb::MuonTileID digitTile, long& 
   L1Number, long& ODENumber,unsigned int & ODEAdd,bool hole=true);
  std::string getBasePath(std::string statname);  
  std::vector<unsigned int> padsinTS(std::vector<unsigned int>& TSDigit, 
                                     std::string TSPath);
  LHCb::MuonTileID getPadTileInODE(std::string ODEName, int firstChannel, int secondChannel); 

  StatusCode updateLUT();

  MuonL1Board* getL1Board(unsigned int board_num);
  MuonODEBoard* getODEBoard(MuonL1Board*l1,unsigned int board);  
  MuonTSMap* getTSMap(MuonL1Board*l1,MuonODEBoard* ode,unsigned int ts);  
  std::vector<int> getTell1InStation(int station)
  {
    return m_tellPerStation[station];
  }
  unsigned int getODENumberInQuadrant(std::string ODEName);

  //GP
  // new method to retrieve the progressive number of Tell1 named L1Name
  // this is neded at the moment because TELL1s are indexed sometimes 
  // by serial number and sometimes by an progressive counter.
  // This is irrelevant when M1 is present by matters if M1 is missing !
  // 
  
  int findL1Index(std::string L1Name);
  
private:

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream(m_msgSvc, "MuonDAQHelper");
    return *m_msgStream;
  }

private:

  mutable MsgStream * m_msgStream;
  IDataProviderSvc* m_detSvc ;
  IMessageSvc* m_msgSvc ;
  friend class DeMuonDetector;
  void resetLUT();
  
  unsigned int m_TotTell1; // TELL1 counter
  unsigned int m_ODENumberInTell1[MuonDAQHelper_maxTell1Number];
  unsigned int m_M1Tell1;
  std::vector<unsigned int> m_ODEInTell1[MuonDAQHelper_maxTell1Number];
  std::vector<LHCb::MuonTileID> m_mapTileInODE[MuonDAQHelper_maxODENumber];
  std::vector<LHCb::MuonTileID> m_mapTileInODEDC06[MuonDAQHelper_maxODENumber];

  std::map<std::string, std::string> basePath;
  std::vector<LHCb::MuonTileID> m_mapPadDC06[MuonDAQHelper_maxTell1Number];
  std::vector<LHCb::MuonTileID> m_mapPadV1[MuonDAQHelper_maxTell1Number];
 

  std::vector<LHCb::MuonTileID> m_mapTileInTell1[MuonDAQHelper_maxTell1Number];
  std::vector<long> m_linkInTell1[MuonDAQHelper_maxTell1Number];


  std::vector<std::string> m_L1Name;
  //  long m_TotODEBoard; 
  std::vector<std::string> m_TELL1Name;
  std::vector<std::string> m_ODEName;
  unsigned int m_ODENameStart[5][4][4];
  unsigned int m_ODENameEnd[5][4][4];

  int m_layoutX[2][20];
  int m_layoutY[2][20];
  int layout[20];
  std::string m_ODENameInECS[MuonDAQHelper_maxODENumber];
  unsigned int m_TSLength[5][4];
  int m_TUSize[5][4];
  std::vector<int> m_tellPerStation[5];

  int m_nStations;
  MuonBasicGeometry* m_basegeometry;

};
#endif // MUONDET_MUONDAQHELPER_H

