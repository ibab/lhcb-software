#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFIO.h"
#include "Writer/Connection.h"
#include "Writer/RPCComm.h"
#include "TMD5.h"

#include <map>

#include <unistd.h>
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

#ifndef MDFWRITERNET_H
#define MDFWRITERNET_H

class ISvcLocator;
class IIncidentSvc;

/*
 * LHCb namespace
 */

namespace LHCb {

  /** @class File
   *  Represents an open file to which data are being streamed.
   *
   * @author: Sai Suman Cherukuwada
   * @version: 1.0
   */
  class File {
  private:
    /// The current file being written to.
    std::string m_fileName;
    
    //// The stream this file belongs to
    std::string m_streamID;
    
    /// The MD5 checksum of the file.
    TMD5 *m_md5;

    /// Pointer to monitoring structure
    struct Monitor {

      /// Local buffer for file name
      char   m_name[255];

      /// The number of bytes written so far in the current file.
      size_t m_bytesWritten;

      /// The number of events
      unsigned int m_events;
      
      /// The number of lumi events, that means bit 34 in the trigger mask
      /// was set
      unsigned int m_lumiEvents;

      /// The sequence number of the last command that was sent for this file.
      unsigned int m_seqNum;

      /// The Adler32 checksum of the file.
      unsigned int m_adler32;

      /// The run number that this file is associated with.
      unsigned int m_runNumber;

      /// The last time a write was made to this file (for determining timeouts).
      time_t m_lastUpdated;

      /// A boolean to keep track of whether the file is open or closed.
      int    m_fileOpen;

      /// DIM publishing service ID
      int m_svcID;

    }   m_monBuffer;

    /// Reference to monitoring structure
    Monitor* m_mon;

    /// A link to the next element in the list.
    File *m_next;

    /// A link to the previous element in the list.
    File *m_prev;

    static void feedMonitor(void* tag, void** buf, int* size, int* first);

    void init(const std::string& fname,  unsigned int runNumber);

  public:
    /// get the monitor values
    inline Monitor* getMonitor() { return m_mon; } 

    /// Increments the sequence number.
    inline void incSeqNum() { ++m_mon->m_seqNum; }

    /// Returns the current sequence number.
    inline unsigned int getSeqNum() { return m_mon->m_seqNum; }

    /// Sets the status of the current file to open.
    inline void open() { m_mon->m_fileOpen = true; }

    /// Returns the number of bytes written to the file so far.
    inline size_t getBytesWritten() { return m_mon->m_bytesWritten; }
    
    /// increases the number of events by one
    inline void incEvents() { m_mon->m_events++; }
    
    /// increases the number of lumi events by one
    inline void incLumiEvents() { m_mon->m_lumiEvents++; }
    
    /// set the value of events, used when there where events before the
    /// file was created
    inline void setEvents(unsigned int events) {
        m_mon->m_events = events;
    }

    /// get the number of events
    inline unsigned int getEvents() { return m_mon->m_events; }

    /// get the number of lumi events
    inline unsigned int getLumiEvents() { return m_mon->m_lumiEvents; }

    /// get the phys stat which is number of events - lumi events
    inline unsigned int getPhysStat() { return m_mon->m_events - m_mon->m_lumiEvents;}

    /// Returns the name of the file.
    inline std::string* getFileName() { return &m_fileName; }

    /// Returns the Adler32 checksum computed so far.
    inline int getAdlerChecksum() { return m_mon->m_adler32; }

    /// Returns the MD5 checksum calculated so far.
    inline TMD5* getMD5Checksum() { return m_md5; }

    /// Returns the run number associated with this file.
    inline unsigned int getRunNumber() { return m_mon->m_runNumber; }

    /// Returns how many seconds elapsed since the last update.
    inline time_t getTimeSinceLastWrite() { return time(NULL) - m_mon->m_lastUpdated; }

    /// Returns true if the file is open, and false otherwise.
    inline bool isOpen() { return m_mon->m_fileOpen; }

    /// Sets the next file after this one.
    inline void setNext(File *next) { m_next = next; }

    /// Sets the previous file before this one.
    inline void setPrev(File *prev) { m_prev = prev; }

    /// Gets the next file after this element.
    inline File* getNext() { return m_next; }

    /// Gets the previous element after this element.
    inline File* getPrev() { return m_prev; }

    /// feed the named message queue which is used for monitoring
    void feedMessageQueue(mqd_t);

    /// Constructor
//    File(std::string fileName, unsigned int runNumber) {
//      m_seqNum = 0;
//      m_fileName = fileName;
//      m_runNumber = runNumber;
//      m_md5 = new TMD5();
//      m_adler32 = adler32Checksum(1L, NULL, 0);
//      m_lastUpdated = time(NULL);
//      m_fileOpen = false;
//      m_bytesWritten = 0;
//      m_prev = NULL;
//      m_next = NULL;
//    }
    File(const std::string& fileName, unsigned int runNumber);

    File(const std::string& fileName, unsigned int runNumber, const std::string& streamID);

    /// Updates the checksums and the bytes written count for the file, given a new chunk to be written.
    /// Returns the number of bytes written in all so far.
    inline size_t updateWrite(const void *data, size_t len) {
      m_md5->Update((UChar_t*)data, (UInt_t)len);
      m_mon->m_adler32 = adler32Checksum(m_mon->m_adler32, (const char*)data, len);
      m_mon->m_bytesWritten += len;
      m_mon->m_lastUpdated = time(NULL);
      return m_mon->m_bytesWritten;
    }

    /// Destructor.
    ~File();
  };

  /**
   * Need to define a custom list here because we need to modify elements on the fly.
   * Can't do that with STL iterators, and don't need anything other than sequential
   * searching because the list will at maximum be 3 elements long.
   */
  class FileList {
  private:
    File *m_top;
    File *m_bottom;

