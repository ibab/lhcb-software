#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFIO.h"
#include "Connection.h"

#ifndef MDFWRITERNET_H
#define MDFWRITERNET_H


/*
 * LHCb namespace
 */

namespace LHCb {

  /** @class MDFWriterNet
    *
    * @author: Sai Suman Cherukuwada
    * @version: 1.0
    */

  class MDFWriterNet : public MDFWriter {

    protected:

      /// The initial storage server hostname to connect to.

      std::string m_serverAddr;

      /// The port of the storage server to connect to.
      int m_serverPort;

      /// The RPC URL of the Run Database service.
      std::string m_runDBURL;

      /// A prefix to a apply to all file names that are created.
      std::string m_filePrefix;

      /// The SO_TIMEOUT value to apply to the socket.
      int m_soTimeout;

      /// The maximum file size (in MB) to write before creating a new file.
      size_t m_maxFileSizeMB;

      /// The number of bytes written so far in the current file.
      size_t m_bytesWritten;

      /// The socket send/receive buffer sizes.
      int m_sndRcvSizes;

      /// The current file being written to.
      std::string m_fileName;

      /// Is a file open currently?
      bool m_fileOpen;

      /// The Adler32 checksum of the file.
      unsigned int m_adler32;

      /// The MD5 checksum of the file.
      TMD5 *m_md5;

      /// The connection object being used to talk to the server.
      Connection *m_connection;

      /// The message stream to log to.
      MsgStream *m_log;

      /// Returns a new file named based on the supplied MDF data.
      void getNewFileName(std::string &newFileName, const void *data, size_t len);

      /// Similar to MDFWriter::construct()
      virtual void constructNet(void);
      
    public:
      /// Destructor.
      virtual ~MDFWriterNet();

      /// Sends a CLOSE command to the server to close a file.
      void closeConnection();

      /// Overrides Algorithm::initialize()
      virtual StatusCode initialize();

      /// Overrides Algorithm::execute()
      virtual StatusCode execute();

      /// Extended Algorithm constructor (delegates to MDFWriter constructor).
      MDFWriterNet(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc);

      /// Standard Algorithm constructor (delegates to MDFWriter constructor).
      MDFWriterNet(const std::string& nam, ISvcLocator* pSvc);

      /// Writes a chunk to the storage cluster server. Overrides Algorithm::writeBuffer().
      virtual StatusCode writeBuffer(void *const ioDesc, const void *data, size_t len);
  };
}

#endif //MDFWRITERNET_H

