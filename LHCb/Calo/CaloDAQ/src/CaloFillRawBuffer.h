// $Id: CaloFillRawBuffer.h,v 1.3 2005-11-10 16:43:22 ocallot Exp $
#ifndef CALOFILLRAWBUFFER_H 
#define CALOFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawBuffer.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloFillRawBuffer CaloFillRawBuffer.h
 *  Fills the Raw Buffer banks for the calorimeter
 *
 *  @author Olivier Callot
 *  @date   2004-12-17
 */
class CaloFillRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloFillRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void fillDataBankShort ( );

  void fillPackedBank ( );

  void fillTriggerBank ( );

private:
  std::string m_inputBank;
  std::string m_triggerBank;
  std::string m_detectorName;
  int    m_bankType;
  int    m_triggerBankType;
  int    m_numberOfBanks;
  int    m_dataCodingType;

  DeCalorimeter* m_calo;
  CaloReadoutTool* m_roTool;

  // Statistics
  
  double m_totDataSize;
  std::vector<double> m_dataSize;
  double m_totTrigSize;
  int m_nbEvents;
  std::vector< std::vector<raw_int> > m_banks;
  std::vector< std::vector<raw_int> > m_trigBanks;
};
#endif // CALOFILLRAWBUFFER_H
