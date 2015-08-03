// $Id: MIFSelector.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
//====================================================================
//  MIFSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : MIF
//
//  Author     : M.Frank
//====================================================================

// Include files
#include "GaudiKernel/MsgStream.h"
#include "MDF/MIFHeader.h"
#include "MDF/MDFIO.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataConnection.h"
#include "MDF/RawDataSelector.h"
#include <map>
#include <memory>

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {
  /** @class MIFSelector
    */
  class MIFSelector : public RawDataSelector  {
  public:
    /** Create a new event loop context
      * @param[in,out] refpCtxt  Reference to pointer to store the context
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
  protected:
    typedef Gaudi::IDataConnection Connection;
    typedef std::map<int,Connection*> FidMap;

    long long            m_fileOffset;
    MDFDescriptor        m_data;
    StreamBuffer         m_buff;
    MIFHeader*           m_header;
    const MIFSelector*   m_mifSel;
    FidMap               m_fidMap;
    int                  m_mifFID;
    IMessageSvc*         m_msg;
  private:
    /// Usage of copy constructor is not allowed
    MIFContext(const MIFContext&)  
      : RawDataSelector::LoopContext(0), 
        MDFIO(MDFIO::MDF_RECORDS,""), m_fileOffset(0), m_mifSel(0), m_mifFID(0)
    {
      m_header = 0;
      m_msg = 0;
    } 
    /// Assignment is not allowed
    MIFContext& operator=(const MIFContext&)  {
      return *this;
    }
  public:
    /// Standard constructor
    MIFContext(const MIFSelector* pSel) 
      : RawDataSelector::LoopContext(pSel), MDFIO(MDFIO::MDF_RECORDS,pSel->name()), 
        m_fileOffset(0), m_mifSel(pSel), m_mifFID(0)
    { 
      m_header = (MIFHeader*)new char[1024];
      m_msg = m_mifSel->messageService();
    }
    /// Standard destructor 
    virtual ~MIFContext();
    MDFDescriptor getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len+m_sel->additionalSpace());
      return MDFDescriptor(m_buff.data(), m_buff.size());
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
    virtual MDFDescriptor data() const              { return m_data;           }
  };
}

using namespace LHCb;
using namespace Gaudi;

/// Create a new event loop context
StatusCode MIFSelector::createContext(Context*& refpCtxt) const {
  refpCtxt = new MIFContext(this);
  return StatusCode::SUCCESS;
}

/// Standard destructor 
MIFContext::~MIFContext()  { 
  delete [] (char*)m_header;
  for(FidMap::iterator i=m_fidMap.begin(); i!=m_fidMap.end();++i)  {
    Connection* c = (*i).second;
    m_ioMgr->disconnect(c);
    delete c;
    (*i).second = 0;
  }
  m_fidMap.clear();
}

/// Read raw byte buffer from input stream
StatusCode 
MIFContext::readBuffer(void* const ioDesc, void* const data, size_t len) {
  return m_ioMgr->read((IDataConnection*)ioDesc,data,len);
}

StatusCode MIFContext::connect(const std::string& spec)  {
  int fid = genChecksum(1,spec.c_str(),spec.length()+1);
  FidMap::iterator i = m_fidMap.find(fid);
  if ( i == m_fidMap.end() )  {
    std::auto_ptr<IDataConnection> c(new RawDataConnection(m_sel,spec));
    StatusCode sc = m_ioMgr->connectRead(false,c.get());
    if ( sc.isSuccess() )  {
      MsgStream log(m_msg,"MIFSelector");
      log << MSG::ALWAYS << "Connected to:" << spec << " " << c->pfn() 
  /* << " " << fid */ 
    << endmsg;
      m_fidMap.insert(std::make_pair(fid,c.release()));
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
StatusCode MIFContext::receiveData(IMessageSvc* msg)  {
  char buff[1024];
  MIFHeader *hdr = (MIFHeader*)buff;
  FidMap::iterator i = m_fidMap.find(m_mifFID);
  setupMDFIO(msg,0);
  m_data.first = 0;
  m_data.second = 0;
  if ( i != m_fidMap.end() )  {
    Connection* c = (*i).second;
Next:
    if ( m_ioMgr->read(c,hdr,sizeof(int)+2*sizeof(char)).isSuccess() )  {
      if ( m_ioMgr->read(c,buff+sizeof(int)+2*sizeof(char),hdr->size()).isSuccess() )  {
        i = m_fidMap.find(hdr->fid());
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
        c = (*i).second;
      }
      m_fileOffset = m_ioMgr->seek(c,0,SEEK_CUR);
      m_data = readBanks(c, 0 == m_fileOffset);
      if ( m_data.second > 0 )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Skip N events
StatusCode MIFContext::skipEvents(IMessageSvc* /* msg */,int numEvt)  {
  for(int i=0; i<numEvt; ++i)  {
  }
  return StatusCode::FAILURE;
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MIFSelector)
