// $Id: MDFSelector.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
//====================================================================
//  MDFSelector.cpp
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
#include "MDF/RawDataSelector.h"
#include "Event/RawBank.h"
#include "GaudiUtils/IIODataManager.h"

// C++ include files
#include <cctype>

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MDFContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class MDFContext : public RawDataSelector::LoopContext, protected MDFIO {
    /// I/O Buffer holder
    StreamBuffer         m_buff;
  public:
    /// Standard constructor
    MDFContext(const RawDataSelector* sel,bool ignoreChcksum) 
    : RawDataSelector::LoopContext(sel),MDFIO(MDFIO::MDF_RECORDS,sel->name())
    { setIgnoreChecksum(ignoreChcksum);                        }
    /// Standard destructor 
    virtual ~MDFContext()                                    { }
    /// Allocate buffer space for reading data
    MDFDescriptor getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len+m_sel->additionalSpace());
      return MDFDescriptor(m_buff.data(),m_buff.size());
    }
    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const /* ioDesc */, void* const data, size_t len)  {
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

  /** @class MDFSelector
    */
  class MDFSelector : public RawDataSelector  {
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
      refpCtxt = new MDFContext(this,c=='Y'||c=='T'); // YES or TRUE
      if ( !m_input.empty() )  {
	StatusCode sc = resetCriteria(m_input,*refpCtxt);
	if ( !sc.isSuccess() ) {
	  delete refpCtxt;
	  refpCtxt = 0;
	}
	return sc;
      }
      return StatusCode::SUCCESS;
    }
    /// Service Constructor
    MDFSelector( const std::string& nam, ISvcLocator* svcloc )
    : RawDataSelector( nam, svcloc)  {
      declareProperty("IgnoreChecksum",m_ignoreChecksum="NO");
    }
    /// Standard destructor
    virtual ~MDFSelector()  {}
  };
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFSelector)
