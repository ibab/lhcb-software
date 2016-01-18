/*
 * MDFWriterNet.cpp
 *
 * Author:  Sai Suman Cherukuwada
 *			Jean-Christophe Garnier
 *			Vijay Kartik (vijay.kartik@cern.ch)
 */

//#ifdef BUILD_WRITER
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
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Writer/Connection.h"
#include "Writer/MDFWriterNet.h"
#include "Writer/Utils.h"
#include "RTL/rtl.h"
#include "TMD5.h"
extern "C"
{
#include "dis.h"
}
#include "Writer/defs.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb, MDFWriterNet)

using namespace LHCb;

/**
 * Macro for initialising a close command.
 */
/*#define INIT_CLOSE_COMMAND(h, fname, adler_32, md_5, seqno, rno, thesize, noofevents, noofphysstat, trgevents) { \
 */
#define INIT_CLOSE_COMMAND(h, fname, seqno, rno, thesize) {	\
    (h)->cmd = CMD_CLOSE_FILE;					\
    (h)->run_no = rno;						\
    (h)->data.chunk_data.seq_num = seqno;			\
    (h)->data.stop_data.size = thesize;				\
    strncpy((h)->file_name, (fname), MAX_FILE_NAME);		\
  }

/**
 * Macro for initialising a close command pdu.
 */
#define INIT_CLOSE_PDU(p, adler_32, md_5, noofevents, noofphysstat, trgevents, statevents) { \
    (p)->adler32_sum = (adler_32);					\
    (md_5)->Final((p)->md5_sum);					\
    (p)->events = noofevents;						\
    (p)->physStat = noofphysstat;					\
    memcpy((p)->trgEvents, (trgevents), MAX_TRIGGER_TYPES*sizeof(unsigned int)); \
    memcpy((p)->statEvents, (statevents), MAX_STAT_TYPES*sizeof(unsigned int)); \
  }

/**
 * Macro for initialising a write command.
 */
#define INIT_WRITE_COMMAND(h, len, off, fname, seqno, rno) {	\
    (h)->cmd = CMD_WRITE_CHUNK;					\
    (h)->data.chunk_data.size = (len);				\
    (h)->run_no = rno;						\
    (h)->data.chunk_data.offset = (off);			\
    (h)->data.chunk_data.seq_num = seqno;			\
    strncpy((h)->file_name, (fname), MAX_FILE_NAME);		\
  }

/**
 * Macro for initialising an open command.
 */
#define INIT_OPEN_COMMAND(h, fname, seqno, rno) {	\
    (h)->cmd = CMD_OPEN_FILE;				\
    strncpy((h)->file_name, (fname), MAX_FILE_NAME);	\
    (h)->data.chunk_data.seq_num = seqno;		\
    (h)->run_no = rno;					\
  }

void File::feedMonitor(void* tag, void** buf, int* size, int* first)
{
  static const char* empty = "";
  Monitor* h = *(Monitor**) tag;
  if (!(*first))
  {
    *buf = h;
    *size = sizeof(Monitor);
    return;
  }
  *size = 0;
  *buf = (void*) empty;
}

void File::feedMessageQueue(mqd_t /* mq */)
{
}

/// New constructor with stream id
File::File(const std::string& fileName, unsigned int runNumber,
    const std::string& streamID, bool enableMD5)
{
  init(fileName, runNumber, enableMD5);
  m_streamID = streamID;
}

/// Constructor
File::File(const std::string& fileName, unsigned int runNumber, bool enableMD5)
{
  init(fileName, runNumber, enableMD5);
}

/// Constructor
void File::init(const std::string& fileName, unsigned int runNumber,
bool enableMD5)
{
  static int s_seqNo = 0;
  char txt[32];
  bzero(txt, 32);
  m_mon = &m_monBuffer;
  m_mon->m_seqNum = 0;
  m_mon->m_runNumber = runNumber;
  ::strncpy(m_mon->m_name, fileName.c_str(), sizeof(m_mon->m_name));
  m_mon->m_name[sizeof(m_mon->m_name) - 1] = 0;
  m_mon->m_adler32 = adler32Checksum(0, NULL, 0);
  m_mon->m_lastUpdated = time(NULL);
  m_mon->m_fileOpen = false;
  m_mon->m_bytesWritten = 0;
  m_mon->m_events = 0;
  m_mon->m_physStat = 0;
  m_fileName = fileName;
  m_md5 = new TMD5();
  m_streamID = "FULL";
  m_prev = NULL;
  m_next = NULL;
  sprintf(txt, "/File#%02d", s_seqNo++);
  std::string svc = RTL::processName() + txt;

  m_enableMD5 = enableMD5;

  memset(m_mon->m_trgEvents, 0, MAX_TRIGGER_TYPES * sizeof(unsigned int));
  memset(m_mon->m_statEvents, 0, MAX_STAT_TYPES * sizeof(unsigned int));

  m_mon->m_svcID = ::dis_add_service((char *) svc.c_str(), (char *) "C", 0, 0,
      feedMonitor, (long) m_mon);

}

/// Destructor.
File::~File()
{
  delete m_md5;
  ::dis_remove_service(m_mon->m_svcID);
}

size_t File::updateWrite(const void *data, size_t len)
{
  if (m_enableMD5)
  {
    m_md5->Update((UChar_t*) data, (UInt_t) len);
  }
  m_mon->m_adler32 = adler32Checksum(m_mon->m_adler32, (const char*) data, len);
  m_mon->m_bytesWritten += len;
  m_mon->m_lastUpdated = time(NULL);
  return m_mon->m_bytesWritten;
}

/// Extended algorithm constructor
MDFWriterNet::MDFWriterNet(MDFIO::Writer_t typ, const std::string& nam,
    ISvcLocator* pSvc) :
    MDFWriter(typ, nam, pSvc)
{
  constructNet();
}

/// Standard algorithm constructor
MDFWriterNet::MDFWriterNet(const std::string& nam, ISvcLocator* pSvc) :
    MDFWriter(nam, pSvc), m_incidentSvc(0)
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
  declareProperty("StorageServerAddr", m_serverAddr = "");
  declareProperty("StorageServerPort", m_serverPort = 45247);
  declareProperty("RunDBServiceURL", m_runDBURL = "");
  declareProperty("MaxFileSizeMB", m_maxFileSizeMB = 2000);
  declareProperty("MaxEventInFile", m_maxEventInFile = 0);
  declareProperty("SndRcvSizes", m_sndRcvSizes = 6553600);
  declareProperty("FilePrefix", m_filePrefix = "MDFWriterNet_File_");
  declareProperty("Directory", m_directory = ".");
  declareProperty("FileExtension", m_fileExtension = "raw");
  declareProperty("StreamID", m_streamID = "NONE");
  declareProperty("RunFileTimeoutSeconds", m_runFileTimeoutSeconds = 10);
  declareProperty("RunFileTimeoutSecondsLHCb1", m_runFileTimeoutSecondsLHCb1 =
      300);
  declareProperty("MaxQueueSizeBytes", m_maxQueueSizeBytes = 1073741824);
  declareProperty("EnableMD5", m_enableMD5 = false);
  declareProperty("UpdatePeriod", m_UpdatePeriod = 2); //0 is no update
  declareProperty("MaxRetry", m_MaxRetry = 5); //0 is no retry, but try once
  m_log = new MsgStream(msgSvc(), name());
}

/** Overrides MDFWriter::initialize(). Initialises the Connection object.
 */
