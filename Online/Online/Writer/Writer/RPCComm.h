#ifndef RPCCOMM_H
#define RPCCOMM_H

#include <string>

#define RUNDB_SERVICE_FAIL    0
#define RUNDB_SERVICE_SUCCESS  1

/*
 * The LHCb namespace
 */
namespace LHCb {

  class RetryException: public std::runtime_error {
    public:
      /** Takes a character string describing the error.  */
      explicit RetryException(const std::string&  __arg) : std::runtime_error(__arg){}
 
      virtual ~RetryException() throw() {};

  };

  class DiscardException: public std::runtime_error {
    public:
      /** Takes a character string describing the error.  */
      explicit DiscardException(const std::string&  __arg) : std::runtime_error(__arg){}
 
      virtual ~DiscardException() throw() {};

  };

  class FailureException: public std::runtime_error {
    public:
      /** Takes a character string describing the error.  */
      explicit FailureException(const std::string&  __arg) : std::runtime_error(__arg){}
 
      virtual ~FailureException() throw() {};
 
  };

  /**
   * A class for obtaining the host, protocol and path from a formatted URL string.
   */
  class URL
  {
    private:
      char host[256];
      char path[256];
      char protocol[10];
      int port;
    public:
      const char *getHost(void) { return host; }
      int getPort(void) { return port; }
      const char *getPath(void) { return path; }
      const char *getProtocol(void) { return protocol; }
      URL(const char *url);
  };

  /**
   * A wrapper over the "confirm" and "create" Run Database RPC calls.
   */
  class RPCComm
  {
    private:
      /**
       * Host, port, and protocol information in a URL object.
       */
      URL *m_serverURL;

      /**
       * Connects, writes RPC information to an HTTP server, and reads the result.
       * Returns 0 on success, -1 on error.
       */
      int requestResponse(char *requestHeader, char *requestData, char *response, int responseLen);

      /**
       * Checks the XML RPC response string for an error code.
       * Returns 1 in case of an error, and 0 if not.
       */
      int isError(char *response);

    public:
      /**
       * Update statistics about the file.
       */
      void updateFile(char *fileName,  unsigned int *trgEvents, unsigned int *statEvents);

      /**
       * "Confirms" that the file is completely written to, closed,
       * and ready for migration to tape.
       */
      void confirmFile(char *fileName, unsigned int adlerSum, unsigned const char *md5Sum, unsigned long size, unsigned long events, unsigned long physEvents);
//, unsigned int *trgEvents, unsigned int *statEvents);

      /**
       * Creates an entry in the Run Database for the specified file,
       * associated with the specified Run Number and Stream.
       */
      void createFile(char *fileName, unsigned int runNumber);
      
      std::string createNewFile(unsigned int runNumber);

      std::string createNewFile(unsigned int runNumber, std::string streamID, std::string identifier);
      
      /**Simple constructor.*/
      RPCComm(const char *serverURL) { m_serverURL = new URL(serverURL); }

      /**Simple destructor.*/
      ~RPCComm() { delete m_serverURL; }
  };
}

/*
 * This can be done much more elegantly, by having a set
 * of nested tag objects that can serialize themselves into
 * XML strings. However, this is the quick and dirty way
 * out. It's probably alright since we have only two
 * very specific XML-RPC commands to send.
 */
#define OPEN_TEMPLATE  "<?xml version=\"1.0\"?>\n" \
"<methodCall>\n" \
  "  <methodName>openFile</methodName>\n" \
  "  <params>\n"   \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%u</string>\n" \
  "    </value></param>\n" \
  "  </params>\n"  \
  "</methodCall>\n"

#define NEWFILE_TEMPLATE  "<?xml version=\"1.0\"?>\n" \
"<methodCall>\n" \
  "  <methodName>getNewFileName</methodName>\n" \
  "  <params>\n"   \
  "    <param><value>\n" \
  "      <string>%u</string>\n" \
  "    </value></param>\n" \
  "  </params>\n"  \
  "</methodCall>\n"

#define NEWFILE_TEMPLATE_STREAM  "<?xml version=\"1.0\"?>\n" \
"<methodCall>\n" \
  "  <methodName>getNewFileName</methodName>\n" \
  "  <params>\n"   \
  "    <param><value>\n" \
  "      <string>%u</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "  </params>\n"  \
"</methodCall>\n"




#define CONFIRM_TEMPLATE "<?xml version=\"1.0\"?>\n" \
  "<methodCall>\n" \
  "  <methodName>confirmFile</methodName>\n" \
  "  <params>\n"   \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%lu</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%lu</string>\n" \
  "    </value></param>\n" \
  "   <param><value>\n" \
  "      <string>%lu</string>\n" \
  "    </value></param>\n" \
  "  </params>\n"  \
  "</methodCall>\n"

#define UPDATE_TEMPLATE "<?xml version=\"1.0\"?>\n" \
  "<methodCall>\n" \
  "  <methodName>updateFile</methodName>\n" \
  "  <params>\n"   \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "    <param><value>\n" \
  "      <string>%s</string>\n" \
  "    </value></param>\n" \
  "  </params>\n"  \
  "</methodCall>\n"


#define HEADER_TEMPLATE "POST /RPC2 HTTP/1.0\n" \
  "User-Agent: MDFWriterNet\n" \
  "Host: %s\n" \
  "Content-Type: text/xml\n" \
  "Content-Length: %ld\n\n" \


#endif /*RPCCOMM_H*/
