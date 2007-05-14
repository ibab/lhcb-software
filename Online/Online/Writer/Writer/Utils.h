#ifndef UTILS_H_
#define UTILS_H_

#include "GaudiKernel/MsgStream.h"

namespace LHCb {
	class Utils {

		public:
			static const int DEFAULT_BUF_SIZE = (1 << 16);
			static int connectToAddress(struct sockaddr_in *serverAddr, int sndSize, int rcvSize, MsgStream *log);
			static int nameLookup(std::string &serverAddr, struct sockaddr_in *destAddr, MsgStream *log);
			static int nameLookup(const char *serverAddr, struct sockaddr_in *destAddr, MsgStream *log);
			static int setupSocket(int sndBufSize, int rcvBufSize, MsgStream *log);

			static int closeSocket(int *sock, MsgStream *log);
			static int send(int sock, void *data, size_t length, MsgStream *log);
			static int brecv(int sock, void *data, size_t length, MsgStream *log);


	};
}

#endif /*UTILS_H_*/