StatusCode MDFWriterNet::initialize(void)
{
  *m_log << MSG::INFO << "Writer " << getpid() << " Initializing." << endmsg;

  if (m_enableMD5)
    *m_log << MSG::INFO << "MD5 sum on-the-fly computing enabled." << endmsg;
  else
    *m_log << MSG::INFO << "MD5 sum on-the-fly computing disabled." << endmsg;

  gettimeofday(&m_prevUpdate, NULL);
  gettimeofday(&m_prevMsgQueue, NULL);

  m_currFile = NULL;
  m_closedRuns.clear();
  m_srvConnection = new Connection(m_serverAddr, m_serverPort, m_sndRcvSizes,
      m_log, this, m_maxQueueSizeBytes);
  m_rpcObj = new RPCComm(m_runDBURL.c_str());
  *m_log << MSG::INFO << "rundb url: " << m_runDBURL.c_str() << endmsg;
  try
  {
    m_srvConnection->initialize();
  } catch (const std::exception& e)
  {
    *m_log << MSG::ERROR << "Caught Exception:" << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_streamID == "NONE")
  {
    *m_log << MSG::ERROR << "Exception: No stream identifier provided."
        << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    *m_log << MSG::INFO << "streamID: " << m_streamID << endmsg;
  }
  // initialize named message queue
  // The message queue is shared between several MDFWriterNet (one per partition ID in activity)
  if ((m_mq = mq_open("/writerqueue", O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR,
  NULL)) == (mqd_t) -1)
  {
    *m_log << MSG::WARNING
        << "Could not establish connection to message queue: " << errno
        << ". No monitoring available. " << endmsg;

    m_mq_available = false;

  }
  else
  {
    *m_log << MSG::INFO << "Established connection to message queue. "
        << "Delimiter is " << DELIMITER << endmsg;
    m_mq_available = true;
    // send message that this writer now exists

    size_t msg_size = snprintf(NULL, 0, "start%c%i", DELIMITER, getpid()) + 1;
    char* msg = (char*) malloc(msg_size);
    snprintf(msg, msg_size, "start%c%i", DELIMITER, getpid());
    if (mq_send(m_mq, msg, msg_size, 0) < 0)
    {
      if (errno != EAGAIN)
      {
        *m_log << MSG::WARNING << "Could not send message, errno=" << errno
            << ". Closing queue" << endmsg;
        m_mq_available = false; // prevent from further trying to protect writer
      }
    }
    free(msg);
    msg = NULL;

  }
  if (!service("IncidentSvc", m_incidentSvc).isSuccess())
  {
    *m_log << MSG::ERROR << "Could not start incident service." << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    m_incidentSvc->addListener(this, "DAQ_CANCEL");
    //        m_incidentSvc->addListener(this, "DAQ_ERROR");
  }

  m_TotEvts = 0;

  if (!service("MonitorSvc", m_MonitorSvc).isSuccess())
  {
    *m_log << MSG::ERROR << "Could not start monitor service." << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    m_MonitorSvc->declareInfo("TotEvts", m_TotEvts, "Total events seen", this);
  }

  //  m_discardCurrentRun = false;
  m_currentRunNumber = 0;
  m_CleanUpStop = false;
  m_Finalized = false;
  m_StopRetry = false;
  std::string utgid;
  utgid = RTL::processName();
  if (utgid.find("LHCb1_") != std::string::npos)
  {
    m_runFileTimeoutSeconds = m_runFileTimeoutSecondsLHCb1;
    *m_log << MSG::INFO << WHERE << "Changed File Timeout to LHCb1 Timeout of: "
        << m_runFileTimeoutSecondsLHCb1 << " seconds" << endmsg;
  }
  if (pthread_mutex_init(&m_SyncFileList, NULL))
  {
    *m_log << MSG::ERROR << WHERE << "Failed to initialize mutex" << endmsg;
    return StatusCode::FAILURE;
  }
  /// Starts thread.
  if (pthread_create(&m_ThreadFileCleanUp, NULL, FileCleanUpStartup, this))
  {
    *m_log << MSG::ERROR << WHERE << "Failed to start File Clean Up Thread "
        << endmsg;
    return StatusCode::FAILURE;
  }

  *m_log << MSG::INFO << " Writer " << getpid() << " Initialized." << endmsg;
  return StatusCode::SUCCESS;
}

/** Overrides MDFWriter::finalize().
 * Closes the file if it is open, and writes its entry in the
 * Run Database.
 */
StatusCode MDFWriterNet::finalize(void)
{
  *m_log << MSG::INFO << "Writer " << getpid() << " Finalizing." << endmsg;

  m_CleanUpStop = true;
  m_StopRetry = true;
  m_Finalized = true;

  if (pthread_mutex_lock(&m_SyncFileList))
  {
    *m_log << MSG::ERROR << WHERE << " Locking mutex" << endmsg;
    return StatusCode::FAILURE;
  }

  File *tmpFile;
  tmpFile = m_openFiles.getFirstFile();

  while (tmpFile)
  {
    *m_log << MSG::INFO << WHERE << "Checking file: "
        << *(tmpFile->getFileName()) << endmsg;

    if (tmpFile->isOpen())
    {
      *m_log << MSG::INFO << WHERE << "Closing file "
          << *(tmpFile->getFileName()) << " with run number "
          << tmpFile->getRunNumber() << endmsg;
      closeFile(tmpFile);
      File *toDelete = tmpFile;
      tmpFile = m_openFiles.removeFile(tmpFile);
      delete toDelete;
      continue;
    }
    tmpFile = tmpFile->getNext();
  }

  if (pthread_mutex_unlock(&m_SyncFileList))
  {
    *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
    return StatusCode::FAILURE;
  }

  // closeConnection() blocks till everything is flushed.
  m_srvConnection->closeConnection();
  delete m_srvConnection;
  delete m_rpcObj;

  m_currFile = NULL;
  m_srvConnection = NULL;
  m_rpcObj = NULL;

  *m_log << MSG::INFO << WHERE << " Writer process finalized" << endmsg;
  // log the termination of this instance of WriterNet

  if (m_mq_available)
  {
    size_t msg_size = snprintf(NULL, 0, "stop%c%i", DELIMITER, getpid()) + 1;
    char* msg = (char*) malloc(msg_size);
    snprintf(msg, msg_size, "stop%c%i", DELIMITER, getpid());
    if (mq_send(m_mq, msg, msg_size, 0) < 0)
    {
      if (errno != EAGAIN)
      {
        *m_log << MSG::WARNING << "Could not send message, errno=" << errno
            << ". Closing queue" << endmsg;
        m_mq_available = false;
      }
    }
    free(msg);
    msg = NULL;
  }

  // close message queue

  if (m_mq_available)
  {
    // no error handling here: a not closed connection to
    // a message queue is nothing to worry about
    if (mq_close(m_mq) == -1)
    {
      // error closing
    }
    m_mq_available = false;
  }

  if (pthread_join(m_ThreadFileCleanUp, NULL))
  {
    *m_log << MSG::ERROR << WHERE << "File CleanUP Thread join" << endmsg;
    return StatusCode::FAILURE;
  }

  m_TotEvts = 0;

  if (m_incidentSvc)
  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }

  if (m_MonitorSvc)
  {
    m_MonitorSvc->undeclareAll(this);
    m_MonitorSvc->release();
    m_MonitorSvc = 0;
  }
  return StatusCode::SUCCESS;
}

/**
 * Creates a new file using a RPC object
 */
std::string MDFWriterNet::createNewFile(unsigned int runNumber)
{
  // override this if the m_rpcObj looks different
  std::string identifier(getenv("UTGID"));
  return m_rpcObj->createNewFile(runNumber, m_streamID, identifier);
}

