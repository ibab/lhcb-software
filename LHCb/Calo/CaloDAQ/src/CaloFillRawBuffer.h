// $Id: CaloFillRawBuffer.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALOFILLRAWBUFFER_H 
#define CALOFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawBuffer.h"
#include "CaloDet/DeCalorimeter.h"

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

  int bufferNumber( const CaloCellID& ID )  {
    int det  = ID.calo();
    int area = ID.area();
    int row  = ID.row();
    int col  = ID.col();
    int bNum = 0;

    if ( 2 == det ) {  // ECAL
      bNum = 2*area;
      if ( 0 == area && 33 < row ) bNum += 1; //== Two L1 boards
      if ( 1 == area && 31 < row ) bNum += 1; //== Two L1 boards
      if ( 31 < col ) bNum += 5;
    } else if ( 3 == det ) {  // HCAL
      bNum = area;
      if ( 15 < col ) bNum += 2;
    }
    return bNum;
  }

  void fillDataBank ( );

  void fillDataBankShort ( );

  void fillTriggerBank ( );

private:
  std::string m_inputBank;
  std::string m_triggerBank;
  std::string m_detectorName;
  int    m_bankType;
  int    m_triggerBankType;
  int    m_numberOfBanks;
  double m_energyScale;
  int    m_dataCodingType;

  DeCalorimeter* m_calo;

  // Statistics
  
  double m_totDataSize;
  double m_totTrigSize;
  int m_nbEvents;
  std::vector< std::vector<raw_int> > m_banks;
  std::vector< std::vector<raw_int> > m_trigBanks;
};
#endif // CALOFILLRAWBUFFER_H
