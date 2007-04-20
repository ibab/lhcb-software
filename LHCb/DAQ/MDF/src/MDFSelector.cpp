// $Id: MDFSelector.cpp,v 1.11 2007-04-20 12:40:25 cattanem Exp $
//====================================================================
//	MDFSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/MDFHeader.h"
#include "MDF/MDFIO.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataSelector.h"

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
    StreamBuffer         m_buff;
  public:
    /// Standard constructor
    MDFContext(const RawDataSelector* sel,bool ignoreChcksum) 
    : RawDataSelector::LoopContext(sel),MDFIO(MDFIO::MDF_RECORDS,sel->name())
    { setIgnoreChecksum(ignoreChcksum);                        }
    /// Standard destructor 
    virtual ~MDFContext()                                    { }
    /// Allocate buffer space for reading data
    std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len);
      return std::pair<char*,int>(m_buff.data(),m_buff.size());
    }
    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const ioDesc, void* const data, size_t len)  {
      DSC::Access* ent = (DSC::Access*)ioDesc;
      if ( ent && ent->ioDesc > 0 ) {
        if ( StreamDescriptor::read(*ent,data,len) )  {
          return StatusCode::SUCCESS;
        }
      }
      return StatusCode::FAILURE;
    }
    /// Receive event and update communication structure
    virtual StatusCode receiveData(IMessageSvc* msg)  {
      m_fileOffset = StreamDescriptor::seek(m_accessDsc,0,SEEK_CUR);
      setupMDFIO(msg,0);
      m_data = readBanks(&m_accessDsc, 0 == m_fileOffset);
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
    StatusCode skipRecord()  {
      MDFHeader h;
      StatusCode sc = readBuffer(&m_accessDsc, &h, 3*sizeof(int));
      if ( sc.isSuccess() )  {
        int len = h.recordSize()-3*sizeof(int);
        m_fileOffset = StreamDescriptor::seek(m_accessDsc,len,SEEK_CUR);
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
      char c = m_ignoreChecksum[0];
      refpCtxt = new MDFContext(this,c=='y'||c=='Y');
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

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFSelector);
