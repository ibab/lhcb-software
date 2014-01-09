#ifdef BUILD_WRITER

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Writer/Connection.h"
#include "Writer/ReprocessingWriter.h"
#include "Writer/Utils.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "CPP/TimeSensor.h"
#include "TMD5.h"

#include <stdexcept>

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,ReprocessingWriter)

using namespace std;
using namespace LHCb;

template <class T> void _delete(T& p) {
  if ( p ) delete p;
  p = 0;
}

/**
 * Macro for initialising a close command.
 */
/*#define INIT_CLOSE_COMMAND(h, fname, adler_32, md_5, seqno, rno, thesize, noofevents, noofphysstat, trgevents) { \
*/
#define INIT_CLOSE_COMMAND(h, fname, seqno, rno, thesize) { \
    (h)->cmd = CMD_CLOSE_FILE; \
    (h)->run_no = rno;  \
    (h)->data.chunk_data.seq_num = seqno; \
    (h)->data.stop_data.size = thesize; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising a close command pdu.
 */
#define INIT_CLOSE_PDU(p, adler_32, md_5, noofevents, noofphysstat, trgevents, statevents) { \
    (p)->adler32_sum = (adler_32); \
    (md_5)->Final((p)->md5_sum); \
    (p)->events = noofevents; \
    (p)->physStat = noofphysstat; \
    memcpy((p)->trgEvents, (trgevents), MAX_TRIGGER_TYPES*sizeof(unsigned int)); \
    memcpy((p)->statEvents, (statevents), MAX_STAT_TYPES*sizeof(unsigned int)); \
}


/// Macro for initialising a write command.
#define INIT_WRITE_COMMAND(h, len, off, fname, seqno, rno) { \
  (h)->cmd = CMD_WRITE_CHUNK; \
    (h)->data.chunk_data.size = (len); \
    (h)->run_no = rno;	\
    (h)->data.chunk_data.offset = (off); \
    (h)->data.chunk_data.seq_num = seqno; \
    ::strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/// Macro for initialising an open command.
#define INIT_OPEN_COMMAND(h, fname, seqno, rno) { \
  (h)->cmd = CMD_OPEN_FILE; \
    ::strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
    (h)->data.chunk_data.seq_num = seqno; \
    (h)->run_no = rno;	\
}

/// Standard algorithm constructor
ReprocessingWriter::ReprocessingWriter(const string& nam, ISvcLocator* pSvc)
  : MDFWriter(nam, pSvc), m_bufferLength(0), m_currFile(0), 
    m_srvConnection(0), m_rpcObj(0), m_log(0), m_flLock(0), m_cancel(false)
{
  // This should ideally be provided as a name and not an
  // IP address, so that the round-robin DNS can kick in.
  declareProperty("StorageServerAddr",     m_serverAddr="");
  declareProperty("StorageServerPort",     m_serverPort=45247);
  declareProperty("RunDBServiceURL",       m_runDBURL="");
  declareProperty("MaxFileSizeMB",         m_maxFileSizeMB=20000);
  declareProperty("SndRcvSizes",           m_sndRcvSizes=6553600);
  declareProperty("RunFileTimeoutSeconds", m_runFileTimeoutSeconds=30);
  declareProperty("RunFileKeepTimeout",    m_runFileKeepTimeout=1800);
  declareProperty("MaxMemBufferMB",        m_memBufferLen=1024);
  declareProperty("MaxQueueSizeBytes",     m_maxQueueSizeBytes=1073741824);
  ::lib_rtl_create_lock(0,&m_flLock);
}

/// Destructor: Checks if a file is open, and closes it before closing the connection.
ReprocessingWriter::~ReprocessingWriter()   {
  if ( m_srvConnection )  {
    finalize();
  }
  m_cancel = false;
  _delete(m_log);
  ::lib_rtl_delete_lock(m_flLock);
}