/**
 * Queues a command that creates a file.
 */
File* MDFWriterNet::createAndOpenFile(unsigned int runNumber)
{
  File *currFile = 0;
  struct cmd_header header;
  memset(&header, 0, sizeof(struct cmd_header));

  /* trying to get an 'official'file name from the RunDatabase
  * If there is any kinf of error generate one locally
  */
  //	*m_log << MSG::INFO << "Getting a new file name from RunDatabase for run: " << runNumber
  //			<< " ..." << endmsg;
  std::string f = this->createNewFile(runNumber);
  *m_log << MSG::INFO << "New filename from RunDatabase: " << f << endmsg;
  currFile = new File(f, runNumber, m_streamID, m_enableMD5);

  if (currFile == NULL)
  {
    throw FailureException(
        "Unknown error creating a file, pointer dereferenced, aborting.");
  }

  INIT_OPEN_COMMAND(&header, currFile->getFileName()->c_str(),
      currFile->getSeqNum(), runNumber);
  m_srvConnection->sendCommand(&header);
  currFile->open();
  currFile->incSeqNum();

  // log opening of file
  if (m_mq_available)
  {

    struct timeval tv;
    gettimeofday(&tv, NULL);

    size_t msg_size = snprintf(NULL, 0, "openfile%c%i%c%s%c%u%c%d",
    DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name,
    DELIMITER, (unsigned int) tv.tv_sec, DELIMITER, (int) tv.tv_usec) + 1;
    char* msg = (char*) malloc(msg_size);
    snprintf(msg, msg_size, "openfile%c%i%c%s%c%u%c%d", DELIMITER, getpid(),
        DELIMITER, currFile->getMonitor()->m_name, DELIMITER,
        (unsigned int) tv.tv_sec, DELIMITER, (int) tv.tv_usec);
    if (mq_send(m_mq, msg, msg_size, 0) < 0)
    {
      if (errno != EAGAIN)
      {
        *m_log << MSG::WARNING << "Could not send message, errno=" << errno
            << ". Closing queue" << endmsg;
        m_mq_available = false;
      }
    }
    free(msg);
    msg = NULL;
  }
  return currFile;
}

/**
 * Queues a command that closes a file.
 */
void MDFWriterNet::closeFile(File *currFile)
{
  struct cmd_header header;
  struct cmd_stop_pdu pdu;
  ::memset(&header, 0, sizeof(struct cmd_header));
  ::memset(&pdu, 0, sizeof(struct cmd_stop_pdu));

  unsigned int trgEvents[MAX_TRIGGER_TYPES];

  //////////////////
  //Printing to check if the close command is queued
  //*m_log << MSG::INFO << WHERE << "Close command queued for file: " << currFile->getFileName()->c_str() << endmsg;
  //////////////////

  if (currFile->getTrgEvents(trgEvents, MAX_TRIGGER_TYPES) != 0)
  {
    *m_log << MSG::ERROR << WHERE
        << "Error getting the triggered event statistics" << endmsg;
  }

  unsigned int statEvents[MAX_STAT_TYPES];
  if (currFile->getStatEvents(statEvents, MAX_STAT_TYPES) != 0)
  {
    *m_log << MSG::ERROR << WHERE << "Error getting the routed event statistics"
        << endmsg;
  }

  INIT_CLOSE_COMMAND(&header, currFile->getFileName()->c_str(),
      currFile->getSeqNum(), currFile->getRunNumber(),
      currFile->getBytesWritten());

  INIT_CLOSE_PDU(&pdu, currFile->getAdlerChecksum(), currFile->getMD5Checksum(),
      currFile->getEvents(), currFile->getPhysStat(), trgEvents, statEvents);

  *m_log << MSG::INFO << WHERE << " Command: " << header.cmd << " "
      << "Filename: " << header.file_name << " " << "RunNumber: "
      << header.run_no << " " << "Seq Nr: " << header.data.chunk_data.seq_num
      << " " << "Size: " << header.data.stop_data.size << " " /*<< "Adler32: "
      << pdu.adler32_sum << " " << "MD5: " << pdu.md5_sum << " "
      << "Events: " << pdu.events << " " << "Phys:  " << pdu.physStat
      << " " << "Trg0 : " << pdu.trgEvents[0] << " " << "Trg1 : "
      << pdu.trgEvents[1] << " " << "Trg2 : " << pdu.trgEvents[2] << " "
      << "Trg3 : " << pdu.trgEvents[3] << " " << "Trg4 : "
      << pdu.trgEvents[4] << " " << "Trg5 : " << pdu.trgEvents[5] << " "
      << "Trg6 : " << pdu.trgEvents[6] << " " << "Trg7 : "
      << pdu.trgEvents[7] << " " << "PHYSIN : " << pdu.statEvents[PHYSIN]
      << " " << "MBIASIN : " << pdu.statEvents[MBIASIN] << " "
      << "BEAMGASIN : " << pdu.statEvents[BEAMGASIN] << " "
      << "LUMIIN : " << pdu.statEvents[LUMIIN] << " " << "RANDIN : "
      << pdu.statEvents[RANDIN] << " " << "PHYSEX : "
      << pdu.statEvents[PHYSEX] << " " << "MBIASEX : "
      << pdu.statEvents[MBIASEX] << " " << "BEAMGASEX : "
      << pdu.statEvents[BEAMGASEX] << " " << "LUMIEX : "
      << pdu.statEvents[LUMIEX] << " " << "RANDEX : "
      << pdu.statEvents[RANDEX] << " " << "LOWLUMI : "
      << pdu.statEvents[LOWLUMI] << " " << "MIDLUMI : "
      << pdu.statEvents[MIDLUMI] << " " << "HLT1IN : "
      << pdu.statEvents[HLT1IN] << " " << "HLT1EX : "
      << pdu.statEvents[HLT1EX] << " " */<< "Command size is: "
      << sizeof(header) + sizeof(pdu) << endmsg;

  m_srvConnection->sendCommand(&header, &pdu);

  // log closing of file
  if (m_mq_available)
  {

    unsigned int statEvents[MAX_STAT_TYPES];
    if (currFile->getStatEvents(statEvents, MAX_STAT_TYPES) != 0)
    {
      *m_log << MSG::ERROR << WHERE
          << "Error getting the routed event statistics" << endmsg;
    }

    size_t stats_size =
        snprintf(
        NULL, 0,
            "PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
            statEvents[PHYSIN], statEvents[MBIASIN], statEvents[LUMIIN],
            statEvents[BEAMGASIN], statEvents[RANDEX], statEvents[PHYSEX],
            statEvents[MBIASEX], statEvents[LUMIEX], statEvents[BEAMGASEX],
            statEvents[RANDEX], statEvents[LOWLUMI], statEvents[MIDLUMI],
            statEvents[HLT1IN], statEvents[HLT1EX]) + 1;

    char *stats_msg = (char *) malloc(stats_size);
    snprintf(stats_msg, stats_size,
        "PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
        statEvents[PHYSIN], statEvents[MBIASIN], statEvents[LUMIIN],
        statEvents[BEAMGASIN], statEvents[RANDEX], statEvents[PHYSEX],
        statEvents[MBIASEX], statEvents[LUMIEX], statEvents[BEAMGASEX],
        statEvents[RANDEX], statEvents[LOWLUMI], statEvents[MIDLUMI],
        statEvents[HLT1IN], statEvents[HLT1EX]);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    //      size_t msg_size = snprintf(NULL, 0, "closefile%c%i%c%s",  DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name) + 1;
    size_t msg_size = snprintf(NULL, 0,
        "closefile%c%i%c%s%c%s%lu%c%s%u%c%s%u%c%u%c%d%c%s", DELIMITER, getpid(),
        DELIMITER, currFile->getMonitor()->m_name, DELIMITER, "bytesWritten=",
        (unsigned long) currFile->getBytesWritten(), DELIMITER, "events=",
        currFile->getEvents(), DELIMITER, "physEvents=",
        currFile->getPhysStat(), DELIMITER, (unsigned int) tv.tv_sec,
        DELIMITER, (int) tv.tv_usec, DELIMITER, stats_msg) + 1; //XXX Change physEvents to physStat, see with Rainer

    char* msg = (char*) malloc(msg_size);
    //      snprintf(msg, msg_size, "closefile%c%i%c%s", DELIMITER, getpid(), DELIMITER, currFile->getMonitor()->m_name);
    ::snprintf(msg, msg_size,
        "closefile%c%i%c%s%c%s%lu%c%s%u%c%s%u%c%u%c%d%c%s", DELIMITER, getpid(),
        DELIMITER, currFile->getMonitor()->m_name, DELIMITER, "bytesWritten=",
        (unsigned long) currFile->getBytesWritten(), DELIMITER, "events=",
        currFile->getEvents(), DELIMITER, "physEvents=",
        currFile->getPhysStat(), DELIMITER, (unsigned int) tv.tv_sec,
        DELIMITER, (int) tv.tv_usec, DELIMITER, stats_msg);
    if (mq_send(m_mq, msg, msg_size, 0) < 0)
    {
      if (errno != EAGAIN)
      {
        *m_log << MSG::WARNING << "Could not send message, errno=" << errno
            << ". Closing queue" << endmsg;
        m_mq_available = false;
      }
    }
    free(msg);
    free(stats_msg);
    msg = NULL;
    stats_msg = NULL;
  }
  //	*m_log << MSG::INFO << WHERE << endmsg;
}

