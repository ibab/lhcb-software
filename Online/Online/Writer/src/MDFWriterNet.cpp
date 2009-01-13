#ifdef BUILD_WRITER

#include <string>
#include <stdexcept>
#include <ctime>
#include <map>

#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Writer/Connection.h"
#include "Writer/MDFWriterNet.h"
#include "Writer/Utils.h"
#include "RTL/rtl.h"
#include "TMD5.h"
extern "C" {
#include "dis.h"
}

#include "Writer/defs.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterNet);

using namespace LHCb;

/**
 * Macro for initialising a close command.
 */
#define INIT_CLOSE_COMMAND(h, fname, adler_32, md_5, seqno, rno, thesize, noofevents, nooflumievents) { \
    (h)->cmd = CMD_CLOSE_FILE; \
    (h)->run_no = rno;  \
    (h)->data.chunk_data.seq_num = seqno; \
    (h)->data.stop_data.adler32_sum = (adler_32); \
    (md_5)->Final((h)->data.stop_data.md5_sum); \
    (h)->data.stop_data.size = thesize; \
    (h)->data.stop_data.events = noofevents; \
    (h)->data.stop_data.lumiEvents = nooflumievents; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising a write command.
 */
#define INIT_WRITE_COMMAND(h, len, off, fname, seqno, rno) { \
  (h)->cmd = CMD_WRITE_CHUNK; \
    (h)->data.chunk_data.size = (len); \
    (h)->run_no = rno;	\
    (h)->data.chunk_data.offset = (off); \
    (h)->data.chunk_data.seq_num = seqno; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising an open command.
 */
#define INIT_OPEN_COMMAND(h, fname, seqno, rno) { \
  (h)->cmd = CMD_OPEN_FILE; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
    (h)->data.chunk_data.seq_num = seqno; \
    (h)->run_no = rno;	\
}

void File::feedMonitor(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  Monitor* h = *(Monitor**)tag;
  if ( !(*first) ) {
    *buf  = h;
    *size = sizeof(Monitor);
    return;
  }
  *size = 0;
  *buf = (void*)empty;
}

void File::feedMessageQueue(mqd_t /* mq */) {
}

/// Constructor
File::File(std::string fileName, unsigned int runNumber) {
  static int s_seqNo = 0;
  char txt[32];
  m_mon = &m_monBuffer;
  m_mon->m_seqNum = 0;
  m_mon->m_runNumber = runNumber;
  ::strncpy(m_mon->m_name,fileName.c_str(),sizeof(m_mon->m_name));
  m_mon->m_name[sizeof(m_mon->m_name)-1]=0;
  m_mon->m_adler32 = adler32Checksum(0, NULL, 0);
  m_mon->m_lastUpdated = time(NULL);
  m_mon->m_fileOpen = false;
  m_mon->m_bytesWritten = 0;
  m_mon->m_events = 0;
  m_mon->m_lumiEvents = 0;
  m_fileName = fileName;
  m_md5 = new TMD5();
  m_prev = NULL;
  m_next = NULL;
  sprintf(txt,"/File#%02d",s_seqNo++);
  std::string svc = RTL::processName()+txt;
  m_mon->m_svcID = ::dis_add_service((char*)svc.c_str(),"C",0,0,feedMonitor,(long)m_mon);
}

/// Destructor.
File::~File() {
  delete m_md5;
  ::dis_remove_service(m_mon->m_svcID);
}


/// Extended algorithm constructor
MDFWriterNet::MDFWriterNet(MDFIO::Writer_t typ, const std::string& nam,  ISvcLocator* pSvc) : MDFWriter(typ, nam, pSvc)
{
  constructNet();
}

/// Standard algorithm constructor
MDFWriterNet::MDFWriterNet(const std::string& nam, ISvcLocator* pSvc) : MDFWriter(nam, pSvc)
{
  constructNet();
}

/// Algorithm::execute() override, delegates to MDFWriter::execute().
StatusCode MDFWriterNet::execute()
{
  return MDFWriter::execute();
}

