#ifndef UTILS_H_
#define UTILS_H_

#include "GaudiKernel/MsgStream.h"
#define WHERE "File: " << __FILE__ << "[" << __LINE__ << "] " << __FUNCTION__ << "() :"
#define IP(x)  (x & 0xff) << "." << ((x & 0xff00) >> 8) << "." << ((x & 0xff0000) >> 16) << "." << ((x & 0xff000000) >> 24)

namespace LHCb {

  /**
   * A buffer in flight, in the process of being sent or received.
   */
  class BIF {
    private:
      char *m_data;
      int m_bytesRead;
      int m_bufLen;
      int m_sockFd;
    public:
      static const int TIMEDOUT = -2;
      static const int THREAD_STOPPED = -3;
      static const int AGAIN = 0;
      static const int DISCONNECTED = -1;
      static const int FAILURE_TIMEOUT_SEC = 10; //XXX set as param?
      static const int POLL_TIMEOUT_MSEC = 1500;
      BIF(int sockFd, void *data, int bufLen)
      {
        m_sockFd = sockFd;
        m_data = (char*)data;
        m_bufLen = bufLen;
        m_bytesRead = 0;
      }
      int getBytesRead() { return m_bytesRead; }
      int nbRecv(MsgStream *log);
      int nbRecvTimeout();
      int nbSend(MsgStream *log);
      int nbSendTimeout();
  };

  class Utils {

    public:
      static const int DEFAULT_BUF_SIZE = (1 << 16);
      static int connectToAddress(struct sockaddr_in *serverAddr, int sndSize, int rcvSize, MsgStream *log);
      static int nameLookup(std::string &serverAddr, struct sockaddr_in *destAddr, MsgStream *log);
      static int nameLookup(const char *serverAddr, struct sockaddr_in *destAddr, MsgStream *log);
      static int setupSocket(int sndBufSize, int rcvBufSize, MsgStream *log);
      static int closeSocket(int *sock, MsgStream *log);
      static void blockSignals(void);
  };
}

#endif /*UTILS_H_*/
