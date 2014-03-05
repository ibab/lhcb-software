// $Id: MDFEvtSelector.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
//====================================================================
//  MDFEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : MDF
//
//  Author     : M.Frank
//====================================================================

// Framework include files
#include "MDF/MDFIO.h"
#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"
#include "OnlineFileSelector/TestEvtSelector.h"
#include "Event/RawBank.h"
#include "GaudiUtils/IIODataManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"


// C++ include files
#include <cctype>
#include <iostream>
/*
 *  LHCb namespace declaration
 */
using namespace std;

namespace LHCb  {

  /** @class MDFContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class MDFPollerContext : public TestEvtSelector::LoopContext, protected MDFIO {
    /// I/O Buffer holder
    StreamBuffer         m_buff;
  public:
    /// Standard constructor
    MDFPollerContext(const TestEvtSelector* sel,bool ignoreChcksum) 
    : TestEvtSelector::LoopContext(sel),MDFIO(MDFIO::MDF_RECORDS,sel->name())
    { setIgnoreChecksum(ignoreChcksum);                        }
    /// Standard destructor 
    virtual ~MDFPollerContext()                                    { }
    /// Allocate buffer space for reading data
    MDFDescriptor getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len+m_sel->additionalSpace());
      return MDFDescriptor(m_buff.data(),m_buff.size());
    }
    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const /* ioDesc */, void* const data, size_t len)   {
      return m_ioMgr->read(m_connection, data, len);
    }
    /// Receive event and update communication structure
    virtual StatusCode receiveData(IMessageSvc* msg)  {
    Next:
      m_fileOffset = m_ioMgr->seek(m_connection,0,SEEK_CUR);
      setupMDFIO(msg,0);
      m_data = readBanks(m_connection, 0 == m_fileOffset);
      if ( m_data.second > 0 )  {
	/// Check if trigger and/or veto masks should be processed
	if ( m_trgMask || m_vetoMask ) {
	  RawBank* b = (RawBank*)m_data.first;
	  MDFHeader* h = b->begin<MDFHeader>();
	  const unsigned int* msk = h->subHeader().H1->triggerMask();
	  if ( m_vetoMask ) {
	    for(size_t i=0; i<4 && i<m_vetoMask->size(); ++i)
	      if ( (m_vetoMask->at(i)&msk[i]) != 0 ) goto Next;
	  }
	  if ( m_trgMask ) {
	    for(size_t i=0; i<4 && i<m_trgMask->size(); ++i)
	      if ( (m_trgMask->at(i)&msk[i]) != 0 ) return StatusCode::SUCCESS;
	  }
	  goto Next;
	}
        return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
    }
    /// Receive event and update communication structure
    virtual StatusCode skipEvents(IMessageSvc* msg, int numEvt)  {
      StatusCode sc = StatusCode::SUCCESS;
      setupMDFIO(msg,0);
      for(int i=0; i<numEvt; ++i)  {
        sc = skipRecord();
        if ( !sc.isSuccess() ) break;
      }
      return sc;
    }
    /// Skip a single record in the file
    StatusCode skipRecord()  {
      MDFHeader h;
      StatusCode sc = m_ioMgr->read(m_connection, &h, 3*sizeof(int));
      if ( sc.isSuccess() )  {
        int len = h.recordSize()-3*sizeof(int);
        m_fileOffset = m_ioMgr->seek(m_connection,len,SEEK_CUR);
        if ( m_fileOffset == -1 ) sc = StatusCode::FAILURE;
      }
      return sc;
    }
  };

  /** @class MDFEvtSelector
    */
  class MDFOnlineEvtSelector : public TestEvtSelector
                         
  {
  protected:
    /// Flags to ignore checksum
    std::string m_ignoreChecksum;

  public:

   /** Create a new event loop context
      * @param[in,out] refpCtxt   Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const {
      int c = ::toupper(m_ignoreChecksum[0]);
      refpCtxt = new MDFPollerContext(this,c=='Y'||c=='T'); // YES or TRUE
      setCurContext(refpCtxt);
      
      TestEvtSelector::m_firstConnection = 0;
      
      
      return StatusCode::SUCCESS;
    }


    /// Service Constructor
    MDFOnlineEvtSelector( const std::string& nam, ISvcLocator* svcloc )
    : TestEvtSelector( nam, svcloc)  {
      declareProperty("IgnoreChecksum",m_ignoreChecksum="NO");
    }
    /// Standard destructor
    virtual ~MDFOnlineEvtSelector()  {}
  };
 }


DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFOnlineEvtSelector)
