// $Id: DecodeVeloFullRawBuffer.h,v 1.3 2008-03-17 14:26:10 krinnert Exp $
#ifndef DECODEVELOFULLRAWBUFFER_H 
#define DECODEVELOFULLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "VeloEvent/VeloFullBank.h"
#include "VeloEvent/VeloTELL1Data.h"
#include "VeloEvent/EvtInfo.h"
#include "Tell1Kernel/VeloDecodeCore.h"
#include "Tell1Kernel/VeloDecodeConf.h"
#include "VeloFullDecoder.h"

/** @class DecodeVeloFullRawBuffer DecodeVeloFullRawBuffer.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-04-21
 */

using namespace VeloTELL1;
using namespace LHCb;

class DecodeVeloFullRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  DecodeVeloFullRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecodeVeloFullRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode getData();
  StatusCode decodeData();

protected:

  std::string adcContName();
  std::string pedContName();
  std::string decADCName();
  std::string decPedName();
  std::string decHeaderName();
  std::string evtInfoName();
  void sortAndWriteDecodedData();
  void setADCDataFlag();
  void setPedDataFlag();
  bool adcDataFlag();
  bool pedDataFlag();
  void resetMemory();
  void unsetADCDataFlag();
  void unsetPedDataFlag();

private:
  // location paths
  std::string m_veloADCLocation;
  std::string m_veloPedLocation;
  std::string m_decodedADCLocation;
  std::string m_decodedPedLocation;
  std::string m_decodedHeaderLocation;
  std::string m_evtInfoLocation;
  // input data
  VeloFullBanks* m_veloADCs;
  VeloFullBanks* m_veloPeds;
  // decoded data for futher processing
  VeloTELL1Datas* m_decodedADC;
  VeloTELL1Datas* m_decodedPed;
  VeloTELL1Datas* m_decodedHeader;
  EvtInfos* m_evtInfo;
  // flags
  bool m_adcDataPresent;
  bool m_pedDataPresent;  
  bool m_sectorCorrection;
  bool m_isDebug;
  // data buffers
  sdataVec m_signADC;
  sdataVec m_signADCReordered;
  sdataVec m_signHeader;
  sdataVec m_signHeaderReordered;
  sdataVec m_signPed;
  sdataVec m_signPedReordered;
  // decoders
  VeloFullDecoder m_ADCDecoder;
  VeloFullDecoder m_HeaderDecoder;
  VeloFullDecoder m_PedDecoder;
  // cable order
  std::vector<unsigned int> m_cableOrder;
  
  
};
#endif // DECODEVELOFULLRAWBUFFER_H