/* Implements a method from the incident listener
 * interface to be able to handle incidenrs. Here
 * the send thread will be stopped.
 */
void MDFWriterNet::handle(const Incident& inc)
{
  *m_log << MSG::INFO << "Got incident: " << inc.source() << " of type "
      << inc.type() << " for Writer " << getpid() << endmsg;
  if (inc.type() == "DAQ_CANCEL" /*||  inc.type() == "DAQ_ERROR"*/)
  {
    this->stopRetrying();
    m_srvConnection->stopRetrying();
  }
}
/*
 * Stop the endless retries on XML RPC call failures.
 */
void MDFWriterNet::stopRetrying()
{
  m_StopRetry = true;
}

/* Writes out the buffer to the socket through the Connection object.
 * This function first checks if a new file needs to be created. After
 * that, it writes data out to the new file. The MD5 and Adler32 checksums
 * are computed incrementally for the current file. In case the file is above
 * the maximum size limit, then a RunDB call is executed, to which the
 * file name, MD5 sum, and the Adler32 sum are supplied.
 */
StatusCode MDFWriterNet::writeBuffer(void * const /*fd*/, const void *data,
    size_t len)
{
  struct cmd_header header;

  MDFHeader *mHeader;
  RawBank* b = (RawBank*) data;
  if (b->magic() == RawBank::MagicPattern)
  {
    mHeader = b->begin<MDFHeader>();
  }
  else
    mHeader = (MDFHeader*) data;

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

  if (len < 10)
  {
    *m_log << MSG::FATAL << WHERE
        << "Very small message received, not forwarding. Length is: " << len
        << endmsg;
    return StatusCode::SUCCESS;
  }

  //  static int nbLate=0;
  unsigned int runNumber = getRunNumber(mHeader, len);
  //		if ((int) runNumber == -1 )
  if ((int) runNumber <= 0)	// checking for <=0 to discard spuriously large run numbers (> 2 billion seen once)
  {
    *m_log << MSG::FATAL << WHERE
        << "Event with runnumber < 0 received. Not forwarding." << endmsg;
    return StatusCode::SUCCESS;
  }

  // If we get a newer run number, start a timeout on the open files of the previous runs.
  if (m_currentRunNumber < runNumber)
  {
#if 0
    if(nbLate != 0)
    *m_log << MSG::WARNING << WHERE << nbLate << " events were lost, for run number <= " << m_currentRunNumber << endmsg;
    nbLate = 0;
#endif
    m_currentRunNumber = runNumber;
    //      m_discardCurrentRun = false;
    m_TotEvts = 0;
  }

  if (pthread_mutex_lock(&m_SyncFileList))
  {
    *m_log << MSG::ERROR << WHERE << " Locking mutex" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_currFile == NULL || runNumber != m_currFile->getRunNumber())
  {
    m_currFile = m_openFiles.getFile(runNumber);
    // Do not accept event from previous runs if no file is open anymore
    if (!m_currFile && (m_closedRuns.find(runNumber) != m_closedRuns.end()))
    {
      m_discardedEvents[runNumber]++;
      if (!(m_discardedEvents[runNumber] % 10000))
        *m_log << MSG::WARNING << WHERE << " Discarded "
            << m_discardedEvents[runNumber] << " events belonging to run# "
            << runNumber << endmsg;

      if (pthread_mutex_unlock(&m_SyncFileList))
      {
        *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    if (!m_currFile)
    {
      // In a loop, catch recoverable first, if so, continue, else catch failure and manage
      do
      {
        /* The RunDb generates file names now */
        try
        {
          m_currFile = createAndOpenFile(runNumber);
        } catch (RetryException &e)
        {
          m_currFile = NULL;
          *m_log << MSG::ERROR << " Could not get new file name for run "
              << runNumber << ". Retrying ... "
              << " ! Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
              << endmsg;
          *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
          continue;
        } catch (DiscardException &e)
        {
          m_currFile = NULL;
          *m_log << MSG::ERROR << " Run " << runNumber
              << " closed, and no file to be written. All subsequent events will be discarded."
              << endmsg;
          *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
          m_closedRuns.insert(runNumber);
          m_discardedEvents[runNumber] = 0;
          if (pthread_mutex_unlock(&m_SyncFileList))
          {
            *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
            return StatusCode::FAILURE;
          }
          return StatusCode::SUCCESS;
        } catch (FailureException &e)
        {
          m_currFile = NULL;
          *m_log << MSG::ERROR << " Could not get new file name for run "
              << runNumber << " because of an unmanaged error"
              << " ! Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
              << " Retrying until you stop run ..." << endmsg;
          *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
          continue;
        } catch (std::exception &e)
        {
          m_currFile = NULL;
          *m_log << MSG::ERROR << " Could not get new file name for run "
              << runNumber << " because of an unmanaged error"
              << " ! Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
              << " Retrying until you stop run ..." << endmsg;
          *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;

          if (pthread_mutex_unlock(&m_SyncFileList))
          {
            *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
            return StatusCode::FAILURE;
          }
          break;
        }
      } while (!m_StopRetry && (m_currFile == NULL));
      if (m_currFile == NULL)
      {
        *m_log << MSG::ERROR << "Giving up to get new file name for run "
            << runNumber << ". You should stop the run." << endmsg;
        if (pthread_mutex_unlock(&m_SyncFileList))
        {
          *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
          return StatusCode::FAILURE;
        }
        // Error managed by the user, we get here only on a DAQ_CANCEL
        return StatusCode::SUCCESS;
      }
      m_openFiles.addFile(m_currFile);
    }

    // This block is entered in 2 cases:
    // 1- An event from a previous run
    // appears after a run has started. This should be relatively infrequent,
    // and therefore, a good place to check if there are files that have been
    // lying open since a very long time.
    // 2- Fast run change

    File *tmpFile = m_openFiles.getFirstFile();
    // Loop over all the files except the one for whom the event just came in.
    while (tmpFile)
    {
      if (tmpFile->getRunNumber() != runNumber
          && tmpFile->getTimeSinceLastWrite() > m_runFileTimeoutSeconds)
      {
        // This file hasn't been written to in a loong time. Close it.
        *m_log << MSG::INFO << WHERE
            << "Closing a file that did not get events for a long time now: "
            << tmpFile->getFileName()->c_str() << endmsg;
        File *toDelete = tmpFile;
        closeFile(tmpFile);
        // removeFile() returns the next element after the removed one.
        tmpFile = m_openFiles.removeFile(tmpFile);
        if (m_currFile == toDelete)
          m_currFile = NULL;
        //					m_currFile = tmpFile; // NOT a good idea to set it to next file in the list
        delete toDelete;
        continue;
      }
      tmpFile = tmpFile->getNext();
    }

  }

  INIT_WRITE_COMMAND(&header, len, m_currFile->getBytesWritten(),
      m_currFile->getFileName()->c_str(), m_currFile->getSeqNum(), runNumber);
  m_srvConnection->sendCommand(&header, (void*) data);
  // *m_log << MSG::INFO << WHERE << "elements in the queue: " << m_srvConnection->getQueueLength() << endmsg;
  size_t totalBytesWritten = m_currFile->updateWrite(data, len);
  m_currFile->incSeqNum();

  // count event
  m_currFile->incEvents();
  ++m_TotEvts;

  // If we can interpret the header, we perform all statistic computing.
  // Else we just go to the next event.
  // Note that this is after that the event was written, so statistic information is always right.
  // The aim here is not to check data integrity or not, just if we can decode the header, if its
  // version is correct.
  if (checkHeader(mHeader, len))
  {
    // check type of event
    if (checkForPhysStat(mHeader, len))
    {
      m_currFile->incPhysStat();
    }
    countRouteStat(mHeader, len);
  }
  else
  {
    if (pthread_mutex_unlock(&m_SyncFileList))
    {
      *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);
  if (m_UpdatePeriod > 0 && tv.tv_sec - m_prevUpdate.tv_sec > m_UpdatePeriod)
  {
    //update rundb stats ...

    unsigned int trgEvents[MAX_TRIGGER_TYPES];
    if (m_currFile->getTrgEvents(trgEvents, MAX_TRIGGER_TYPES) != 0)
    {
      *m_log << MSG::ERROR << WHERE
          << "Error getting the triggered event statistics" << endmsg;
    }

    unsigned int statEvents[MAX_STAT_TYPES];
    if (m_currFile->getStatEvents(statEvents, MAX_STAT_TYPES) != 0)
    {
      *m_log << MSG::ERROR << WHERE
          << "Error getting the routed event statistics" << endmsg;
    }

    try
    {
      m_rpcObj->updateFile((char *) m_currFile->getFileName()->c_str(),
          trgEvents, statEvents);
    } catch (std::exception& e)
    {
      // Not important error, as update in the middle of the run, so not processed
      *m_log << MSG::WARNING << " Exception: " << e.what() << endmsg;
      *m_log << MSG::WARNING
          << " Could not update Run Database Record. Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log";
      *m_log << " Record is: FileName=" << *(m_currFile->getFileName());
      *m_log << " Run Number=" << m_currentRunNumber << endmsg;
    }
    m_prevUpdate.tv_sec = tv.tv_sec;
    m_prevUpdate.tv_usec = tv.tv_usec;
  }

  // after every MB send statistics
  if (m_mq_available
      && (totalBytesWritten % (30 * 1048576) < len
          || tv.tv_sec - m_prevMsgQueue.tv_sec > 0.5))
  {
    unsigned int statEvents[MAX_STAT_TYPES];
    if (m_currFile->getStatEvents(statEvents, MAX_STAT_TYPES) != 0)
    {
      *m_log << MSG::ERROR << WHERE
          << "Error getting the routed event statistics" << endmsg;
    }

    size_t stats_size =
        snprintf(
        NULL, 0,
            "PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
            statEvents[PHYSIN], statEvents[MBIASIN], statEvents[LUMIIN],
            statEvents[BEAMGASIN], statEvents[RANDEX], statEvents[PHYSEX],
            statEvents[MBIASEX], statEvents[LUMIEX], statEvents[BEAMGASEX],
            statEvents[RANDEX], statEvents[LOWLUMI], statEvents[MIDLUMI],
            statEvents[HLT1IN], statEvents[HLT1EX]) + 1;

    char *stats_msg = (char *) malloc(stats_size);
    snprintf(stats_msg, stats_size,
        "PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
        statEvents[PHYSIN], statEvents[MBIASIN], statEvents[LUMIIN],
        statEvents[BEAMGASIN], statEvents[RANDEX], statEvents[PHYSEX],
        statEvents[MBIASEX], statEvents[LUMIEX], statEvents[BEAMGASEX],
        statEvents[RANDEX], statEvents[LOWLUMI], statEvents[MIDLUMI],
        statEvents[HLT1IN], statEvents[HLT1EX]);

    size_t msg_size = snprintf(NULL, 0,
        "log%c%i%c%s%c%s%lu%c%s%u%c%s%u%c%u%c%d%c%s", DELIMITER, getpid(),
        DELIMITER, m_currFile->getMonitor()->m_name,
        DELIMITER, "bytesWritten=", (unsigned long) totalBytesWritten,
        DELIMITER, "events=", m_currFile->getEvents(), DELIMITER, "physEvents=",
        m_currFile->getPhysStat(), DELIMITER, (unsigned int) tv.tv_sec,
        DELIMITER, (int) tv.tv_usec, DELIMITER, stats_msg) + 1;
    char* msg = (char*) malloc(msg_size);
    snprintf(msg, msg_size, "log%c%i%c%s%c%s%lu%c%s%u%c%s%u%c%u%c%d%c%s",
    DELIMITER, getpid(), DELIMITER, m_currFile->getMonitor()->m_name, DELIMITER,
        "bytesWritten=", (unsigned long) totalBytesWritten, DELIMITER,
        "events=", m_currFile->getEvents(), DELIMITER, "physEvents=",
        m_currFile->getPhysStat(), DELIMITER, (unsigned int) tv.tv_sec,
        DELIMITER, (int) tv.tv_usec, DELIMITER, stats_msg);

    if (mq_send(m_mq, msg, msg_size, 0) < 0)
    {
      if (errno != EAGAIN)
      {
        *m_log << MSG::WARNING << "Could not send message, errno=" << errno
            << ". Closing queue" << endmsg;
        m_mq_available = false;
      }
    }
    free(msg);
    free(stats_msg);
    msg = NULL;
    stats_msg = NULL;

    m_prevMsgQueue.tv_sec = tv.tv_sec;
    m_prevMsgQueue.tv_usec = tv.tv_usec;
  }

  //How much have we written?
  if (totalBytesWritten > (m_maxFileSizeMB << 20)
      || (m_maxEventInFile != 0 && m_maxEventInFile <= m_currFile->getEvents()))
  {
    *m_log << MSG::INFO << WHERE << "Max file size reached; closing file: "
        << m_currFile->getFileName()->c_str() << endmsg;
    closeFile(m_currFile);
    m_openFiles.removeFile(m_currFile);
    delete (m_currFile);
    m_currFile = NULL;
  }

  // WARNING: m_currFile might be NULL after the totalBytesWritten check above!
  // -> segmentation fault

  if (pthread_mutex_unlock(&m_SyncFileList))
  {
    *m_log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
    return StatusCode::FAILURE;
  }

  //Close it, reset counter.
  return StatusCode::SUCCESS;
}

/** Obtains the run number from the MDF header.
 * @param data  The data from which MDF information may be retrieved
 * @param len   The length of the data.
 * @return The run number.
 */
inline unsigned int MDFWriterNet::getRunNumber(MDFHeader *mHeader,
    size_t /*len*/)
{
  /*
  MDFHeader *mHeader;
  RawBank* b = (RawBank*)data;
  if ( b->magic() == RawBank::MagicPattern )
  mHeader = b->begin<MDFHeader>();
  else
  mHeader = (MDFHeader*)data;
  */
  return mHeader->subHeader().H1->m_runNumber;
}

/**
 * For statistics, get some routing bit information from the MDFHeader and count them exclusive and inclusive.
 * Interesting bits are:
 * - Physics Hlt1 = Bit 46 AND Bit 11
 * - Physics Hlt2 = Bit 77
 * - Minimum bias = bit 47
 * - Lumi = bit 33
 * - Beam-gas = bit 49
 * - Random/Other = the rest
 */
inline void MDFWriterNet::countRouteStat(MDFHeader *mHeader, size_t)
{
  static const unsigned int bit77 = 0x2000;
  static const unsigned int bit46 = 0x4000;
  //  static const unsigned int bit11 =0x800;
  static const unsigned int bit33 = 0x2;
  static const unsigned int bit47 = 0x8000;
  static const unsigned int bit49 = 0x20000;
  static const unsigned int bit50 = 0x40000;
  static const unsigned int bit51 = 0x80000;

  //  unsigned int routeBitMask0 = mHeader->subHeader().H1->m_trMask[0];
  unsigned int routeBitMask1 = mHeader->subHeader().H1->m_trMask[1];
  unsigned int routeBitMask2 = mHeader->subHeader().H1->m_trMask[2];
  //  unsigned int routeBitMask3 = mHeader->subHeader().H1->m_trMask[3];

  bool isPhysHlt2 = false, isPhysHlt1 = false, isMBias = false, isLumi =
  false, isBeamGas = false;

  isPhysHlt1 = (routeBitMask1 & bit46);
  isPhysHlt2 = (routeBitMask2 & bit77);
  isMBias = (routeBitMask1 & bit47);
  isLumi = (routeBitMask1 & bit33);
  isBeamGas = (routeBitMask1 & bit49);

  //  if(isPhysHlt1) m_currFile->incPhysEventsIn();
  if (isPhysHlt2)
    m_currFile->incPhysEventsIn();
  if (isMBias)
    m_currFile->incMBiasEventsIn();
  if (isLumi)
    m_currFile->incLumiEventsIn();
  if (isBeamGas)
    m_currFile->incBeamGasEventsIn();
  if (isPhysHlt1)
    m_currFile->incHlt1EventsIn();

  if (routeBitMask1 & bit50)
    m_currFile->incLowLumi();
  if (routeBitMask1 & bit51)
    m_currFile->incMidLumi();

  if (!isPhysHlt2 && !isMBias && !isLumi && !isBeamGas)
    m_currFile->incRandEventsEx();
  if (isPhysHlt2 && !isMBias && !isLumi && !isBeamGas)
    m_currFile->incPhysEventsEx();
  if (!isPhysHlt2 && isMBias && !isLumi && !isBeamGas)
    m_currFile->incMBiasEventsEx();
  if (!isPhysHlt2 && !isMBias && isLumi && !isBeamGas)
    m_currFile->incLumiEventsEx();
  if (!isPhysHlt2 && !isMBias && !isLumi && isBeamGas)
    m_currFile->incBeamGasEventsEx();

}
/*
 > -          Physics  = OR(40:45) AND bits(0:1) = ‘11’

 alternative: bit 46 AND bits(0:1) = '11', or just plain bit 46.
 Note that offline we have been advertising bit 46 as 'the physics triggers',
 see https://twiki.cern.ch/twiki/bin/view/LHCb/HltEfficiency.

 > -          MinBias = bit 47
 > -          Lumi       = bit 33
 > -          BeamGas = bit 49 AND ( bits(0:1) = ‘01’  OR bits(0:1) = ‘10’ )
 > -          Other    = the rest. This means random, “physics” trigger on beam-empty not seen by the beam gas, and anything else

 routingBits = {  0 : '( ODIN_BXTYP == LHCb.ODIN.Beam1 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
 ,  1 : '( ODIN_BXTYP == LHCb.ODIN.Beam2 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
 ,  8 : 'L0_DECISION_PHYSICS'
 ,  9 : "L0_CHANNEL_RE('B?gas')"
 , 10 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'SPD','CALO','MUON,minbias','PU','SPD40','PU20' ] ] )
 , 11 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'Electron','Photon','Hadron','Muon','DiMuon','Muon,lowMult','DiMuon,lowMult','LocalPi0','GlobalPi0'] ] )
 , 12 : "L0_CHANNEL('CALO')" # note: need to take into account prescale in L0...
 , 32 : "HLT_PASS('Hlt1Global')"
 , 33 : "HLT_PASS_SUBSTR('Hlt1Lumi')"
 , 34 : "HLT_PASS_RE('Hlt1(?!Lumi).*Decision')"  # note: we need the 'Decision' at the end to _exclude_ Hlt1Global
 , 35 : "HLT_PASS_SUBSTR('Hlt1Velo')"
 , 36 : "scale(%s,RATE(%s))" % ( "HLT_PASS_RE('Hlt2Express.*Decision')|Hlt1ExpressPhysics", self.getProp('ExpressStreamRateLimit') )
 , 37 : "HLT_PASS('Hlt1ODINPhysicsDecision')"
 , 38 : "HLT_PASS('Hlt1ODINTechnicalDecision')"
 , 39 : "HLT_PASS_SUBSTR('Hlt1L0')"
 , 40 : "HLT_PASS_RE('Hlt1.*Hadron.*Decision')"
 , 41 : "HLT_PASS_RE('Hlt1.*SingleMuon.*Decision')"
 , 42 : "HLT_PASS_RE('Hlt1.*DiMuon.*Decision')"
 , 43 : "HLT_PASS_RE('Hlt1.*MuTrack.*Decision')"
 , 44 : "HLT_PASS_RE('Hlt1.*Electron.*Decision')"
 , 45 : "HLT_PASS_RE('Hlt1.*Pho.*Decision')"
 , 46 : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"    # exclude 'non-physics' lines
 , 47 : "HLT_PASS_RE('Hlt1MBMicroBias.*Decision')"
 , 48 : "HLT_PASS('Hlt1MBNoBiasDecision')"
 , 49 : "HLT_PASS_SUBSTR('Hlt1BeamGas')"
 , 50 : "HLT_PASS('Hlt1LumiLowBeamCrossingDecision')"
 , 51 : "HLT_PASS('Hlt1LumiMidBeamCrossingDecision')"
 , 52 : "HLT_PASS_RE('Hlt1.*SingleHadron.*Decision')"
 , 53 : "HLT_PASS_RE('Hlt1.*DiHadron.*Decision')"
 , 54 : "HLT_PASS_RE('Hlt1.*(SingleMuon|DiMuon|MuTrack).*Decision')"
 # 64--96: Hlt2
 , 64 : "HLT_PASS('Hlt2Global')"
 , 65 : "HLT_PASS('Hlt2DebugEventDecision')"
 , 66 : "HLT_PASS_RE('Hlt2(?!Transparent).*Decision')"
 , 67 : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"
 , 68 : "HLT_PASS_RE('Hlt2.*DiMuon.*Decision')"
 , 69 : "HLT_PASS_RE('Hlt2.*MuTrack.*Decision')"
 , 70 : "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
 , 71 : "HLT_PASS_RE('Hlt2.*Charm.*Decision')"
 , 72 : "HLT_PASS_RE('Hlt2.*IncPhi.*Decision')"
 , 73 : "HLT_PASS_RE('Hlt2.*B.*Gamma.*Decision')"
 , 74 : "HLT_PASS_RE('Hlt2.*B2D2.*Decision')"
 , 75 : "HLT_PASS_RE('Hlt2.*IncDiProton.*Decision')"
 , 76 : "HLT_PASS_RE('Hlt2.*(Bu2|Bs2|Bd2|Bc2|B2HH|B2JpsiX|Dst2|diphotonDiMuon|DisplVertices).*Decision')"
 , 77 : "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Transparent)(?!PassThrough).*Decision')"
 , 78 : "HLT_PASS_RE('Hlt2.*(SingleMuon|DiMuon|MuTrack).*Decision')"
 , 79 : "HLT_PASS_RE('Hlt2.*(Topo|Charm|IncPhi|B2D2).*Decision')"
 }

 */

/**
 * Reach the Odin bank in the MDF, and increment the number of event of this Odin trigger for the current file
 */
inline bool MDFWriterNet::incTriggerType(const MDFHeader *mHeader, size_t)
{
  RawBank *rBanks = (RawBank *) mHeader->data();
  char *ccur = (char*) mHeader->data();

  while (rBanks->magic() == 0xCBCB && rBanks->type() != RawBank::ODIN)
  {
    ccur = ccur + (rBanks->totalSize());
    rBanks = (RawBank*) ccur;
  }

  if (rBanks->magic() == 0xCBCB && rBanks->type() == RawBank::ODIN)
  {
    OnlineRunInfo *ori = (OnlineRunInfo *) rBanks->data();
    if (!m_currFile->incTriggerType(ori->triggerType))
    {
      *m_log << MSG::ERROR << WHERE
          << "No counter increment for event triggered by " << ori->triggerType
          << "." << endmsg;
    }
  }
  else
  {
    *m_log << MSG::ERROR << WHERE
        << "No Odin bank in the event, no trigger type incremented." << endmsg;
    return false;
  }

  return true;
}

/**
 *
 */
inline bool MDFWriterNet::checkHeader(const MDFHeader *mHeader, size_t)
{

  if (!((mHeader->size0() == mHeader->size1())
      && (mHeader->size0() == mHeader->size2())))
  {
    *m_log << MSG::ERROR << WHERE << "MDFHeader corrupted, aborting!" << endmsg;
    //      Incident incident(name(),"DAQ_ERROR");
    //      m_incidentSvc->fireIncident(incident);
    return false;
  }

  // Expect a version 3 only, to apply header1 later :)
  if (mHeader->headerVersion() != 3)
  {
    *m_log << MSG::ERROR << WHERE << "Unknown MDFHeader version "
        << mHeader->headerVersion() << ", aborting!" << endmsg;
    //      Incident incident(name(),"DAQ_ERROR");
    //      m_incidentSvc->fireIncident(incident);
    return false;
  }

  return true;
}

/** Checks if it is a phys event (which means that bit 34 is set).
 * @param data  The data from which MDF information may be retrieved
 * @param len   The length of the data.
 * @return true if it is a phys event, false otherwise
 */
inline bool MDFWriterNet::checkForPhysStat(MDFHeader *mHeader, size_t)
{
  /*
  MDFHeader *mHeader;
  RawBank* b = (RawBank*)data;
  if ( b->magic() == RawBank::MagicPattern )
  mHeader = b->begin<MDFHeader>();
  else
  mHeader = (MDFHeader*)data;
  */
  unsigned int physBitMask = mHeader->subHeader().H1->m_trMask[1];

  if (physBitMask & (unsigned int) 4)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/** Generates a new file name from the MDF information.
 * @param runNumber  The current run number, to be included in the file name.
 */
std::string MDFWriterNet::getNewFileName(unsigned int runNumber)
{
  char buf[MAX_FILE_NAME];
  bzero(buf, MAX_FILE_NAME);
  static unsigned long random = 0;
  random++;
  sprintf(buf, "/daqarea/lhcb/data/2008/RAW/TEST/%s/%u/%s%09u.%02u%06lu.%s",
      m_directory.c_str(), runNumber, m_filePrefix.c_str(), runNumber, 0, // random,
      time(NULL) & 0xFFFF, m_fileExtension.c_str());
  return std::string(buf);
}

/** Destructor.
 * Checks if a file is open, and closes it before closing the connection.
 */
MDFWriterNet::~MDFWriterNet()
{
  if (m_srvConnection)
  {
    *m_log << MSG::WARNING << "Destructor called before finalize. Normal?"
        << endmsg;
    finalize();
  }
  delete m_log;
}

/** A notify listener callback, which is executed when an open command is acked.
 */
void MDFWriterNet::notifyOpen(struct cmd_header *cmd)
{
  // In a loop, catch recoverable first, if so, continue, else catch failure and manage
  while (!m_StopRetry)
  {
    /* The RunDb generates file names now */
    try
    {
      m_rpcObj->createFile(cmd->file_name, cmd->run_no);
    } catch (RetryException &e)
    {
      *m_log << MSG::ERROR << " Could not create Run Database Record."
          << " Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Run Number="
          << cmd->run_no << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      continue;
    } catch (FailureException &e)
    {
      m_currFile = NULL;
      *m_log << MSG::ERROR << " Could not create Run Database Record"
          << " because of a failure! Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Retrying until you stop run ..." << " Record is: FileName="
          << cmd->file_name << " Run Number=" << cmd->run_no << endmsg
          << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      continue;
    } catch (std::exception &e)
    {
      *m_log << MSG::ERROR << " Could not create Run Database Record"
          << " because of an unmanaged exception!"
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Run Number="
          << cmd->run_no << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
    }
    break;
  }
}

/** A notify listener callback, which is executed  when a close command is acked.
 */
void MDFWriterNet::notifyClose(struct cmd_header *cmd)
{
  struct cmd_stop_pdu *pdu = (struct cmd_stop_pdu *) ((char*) cmd
      + sizeof(struct cmd_header));

  unsigned int i = 0;

  // In a loop, catch recoverable first, if so, continue, else catch failure and manage
  while (!m_StopRetry || (m_Finalized && i++ <= m_MaxRetry))
  {
    /* The RunDb generates file names now */
    try
    {
      m_rpcObj->updateFile(cmd->file_name, pdu->trgEvents, pdu->statEvents);
    } catch (RetryException &e)
    {
      char md5buf[33];
      bzero(md5buf, 33);
      unsigned char *md5sum = pdu->md5_sum;
      sprintf(md5buf,
          "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
          md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5],
          md5sum[6], md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11],
          md5sum[12], md5sum[13], md5sum[14], md5sum[15]);
      *m_log << MSG::ERROR << " Could not update Run Database Record"
          << ". Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Adler32 Sum="
          << pdu->adler32_sum << " MD5 Sum=" << md5buf << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      continue;
    } catch (FailureException &e)
    {
      m_currFile = NULL;
      *m_log << MSG::ERROR << " Could not update Run Database Record"
          << " because of a failure. Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Run Number="
          << cmd->run_no << endmsg << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      continue;
    } catch (std::exception &e)
    {
      char md5buf[33];
      bzero(md5buf, 33);
      unsigned char *md5sum = pdu->md5_sum;
      sprintf(md5buf,
          "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
          md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5],
          md5sum[6], md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11],
          md5sum[12], md5sum[13], md5sum[14], md5sum[15]);
      *m_log << MSG::ERROR << " Could not update Run Database Record"
          << " because of an unmanaged error!"
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Adler32 Sum="
          << pdu->adler32_sum << " MD5 Sum=" << md5buf << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
    }
    break;
  }

  i = 0;
  // In a loop, catch recoverable first, if so, continue, else catch failure and manage
  while (!m_StopRetry || (i++ <= m_MaxRetry))
  {
    /* The RunDb generates file names now */
    try
    {
      m_rpcObj->confirmFile(cmd->file_name, pdu->adler32_sum, pdu->md5_sum,
          cmd->data.stop_data.size, pdu->events, pdu->physStat);
    } catch (RetryException &e)
    {
      char md5buf[33];
      bzero(md5buf, 33);
      unsigned char *md5sum = pdu->md5_sum;
      sprintf(md5buf,
          "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
          md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5],
          md5sum[6], md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11],
          md5sum[12], md5sum[13], md5sum[14], md5sum[15]);
      *m_log << MSG::ERROR << " Could not confirm Run Database Record"
          << ". Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Adler32 Sum="
          << pdu->adler32_sum << " MD5 Sum=" << md5buf << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      if (strstr(e.what(),"cannot be modified anymore")!= 0)
      {
        *m_log << MSG::ERROR << "Cannot update File anymore. Breaking Loop" << endmsg;
        break;
      }
      else
      {
        continue;
      }
    } catch (FailureException &e)
    {
      m_currFile = NULL;
      *m_log << MSG::ERROR << " Could not get new file name "
          << " because of a failure. Retrying."
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Run Number="
          << cmd->run_no << endmsg << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
      if (strstr(e.what(),"cannot be modified anymore")!= 0)
      {
        *m_log << MSG::ERROR << "Cannot update File anymore. Breaking Loop" << endmsg;
        break;
      }
      else
      {
        continue;
      }
    } catch (std::exception &e)
    {
      char md5buf[33];
      bzero(md5buf, 33);
      unsigned char *md5sum = pdu->md5_sum;
      sprintf(md5buf,
          "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
          md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5],
          md5sum[6], md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11],
          md5sum[12], md5sum[13], md5sum[14], md5sum[15]);
      *m_log << MSG::ERROR << " Could not update Run Database Record"
          << " because of an unmanaged error!"
          << " Check the RunDB XML_RPC logfile /clusterlogs/services/xmlrpc.log"
          << " Record is: FileName=" << cmd->file_name << " Adler32 Sum="
          << pdu->adler32_sum << " MD5 Sum=" << md5buf << endmsg;
      *m_log << MSG::ERROR << " Exception: " << e.what() << endmsg;
    }
    *m_log << MSG::INFO << "Confirmed file " << cmd->file_name << " RunNumber: "
        << cmd->run_no << " " /*<< "Adler32: "
        << pdu->adler32_sum << " " << "MD5: " << pdu->md5_sum << " "
        << "Seq Nr: " << cmd->data.stop_data.seq_num << " " << "Size: "
        << cmd->data.stop_data.size << " " << "Events: " << pdu->events
        << " " << "Phys:  " << pdu->physStat << " " << "Trg0 : "
        << pdu->trgEvents[0] << " " << "Trg1 : " << pdu->trgEvents[1]
        << " " << "Trg2 : " << pdu->trgEvents[2] << " " << "Trg3 : "
        << pdu->trgEvents[3] << " " << "Trg4 : " << pdu->trgEvents[4]
        << " " << "Trg5 : " << pdu->trgEvents[5] << " " << "Trg6 : "
        << pdu->trgEvents[6] << " " << "Trg7 : " << pdu->trgEvents[7]
        << " " << "PHYSIN : " << pdu->statEvents[PHYSIN] << " "
        << "MBIASIN : " << pdu->statEvents[MBIASIN] << " "
        << "BEAMGASIN : " << pdu->statEvents[BEAMGASIN] << " "
        << "LUMIIN : " << pdu->statEvents[LUMIIN] << " " << "RANDIN : "
        << pdu->statEvents[RANDIN] << " " << "PHYSEX : "
        << pdu->statEvents[PHYSEX] << " " << "MBIASEX : "
        << pdu->statEvents[MBIASEX] << " " << "BEAMGASEX : "
        << pdu->statEvents[BEAMGASEX] << " " << "LUMIEX : "
        << pdu->statEvents[LUMIEX] << " " << "RANDEX : "
        << pdu->statEvents[RANDEX] << " " << "LOWLUMI : "
        << pdu->statEvents[LOWLUMI] << " " << "MIDLUMI : "
        << pdu->statEvents[MIDLUMI] << " " << "HLT1IN : "
        << pdu->statEvents[HLT1IN] << " " << "HLT1EX : "
        << pdu->statEvents[HLT1EX] << " " */<< endmsg;
    break;
  }
}

