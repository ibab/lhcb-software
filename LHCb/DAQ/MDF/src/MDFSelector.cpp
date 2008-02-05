// $Id: MDFSelector.cpp,v 1.16 2008-02-05 16:44:18 frankb Exp $
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
      m_buff.reserve(len);
      return MDFDescriptor(m_buff.data(),m_buff.size());
    }
    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const /* ioDesc */, void* const data, size_t len)  {
      return m_ioMgr->read(m_connection, data, len);
    }
    /// Receive event and update communication structure
    virtual StatusCode receiveData(IMessageSvc* msg)  {
      m_fileOffset = m_ioMgr->seek(m_connection,0,SEEK_CUR);
      setupMDFIO(msg,0);
      m_data = readBanks(m_connection, 0 == m_fileOffset);
      if ( m_data.second > 0 )  {
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
      char c = ::toupper(m_ignoreChecksum[0]);
      refpCtxt = new MDFContext(this,c=='Y'||c=='T'); // YES or TRUE
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

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFSelector);
