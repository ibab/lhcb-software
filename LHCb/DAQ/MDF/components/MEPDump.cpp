// $Id: MEPDump.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventHelpers.h"

namespace LHCb  {

  /** @class MEPDump MEPDump.cpp tests/MEPDump.cpp
    *   Dump MEP file content
    *
    *  @author Markus Frank
    *  @date   2005-10-13
    */
  class MEPDump : public Algorithm {
  public: 
    int m_evt;
    std::string              m_con;
    StreamDescriptor         m_desc;
    StreamDescriptor::Access m_bindDsc;
    StreamDescriptor::Access m_accessDsc;

    /// Standard constructor
    MEPDump( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_evt(0)   {
      declareProperty("Connection", m_con="");
    }

    /// Destructor
    virtual ~MEPDump()  {} 

    virtual StatusCode initialize()  {
      m_bindDsc = StreamDescriptor::bind(m_con);
      if ( m_bindDsc.ioDesc > 0 )   {
        m_accessDsc = StreamDescriptor::accept(m_bindDsc);
        return m_accessDsc.ioDesc > 0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      return StatusCode::FAILURE;
    }

    virtual StatusCode finalize()  {
      StreamDescriptor::close(m_accessDsc);
      StreamDescriptor::close(m_bindDsc);
      return StatusCode::SUCCESS;
    }

    /// Main execution
    virtual StatusCode execute()  {
      MsgStream log(msgSvc(),name());
      unsigned int partitionID;
      try  {
        typedef std::map<unsigned int, RawEvent*> Events;
        Events events;
        if ( readMEPrecord(m_desc, m_accessDsc).isSuccess() )  {
          Events::const_iterator i;
          MEPEvent* me = (MEPEvent*)m_desc.data();
          decodeMEP(me, false, partitionID, events);
          for(i=events.begin();i!=events.end();++i)  {
            m_evt++;
            log << MSG::ALWAYS << (*i).first << ".";
          }
          log << MSG::ALWAYS << "Read " << m_evt << " events." << endmsg;
          for(i=events.begin();i!=events.end();++i)  {
            const RawEvent* e = (*i).second;
            size_t nbank = numberOfBanks(e);
            size_t ntypes = numberOfBankTypes(e);
            log << MSG::ALWAYS << "Event:" << (*i).first << " has " 
                << nbank << " banks of " << ntypes << " types." << endmsg;
          }
          for(i=events.begin();i!=events.end();++i)  {
            delete (*i).second;
          }
          return StatusCode::SUCCESS;
        }
        log << MSG::ERROR << "Read Error: End-Of-File:" << m_con << endmsg;
      }
      catch(std::exception& e)  {
        log << MSG::ERROR << e.what() << endmsg;
      }
      return StatusCode::FAILURE;
    }
  };
}

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MEPDump)
