// $Id: RawEventTestDump.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 
#include "MDF/RawEventPrintout.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

/** @class RawEventTestDump RawEventTestDump.cpp
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RawEventTestDump : public Algorithm {
    bool m_full;                 ///< Property "FullDump". If true, full bank contents are dumped
    bool m_dump;                 ///< Property "DumpData". If true, full bank contents are dumped
    bool m_check;                ///< Property "CheckData". If true, full bank contents are checked
    int  m_debug;                ///< Number of events where all dump flags should be considered true
    int  m_numEvent;             ///< Event counter
    std::string  m_rawLocation;  ///< Property "RawLocation". Raw Event location
  public: 
    
    /// Standard constructor
    RawEventTestDump( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_numEvent(0)
    {
      declareProperty( "DumpData",    m_dump = false );
      declareProperty( "FullDump",    m_full = false );
      declareProperty( "CheckData",   m_check = false );
      declareProperty( "RawLocation", m_rawLocation=RawEventLocation::Default);
      declareProperty( "Debug",       m_debug = 0 );
    }
    /// Destructor
    virtual ~RawEventTestDump()  {
    }
    /// Algorithm initialization
    virtual StatusCode initialize()  {
      m_numEvent = 0;
      return StatusCode::SUCCESS;
    }
    /// Algorithm finalization
    virtual StatusCode finalize()  {
      m_numEvent = 0;
      return StatusCode::SUCCESS;
    }
    /// Main execution
    virtual StatusCode execute()  {
      SmartDataPtr<RawEvent> raw(eventSvc(),m_rawLocation);
      MsgStream info(msgSvc(),name());
      bool dmp = m_numEvent<m_debug || m_dump;
      bool chk = m_numEvent<m_debug || m_check;
      bool ful = m_numEvent<m_debug || m_full;
      static int evt = 0;
      ++evt;
      info << MSG::INFO;
      if ( !raw ) {
        info << MSG::ERROR << "Failed to access " << RawEventLocation::Default << endmsg;
        // Don't want to exit for this -- this is a debugging algorithm only!
        return StatusCode::SUCCESS;        
      }
      for(int j=0; j<RawBank::LastType; ++j)  {
        RawBank::BankType i = RawBank::BankType(j);
        const std::vector<RawBank*>& b = raw->banks(i);
        int cnt, inc = (i == RawBank::Rich) ? 64 : 32;
        const int *p;
        if ( b.size() > 0 )  {
          if ( dmp )  {
            info << "Evt No:" << std::left << std::setw(6) << evt 
                 << " has " << b.size() << " bank(s) of type " << i 
                 << " (" << RawEventPrintout::bankType(i) << ") " << endmsg;
          }
          std::vector<RawBank*>::const_iterator itB;
          int k = 0;
          for( itB = b.begin(); itB != b.end(); itB++, k++ ) {
            const RawBank* r = *itB;
            if ( dmp ) {
              info << "Bank:  [" << RawEventPrintout::bankHeader(r) << "] " << endmsg;
            }
            if( ful ) {
              cnt = 0;
              std::stringstream s;
              for(p=r->begin<int>(); p != r->end<int>(); ++p)  {
                s << std::hex << std::setw(8) << std::hex << *p << " ";
                if ( ++cnt == 10 ) {
                  info << "  Data:" << s.str() << endmsg;
                  s.str("");
                  cnt = 0;
                }
              }
              if ( cnt > 0 ) info << "  Data:" << s.str() << endmsg;
            }
            if( chk ) { // Check the patterns put in by RawEventCreator
              int kc = k;
              int ks = k+1;
              if ( r->type() != RawBank::DAQ )  {
                if ( r->size() != inc*ks )  {
                  info << "Bad bank size:" << r->size() << " expected:" << ks*inc << endmsg;
                }
                if ( r->sourceID() != kc )  {
                  info << "Bad source ID:" << r->sourceID() << " expected:" << kc << endmsg;
                }
                for(p=r->begin<int>(), cnt=0; p != r->end<int>(); ++p, ++cnt)  {
                  if ( *p != cnt )  {
                    info << "Bad BANK DATA:" << *p << endmsg;
                  }
                }
                if ( cnt != (inc*ks)/int(sizeof(int)) )  {
                  info << "Bad amount of data in bank:" << cnt << " word" << endmsg;
                }
              }
            }
          }
        }
      }
      m_numEvent++;
      return StatusCode::SUCCESS;
    }
  };
}

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventTestDump)