  public:
    FileList(void) { m_top = NULL; m_bottom = NULL; }

    void addFile(File *f)
    {
      if(!m_top) {
	m_top = f;
	m_bottom = f;
        f->setNext(NULL); 
	f->setPrev(NULL);
      } else {
	m_bottom->setNext(f);
	f->setPrev(m_bottom);
	f->setNext(NULL);
	m_bottom = f;
      }
    }

    File *getFile(unsigned int runNumber)
    {
      File *tmp = m_top;
      while(tmp) {
        if(tmp->getRunNumber() == runNumber)
          return tmp;
        tmp = tmp->getNext();
      }
      return NULL;
    }
    File* removeFile(File *f) 
    {
      if(f->getPrev() == NULL)
	m_top = f->getNext();
      else
	f->getPrev()->setNext(f->getNext());
      if(f->getNext() == NULL)
	m_bottom = f->getPrev();
      else
	f->getNext()->setPrev(f->getPrev());
      return f->getNext();
    }

    File *getFirstFile(void)
    {
      return m_top;
    }
  };


  /** @class MDFWriterNet
   *
   * @author: Sai Suman Cherukuwada
   * @version: 1.0
   */

  class MDFWriterNet : public MDFWriter, INotifyClient, virtual public IIncidentListener {
    typedef LHCb::Connection Connection;
  protected:
    /// The initial storage server hostname to connect to.
    std::string m_serverAddr;

    /// The port of the storage server to connect to.
    int m_serverPort;

    /// The RPC URL of the Run Database service.
    std::string m_runDBURL;

    /// A prefix to a apply to all file names that are created.
    std::string m_filePrefix;

    /// Directory name to apply to all file names that are created.
    std::string m_directory;

    /// File extension to apply to all file names that are created.
    std::string m_fileExtension;

    /// The maximum amount of seconds to wait for events after which a file is closed.
    int m_runFileTimeoutSeconds;

    /// The maximum file size (in MB) to write before creating a new file.
    size_t m_maxFileSizeMB;

    /// The maximum size of the queue in m_mmObj
    size_t m_maxQueueSizeBytes;

    /// The socket send/receive buffer sizes.
    int m_sndRcvSizes;

    /// The current run number in progress.
    unsigned int m_currentRunNumber;

    /// Property: The stream identifier
    std::string m_streamID;

    /// A map of all current open files.
    FileList m_openFiles;

    /// The current open file.
    File *m_currFile;

    /// The connection object being used to talk to the server.
    Connection *m_srvConnection;

    IIncidentSvc* m_incidentSvc;
    
    /// The object that encapsulates all RPC communication.
    RPCComm *m_rpcObj;

    /// The message stream to log to.
    MsgStream *m_log;
    
    /// the named queue which is used to send the monitoring messages to the
    /// daemon running on this machines which collects all monitoring values
    /// from all writers on this machine and then publishes them over DIM
    mqd_t  m_mq;

    /// indicator if the named queue is broken or not
    /// a broken monitoring system should not prevent the writer from working
    bool m_mq_available;

    /// The boolean to stop or not the clean up thread routine.
    bool m_CleanUpStop;

    /// The thread which checks file time outs.
    pthread_t m_ThreadFileCleanUp;

    /// A mutex to protect shared resources between the main process and the clean up thread.
    pthread_mutex_t m_SyncFileList; 

    /** Generates a new file name from the MDF information.
     * @param runNumber  The current run number, to be included in the file name.
     */
    virtual std::string getNewFileName(unsigned int runNumber);

    /// Similar to MDFWriter::construct()
    virtual void constructNet(void);

    /// Returns the run number from an MDF header.
    virtual unsigned int getRunNumber(const void *data, size_t len);

    /// check if an event is a lumi event
    virtual bool checkForLumiEvent(const void*, size_t);

    /// Returns a File object for the specified run number

  public:
    /// Destructor.
    virtual ~MDFWriterNet();

    /// Sends a CLOSE command to the server to close a file.
    void closeConnection();

    /// Overrides Algorithm::initialize()
    virtual StatusCode initialize();
    
    /// Overrides Algorith::finalize()
    virtual StatusCode finalize();

    /// Overrides Algorithm::execute()
    virtual StatusCode execute();

    /// Extended Algorithm constructor (delegates to MDFWriter constructor).
    MDFWriterNet(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc);

    /// Standard Algorithm constructor (delegates to MDFWriter constructor).
    MDFWriterNet(const std::string& nam, ISvcLocator* pSvc);

    /// Creates a File using a rpc object
    virtual std::string createNewFile(unsigned int runNumber);

    /// Sends the command required to open a file and returns the File object.
    File *createAndOpenFile(unsigned int runNumber);

    /// Sends the command required to close a file given the file object.
    void closeFile(File *file);

    // handle incidents
    void handle(const Incident&);
    
    /// Writes a chunk to the storage cluster server. Overrides Algorithm::writeBuffer().
    virtual StatusCode writeBuffer(void *const ioDesc, const void *data, size_t len);

    /// Implemented from INotifyClient.
    virtual void notifyClose(struct cmd_header *cmd);

    /// Implemented from INotifyClient.
    virtual void notifyError(struct cmd_header *cmd, int err_no);

    /// Implemented from INotifyClient.
    virtual void notifyOpen(struct cmd_header *cmd);

    /// Cleans timed out files
    virtual StatusCode CleanUpFiles();

  };
}

#ifndef MDF_THREADINTERFACE
#define MDF_THREADINTERFACE
extern "C" void *FileCleanUpStartup(void *);
#endif // MEP_THREADINTERFACE


#endif //MDFWRITERNET_H