/// Algorithm::execute() override, delegates to MDFWriter::execute().
StatusCode ReprocessingWriter::execute()   {
  StatusCode sc = MDFWriter::execute();
  if ( m_currFile ) {
    if ( sc.isSuccess() ) m_currFile->incEvents();
    // Close file if it exceeded file size limit
    if (m_currFile->getBytesWritten() > (m_maxFileSizeMB << 20)) {
      RTL::Lock file_list_lock(m_flLock);
      closeFile(m_currFile);
      m_openFiles.removeFile(m_currFile);
      _delete(m_currFile);
    }
  }
  return sc;
}

/// Close files on request
void ReprocessingWriter::closeFiles(bool force) {
  File* tmp, *toDelete;
  RTL::Lock file_list_lock(m_flLock);
  for(tmp = m_openFiles.getFirstFile(); tmp; )   {
    if( force || tmp->getTimeSinceLastWrite() > m_runFileTimeoutSeconds) {
      if ( tmp == m_currFile ) m_currFile = 0;
      *m_log << MSG::INFO << "Close/retire file:" << *(tmp->getFileName()) << endmsg;
      toDelete = tmp;
      closeFile(tmp);
      tmp = m_openFiles.removeFile(tmp);
      m_parkFiles.addFile(toDelete);
      continue;
    }
    tmp = tmp->getNext();
  }
  for(tmp = m_parkFiles.getFirstFile(); tmp; )   {
    toDelete = (force || tmp->getTimeSinceLastWrite()>m_runFileKeepTimeout) ? tmp : 0;
    if ( toDelete ) {
      if(tmp->getTimeSinceLastWrite() > m_runFileTimeoutSeconds) {
	toDelete = tmp;
	*m_log << MSG::INFO << "Remove file:" << *(tmp->getFileName()) << endmsg;
	tmp = m_parkFiles.removeFile(tmp);
	delete toDelete;
	continue;
      }
    }
    tmp = tmp->getNext();
  }
}

/// Close file if it is open, and writes its entry in the Run Database.
StatusCode ReprocessingWriter::finalize(void)   {
  TimeSensor::instance().remove(this,0);
  // Unconditionally close all open files
  closeFiles(true);
  // closeConnection() blocks till everything is flushed.
  m_srvConnection->closeConnection();
  _delete(m_srvConnection);
  _delete(m_rpcObj);
  _delete(m_log);
  m_currFile = 0;
  return StatusCode::SUCCESS;
}

/// Overrides MDFWriter::initialize(). Initialises the Connection object.
StatusCode ReprocessingWriter::initialize(void)   {
  _delete(m_log);
  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::INFO << "Writer " << getpid() << " Initializing." << endmsg;

  m_cancel = false;
  m_currFile = 0;
  m_bufferLength = 0;
  m_srvConnection = new Connection(m_serverAddr, 
				   m_serverPort,
				   m_sndRcvSizes,
				   m_log,
				   this,
				   m_maxQueueSizeBytes);
  m_rpcObj = new RPCComm(m_runDBURL.c_str());
  *m_log << MSG::INFO << "rundb url: " << m_runDBURL.c_str() <<endmsg;
  try {
    m_srvConnection->initialize();
  }
  catch(const exception& e) {
    *m_log << MSG::ERROR << "Writer " << getpid() << " Caught Exception:" << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_runFileTimeoutSeconds > 0 ) {
    *m_log << "Writer " << getpid() << " Started timer to close files." << endmsg;
    TimeSensor::instance().add(this,m_runFileTimeoutSeconds,0);
  }
  *m_log << "Writer " << getpid() << " Initialized." << endmsg;
  return StatusCode::SUCCESS;
}

/// Interactor overload: handle
void ReprocessingWriter::handle(const Event& ev) {
  switch(ev.eventtype) {
  case TimeEvent:
    closeFiles(false);
    TimeSensor::instance().add(this,m_runFileTimeoutSeconds,0);
    break;
  default:
    break;
  }
}

/// IIncidentlistern overload
void ReprocessingWriter::handle(const Incident& incident) {
  *m_log << MSG::INFO << "Incident:" << incident.source() << ": " << incident.type() << endmsg;
  if ( incident.type() == "DAQ_CANCEL" ) m_cancel = true;
}

