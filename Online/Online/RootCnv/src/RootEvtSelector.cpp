#if 0
// $Id: RootEvtSelector.cpp,v 1.1 2009-12-15 15:37:25 frankb Exp $
//====================================================================
//	SerializeSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : Serialize
//
//	Author     : M.Frank
//	Author		 : A. Puig
//====================================================================

// Include files
#include "MDF/MDFIO.h"
#include "MDF/MDFHeader.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawDataSelector.h"
#include "GaudiSerialize/SerializeCnvSvc.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class SerializeContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class SerializeContext : public RawDataSelector::LoopContext, protected MDFIO {
    StreamBuffer              m_buff;
    StreamDescriptor::Access  m_accessDsc;
  public:
    /// Standard constructor
    SerializeContext(const RawDataSelector* sel,bool ignoreChcksum) 
    : RawDataSelector::LoopContext(sel),MDFIO(MDFIO::MDF_RECORDS,sel->name())
    { setIgnoreChecksum(ignoreChcksum);                        }
    /// Standard destructor 
    virtual ~SerializeContext()                                    { }
    /// Allocate buffer space for reading data
    std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len);
      return std::pair<char*,int>(m_buff.data(),m_buff.size());
    }
    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const ioDesc, void* const data, size_t len)  {
      StreamDescriptor::Access* ent = (StreamDescriptor::Access*)ioDesc;
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

  /** @class SerializeSelector
    */
  class SerializeSelector : public RawDataSelector  {
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
      refpCtxt = new SerializeContext(this,c=='y'||c=='Y');
      return StatusCode::SUCCESS;
    }
    /// Service Constructor
    SerializeSelector( const std::string& nam, ISvcLocator* svcloc )
    : RawDataSelector( nam, svcloc)  {
      declareProperty("IgnoreChecksum",m_ignoreChecksum="NO");
    }
    /// Standard destructor
    virtual ~SerializeSelector()  {}
    StatusCode createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const {
      const LoopContext* pctxt = dynamic_cast<const LoopContext*>(&ctxt);
      if ( pctxt )   {
	if ( pctxt->data().second > 0 )  {
	  RawDataAddress* pA = new RawDataAddress(SERIALIZE_StorageType,m_rootCLID,pctxt->specs(),"0",0,0);
	  pA->setType(RawDataAddress::DATA_TYPE);
	  pA->setFileOffset(pctxt->offset());
	  pA->setData(pctxt->data());
	  pAddr = pA;
	  return StatusCode::SUCCESS;
	}
      }
      pAddr = 0;
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,SerializeSelector);
#endif
