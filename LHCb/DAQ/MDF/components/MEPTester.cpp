// $Id: MEPTester.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/SmartIF.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 
#include "Event/RawEvent.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"


/*
 *    LHCb namespace declaration
 */
namespace LHCb  {
  /** @class MEPTester MEPTester.cpp
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class MEPTester : public Algorithm {
    int m_numPrint;  ///< Property "Print". Number of events with printout
    int  m_numDump;   ///< Number of events with dump
  public: 
    /// Standard constructor
    MEPTester(const std::string& nam,ISvcLocator* svc) : Algorithm(nam,svc){
      declareProperty("Print", m_numPrint=10);
      declareProperty("Dump",  m_numDump=2);
    }
    /// Destructor
    virtual ~MEPTester()              {                                     }
    /// Algorithm initialization
    virtual StatusCode initialize()   {      return StatusCode::SUCCESS;    }
    /// Algorithm finalization
    virtual StatusCode finalize()     {      return StatusCode::SUCCESS;    }
    /// Main execution
    virtual StatusCode execute()  {
      //typedef std::map<std::string,RawEvent*> _E;
      typedef std::vector<std::pair<std::string,RawEvent*> > _E;
      typedef std::vector<IRegistry*> _L;
      typedef std::vector<RawBank*>   _B;
      static int evt_count=0;
      MsgStream log(msgSvc(),name());
      SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
      if ( evt )  {
        SmartIF<IDataManagerSvc> mgr(eventSvc());
        if ( mgr )  {
          _L leaves;
          StatusCode sc = mgr->objectLeaves(evt.ptr(),leaves);
          if ( sc.isSuccess() )  {
            std::string path, id;
            _E evts;
            evt_count++;
            for(_L::const_iterator i=leaves.begin(); i!=leaves.end(); ++i) {
              id = (*i)->identifier().substr(7,3);
              // /Event/Prev
              if ( id == "Pre" || id == "Nex" )
                path = (*i)->identifier()+"/DAQ/RawEvent";
              else if ( id == "DAQ" )
                path = (*i)->identifier()+"/RawEvent";
              else 
                path = "";
              if ( !path.empty() )  {
                SmartDataPtr<RawEvent> raw(eventSvc(),path);
                evts.insert(evts.end(),std::make_pair(path,raw.ptr()));
              }
            }
            if ( evt_count < m_numPrint ) {
              log << MSG::ALWAYS << "+------------------> MEP [" << evt_count << "]" << endmsg;              
              for(_E::const_iterator ir=evts.begin(); ir!=evts.end(); ++ir) {
                log << MSG::ALWAYS << "+-> " << std::setw(30) << std::left << (*ir).first << " ";
                const _B& odin = (*ir).second->banks(RawBank::ODIN);
                if ( odin.size() == 1 )  {
                  OnlineRunInfo* inf = odin[0]->begin<OnlineRunInfo>();
                  log << " Run:"   << std::setw(6) << inf->Run 
                      << " Orbit:" << std::setw(3) << inf->Orbit
                      << " L0:"    << std::setw(6) << inf->L0ID 
                      << " BID:"   << std::setw(3) << inf->bunchID;
                }
                log << endmsg;
                if ( evt_count < m_numDump ) {
                  for(int ib=0; ib<RawBank::LastType; ++ib)  {
                    const _B& b = (*ir).second->banks(RawBank::BankType(ib));
                    if ( b.size() > 0 )  {
                      log << " +> " << b.size() << " bank(s) of type " << ib
                          << " (" << RawEventPrintout::bankType(ib) << ") " << endmsg;
                    }
                    for(_B::const_iterator itB = b.begin(); itB != b.end(); itB++) {
                      log << " +> Bank:  [" << RawEventPrintout::bankHeader(*itB) << "] " << endmsg;
                    }
                  }
                }
              }
            }
            return sc;
          }
          log << MSG::ERROR << "Failed to access leaves of /Event" << endmsg;
          return StatusCode::FAILURE;
        }
      }
      log << MSG::ERROR << "Failed to retrieve top level event." << endmsg;
      return StatusCode::FAILURE;
    }
  };
}

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MEPTester)
