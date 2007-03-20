#ifndef SENDTHREAD_H_
#define SENDTHREAD_H_

#include "Writer/Connection.h"
#include "Writer/MM.h"
#include "GaudiKernel/MsgStream.h"

#include <pthread.h>

namespace LHCb {

	class Connection;
	/**
	 * Takes care of purging the queue and sending chunks to the server.
	 */
	class SendThread {

		private:

			Connection *m_conn;
			MsgStream *m_log;
			MM *m_mmObj;
			int m_stopSending;
			int m_sockfd;
			pthread_t m_sendThread;

		public:
			SendThread(Connection *conn, int sockfd, MM *mmObj, MsgStream *log) {
				m_conn = conn;
				m_log = log;
				m_mmObj = mmObj;
				m_sockfd = sockfd;
			}
			int getState(void) { return m_stopSending; }
			void restoreState(int state) { m_stopSending = state; }
			void start(void);
			void stop(int stopLevel);
			void reInit(void);
			int processSends(void);
	};


}

#endif /*SENDTHREAD_H_*/
