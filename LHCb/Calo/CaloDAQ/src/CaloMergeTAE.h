// $Id: CaloMergeTAE.h,v 1.1 2008-10-27 11:50:48 odescham Exp $
#ifndef CALOMERGETAE_H 
#define CALOMERGETAE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"
#include "Event/CaloAdc.h"


/** @class CaloMergeTAE CaloMergeTAE.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-10-24
 */
class CaloMergeTAE : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloMergeTAE( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloMergeTAE( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void mergeDigits();
  void mergeAdcs();
  std::vector<std::string> m_slots;
  std::string m_detectorName;
  double m_threshold;
  std::string m_locDigit;
  std::string m_outDigit;
  std::string m_locAdc;
  std::string m_outAdc;
  std::string m_data;
  
  DeCalorimeter* m_calo;
  
};
#endif // CALOMERGETAE_H
