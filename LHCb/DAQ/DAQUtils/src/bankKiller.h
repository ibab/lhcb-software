// $Id: bankKiller.h,v 1.4 2008/08/25 11:05:05 panmanj Exp $
#ifndef BANKKILLER_H
#define BANKKILLER_H 1

// STL
#include <string>
#include <vector>
#include <map>

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "Event/RawEvent.h"

/** @class bankKiller bankKiller.h   
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */

class bankKiller : public GaudiAlgorithm { 
  friend class AlgFactory<bankKiller>;
 public:
  
  bankKiller( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~bankKiller();
  
  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  
 protected:
  void  killBankType( LHCb::RawBank::BankType bankType, 
		      bool warningmsg ); // kill all banks of given type
  
 private:   
  std::vector< std::string > m_bankTypes;
  bool m_defaultIsKill;
  LHCb::RawEvent* m_rawEvt;
};
#endif //    BANKKILLER_H
