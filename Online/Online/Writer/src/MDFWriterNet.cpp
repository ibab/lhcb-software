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
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Writer/Connection.h"
#include "Writer/MDFWriterNet.h"
#include "TMD5.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterNet);

using namespace LHCb;

/**
 * Macro for initialising a close command.
 */
#define INIT_CLOSE_COMMAND(h, fname, adler32, md5, rno) { \
  (h)->cmd = CMD_CLOSE_FILE; \
    (md5)->Final((h)->data.stop_data.md5_sum); \
    (h)->data.stop_data.adler32_sum = (adler32); \
    (h)->run_no = rno;	\
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising a write command.
 */
#define INIT_WRITE_COMMAND(h, len, off, fname, rno) { \
  (h)->cmd = CMD_WRITE_CHUNK; \
    (h)->data.chunk_data.size = (len); \
    (h)->run_no = rno;	\
    (h)->data.chunk_data.offset = (off); \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising an open command.
 */
#define INIT_OPEN_COMMAND(h, fname, rno) { \
  (h)->cmd = CMD_OPEN_FILE; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
    (h)->run_no = rno;	\
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

  m_log = new MsgStream(msgSvc(), name());
}

/** Overrides MDFWriter::initialize(). Initialises the Connection object.
 */
StatusCode MDFWriterNet::initialize(void)
{
  *m_log << MSG::INFO << " Writer " << getpid() <<
    " Initializing." << endmsg;

  m_connection = new Connection(m_serverAddr, m_serverPort,
      m_sndRcvSizes, m_log, this);
  m_rpcObj = new RPCComm(m_runDBURL.c_str());
  try {

    m_connection->initialize();

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
  *m_log << MSG::INFO << " Writer " << getpid() <<
    " Finalizing." << endmsg;

  FileIterator fi = m_openFiles.begin();
  File *currFile;

  for(; fi != m_openFiles.end(); fi++) {
	currFile = fi->second;
	if(currFile->isOpen()) {
	    struct cmd_header header;
	    *m_log << MSG:: INFO << "Closing file " << currFile->getFileName()
	    	<< " with run number " << currFile->getRunNumber() << endmsg;
		INIT_CLOSE_COMMAND(&header,
				currFile->getFileName()->c_str(),
				currFile->getAdlerChecksum(),
				currFile->getMD5Checksum(),
				currFile->getRunNumber());
		m_connection->sendCommand(&header);
		delete currFile;
	}
  }

  // closeConnection() blocks till everything is flushed.
  m_connection->closeConnection();
  delete m_connection;
  delete m_rpcObj;

  m_connection = NULL;
  m_rpcObj = NULL;

  *m_log << MSG::INFO << " Writer " << getpid() <<
    " Finalized." << endmsg;
  return StatusCode::SUCCESS;
}



/** Overrides MDFWriter::writeBuffer().
 * Writes out the buffer to the socket through the Connection object.
 * This function first checks if a new file needs to be created. After
 * that, it writes data out to the new file. The MD5 and Adler32 checksums
 * are computed incrementally for the current file. In case the file is above
 * the maximum size limit, then a RunDB call is executed, to which the
 * file name, MD5 sum, and the Adler32 sum are supplied.
 */
StatusCode MDFWriterNet::writeBuffer(void *const /*fd*/, const void *data, size_t len)
{
  struct cmd_header header;

  if ( len < 10 ) {
    *m_log << MSG::FATAL << "Writer " << getpid() <<
      "Very small message received, not forwarding." << len << endmsg;
     return StatusCode::SUCCESS;
  }

  //Try to see if there is a file open for the specific run number in this chunk.
  unsigned int runNumber = getRunNumber(data, len);

  FileIterator fi = m_openFiles.find(runNumber);
  File *currFile;
  if(fi == m_openFiles.end()) {
     /// A file is not open for this run number. So open a new file.
    currFile = new File(getNewFileName(runNumber), runNumber);
    *m_log << MSG::INFO << "No file exists for run " << runNumber << " Creating a new one." << endmsg;
    m_openFiles.insert(std::make_pair(runNumber, currFile));
  } else {
    /// Already exists. Get its information.
    currFile = fi->second;
  }


  //Is the current file open, or just created?
  if(!currFile->isOpen()) {
	//Just created. Send the open command.
    INIT_OPEN_COMMAND(&header, currFile->getFileName()->c_str(), runNumber);
    m_connection->sendCommand(&header);
    currFile->open();
  }

  INIT_WRITE_COMMAND(&header, len,
		  currFile->getBytesWritten(),
		  currFile->getFileName()->c_str(),
		  runNumber);
  m_connection->sendCommand(&header, (void*)data);
  size_t totalBytesWritten = currFile->updateWrite(data, len);

  //How much have we written?
  if(totalBytesWritten > (m_maxFileSizeMB << 20)) {

    //Write out close command; runDB record created upon receiving ack.
    struct cmd_header header;
    INIT_CLOSE_COMMAND(&header,
    		currFile->getFileName()->c_str(),
    		currFile->getAdlerChecksum(),
    		currFile->getMD5Checksum(),
    		runNumber);

    m_connection->sendCommand(&header);

    //Remove file from map.
    m_openFiles.erase(runNumber);
    delete currFile;
  }

  //Close it, reset counter.
  return StatusCode::SUCCESS;
}


/** Obtains the run number from the MDF header.
 * @param data  The data from which MDF information may be retrieved
 * @param len   The length of the data.
 * @return The run number.
 */
inline unsigned int MDFWriterNet::getRunNumber(const void *data, size_t /*len*/)
{
  MDFHeader *mHeader;
  mHeader = (MDFHeader*)data;
  return mHeader->subHeader().H1->m_runNumber;
}


/** Generates a new file name from the MDF information.
 * @param runNumber  The current run number, to be included in the file name.
 */
std::string MDFWriterNet::getNewFileName(unsigned int runNumber)
{
  char buf[MAX_FILE_NAME];
  static unsigned long random;
  random++;
  sprintf(buf, "%s/%s.%u.%lu.%lu",
      m_directory.c_str(),
      m_filePrefix.c_str(),
      runNumber,
      random,
      time(NULL));
  return std::string(buf);
}

/** Destructor.
 * Checks if a file is open, and closes it before closing the connection.
 */
MDFWriterNet::~MDFWriterNet()
{
  if(m_connection) {
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
        cmd->data.stop_data.md5_sum);
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
