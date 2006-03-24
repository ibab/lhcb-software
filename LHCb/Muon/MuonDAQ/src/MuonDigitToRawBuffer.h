// $Id: MuonDigitToRawBuffer.h,v 1.5 2006-03-24 11:03:22 asatta Exp $
#ifndef MUONDIGITTORAWBUFFER_H 
#define MUONDIGITTORAWBUFFER_H 1

// Include files
// from STL
#include <string>

#include "MuonKernel/MuonTile.h"
#include "Kernel/MuonTileID.h"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class MuonDigitToRawBuffer MuonDigitToRawBuffer.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-19
 */
class MuonDigitToRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonDigitToRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDigitToRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
void TilePrintOut(LHCb::MuonTileID digitTile);
  LHCb::MuonTileID findTS(LHCb::MuonTileID digit);
  unsigned int findDigitInTS(std::string TSPath, LHCb::MuonTileID TSTile,
                     LHCb::MuonTileID digit);  
  unsigned int findODENumber(std::string odePath);
  unsigned int findODEPosition(std::string L1Path, long odeNumber);  
  std::string findODEPath(LHCb::MuonTileID TS);
  std::string findL1(LHCb::MuonTileID TS);
  unsigned int findTSPosition(std::string ODEPath, LHCb::MuonTileID TSTile);
  std::string findTSPath(std::string ODEPath, long TSPosition,int station); 
  long channelsInL1BeforeODE(std::string L1Path,long ODENumber);	
  unsigned int DAQaddress(LHCb::MuonTileID digitTile, long& L1Number, long& ODENumber);
  std::string getBasePath(int station);  
  std::vector<unsigned int> padsinTS(std::vector<unsigned int>& TSDigit, 
                                   std::string TSPath);
  std::vector<std::string> m_L1Name;
  std::vector<std::string> m_ODEName;
  std::vector<std::string> m_TELL1Name;
  unsigned int m_ODENameStart[5][4][4];
  unsigned int m_ODENameEnd[5][4][4];
  std::vector<unsigned int> m_digitsInODE[180];
  unsigned int firedInODE[180];
  std::vector<unsigned int> m_ODEInL1[20];
  std::vector<unsigned int> m_padInL1[20];  
  long m_TotL1Board;
  long m_TotODEBoard;  
  std::string basePath[5] ;
  unsigned int m_M1Tell1;
  //unsigned int m_maxSubSectorInTS;
  
};
#endif // MUONDIGITTORAWBUFFER_H
