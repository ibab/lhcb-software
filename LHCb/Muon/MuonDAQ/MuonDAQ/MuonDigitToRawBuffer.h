// $Id: MuonDigitToRawBuffer.h,v 1.1 2004-08-31 10:06:10 asatta Exp $
#ifndef MUONDIGITTORAWBUFFER_H 
#define MUONDIGITTORAWBUFFER_H 1

// Include files
// from STL
#include <string>
#include "MuonKernel/MuonTile.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/RndmGenerators.h"
#include <GaudiKernel/DataSvc.h> 
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "Event/MuonDigit.h" 
#include "Event/DAQTypes.h"
#include "Event/RawBuffer.h"
#include "Event/RawEvent.h"
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "MuonKernel/MuonTile.h"
#include "MuonHLTData.h"

/** @class MuonDigitToRawBuffer MuonDigitToRawBuffer.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-19
 */
class MuonDigitToRawBuffer : public Algorithm {
public: 
  /// Standard constructor
  MuonDigitToRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDigitToRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
void TilePrintOut(MuonTileID digitTile);
  MuonTileID findTS(MuonTileID digit);
  long findDigitInTS(std::string TSPath, MuonTileID TSTile,
                     MuonTileID digit);  
  long findODENumber(std::string odePath);
  long findODEPosition(std::string L1Path, long odeNumber);  
  std::string findODEPath(MuonTileID TS);
  std::string findL1(MuonTileID TS);
  long findTSPosition(std::string ODEPath, MuonTileID TSTile);
  std::string findTSPath(std::string ODEPath, long TSPosition,int 
station); 
  long channelsInL1BeforeODE(std::string L1Path,long ODENumber);
	
long DAQaddress(MuonTileID digitTile, long& L1Number);
  std::string getBasePath(int station);  
  std::vector<std::string> m_L1Name;
  std::vector<std::string> m_ODEName;
  unsigned int m_ODENameStart[5][4][4];
  unsigned int m_ODENameEnd[5][4][4];
  std::vector<int> m_digitsInL1[20];	
  long m_TotL1Board;
 std::string basePath[5] ;
};
#endif // MUONDIGITTORAWBUFFER_H
