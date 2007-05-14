#ifdef BUILD_WRITER

#include <string>
#include <stdexcept>
#include <ctime>

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

static inline unsigned long adler32(unsigned long adler,
    const char *buf,
    unsigned int len);

/**
 * Macro for initialising a close command.
 */
#define INIT_CLOSE_COMMAND(h, fname, adler32, md5) { \
  (h)->cmd = CMD_CLOSE_FILE; \
    (md5)->Final((h)->data.stop_data.md5_sum); \
    (h)->data.stop_data.adler32_sum = (adler32); \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising a write command.
 */
#define INIT_WRITE_COMMAND(h, len, off, fname) { \
  (h)->cmd = CMD_WRITE_CHUNK; \
    (h)->data.chunk_data.size = (len); \
    (h)->data.chunk_data.offset = (off); \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
}

/**
 * Macro for initialising an open command.
 */
#define INIT_OPEN_COMMAND(h, fname, r_num) { \
  (h)->cmd = CMD_OPEN_FILE; \
    strncpy((h)->file_name, (fname), MAX_FILE_NAME); \
    (h)->data.start_data.run_num = r_num; \
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

  m_log = new MsgStream(msgSvc(), name());
}

/** Overrides MDFWriter::initialize(). Initialises the Connection object.
 */
StatusCode MDFWriterNet::initialize(void)
{

  m_fileOpen = 0;
  m_bytesWritten = 0;
  m_connection = new Connection(m_serverAddr, m_serverPort,
  	m_sndRcvSizes, m_log, this);
  m_rpcObj = new RPCComm(m_runDBURL.c_str());
  try {

    m_connection->initialize();

  } catch(const std::exception& e) {
    *m_log << MSG::ERROR << "Caught Exception:" << e.what() << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/** Overrides MDFWriter::finalize().
 * Closes the file if it is open, and writes its entry in the
 * Run Database.
 */
StatusCode MDFWriterNet::finalize(void)
{
  if(m_fileOpen) {
    struct cmd_header header;
    INIT_CLOSE_COMMAND(&header, m_fileName.c_str(), m_adler32, m_md5);

    delete m_md5;
    m_connection->sendCommand(&header);
    m_bytesWritten = 0;
    m_fileOpen = 0;
  }

  // closeConnection() blocks till everything is flushed.
  m_connection->closeConnection();
  delete m_connection;
  delete m_rpcObj;

  m_connection = NULL;
  m_rpcObj = NULL;

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

  //Is a file already open?
  if(!m_fileOpen) {
    unsigned int runNumber = getRunNumber(data, len);

    getNewFileName(m_fileName, data, len);
    INIT_OPEN_COMMAND(&header, m_fileName.c_str(), runNumber);
    m_connection->sendCommand(&header);
    m_fileOpen = 1;

    m_md5 = new TMD5();
    m_adler32 = adler32(0, NULL, 0);

    *m_log << MSG::INFO << "Opening a file." << endmsg;

  }

  INIT_WRITE_COMMAND(&header, len, m_bytesWritten, m_fileName.c_str());
  m_connection->sendCommand(&header, (void*)data);
  m_md5->Update((UChar_t*)data, (UInt_t)len);
  m_adler32 = adler32(m_adler32, (const char*)data, len);

  //How much have we written?
  m_bytesWritten += len;
  if(m_bytesWritten > (m_maxFileSizeMB << 20)) {

    //Write out close command; runDB record created upon receiving ack.
    struct cmd_header header;
    INIT_CLOSE_COMMAND(&header, m_fileName.c_str(), m_adler32, m_md5);

    *m_log << MSG::INFO << "Closing a file." << endmsg;

    delete m_md5;
    m_connection->sendCommand(&header);
    m_bytesWritten = 0;
    m_fileOpen = 0;
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
 * @param newFileName A string to put the new file name in.
 * @param data        The data from which MDF information may be retrieved.
 * @param len         The length of the data buffer supplied.
 */
void MDFWriterNet::getNewFileName(std::string &newFileName,
    const void * /*data*/, size_t /*len*/)
{
  char buf[MAX_FILE_NAME];
  static unsigned long random;
  random++;
  sprintf(buf, "%s.%lu.%lu",
      m_filePrefix.c_str(),
      random,
      time(NULL));
  newFileName = buf;
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
  	*m_log << MSG::INFO << "Received open notify. . ." << cmd->file_name;
    m_rpcObj->createFile(cmd->file_name, cmd->data.start_data.run_num);

  } catch(std::exception& e) {
    *m_log << MSG::ERROR << "Could not create Run Database Record ";
    *m_log << "Cause: " << e.what() << std::endl;
    *m_log << "Record is: FileName=" << cmd->file_name;
    *m_log << " Run Number=" << cmd->data.start_data.run_num << endmsg;
  }
  *m_log << "Done." << endmsg;
}

/** A notify listener callback, which is executed  when a close command is acked.
 */
void MDFWriterNet::notifyClose(struct cmd_header *cmd)
{
  try {

  	*m_log << MSG::INFO << "Received close notify. . ." << cmd->file_name;

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

  *m_log << "Done." << endmsg;

}

/** A notify listener callback, which is executed  when an error occurs.
 */
void MDFWriterNet::notifyError(struct cmd_header* /*cmd*/, int /*errno*/)
{
  /* Not Used Yet. */
}

/* The standard Adler32 algorithm taken from the Linux kernel.*/

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

static inline unsigned long adler32(unsigned long adler,
    const char *buf,
    unsigned int len)
{
  unsigned long s1 = adler & 0xffff;
  unsigned long s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == NULL) return 1L;

  while (len > 0) {
    k = len < NMAX ? len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0) do {
      s1 += *buf++;
      s2 += s1;
    } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  return (s2 << 16) | s1;
}

#endif /* _WIN32 */
