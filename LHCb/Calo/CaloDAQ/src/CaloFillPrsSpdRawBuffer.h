// $Id: CaloFillPrsSpdRawBuffer.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALOFILLPRSSPDRAWBUFFER_H 
#define CALOFILLPRSSPDRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawBuffer.h"
#include "CaloDet/DeCalorimeter.h"

/** @class CaloFillPrsSpdRawBuffer CaloFillPrsSpdRawBuffer.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-04
 */
class CaloFillPrsSpdRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloFillPrsSpdRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloFillPrsSpdRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  int bufferNumber( const CaloCellID& ID )  {
    int area = ID.area();
    int row  = ID.row();
    int col  = ID.col();
    int bNum = 0;

    bNum = 1 + area;
    if ( 0 == area && 34 > row ) bNum = 0; //== Two L1 boards
    if ( 31 < col ) bNum += 4;
    return bNum;
  }

  void fillDataBank ( );

  void fillDataBankShort ( );

  void fillTriggerBank ( );

private:
  std::string m_inputBank;
  std::string m_prsBank;
  std::string m_spdBank;
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
#endif // CALOFILLPRSSPDRAWBUFFER_H
