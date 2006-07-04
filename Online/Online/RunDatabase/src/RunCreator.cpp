// $Id: RunCreator.cpp,v 1.1 2006-07-04 17:04:37 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "RunDatabase/IRunDatabaseWriter.h"
#include "PyRPC.h"
#include <ctime>

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RunCreator RunCreator.h tests/RunCreator.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RunCreator : public Algorithm {
    int m_partID, m_fill;
    std::string  m_activity;
    std::string  m_progVsn;
    std::string  m_progName;
    /// Pointer to run database interface
    IRunDatabaseWriter* m_runDb;
  public: 
    /// Standard constructor
    RunCreator( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_runDb(0)
    {
      declareProperty("Fill",m_fill=1000);
      declareProperty("PartitionID",m_partID=0x103);
      declareProperty("Activity",m_activity="Physics");
      declareProperty("Program",m_progName="Moore");
      declareProperty("ProgramVersion",m_progVsn="v15r3");
    }

    /// Destructor
    virtual ~RunCreator()  {
    } 

    /// Initialize the algorithm.
    StatusCode initialize()   {
      MsgStream log(msgSvc(),name());
      StatusCode sc = service("RunDatabase",m_runDb);
      if ( !sc.isSuccess() )  {
        log << MSG::ERROR << "Failed to connect to run database." << endmsg;
        return sc;
      }
      return sc;
    }

    /// Finalize
    StatusCode finalize() {
      if ( m_runDb ) m_runDb->release();
      m_runDb = 0;
      return StatusCode::SUCCESS;
    }
    
    /// Main execution
    virtual StatusCode execute()  {
      MsgStream log(msgSvc(),name());
      static int last_run = 0;
      static int last_booked_run = 0;
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      if ( raw )  {
        typedef std::vector<RawBank*> _V;
        const _V& bnks = raw->banks(RawBank::DAQ);
        for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
          RawBank* b = *i;
          if ( b->version() == DAQ_STATUS_BANK )  {
            MDFHeader* hdr = (MDFHeader*)b->data();
            MDFHeader::SubHeader h = hdr->subHeader();
            int run_no = h.H1->runNumber();
            if ( run_no != last_run )  {
              IRunDatabaseWriter::Result<int> res = 
                m_runDb->createRun(m_fill,m_partID,m_activity,time(0),m_progName, m_progVsn);
              if ( res.ok() )  {
                last_run = run_no;
                last_booked_run = res.data;
                log << MSG::ALWAYS << "Successfully booked new run:" << last_run << endmsg;
              }
              else {
                log << MSG::ERROR << "Failed to book new run!" << endmsg;
                return StatusCode::FAILURE;
              }
            }
            h.H1->setRunNumber(last_booked_run);
            return StatusCode::SUCCESS;
          }
        }
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RunCreator)