/// Queues a command that creates a file.
File* ReprocessingWriter::createAndOpenFile(unsigned int runNumber)    {
  // The RunDb generates file names now
  string fname = m_rpcObj->createNewFile(runNumber);
  struct cmd_header header;
  *m_log << MSG::INFO << "RUN:" << std::dec << runNumber << ": new file " << fname << endmsg;
  File* currFile = new File(fname, runNumber);

  ::memset(&header, 0, sizeof(struct cmd_header));
  INIT_OPEN_COMMAND(&header, currFile->getFileName()->c_str(),
                    currFile->getSeqNum(), runNumber);
  m_srvConnection->sendCommand(&header);
  currFile->open();
  currFile->incSeqNum();
  // it is likely that there have been events before the file was created
  // so copy the actual value of events to the file monitor
  // from now the events will be counted in the monitor
  currFile->setEvents(0);
  return currFile;
}

/// Queues a command that closes a file.
void ReprocessingWriter::closeFile(File *f)   {
  struct cmd_header header;
  struct cmd_stop_pdu pdu;
  ::memset(&header, 0, sizeof(struct cmd_header));
  ::memset(&pdu, 0, sizeof(struct cmd_stop_pdu));
  *m_log << MSG::INFO << "RUN:" << std::dec << f->getRunNumber()
	 << " ...closing file:" << *(f->getFileName()) << endmsg;

  *m_log << MSG::ERROR << "RUN:" << std::dec << f->getRunNumber()
	 << " ...closing file:" << *(f->getFileName()) << endmsg;

  unsigned int trgEvents[MAX_TRIGGER_TYPES];
  if(f->getTrgEvents(trgEvents, MAX_TRIGGER_TYPES) != 0) {
    *m_log << MSG::ERROR << WHERE << "Error getting the triggered event statistics" << endmsg;
  }

  unsigned int statEvents[MAX_STAT_TYPES];
  if(f->getStatEvents(statEvents, MAX_STAT_TYPES) != 0) {
    *m_log << MSG::ERROR << WHERE << "Error getting the routed event statistics" << endmsg;
  }


  INIT_CLOSE_COMMAND(&header,
                     f->getFileName()->c_str(),
                     f->getSeqNum(),
                     f->getRunNumber(),
		     f->getBytesWritten());
  
  INIT_CLOSE_PDU(&pdu,
		 f->getAdlerChecksum(),
		 f->getMD5Checksum(),
		 f->getEvents(),
		 f->getPhysStat(),
		 trgEvents,
		 statEvents);
  
  m_srvConnection->sendCommand(&header, &pdu);
  *m_log << MSG::INFO << "RUN:" << std::dec << f->getRunNumber()
	 << " Successfully closed file:" << *(f->getFileName()) << endmsg;
}

/* Writes out the buffer to the socket through the Connection object.
 * This function first checks if a new file needs to be created. After
 * that, it writes data out to the new file. The MD5 and Adler32 checksums
 * are computed incrementally for the current file. In case the file is above
 * the maximum size limit, then a RunDB call is executed, to which the
 * file name, MD5 sum, and the Adler32 sum are supplied.
 */
