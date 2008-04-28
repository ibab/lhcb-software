// $Id: RawBankSizeMonitor.h,v 1.2 2008-04-28 16:21:31 frankb Exp $
#ifndef COMPONENT_RAWBANKSIZEMONITOR_H 
#define COMPONENT_RAWBANKSIZEMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// from LHCb
#include "Event/RawEvent.h"

namespace LHCb {

  /** @class RawBankSizeMonitor RawBankSizeMonitor.h component/RawBankSizeMonitor.h
   *  
   *
   *  @author Olivier Deschamps
   *  @date   2008-03-27
   */
  class RawBankSizeMonitor : public GaudiHistoAlg {
  public: 
    /// Standard constructor
    RawBankSizeMonitor( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~RawBankSizeMonitor( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:

  private:
    LHCb::RawEvent* m_rawEvt;
    //
    std::vector<std::string> m_bankNames;
    std::vector<LHCb::RawBank::BankType> m_bankTypes;
    std::map<std::string,int> m_max;
    int m_def;
    int m_bin;
    bool m_prof;
    bool m_firstevent;
  };
}
#endif // COMPONENT_RAWBANKSIZEMONITOR_H
