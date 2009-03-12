#ifndef WRITER_REPROCESSINGWRITER_H
#define WRITER_REPROCESSINGWRITER_H

#include "MDF/MDFWriter.h"
#include "Writer/MDFWriterNet.h"
#include "GaudiKernel/IIncidentListener.h"

/*
 * LHCb namespace
 */
namespace LHCb {

  /** @class ReprocessingWriter
   *
   * @author: Sai Suman Cherukuwada
   * @version: 1.0
   */

  class ReprocessingWriter : public MDFWriter, 
    virtual public INotifyClient,
    virtual public IIncidentListener
    {
      typedef LHCb::Connection Connection;
    protected:
      /// The initial storage server hostname to connect to.
      std::string m_serverAddr;
      
      /// The port of the storage server to connect to.
      int m_serverPort;

      /// The RPC URL of the Run Database service.
      std::string m_runDBURL;

      /// The maximum amount of seconds to wait for events after which a file is closed.
      int m_runFileTimeoutSeconds;

      /// The maximum file size (in MB) to write before creating a new file.
      size_t m_maxFileSizeMB;
      
      /// The maximum size for the message queue
      size_t m_maxQueueSizeBytes;

      /// The socket send/receive buffer sizes.
      int m_sndRcvSizes;

      /// Maximum memory buffer length in MBytes
      int m_memBufferLen;

      /// Current number of buffered bytes
      long long int m_bufferLength;

      /// A map of all current open files.
      FileList m_openFiles;

      /// The current open file.
      File *m_currFile;

      /// The connection object being used to talk to the server.
      Connection *m_srvConnection;

      /// The object that encapsulates all RPC communication.
      RPCComm *m_rpcObj;

      /// The message stream to log to.
      MsgStream *m_log;

      /// Flag to indicate a CANCEL command was received.
      bool    m_cancel;

      /// Returns the run number from an MDF header.
      virtual unsigned int getRunNumber(const void *data, size_t len);

    public:
      /// Standard Algorithm constructor (delegates to MDFWriter constructor).
      ReprocessingWriter(const std::string& nam, ISvcLocator* pSvc);

      /// Destructor.
      virtual ~ReprocessingWriter();

      /// Sends a CLOSE command to the server to close a file.
      void closeConnection();

      /// Overrides Algorithm::initialize()
      virtual StatusCode initialize();
    
      /// Overrides Algorith::finalize()
      virtual StatusCode finalize();

      /// Overrides Algorithm::execute()
      virtual StatusCode execute();

      /// Sends the command required to open a file and returns the File object.
      File *createAndOpenFile(unsigned int runNumber);

      /// Sends the command required to close a file given the file object.
      void closeFile(File *file);

      /// Writes a chunk to the storage cluster server. Overrides Algorithm::writeBuffer().
      virtual StatusCode writeBuffer(void *const ioDesc, const void *data, size_t len);

      /// IIncidentlistern overload
      virtual void handle(const Incident& incident);

      /// Implemented from INotifyClient.
      virtual void notifyClose(struct cmd_header *cmd);

      /// Implemented from INotifyClient.
      virtual void notifyError(struct cmd_header *cmd, int err_no);

      /// Implemented from INotifyClient.
      virtual void notifyOpen(struct cmd_header *cmd);
    };
}
#endif //  WRITER_REPROCESSINGWRITER_H

