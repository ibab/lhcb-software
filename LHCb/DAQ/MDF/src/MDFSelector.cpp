// $Id: MDFSelector.cpp,v 1.8 2006-09-26 09:24:04 frankb Exp $
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
    typedef std::vector<RawBank*> Banks;
    Banks        m_banks;
    long long    m_fileOffset;
    StreamBuffer m_data;
    size_t       m_dataLen;
  public:
    /// Standard constructor
    MDFContext(const RawDataSelector* pSel,bool ignoreChcksum) 
      : RawDataSelector::LoopContext(pSel), 
        MDFIO(MDFIO::MDF_RECORDS,pSel->name()), 
        m_fileOffset(0), m_dataLen(0)
    { setIgnoreChecksum(ignoreChcksum); }
    /// Standard destructor 
    virtual ~MDFContext()                                    { }

    std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_data.reserve(len);
      return std::pair<char*,int>(m_data.data(), m_data.size());
    }

    /// Read raw byte buffer from input stream
    StatusCode readBuffer(void* const ioDesc, void* const data, size_t len)  {
      DSC::Access* ent = (DSC::Access*)ioDesc;
      if ( ent && ent->ioDesc > 0 ) {
        if ( StreamDescriptor::read(*ent,data,len) )  {
          m_dataLen = len;
          return StatusCode::SUCCESS;
        }
      }
      m_dataLen = 0;
      return StatusCode::FAILURE;
    }
    /// Receive event and update communication structure
    virtual StatusCode receiveData(IMessageSvc* msg)  {
      m_banks.clear();
      m_fileOffset = StreamDescriptor::seek(m_accessDsc,0,SEEK_CUR);
      setupMDFIO(msg,0);
      std::pair<char*,int> res = readBanks(&m_accessDsc, 0 == m_fileOffset);
      if ( res.second > 0 )  {
        return decodeRawBanks(res.first,res.first+res.second,m_banks);
      }
      return StatusCode::FAILURE;
    }
    long long offset()  const                       { return m_fileOffset;            }
    /// Raw data buffer (if it exists)
    virtual const void* data() const                { return m_data.data();           }
    /// Raw data buffer length (if it exists)
    virtual const size_t dataLength() const         { return m_dataLen;               }
    /// Access to RawBank array
    virtual const Banks& banks()  const             { return m_banks;                 }
    /// Access to RawBank array (NON const)
    Banks& banks()                                  { return m_banks;                 }
  };

  /** @class MDFSelector
    */
  class MDFSelector : public RawDataSelector  {
  protected:
    /// Flags to ignore checksum
    std::string m_ignoreChecksum;

  public:

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
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
    : RawDataSelector( nam, svcloc)   
    {
      declareProperty("IgnoreChecksum",m_ignoreChecksum="NO");
    }

    /// Standard destructor
    virtual ~MDFSelector()  {}
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFSelector);
