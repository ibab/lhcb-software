// $Id: bankKiller.h,v 1.3 2008-01-23 18:24:25 odescham Exp $
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
  
 private:   
  std::vector< std::string > m_bankTypes;
};
#endif //    BANKKILLER_H
