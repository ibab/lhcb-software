#ifdef BUILD_WRITER

#include <string>
#include <stdexcept>
#include <ctime>
#include <map>

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

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterNet);

using namespace LHCb;

/**
 * Macro for initialising a close command.
 */
#define INIT_CLOSE_COMMAND(h, fname, adler_32, md_5, seqno, rno, thesize, noofevents) { \
    (h)->cmd = CMD_CLOSE_FILE; \
    (h)->run_no = rno;  \
    (h)->data.chunk_data.seq_num = seqno; \
    (h)->data.stop_data.adler32_sum = (adler_32); \
    (md_5)->Final((h)->data.stop_data.md5_sum); \
    (h)->data.stop_data.size = thesize; \
    (h)->data.stop_data.events = noofevents; \
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
  try {

    m_srvConnection->initialize();

  } catch(const std::exception& e) {
    *m_log << MSG::ERROR << "Caught Exception:" << e.what() << endmsg;
    return StatusCode::FAILURE;
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
  return StatusCode::SUCCESS;
}


/**
 * Queues a command that creates a file.
 */
File* MDFWriterNet::createAndOpenFile(unsigned int runNumber)
{
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
    currFile = new File(m_rpcObj->createNewFile(runNumber), runNumber);
  } catch (...) {
    currFile = new File(getNewFileName(runNumber), runNumber);
    *m_log << MSG::WARNING
           << "Could not get new file name! Generating local filename: "
           << *(currFile->getFileName()) << endmsg ;
  }

  INIT_OPEN_COMMAND(&header, currFile->getFileName()->c_str(),
                    currFile->getSeqNum(), runNumber);
  m_srvConnection->sendCommand(&header);
  currFile->open();
  currFile->incSeqNum();
  return currFile;
}

/**
 * Queues a command that closes a file.
 */
void MDFWriterNet::closeFile(File *currFile)
{
  struct cmd_header header;
  unsigned long events=0;
  memset(&header, 0, sizeof(struct cmd_header));

  INIT_CLOSE_COMMAND(&header,
		     currFile->getFileName()->c_str(),
		     currFile->getAdlerChecksum(),
		     currFile->getMD5Checksum(),
		     currFile->getSeqNum(),
		     currFile->getRunNumber(),
                     currFile->getBytesWritten(),
                     events);
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
  size_t totalBytesWritten = m_currFile->updateWrite(data, len);
  m_currFile->incSeqNum();

  //How much have we written?
  if (totalBytesWritten > (m_maxFileSizeMB << 20)) {
    closeFile(m_currFile);
    m_openFiles.removeFile(m_currFile);
    delete(m_currFile);
    m_currFile = NULL;
  }

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


/** Generates a new file name from the MDF information.
 * @param runNumber  The current run number, to be included in the file name.
 */
std::string MDFWriterNet::getNewFileName(unsigned int runNumber)
{
  char buf[MAX_FILE_NAME];
  static unsigned long random = 0;
  random++;
  sprintf(buf, "%s/%u/%s%09u.%02u%06lu.%s",
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
  try {
    m_rpcObj->confirmFile(cmd->file_name,
			              cmd->data.stop_data.adler32_sum,
			              cmd->data.stop_data.md5_sum,
                          cmd->data.stop_data.size,
                          0);
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
}

/** A notify listener callback, which is executed  when an error occurs.
 */
void MDFWriterNet::notifyError(struct cmd_header* /*cmd*/, int /*errno*/)
{
  /* Not Used Yet. */
}
#endif /* _WIN32 */
