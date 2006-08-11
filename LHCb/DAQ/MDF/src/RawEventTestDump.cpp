// $Id: RawEventTestDump.cpp,v 1.3 2006-08-11 09:29:44 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "MDF/MDFHeader.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  struct RawEventPrintout  {

    static std::string bankHeader(const RawBank* r)  {
      std::stringstream s; 
      s << "Size:"    << std::setw(4) << int(r->size()) 
        << " Type:"   << std::setw(2) << int(r->type())
        << ":"        << std::setw(5) << bankType(r->type())
        << " Source:" << std::setw(3) << int(r->sourceID())
        << " Vsn:"    << std::setw(2) << int(r->version()) 
        << " "        << std::hex << r->magic();
      return s.str();
    }
    static std::string bankType(const RawBank* r)  {
      if ( r ) return bankType(r->type());
      return "BAD_BANK";
    }
    static std::string bankType(int i)  {
#define PRINT(x)  case RawBank::x : return #x;
      switch(i)  {
        PRINT(L0Calo);
        PRINT(L0DU);
        PRINT(PrsE);
        PRINT(EcalE);
        PRINT(HcalE);
        PRINT(PrsTrig);
        PRINT(EcalTrig);
        PRINT(HcalTrig);
        PRINT(Velo);
        PRINT(Rich);
        PRINT(TT);
        PRINT(IT);
        PRINT(OT);
        PRINT(Muon);
        PRINT(L0PU);
        PRINT(DAQ);
        PRINT(ODIN);
        PRINT(HLT);
        PRINT(VeloFull);
        PRINT(TTFull);
        PRINT(ITFull);
        PRINT(EcalPacked);
        PRINT(HcalPacked);
        PRINT(PrsPacked);
        PRINT(L0Muon);
        PRINT(ITError);
        PRINT(TTError);
        PRINT(ITPedestal);
        PRINT(TTPedestal);
        PRINT(VeloError);
        PRINT(VeloPedestal);
        PRINT(VeloProcFull);
        default: return "UNKNOWN";
#undef PRINT
      }
      return "UNKNOWN";
    }
  };

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
    int  m_debug; ///< Number of events where all dump flags should be considered true
    int  m_numEvent;  ///< Event counter
  public: 
    /// Standard constructor
    RawEventTestDump( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_numEvent(0)
    {
      declareProperty( "DumpData",  m_dump = false );
      declareProperty( "FullDump",  m_full = false );
      declareProperty( "CheckData", m_check = false );
      declareProperty( "Debug",     m_debug = 0 );
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
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      MsgStream info(msgSvc(),name());
      bool dmp = m_numEvent<m_debug || m_dump;
      bool chk = m_numEvent<m_debug || m_check;
      bool ful = m_numEvent<m_debug || m_full;

      info << MSG::INFO;
      for(int j=0; j<RawBank::LastType; ++j)  {
        RawBank::BankType i = RawBank::BankType(j);
        const std::vector<RawBank*>& b = raw->banks(i);
        int cnt, inc = (i == RawBank::Rich) ? 64 : 32;
        const int *p;
        if ( b.size() > 0 )  {
          if ( dmp )  {
            info << b.size() << " banks of type " << i 
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
              int cnt = 0;
              std::stringstream s;
              for(const int* p=r->begin<int>(); p != r->end<int>(); ++p)  {
                s << std::hex << std::setw(8) << (void*)*p << " ";
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

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventTestDump)
