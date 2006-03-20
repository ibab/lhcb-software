// $Id: RawEventTestDump.cpp,v 1.2 2006-03-20 15:37:53 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RawEventTestDump RawEventTestDump.h tests/RawEventTestDump.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RawEventTestDump : public Algorithm {
    bool m_full;  ///< Property "FullDump". If true, full bank contents are dumped
    bool m_dump;  ///< Property "DumpData". If true, full bank contents are dumped
    bool m_check; ///< Property "CheckData". If true, full bank contents are checked
  public: 
    /// Standard constructor
    RawEventTestDump( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator) {
      declareProperty( "DumpData",  m_dump = false );
      declareProperty( "FullDump",  m_full = false );
      declareProperty( "CheckData", m_check = false );
    }

    /// Destructor
    virtual ~RawEventTestDump()  {} 

    /// Main execution
    virtual StatusCode execute()  {
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      MsgStream info(msgSvc(),name());
      info << MSG::INFO;
      for(int j=0; j<RawBank::LastType; ++j)  {
        RawBank::BankType i = RawBank::BankType(j);
        const std::vector<RawBank*>& b = raw->banks(i);
        if ( b.size() > 0 )  {
          if ( m_dump ) info << b.size() << " banks of type " << i << ": [size, source, version, magic]";
          std::vector<RawBank*>::const_iterator itB;
          int k = 0;
          for( itB = b.begin(); itB != b.end(); itB++ ) {
            if ( ((k++)%4) == 0 && m_dump ) info << endmsg << "  " ;
            const RawBank* r = *itB;
            if ( m_dump ) info << "[" << int(r->size()) << ", "
                  << int(r->sourceID()) << ", "
                  << int(r->version()) << ", "
                  << std::hex << r->magic() << "] ";
            if( m_full ) {
              info << "Data follows...";
              int cnt = 0;
              for(const int* p=r->begin<int>(); p != r->end<int>(); ++p)  {
                if ( ((cnt++)%10) == 0 ) info << endmsg << "   ... " ;
                info << "[" << *p << "] ";
              }
              info << std::dec << endmsg << "  ";
            }
          }
          if ( m_dump ) info << endmsg;
          if( m_check ) { // Check the patterns put in by RawEventCreator
            int cnt, inc = i == RawBank::DAQ ? 64 : 32;
            const int *p;
            for( itB = b.begin(), k = 1; itB != b.end(); itB++, k++ ) {
              const RawBank* r = *itB;
              if ( r->size() != inc*k )  {
                info << "Bad bank size:" << r->size() << endmsg;
              }
              if ( r->sourceID() != k-1 )  {
                info << "Bad source ID:" << r->sourceID() << endmsg;
              }
              for(p=r->begin<int>(), cnt=0; p != r->end<int>(); ++p, ++cnt)  {
                if ( *p != cnt )  {
                  info << "Bad BANK DATA:" << *p << endmsg;
                }
              }
              if ( cnt != (inc*k)/int(sizeof(int)) )  {
                info << "Bad amount of data in bank:" << cnt << " word" << endmsg;
              }
            }
          }
        }
      }

      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventTestDump)
