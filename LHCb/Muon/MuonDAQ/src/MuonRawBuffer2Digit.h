// $Id: MuonRawBuffer2Digit.h,v 1.1.1.1 2004-02-05 16:26:03 cattanem Exp $
#ifndef MUONRAWBUFFER2DIGIT_H 
#define MUONRAWBUFFER2DIGIT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
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
#include "MuonHLTDigitFormat.h"

//#include "MuonRawBuffer.h"

/** @class MuonRawBuffer2Digit MuonRawBuffer2Digit.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-26
 */
class MuonRawBuffer2Digit : public Algorithm {
public: 
  /// Standard constructor
  MuonRawBuffer2Digit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonRawBuffer2Digit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
void TilePrintOut(MuonTileID digitTile);
  int m_TotL1Board;
  std::vector<int> m_L1Map[12];
  std::string getBasePath(int station);
  std::string basePath[5] ;
  
 
};
#endif // MUONRAWBUFFER2DIGIT_H
