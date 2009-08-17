// $Id: CaloAdcFromRaw.h,v 1.1 2009-08-17 12:11:15 odescham Exp $
#ifndef CALOADCFROMRAW_H
#define CALOADCFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"
#include "CaloDAQ/ICaloDataProvider.h"
#include "CaloDAQ/ICaloL0DataProvider.h"

/** @class CaloAdcFromRaw CaloAdcFromRaw.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-04-07
 */
class CaloAdcFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloAdcFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloAdcFromRaw( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_detectorName;
  std::string m_caloName;
  std::string m_location;
  std::string m_l0Location;
  std::string m_l0BitLocation;
  int m_offset;
  bool m_calib;
  DeCalorimeter* m_calo;
  ICaloDataProvider*    m_data;
  ICaloL0DataProvider*  m_l0data;
};
#endif // CALOADCFROMRAW_H
