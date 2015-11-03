// $Id: RawDataWriter.cpp,v 1.15 2009-04-17 13:24:36 cattanem Exp $
//  ====================================================================
//  RawDataWriter.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/RawDataWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "TMD5.h"
#include <ctime>
#include <limits>
#include <memory>

enum { KBYTE=1024, MBYTE=1048576, GBYTE=1073741824 };

using namespace LHCb;
using namespace Gaudi;
using namespace std;

/// Initializing constructor
RawDataFile::RawDataFile(Gaudi::IIODataManager* m, IInterface* owner, const string& fname, bool md5, unsigned int run_no, unsigned int orb)
: m_bytesWritten(0), m_name(fname), m_runNumber(run_no), 
  m_firstOrbit(orb), m_lastOrbit(numeric_limits<unsigned int>::max()),
  m_closeStamp(0), m_eventCounter(0), m_ioMgr(m), m_owner(owner)
{
  m_connection = 0;
  m_md5 = md5 ? new TMD5 : 0;
}

/// Standard destructor
RawDataFile::~RawDataFile()  {
  if ( m_md5 ) delete ((TMD5*)m_md5);
}

/// Write byte buffer to output stream
StatusCode RawDataFile::writeBuffer(const void* data, size_t len)  {
  StatusCode sc = m_ioMgr->write(m_connection,data,len);
  if ( sc.isSuccess() )   {
    // Update MD5 checksum:
    if ( m_md5 )  {
      ((TMD5*)m_md5)->Update((const unsigned char*)data, len);
    }
    // Update statistics
    m_bytesWritten += len;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Open file
StatusCode RawDataFile::open()   {
  m_connection = new RawDataConnection(m_owner,m_name);
  return m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
}

/// Close file
StatusCode RawDataFile::close()    {
  if ( m_md5 )  {
    TMD5* s = (TMD5*)m_md5;
    s->Final();
    m_md5Sum = s->AsString();
  }
  m_ioMgr->disconnect(m_connection);
  delete m_connection;
  return StatusCode::SUCCESS;
}
/// Allocate space for IO buffer
pair<char*,int> RawDataFile::getDataSpace(size_t len)  {
  m_data.reserve(len);
  return pair<char*,int>(m_data.data(), m_data.size());
}

/// Set last valid orbit. Only lower orbits are still accepted
void RawDataFile::setLastOrbit(unsigned int orbit)   {  
  m_lastOrbit = orbit; 
  m_closeStamp = time(0); 
}

/// Standard algorithm constructor
RawDataWriter::RawDataWriter(const string& nam, ISvcLocator* pSvc)
  : Algorithm(nam, pSvc), MDFIO(MDFIO::MDF_RECORDS, nam), m_bytesWritten(0), m_fileNo(0), m_ioMgr(0)
{
  declareProperty("MbytesPerFile",  m_MbytesPerFile);               // kBytes to be written per file
  declareProperty("Volume",         m_volume="/tmp");
  declareProperty("Stream",         m_stream="MDF");
  declareProperty("Connection",     m_connect="Unknown");
  declareProperty("Compress",       m_compress=2);                  // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);               // Generate checksum
  declareProperty("GenerateMD5",    m_genMD5=false);                // Generate MD5 checksum
  declareProperty("CloseTimeout",   m_closeTMO=0);                  // Timeout before really closing the file
  declareProperty("DataType",       m_dataType=MDFIO::MDF_RECORDS); // Input data type
  declareProperty("BankLocation",   m_bankLocation=RawEventLocation::Default);  // Location of the banks in the TES
  declareProperty("DataManager",    m_ioMgrName="Gaudi::IODataManager/IODataManager");   // Name of IO manager service
}

/// Initialize the algorithm.
StatusCode RawDataWriter::initialize()   {
  m_connectParams = m_volume + m_connect;
  setupMDFIO(msgSvc(), eventSvc());
  // Retrieve conversion service handling event iteration
  StatusCode status = service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgr << endmsg;
    return status;
  }
  return status;
}

/// Finalize
StatusCode RawDataWriter::finalize() {
  for(Connections::iterator i=m_connections.begin(); i != m_connections.end(); ++i)  {
    (*i)->close();
    submitRunDbCloseInfo(*i, true);
    delete (*i);
  }
  m_connections.clear();
  if ( m_ioMgr )  {
    m_ioMgr->release();
    m_ioMgr = 0;
  }
  return StatusCode::SUCCESS;
}

/// Allocate space for IO buffer
pair<char*,int> RawDataWriter::getDataSpace(void* const ioDesc, size_t len)  {
  RawDataFile* f = (RawDataFile*)ioDesc;
  return f->getDataSpace(len);
}

/// Write byte buffer to output stream
StatusCode RawDataWriter::writeBuffer(void* const ioDesc, const void* data, size_t len)    {
  RawDataFile* f = (RawDataFile*)ioDesc;
  return f->writeBuffer(data, len);
}

/// Access output file according to runnumber and orbit
RawDataFile* RawDataWriter::outputFile(unsigned int run_no, unsigned int orbit)  {
  unsigned int the_first_orb = 0;
  for(Connections::iterator i=m_connections.begin(); i != m_connections.end(); ++i)  {
    unsigned int frst = (*i)->firstOrbit();
    unsigned int last = (*i)->lastOrbit();
    if ( the_first_orb < frst ) the_first_orb = frst;
    if ( orbit >= frst && orbit < last )  {
      return *i;
    }
  }
  if ( the_first_orb < orbit || m_connections.empty() )   {
    static unsigned int last_run_no = 0;
    char txt[32];
    string filePath = m_connectParams;
    size_t idx;

    if ( last_run_no != run_no )  {
      last_run_no = run_no;
      m_fileNo = 0;
    }
    idx = filePath.find("%STREAM");
    while ( idx != string::npos )  {
      filePath.replace(idx,7,m_stream.c_str());
      idx = filePath.find("%STREAM");
    }
    sprintf(txt,"%03d",m_fileNo++);
    idx = filePath.find("%FNO");
    while ( idx != string::npos )  {
      filePath.replace(idx,4,txt);
      idx = filePath.find("%FNO");
    }
    sprintf(txt,"Run%08d",int(run_no));
    idx = filePath.find("%RNO");
    while ( idx != string::npos )  {
      filePath.replace(idx,4,txt);
      idx = filePath.find("%RNO");
    }
    std::unique_ptr<RawDataFile> f(new RawDataFile(m_ioMgr,this,filePath,m_genMD5,run_no,orbit));
    if ( f->open().isSuccess() )  {
      /// register file with run database
      if ( !submitRunDbOpenInfo(f.get(), false).isSuccess() )  {
        MsgStream log(msgSvc(), name());
        log << MSG::ERROR << "Failed to register file:" << filePath 
            << " with run database." << endmsg;
        return nullptr;
      }
      m_connections.push_back(f.release());
      return m_connections.back();
    }
  }
  else  {
    // This means the event is soooooo late that the file already was closed...
  }
  return nullptr;
}

// Submit information to register file to run database
StatusCode RawDataWriter::submitRunDbOpenInfo(RawDataFile* f, bool /* blocking */ )  {
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Opened Stream:" << f->name() 
      << " Run:" << f->runNumber() 
      << " First orbit:" << f->firstOrbit() << endmsg;
  return StatusCode::SUCCESS;
}

// Submit information about closed file to run database
StatusCode RawDataWriter::submitRunDbCloseInfo(RawDataFile* f, bool /* blocking */ )  {
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Closed Stream:" << f->name()
      << " MD5:" << f->md5Sum()
      << " Run:" << f->runNumber() 
      << " First/Last orbit:" << f->firstOrbit() << "/" << f->lastOrbit() << endmsg;
  return StatusCode::SUCCESS;
}

/// Execute procedure
StatusCode RawDataWriter::execute()    {
  StatusCode sc = StatusCode::FAILURE;
  MDFHeader* hdr = getHeader();
  MDFHeader::SubHeader h = hdr->subHeader();
  RawDataFile* f = outputFile(h.H1->runNumber(),h.H1->orbitNumber());
  if ( f )  {
    switch(m_dataType)  {
    case MDF_NONE:    // Pure RawEvent structure with MDF Header encoded as bank
      sc = commitRawBanks(m_compress, m_genChecksum, f, m_bankLocation);
      break;
    case MDF_RECORDS: // Ready to write MDF records...
    case MDF_BANKS:   // Ready to write banks structure with first bank containing MDF header...
      sc = commitRawBuffer(m_dataType, m_compress, m_genChecksum, f);
      break;
    }
    if ( sc.isSuccess() )  {
      /// Mark the file to be used only for event, which were taken _before_ this one.
      if ( f->bytesWritten() > MBYTE*m_MbytesPerFile )  {
        if ( f->lastOrbit() == numeric_limits<unsigned int>::max() )  {
          f->setLastOrbit(h.H1->orbitNumber());
        }
      }
      (f->eventCounter())++;
      time_t now = time(0);
      for(Connections::iterator i=m_connections.begin(); i != m_connections.end(); )  {
        f = *i;
        time_t stamp = f->closeStamp();
        if ( stamp != 0 && now-stamp >= m_closeTMO )  {
          /// Now really close the file
          f->close();
          m_connections.erase(i);
          i = m_connections.begin();
          StatusCode iret = submitRunDbCloseInfo(f, false);
          if ( !iret.isSuccess() )  {
            MsgStream log(msgSvc(),name());
            log << MSG::ERROR << "Failed to send close info for file:" << f->name() 
                << " with run database." << endmsg;
            m_connections.push_back(f);
            i = m_connections.begin();
            continue;
          }
          delete f;
        }
        else {
          ++i;
        }
      }
      return sc;
    }
  }
  return sc;
}