/// Read job options.
void MDFWriterNet::constructNet()
{

  //This should ideally be provided as a name and not an
  //IP address, so that the round-robin DNS can kick in.
  declareProperty("StorageServerAddr",  m_serverAddr="");
  declareProperty("StorageServerPort",  m_serverPort=45247);
  declareProperty("RunDBServiceURL",    m_runDBURL="");
  declareProperty("MaxFileSizeMB",      m_maxFileSizeMB=1);
  declareProperty("SndRcvSizes",        m_sndRcvSizes=6553600);
  declareProperty("FilePrefix",         m_filePrefix="MDFWriterNet_File_");
  declareProperty("Directory",          m_directory=".");
  declareProperty("FileExtension",      m_fileExtension="raw");
  declareProperty("StreamID",           m_streamID=0);
  declareProperty("RunFileTimeoutSeconds", m_runFileTimeoutSeconds=30);

  m_log = new MsgStream(msgSvc(), name());
}

/** Overrides MDFWriter::initialize(). Initialises the Connection object.
 */
StatusCode MDFWriterNet::initialize(void)
{
  *m_log << MSG::INFO << " Writer " << getpid() <<
    " Initializing." << endmsg;

  m_currFile = NULL;
  m_srvConnection = new Connection(m_serverAddr, m_serverPort,
				   m_sndRcvSizes, m_log, this);
  m_rpcObj = new RPCComm(m_runDBURL.c_str());
  *m_log << MSG::INFO << "rundb url: " << m_runDBURL.c_str() <<endmsg;
  try {
    m_srvConnection->initialize();
  } catch(const std::exception& e) {
    *m_log << MSG::ERROR << "Caught Exception:" << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  
  // initialize named message queue
  if((m_mq = mq_open("/writerqueue", O_RDWR, S_IRUSR|S_IWUSR, NULL)) == (mqd_t) -1)  {
      *m_log << MSG::ERROR
             << "Could not establish connection to message queue"
             << endmsg;
      m_mq_available = false;
  } else {
      *m_log << MSG::INFO
             << "Established connection to message queue. "
             << "Delimiter is " << DELIMITER
             << endmsg;
      m_mq_available = true;
      // send message that this writer now exists
      
      size_t msg_size = snprintf(NULL, 0, "start%c%i", DELIMITER, getpid()) + 1;
      char* msg = (char*) malloc(msg_size);
      snprintf(msg, msg_size, "start%c%i", DELIMITER, getpid());
      if(mq_send(m_mq, msg, msg_size, 0) < 0)  {
          *m_log << MSG::ERROR
                << "Error sending message to the queue"
                << "deactivating message queue"
                << endmsg;
          m_mq_available = false; // prevent from further trying to protect writer
      }
      free(msg);
      msg = NULL;
      
  }
                      
  *m_log << " Writer " << getpid() << " Initialized." << endmsg;
  return StatusCode::SUCCESS;
}

/** Overrides MDFWriter::finalize().
 * Closes the file if it is open, and writes its entry in the
 * Run Database.
 */
StatusCode MDFWriterNet::finalize(void)
{
  *m_log << MSG::INFO << " Writer " << getpid() 
         << " Finalizing." << endmsg;

  File *tmpFile;
  tmpFile = m_openFiles.getFirstFile();

  while(tmpFile) {

    if(tmpFile->isOpen()) {
      *m_log << MSG:: INFO << WHERE
             << "Closing file " << *(tmpFile->getFileName())
	     << " with run number " << tmpFile->getRunNumber() << endmsg;
      closeFile(tmpFile);
      File *toDelete = tmpFile;
      tmpFile = m_openFiles.removeFile(tmpFile);
      delete toDelete;
      continue;
    }
    tmpFile = tmpFile->getNext();
  }

  // closeConnection() blocks till everything is flushed.
  m_srvConnection->closeConnection();
  delete m_srvConnection;
  delete m_rpcObj;

  m_currFile = NULL;
  m_srvConnection = NULL;
  m_rpcObj = NULL;

  *m_log << MSG::INFO << WHERE
         << " Writer process finalized"
         << endmsg;
  // log the termination of this instance of WriterNet
  
  if(m_mq_available) {
      size_t msg_size = snprintf(NULL, 0, "stop%c%i", DELIMITER, getpid() ) + 1;
      char* msg = (char*) malloc(msg_size);
      snprintf(msg, msg_size, "stop%c%i", DELIMITER,  getpid() );
      if(mq_send(m_mq, msg, msg_size, 0) < 0) {
          *m_log << MSG::ERROR
                 << "Could not send message"
                 << "closing queue"
                 << endmsg;
          m_mq_available = false;
      }
      free(msg);
      msg = NULL;
  }
  
  // close message queue
  
  if(m_mq_available) {
      // no error handling here: a not closed connection to 
      // a message queue is nothing to worry about
      if(mq_close(m_mq) == -1) {
         // error closing
      }
      m_mq_available = false;
  }
  return StatusCode::SUCCESS;
}

/**
 * Creates a new file using a RPC object
 */
std::string MDFWriterNet::createNewFile(unsigned int runNumber)
{
  // override this if the m_rpcObj looks different
  return m_rpcObj->createNewFile(runNumber);
}

/**
 * Queues a command that creates a file.
 */
File* MDFWriterNet::createAndOpenFile(unsigned int runNumber)
{
  *m_log << MSG::INFO << "createAndOpenFile start" << endmsg;
  File *currFile = 0;
  struct cmd_header header;
  memset(&header, 0, sizeof(struct cmd_header));

  /* The RunDb generates file names now */
  try {
    /* trying to get an 'official'file name from the RunDatabase
     * If there is any kinf of error generate one locally
     */
    *m_log << MSG::INFO << "Getting a new file name for run "
           << runNumber << " ..." << endmsg;
    currFile = new File(this->createNewFile(runNumber), runNumber);
  } catch (std::exception e) {
    currFile = new File(getNewFileName(runNumber), runNumber);
    *m_log << MSG::WARNING
           << " Exception: "
           << e.what()
           << "Could not get new file name! Generating local filename: "
           << *(currFile->getFileName()) << endmsg ;
  }

  INIT_OPEN_COMMAND(&header, currFile->getFileName()->c_str(),
                    currFile->getSeqNum(), runNumber);
  m_srvConnection->sendCommand(&header);
  currFile->open();
  currFile->incSeqNum();

  // log opening of file
  if(m_mq_available) {
      size_t msg_size = snprintf(NULL, 0, "openfile%c%i%c%s", DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name) + 1;
      char* msg = (char*) malloc(msg_size);
      snprintf(msg, msg_size, "openfile%c%i%c%s", DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name);
      if(mq_send(m_mq, msg, msg_size, 0) < 0) {
          *m_log << MSG::ERROR
                 << "Could not send message"
                 << "closing queue"
                 << endmsg;
          m_mq_available = false;
      }
      free(msg);
      msg = NULL;
  }
  *m_log << MSG::INFO << "createAndOpenFile end" << endmsg;
  return currFile;
}

/**
 * Queues a command that closes a file.
 */
void MDFWriterNet::closeFile(File *currFile)
{
  struct cmd_header header;
  memset(&header, 0, sizeof(struct cmd_header));

  INIT_CLOSE_COMMAND(&header,
		     currFile->getFileName()->c_str(),
		     currFile->getAdlerChecksum(),
		     currFile->getMD5Checksum(),
		     currFile->getSeqNum(),
		     currFile->getRunNumber(),
             currFile->getBytesWritten(),
             currFile->getEvents(),
             currFile->getLumiEvents());
//  *m_log << MSG::INFO << " Command: " << header.cmd << " "
//         << "Filename: "   << header.file_name << " "
//	 << "RunNumber: "  << header.run_no << " "
//         << "Adler32: "    << header.data.stop_data.adler32_sum << " "
//         << "MD5: "        << header.data.stop_data.md5_sum << " "
//         << "Seq Nr: "     << header.data.chunk_data.seq_num << " "
//         << "Size: "       << header.data.stop_data.size << " "
//         << "Events: "     << header.data.stop_data.events << " "
//         << "Command size is: " << sizeof(header)
//         << endmsg;
  m_srvConnection->sendCommand(&header);
  
  // log closing of file
  if(m_mq_available) {
      size_t msg_size = snprintf(NULL, 0, "closefile%c%i%c%s",  DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name) + 1;
      char* msg = (char*) malloc(msg_size);
      snprintf(msg, msg_size, "closefile%c%i%c%s", DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name);
      if(mq_send(m_mq, msg, msg_size, 0) < 0) {
          *m_log << MSG::ERROR
                 << "Could not send message"
                 << "closing queue"
                 << endmsg;
          m_mq_available = false;
      }
      free(msg);
      msg = NULL;
  } 
  *m_log << MSG::INFO << WHERE << endmsg;
}

/* Writes out the buffer to the socket through the Connection object.
 * This function first checks if a new file needs to be created. After
 * that, it writes data out to the new file. The MD5 and Adler32 checksums
 * are computed incrementally for the current file. In case the file is above
 * the maximum size limit, then a RunDB call is executed, to which the
 * file name, MD5 sum, and the Adler32 sum are supplied.
  */
StatusCode MDFWriterNet::writeBuffer(void *const /*fd*/, const void *data, size_t len)
{
  struct cmd_header header;

  /*DEBUGGING ONLY - To inject run numbers into the events by reading the run number
   * off a file.
   */
  /*FILE *fil;
   *int myRun;
   *fil = fopen("/tmp/runno", "r");
   *if(fil)
   *fscanf(fil, "%d", &myRun);
   *fclose(fil);
   *((MDFHeader*)data)->subHeader().H1->m_runNumber = myRun;
   */

  if ( len < 10 ) {
    *m_log << MSG::FATAL << WHERE
           << "Very small message received, not forwarding. Length is: " << len
           << endmsg;
    return StatusCode::SUCCESS;
  } 

  unsigned int runNumber = getRunNumber(data, len);
  if(m_currFile == NULL || runNumber != m_currFile->getRunNumber()) {
    m_currFile = m_openFiles.getFile(runNumber);
    if(!m_currFile) {
      *m_log << MSG::INFO << WHERE
             << "No file exists for run " << runNumber
             << " Creating a new one."
             << endmsg;
      m_currFile = createAndOpenFile(runNumber);
      m_openFiles.addFile(m_currFile);
    }

    // This block is entered only in case an event from a previous run
    // appears after a run has started. This should be relatively infrequent,
    // and therefore, a good place to check if there are files that have been
    // lying open since a very long time.
    //

    File *tmpFile =  m_openFiles.getFirstFile();
    // Loop over all the files except the one for whom the event just came in.
    while(tmpFile) {
      if(tmpFile->getRunNumber() != runNumber &&
	 tmpFile->getTimeSinceLastWrite() > m_runFileTimeoutSeconds) {
	// This file hasn't been written to in a loong time. Close it.
	*m_log << MSG::INFO << WHERE
	       << "Closing a file that did not get events for a long time now."
	       << endmsg;
	File *toDelete = tmpFile;
	closeFile(tmpFile);
	// removeFile() returns the next element after the removed one.
	tmpFile = m_openFiles.removeFile(tmpFile);
	delete toDelete;
	continue;
      }
      tmpFile = tmpFile->getNext();
    }
  }

  INIT_WRITE_COMMAND(&header, len,
		     m_currFile->getBytesWritten(),
		     m_currFile->getFileName()->c_str(),
		     m_currFile->getSeqNum(),
		     runNumber);
  m_srvConnection->sendCommand(&header, (void*)data);
  // *m_log << MSG::INFO << WHERE << "elements in the queue: " << m_srvConnection->getQueueLength() << endmsg;
  size_t totalBytesWritten = m_currFile->updateWrite(data, len);
  m_currFile->incSeqNum();

  // count event
  m_currFile->incEvents();
  
  // physstat = (no of all events) - (lumi events)
  // check type of event
  if( checkForLumiEvent(data, len)) {
      m_currFile->incLumiEvents();
  }
 
  // after every MB send statistics
  if (m_mq_available && totalBytesWritten % 1048576 <= 1000) {
      size_t msg_size = snprintf(NULL, 0, "log%c%i%c%s%c%s%zu",  
      DELIMITER, getpid(), 
      DELIMITER, m_currFile->getMonitor()->m_name,
      DELIMITER, "bytesWritten=", totalBytesWritten) + 1;
      char* msg = (char*) malloc(msg_size);
      snprintf(msg, msg_size, "log%c%i%c%s%c%s%zu", 
      DELIMITER, getpid(), 
      DELIMITER, m_currFile->getMonitor()->m_name,
      DELIMITER, "bytesWritten=", totalBytesWritten);
      if(mq_send(m_mq, msg, msg_size, 0) < 0) {
          *m_log << MSG::ERROR
                 << "Could not send message"
                 << "closing queue"
                 << endmsg;
          m_mq_available = false;
      }
      free(msg);
      msg = NULL;
  }
  
  //How much have we written?
  if (totalBytesWritten > (m_maxFileSizeMB << 20)) {
    closeFile(m_currFile);
    m_openFiles.removeFile(m_currFile);
    delete(m_currFile);
    m_currFile = NULL;
  }
  
  // WARNING: m_currFile might be NULL after the totalBytesWritten check above!
  // -> segmentation fault
  
  //Close it, reset counter.
  return StatusCode::SUCCESS;
}


/** Obtains the run number from the MDF header.
 * @param data  The data from which MDF information may be retrieved
 * @param len   The length of the data.
 * @return The run number.
 */
inline unsigned int MDFWriterNet::getRunNumber(const void *data, size_t /*len*/)    {
  MDFHeader *mHeader;
  RawBank* b = (RawBank*)data;
  if ( b->magic() == RawBank::MagicPattern )
    mHeader = b->begin<MDFHeader>();
  else
    mHeader = (MDFHeader*)data;
  return mHeader->subHeader().H1->m_runNumber;
}

/** Checks if it is an lumi event (which means that bit 34 is set).
 * @param data  The data from which MDF information may be retrieved
 * @param len   The length of the data.
 * @return true if it is an lumi event, false otherwise
 */
inline bool MDFWriterNet::checkForLumiEvent(const void *data, size_t ) {
  MDFHeader *mHeader;
  RawBank* b = (RawBank*)data;
  if ( b->magic() == RawBank::MagicPattern )
    mHeader = b->begin<MDFHeader>();
  else
    mHeader = (MDFHeader*)data;
  unsigned int lumiBitMask = mHeader->subHeader().H0->m_trMask[1];
  if(lumiBitMask & (unsigned int) 4) {
      return true;
  } else {
      return false;
  }
}


/** Generates a new file name from the MDF information.
 * @param runNumber  The current run number, to be included in the file name.
 */
std::string MDFWriterNet::getNewFileName(unsigned int runNumber)
{
  char buf[MAX_FILE_NAME];
  static unsigned long random = 0;
  random++;
  sprintf(buf, "/daqarea/lhcb/data/2008/RAW/TEST/%s/%u/%s%09u.%02u%06lu.%s",
	  m_directory.c_str(),runNumber,
	  m_filePrefix.c_str(),
	  runNumber,m_streamID,// random,
       	  time(NULL)&0xFFFF,
	  m_fileExtension.c_str());
  return std::string(buf);
}

/** Destructor.
 * Checks if a file is open, and closes it before closing the connection.
 */
MDFWriterNet::~MDFWriterNet()
{
  if(m_srvConnection) {
    *m_log << MSG::WARNING << "Destructor called before finalize. Normal?" << endmsg;
    finalize();
  }
  delete m_log;
}

/** A notify listener callback, which is executed when an open command is acked.
 */
void MDFWriterNet::notifyOpen(struct cmd_header *cmd)
{
  try {
    m_rpcObj->createFile(cmd->file_name, cmd->run_no);
  } catch(std::exception& e) {
    *m_log << MSG::ERROR << "Could not create Run Database Record ";
    *m_log << "Cause: " << e.what() << std::endl;
    *m_log << "Record is: FileName=" << cmd->file_name;
    *m_log << " Run Number=" << cmd->run_no << endmsg;
  }
}

/** A notify listener callback, which is executed  when a close command is acked.
 */
void MDFWriterNet::notifyClose(struct cmd_header *cmd)
{
    *m_log << MSG::INFO << WHERE << " notifyClose start" << endmsg;
  try {
    m_rpcObj->confirmFile(cmd->file_name,
			              cmd->data.stop_data.adler32_sum,
			              cmd->data.stop_data.md5_sum,
                          cmd->data.stop_data.size,
                          cmd->data.stop_data.events,
                          cmd->data.stop_data.lumiEvents);
    *m_log << MSG::INFO << "Confirmed file " << cmd->file_name << endmsg;
  } catch(std::exception& rte) {
    char md5buf[33];
    unsigned char *md5sum = cmd->data.stop_data.md5_sum;
    sprintf(md5buf, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
	        md5sum[0],md5sum[1],md5sum[2],md5sum[3],
	        md5sum[4],md5sum[5],md5sum[6],md5sum[7],
	        md5sum[8],md5sum[9],md5sum[10],md5sum[11],
	        md5sum[12],md5sum[13],md5sum[14],md5sum[15]);

    *m_log << MSG::ERROR << "Could not update Run Database Record ";
    *m_log << "Cause: " << rte.what() << std::endl;
    *m_log << "Record is: FileName=" << cmd->file_name;
    *m_log << " Adler32 Sum=" << cmd->data.stop_data.adler32_sum;
    *m_log << " MD5 Sum=" << md5buf << endmsg;
  }
    *m_log << MSG::INFO << WHERE << " notifyClose end" << endmsg;
}

/** A notify listener callback, which is executed  when an error occurs.
 */
void MDFWriterNet::notifyError(struct cmd_header* /*cmd*/, int /*errno*/)
{
  /* Not Used Yet. */
}
#endif /* _WIN32 */
