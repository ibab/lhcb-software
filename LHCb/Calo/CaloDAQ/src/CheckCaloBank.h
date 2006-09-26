// $Id: 
#ifndef   CHECKCALOBANK_H
#define   CHECKCALOBANK_H 1

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "Event/RawEvent.h"

/** @class CheckCaloBank CheckCaloBank.h   
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */

class CheckCaloBank : public GaudiAlgorithm { 
  friend class AlgFactory<CheckCaloBank>;
 public:
  
  CheckCaloBank( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~CheckCaloBank();
  
  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  
 protected:
  
 private:   
  LHCb::RawBank::BankType m_trig;
  LHCb::RawBank::BankType m_bank;
  std::string m_typ;
};
#endif //    CHECKCALOBANK_H
