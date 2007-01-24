// $Id: MIFSelector.cpp,v 1.6 2007-01-24 20:42:24 frankb Exp $
//====================================================================
//	MIFSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MIF
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "GaudiKernel/MsgStream.h"
#include "MDF/MIFHeader.h"
#include "MDF/MDFIO.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataSelector.h"
#include <map>

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {
  /** @class MIFSelector
    */
  class MIFSelector : public RawDataSelector  {
  public:
    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /// Service Constructor
    MIFSelector( const std::string& nam, ISvcLocator* svcloc )
    : RawDataSelector( nam, svcloc)   {     }

    /// Standard destructor
    virtual ~MIFSelector()  {}
  };

  /** @class MIFContext
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  class MIFContext : public RawDataSelector::LoopContext, protected MDFIO {
    long long            m_fileOffset;
    std::pair<char*,int> m_data;
    StreamBuffer         m_buff;
    MIFHeader*           m_header;
    struct MDFMapEntry  {
      std::string                name;
      StreamDescriptor::Access   bind;
      StreamDescriptor::Access   con;
    };
    const MIFSelector*   m_mifSel;
    typedef std::map<int,MDFMapEntry*> FidMap;
    FidMap               m_fidMap;
    int                  m_mifFID;
    IMessageSvc*         m_msg;
  public:
    /// Standard constructor
    MIFContext(const MIFSelector* pSel) 
      : RawDataSelector::LoopContext(pSel), 
        MDFIO(MDFIO::MDF_RECORDS,pSel->name()), 
        m_fileOffset(0), m_mifSel(pSel), m_mifFID(0)
    { 
      m_header = (MIFHeader*)new char[1024];
      m_msg = pSel->messageService();
    }
    /// Standard destructor 
    virtual ~MIFContext();
    std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len);
      return std::pair<char*,int>(m_buff.data(), m_buff.size());
    }
    virtual StatusCode connect(const std::string& spec);
    /// Receive event and update communication structure
    virtual StatusCode receiveData(IMessageSvc* msg);
    /// Skip N events
    virtual StatusCode skipEvents(IMessageSvc* msg,int numEvt);
    /// Read raw byte buffer from input stream
    virtual StatusCode readBuffer(void* const ioDesc, void* const data, size_t len);
    long long offset()  const                       { return m_fileOffset;     }
    /// Raw data buffer (if it exists)
    virtual std::pair<char*,int> data() const       { return m_data;           }
  };

}

/// Create a new event loop context
StatusCode LHCb::MIFSelector::createContext(Context*& refpCtxt) const {
  refpCtxt = new MIFContext(this);
  return StatusCode::SUCCESS;
}

/// Standard destructor 
LHCb::MIFContext::~MIFContext()  { 
  delete [] (char*)m_header;
  for(FidMap::iterator i=m_fidMap.begin(); i!=m_fidMap.end();++i)  {
    MDFMapEntry* ent = (*i).second;
    DSC::close(ent->con);
    DSC::close(ent->bind);
    delete ent;
    (*i).second = 0;
  }
  m_fidMap.clear();
}

/// Read raw byte buffer from input stream
StatusCode 
LHCb::MIFContext::readBuffer(void* const ioDesc, void* const data, size_t len)  
{
  DSC::Access* ent = (DSC::Access*)ioDesc;
  if ( ent && ent->ioDesc > 0 ) {
    if ( StreamDescriptor::read(*ent,data,len) )  {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::MIFContext::connect(const std::string& spec)  {
  int fid = LHCb::genChecksum(1,spec.c_str(),spec.length()+1);
  FidMap::iterator i = m_fidMap.find(fid);
  if ( i == m_fidMap.end() )  {
    StatusCode sc = RawDataSelector::LoopContext::connect(spec);
    if ( sc.isSuccess() )  {
      MDFMapEntry* ent = new MDFMapEntry;
      ent->name = spec;
      ent->bind = m_bindDsc;
      ent->con  = m_accessDsc;
      m_fidMap.insert(std::make_pair(fid,ent));
      m_mifFID = fid;
      return sc;
    }
    MsgStream err(m_msg,"MIFSelector");
    err << MSG::ERROR << "Failed to connect to:" << spec << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
}

/// Receive event and update communication structure
StatusCode LHCb::MIFContext::receiveData(IMessageSvc* msg)  {
  char buff[1024];
  MIFHeader *hdr = (MIFHeader*)buff;
  FidMap::iterator i = m_fidMap.find(m_mifFID);
  setupMDFIO(msg,0);
  m_data.first = 0;
  m_data.second = 0;
  if ( i != m_fidMap.end() )  {
    MDFMapEntry* ent = (*i).second;
Next:
    if ( StreamDescriptor::read(ent->con,hdr,sizeof(int)+2*sizeof(char)) )  {
      if ( StreamDescriptor::read(ent->con,buff+sizeof(int)+2*sizeof(char),hdr->size()) )  {
        FidMap::iterator i = m_fidMap.find(hdr->fid());
        if ( i == m_fidMap.end() )  {
          if ( hdr->type() == MIFHeader::MIF_FID )  {
            int id = m_mifFID;  // need to save MIF FID - will be overwritten in connect()
            StatusCode sc = connect(hdr->data<char>());
            m_mifFID = id;
            if ( !sc.isSuccess() )  {
              return sc;
            }
            goto Next;
          }
          else  {
            MsgStream err(msg,"MIFSelector");
            err << MSG::ERROR << "Unknown input specification:"
                << std::hex << std::setw(8) << hdr->fid() << endmsg;
            return StatusCode::FAILURE;
          }
        }
        ent = (*i).second;
      }
      m_fileOffset = StreamDescriptor::seek(ent->con,0,SEEK_CUR);
      m_data = readBanks(&ent->con, 0 == m_fileOffset);
      if ( m_data.second > 0 )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Skip N events
StatusCode LHCb::MIFContext::skipEvents(IMessageSvc* /* msg */,int numEvt)  {
  for(int i=0; i<numEvt; ++i)  {
  }
  return StatusCode::FAILURE;
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MIFSelector);
