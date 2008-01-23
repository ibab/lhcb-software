// $Id: RawBankReadoutStatusMonitor.h,v 1.2 2008-01-23 23:43:05 odescham Exp $
#ifndef COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H 
#define COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class RawBankReadoutStatusMonitor RawBankReadoutStatusMonitor.h component/RawBankReadoutStatusMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-01-23
 */
class RawBankReadoutStatusMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  RawBankReadoutStatusMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankReadoutStatusMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::vector<std::string> m_bankTypes;
  long m_counter;
  int m_degree;
  int m_nSources;
   std::stringstream m_stat;

};
#endif // COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H