StatusCode ReprocessingWriter::writeBuffer(void *const /*fd*/, const void *data, size_t len)  {
  struct cmd_header header;
  if ( len < sizeof(MDFHeader) ) {
    *m_log << MSG::FATAL
           << "Very small message received, not forwarding. Length is: " << len
           << endmsg;
    return StatusCode::SUCCESS;
  }
  unsigned int runNumber = getRunNumber(data, len);
  RTL::Lock file_list_lock(m_flLock);
  if( m_currFile == 0 || runNumber != m_currFile->getRunNumber() ) {
    m_currFile = m_openFiles.getFile(runNumber);
    if(!m_currFile) {
      m_currFile = m_parkFiles.getFile(runNumber);
      if ( m_currFile ) {
	struct cmd_header header;
	*m_log << MSG::INFO << "RUN:" << std::dec << runNumber
	       << ": Resurrect existing file:" << m_currFile->getFileName() << endmsg;
	m_parkFiles.removeFile(m_currFile);
	m_openFiles.addFile(m_currFile);
	INIT_OPEN_COMMAND(&header, m_currFile->getFileName()->c_str(),
			  m_currFile->getSeqNum(), runNumber);
	m_srvConnection->sendCommand(&header);
	m_currFile->open();
	m_currFile->incSeqNum();
      }
    }
    if(!m_currFile) {
      *m_log << MSG::INFO << "RUN:" << std::dec << runNumber
	     << ": No file exists. Creating a new one." << endmsg;
      m_currFile = createAndOpenFile(runNumber);
      m_openFiles.addFile(m_currFile);
    }
  }

  INIT_WRITE_COMMAND(&header, len,
		     m_currFile->getBytesWritten(),
		     m_currFile->getFileName()->c_str(),
		     m_currFile->getSeqNum(),
		     m_currFile->getRunNumber());
  m_srvConnection->sendCommand(&header, (void*)data);
  m_currFile->updateWrite(data, len);
  m_currFile->incSeqNum();
  m_bufferLength += len;
  return StatusCode::SUCCESS;
}


/// Obtains the run number from the MDF header.
unsigned int ReprocessingWriter::getRunNumber(const void *data, size_t /*len*/)    {
  MDFHeader* h = (MDFHeader*)data;
  const char* start = ((char*)h)+h->sizeOf(h->headerVersion());
  const char* end   = start+h->size();
  while(start<end)    {
    RawBank* b = (RawBank*)start;
    if ( b->type() == RawBank::DAQ && b->version() == DAQ_FILEID_BANK ) {
      return *b->begin<int>();
    }
    start += b->totalSize();
  }
  *m_log << MSG::ERROR << "No FileID bank present. Such events are not foreseen!!!!" << endmsg;
  throw runtime_error("No FileID bank present. Such events are not foreseen!!!!");
}

/// A notify listener callback, which is executed when an open command is acked.
void ReprocessingWriter::notifyOpen(struct cmd_header *cmd)   {
  try {
    m_rpcObj->createFile(cmd->file_name, cmd->run_no);
    *m_log << MSG::INFO << "Opened ";    
  }
  catch(exception& e) {
    *m_log << MSG::ERROR << "Could not create Run Database Record:" << e.what() << endmsg;
  }
  *m_log << "File:" << cmd->file_name << " Run:" << std::dec << cmd->run_no << endmsg;
}

/// A notify listener callback, which is executed  when a close command is acked.
void ReprocessingWriter::notifyClose(struct cmd_header *cmd)   {
  struct cmd_stop_pdu *pdu = (struct cmd_stop_pdu *) ((char *) cmd + sizeof(struct cmd_header));
  char md5buf[33];
  unsigned char *md5 = pdu->md5_sum;
  try {
    m_bufferLength -= cmd->data.stop_data.size;
    m_rpcObj->updateFile(cmd->file_name,
                          pdu->trgEvents,
                          pdu->statEvents);
    m_rpcObj->confirmFile(cmd->file_name,
			  pdu->adler32_sum,
			  pdu->md5_sum,
                          cmd->data.stop_data.size,
                          pdu->events,
                          pdu->physStat);
    *m_log << MSG::INFO << "Confirmed ";
  }
  catch(exception& rte) {
    *m_log << MSG::ERROR << "Could not update Run Database Record ";
    *m_log << "Cause: " << rte.what() << endmsg;
  }
  ::sprintf(md5buf,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
	    md5[0],md5[1],md5[2],md5[3],
	    md5[4],md5[5],md5[6],md5[7],
	    md5[8],md5[9],md5[10],md5[11],
	    md5[12],md5[13],md5[14],md5[15]);
  *m_log << "File:" << cmd->file_name;
  *m_log << " Adler32:" << hex << pdu->adler32_sum;
  *m_log << " MD5:" << md5buf << endmsg;
}

/// A notify listener callback, which is executed  when an error occurs.
void ReprocessingWriter::notifyError(struct cmd_header* /*cmd*/, int /*errno*/)  {
  /* Not Used Yet. */
}
#endif /* _WIN32 */