/** A notify listener callback, which is executed  when an error occurs.
 */
void MDFWriterNet::notifyError(struct cmd_header* /*cmd*/, int /*errno*/)
{
  /* Not Used Yet. */
}

StatusCode MDFWriterNet::CleanUpFiles()
{

  MsgStream *log = new MsgStream(*m_log);
  *log << MSG::INFO << WHERE << " Clean up routine started" << endmsg;
  while (!m_CleanUpStop)
  {
    sleep(1);

    if (pthread_mutex_lock(&m_SyncFileList))
    {
      *log << MSG::ERROR << WHERE << " Locking mutex" << endmsg;
      return StatusCode::FAILURE;
    }
    File *tmpFile = m_openFiles.getFirstFile();
    // Loop over all the files except the one for whom the event just came in.
    while (tmpFile)
    {
      //			if (tmpFile->getRunNumber() != m_currentRunNumber && tmpFile->getTimeSinceLastWrite()	> m_runFileTimeoutSeconds)
      if (tmpFile->getTimeSinceLastWrite() > 3 * m_runFileTimeoutSeconds)
      {
        // This file hasn't been written to in a loong time. Close it.
        *log << MSG::INFO << WHERE << "Closing file "
            << tmpFile->getMonitor()->m_name << " after time out." << endmsg;
        File *toDelete = tmpFile;
        closeFile(tmpFile);
        // removeFile() returns the next element after the removed one.
        tmpFile = m_openFiles.removeFile(tmpFile);
        if (m_currFile == toDelete)
          m_currFile = NULL;
        //					m_currFile = tmpFile; // NOT a good idea to set it to next file in the list
        delete toDelete;
        continue;
      }
      tmpFile = tmpFile->getNext();
    }
    if (pthread_mutex_unlock(&m_SyncFileList))
    {
      *log << MSG::ERROR << WHERE << " Unlocking mutex" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  *log << MSG::INFO << WHERE << " Clean up routine ended" << endmsg;
  return StatusCode::SUCCESS;
}

/** pthread startup function to manage Odin MEP.
 */
void *FileCleanUpStartup(void *object)
{
  MDFWriterNet *writer = (MDFWriterNet *) object;

  StatusCode sc = writer->CleanUpFiles();
  //printf("Deleting object\n");
  //delete tgtObject;
  if (sc.isFailure())
    return (void *) -1;
  else
    return (void *) 0;
}

//#endif /* _WIN32 */
