/*
 * MM.cpp
 *
 * Author:  Sai Suman Cherukuwada
 *			Vijay Kartik (vijay.kartik@cern.ch)
 */

#ifdef BUILD_WRITER

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include <limits.h>
#include "Writer/MM.h"
#include "Writer/chunk_headers.h"

using namespace LHCb;

/// Initialise the condition for an empty MM with no commands to send.
/// The condition (plus lock) is used to synchronise access to the MM map
/// between the sender and the MDFWriter threads.
pthread_mutex_t MM::m_emptyLock;
pthread_cond_t MM::m_emptyCondition;

/**
 * A malloc that does not return until it succeeds.
 * It loops over 1-second sleeps and ensures that an
 * out-of-memory situation does not throw an exception.
 */
static inline void* malloc_blocking(size_t size)
{
	void *ret = NULL;
	while ((ret = malloc(size)) == NULL)
	{
		sleep(1);
	}
	return ret;
}

/**
 * Constructor.
 * Initialises the list pointers and mutex.
 * There shall be one MMQueue for every run that the MDFWriterNet processes.
 */
MMQueue::MMQueue(size_t maxQueueSize)
{
	pthread_mutex_init(&m_listLock, NULL);
	m_head = NULL;
	m_tail = NULL;
	m_sendPointer = NULL;
	m_queueLength = 0;
	m_maxQueueSize = maxQueueSize;
}

/**
 * Destructor.
 */
MMQueue::~MMQueue()
{
	if (m_head)
		free(m_head);
}

/**
 * Adds a command to the queue.
 * This command is intended to be queued till it is acknowledged,
 * after which the dequeueCommand() method should be used to
 * dequeue it, and then the freeCommand() method should be used
 * to free it.
 * @param cmd A pointer to the command header of the command to
 * be added to the queue.
 */
void MMQueue::enqueueCommand(struct cmd_header *cmd)
{
	if (cmd == NULL)
	{
		return; // should only be the case when allocAndCopyCommand
				// returns NULL because the queueSize was too big
	}

	struct list_head *newElem;

	newElem = (struct list_head *) malloc_blocking(sizeof(struct list_head));
	newElem->cmd = cmd;
	newElem->next = NULL;

	pthread_mutex_lock(&m_listLock);

	if (m_head == NULL)
	{
		m_head = newElem;
		m_tail = newElem;
		m_sendPointer = m_head;
	}
	else
	{
		m_tail->next = newElem;
		m_tail = newElem;
		/*In case everything in the queue has already been sent.*/
		if (m_sendPointer == NULL)
			m_sendPointer = newElem;
	}

	m_queueLength++;

	pthread_mutex_unlock(&m_listLock);

	/* Let the sender thread know that the list isn't empty. */
	/* using a static member of MM here, so that MM is notified when _any_
	   one of the run-queues has a command to be sent. */
	pthread_cond_broadcast(&(MM::m_emptyCondition));
}

/**
 * Moves the send pointer ahead by one.
 * All the elements enqueued in a queue are messages
 * that must be sent in an order. The messages remain
 * in queue till they are acknowledged. The send pointer
 * is a location that indicates that all messages before
 * it have been sent.
 * @return The object that was _at_ the send pointer
 *         (the object that must now be sent).
 */
struct cmd_header* MMQueue::moveSendPointer(void)
{
	struct list_head *oldSendPointer;

	pthread_mutex_lock(&m_listLock);

	if (m_sendPointer == NULL)
	{
		pthread_mutex_unlock(&m_listLock);
		return NULL;
	}

	oldSendPointer = m_sendPointer;
	m_sendPointer = m_sendPointer->next;

	pthread_mutex_unlock(&m_listLock);
	return oldSendPointer->cmd;
}

/**
 * Moves the send pointer to the first element.
 * This would normally be required only during failover,
 * where all the queue elements would need to be
 * sent again over the network.
 */
void MMQueue::resetSendPointer(void)
{
	pthread_mutex_lock(&m_listLock);
	m_sendPointer = m_head;
	pthread_mutex_unlock(&m_listLock);
}

/**
 * Removes a command from the queue.
 * @param sequenceNum The sequence number of the command to be
 * dequeued.
 * @param run_no The run number of the command to be
 * dequeued.
 */
struct cmd_header* MMQueue::dequeueCommand(unsigned int sequenceNum, unsigned int run_no)
{
	struct list_head *tmp, *prev;
	struct cmd_header *retCmd;

	prev = NULL;

	pthread_mutex_lock(&m_listLock);
	tmp = m_head;
	while (tmp != NULL)
	{
		if (tmp->cmd->data.chunk_data.seq_num == sequenceNum && tmp->cmd->run_no == run_no)
		{
			if (tmp == m_sendPointer)
				m_sendPointer = NULL;
			if (!prev)
			{
				m_head = tmp->next;
				if (!m_head)
				{
					m_tail = NULL;
					m_sendPointer = NULL;
				}
			}
			else
			{
				prev->next = tmp->next;
				if (tmp == m_tail)
					m_tail = prev;
			}
			m_queueLength--;
			retCmd = tmp->cmd;
			free(tmp);

			pthread_mutex_unlock(&m_listLock);
			return retCmd;
		}
		prev = tmp;
		tmp = tmp->next;
	}

	pthread_mutex_unlock(&m_listLock);
	return NULL;
}

/**
 * Constructor.
 * Initialises the map mutex, the 'empty' condition and corresponding mutex.
 * Note that the maxQueueSize is the total number of bytes allowed for
 * the entire map (i.e., including all commands in all run-queues).
 */
MM::MM(size_t maxQueueSize)
{
	pthread_mutex_init(&(MM::m_emptyLock), NULL);
	pthread_cond_init(&(MM::m_emptyCondition), NULL);
	pthread_mutex_init(&m_mapLock, NULL);
	pthread_mutex_init(&m_allocLock, NULL);
	m_maxQueueSize = maxQueueSize;
	m_runMap.clear();
	m_queueLength = 0;
	m_allocByteCount = 0;
	m_allocCmdCount = 0;

#if WITHDEBUG
	fd = fopen("/home/vijay/writerprintfs.log","a");
	fprintf(fd,"Constructor called\n");
	fflush(fd);
#endif
}

/**
 * Destructor.
 */
MM::~MM()
{
	m_runMap.clear();
#if WITHDEBUG
	fclose(fd);
#endif
}

/** Allocates space for a command.
 * @param header A command header, a copy of which must be made.
 * @param data   In case the command is a WRITE_DATA command, then
 * this argument is non-NULL. The length of data is determined from
 * the header, and a copy of this is made, too.
 * @return A pointer to the command header. If data is non-NULL,
 * then the copy of the data contiguously follows the command header.
 */
struct cmd_header* MM::allocAndCopyCommand(struct cmd_header *header,
		void *data)
{
	if (MM::m_allocByteCount > m_maxQueueSize)
	{
		return NULL;
	}
	struct cmd_header *newHeader;
	void *newData;
	int dataSize = 0;

	switch (header->cmd)
	{
	case CMD_OPEN_FILE:
		dataSize = 0;
		break;
	case CMD_CLOSE_FILE:
		dataSize = sizeof(struct cmd_stop_pdu);
		break;
	case CMD_WRITE_CHUNK:
		dataSize = header->data.chunk_data.size;
		break;
	}

	newHeader = (struct cmd_header*) malloc_blocking(dataSize
			+ sizeof(struct cmd_header));
	newData = ((unsigned char*) newHeader) + sizeof(struct cmd_header);

	if (newHeader)
	{
		pthread_mutex_lock(&m_allocLock);
		MM::m_allocByteCount += dataSize + sizeof(struct cmd_header);
		MM::m_allocCmdCount++;
		pthread_mutex_unlock(&m_allocLock);
		memcpy(newHeader, header, sizeof(struct cmd_header));
		if (dataSize > 0)
		{
			memcpy(newData, data, dataSize);
		}
	}
	return newHeader;
}

/** Frees a command header and any associated data.
 * @param cmd A pointer to the command header to be freed. If
 * the command is of type WRITE_CHUNK, then any associated
 * data is also freed. NB: Command should have been dequeued first.
 */
void MM::freeCommand(struct cmd_header *cmd)
{
	int totLen = 0;
	switch (cmd->cmd)
	{
	case CMD_WRITE_CHUNK:
		totLen += cmd->data.chunk_data.size;
		totLen += sizeof(struct cmd_header);
		break;
	case CMD_CLOSE_FILE:
		totLen += sizeof(struct cmd_stop_pdu);
	case CMD_OPEN_FILE:
		totLen += sizeof(struct cmd_header);
		break;
	default:
		return;
		break;
	}
	//  pthread_mutex_unlock(&m_listLock);

	free(cmd);
	pthread_mutex_lock(&m_allocLock);

	m_allocByteCount -= totLen;
	m_allocCmdCount--;
	pthread_mutex_unlock(&m_allocLock);

}

/**
 * Adds a command to the queue for the appropriate run that the command is for.
 * @param cmd A pointer to the command header of the command to
 * be added to the queue.
 */
void MM::enqueueCommand(struct cmd_header *cmd)
{
	if (cmd == NULL)
		return;

	unsigned int runNum = cmd->run_no;

	pthread_mutex_lock(&m_mapLock);

	if (m_runMap.find(runNum) == m_runMap.end())
	{
		if (((m_runMap[runNum]) = new MMQueue(m_maxQueueSize)) == NULL)
		{
			pthread_mutex_unlock(&m_mapLock);
			return;
		}
	}
	(m_runMap[runNum])->enqueueCommand(cmd);
	if (m_runMap.size() == 1)
		m_runMapIter = m_runMap.begin();
	++m_queueLength;

	pthread_mutex_unlock(&m_mapLock);
}

struct cmd_header* MM::moveSendPointer(void)
{
	/*
	 * The entire map of runs could be empty, or the map could have entries but
	 * any number of run queues could also be empty. If ALL run queues are empty,
	 * we want this thread to be put to sleep, waiting for something to come in.
	 * This is done with a conditional wait, which waits for LOCK_TIMEOUT seconds.
	 * We don't care about the return value of  pthread_cond_timedwait() because the
	 * wait is just to avoid endless repetitive checking to see if a new command has
	 * been enqueued. pthread_cond_timedwait() would return immediately if m_emptyCondition
	 * is broadcast (by MMQueue::enqueueCommand()), and moveSendPointer() would continue.
	 *
	 * We iterate over the entire run map until we find a non-empty queue
	 * corresponding to a run that has a command to send. The moveSendPointer()
	 * of this non-empty queue is then called.
	 */

	std::map<unsigned int, MMQueue*>::iterator old_runMapIter;
	struct cmd_header* retCmd;
	struct timespec tSpec;
	tSpec.tv_nsec = 0;
	tSpec.tv_sec = time(NULL) + LOCK_TIMEOUT;

	while(1)
	{
		pthread_mutex_lock(&m_mapLock);

		if(m_runMap.size() != 0)
		{
			if (m_runMap.size() == 1)
				m_runMapIter = m_runMap.begin();
			old_runMapIter = m_runMapIter;
			do
			{
				retCmd = ((*m_runMapIter).second)->moveSendPointer();
				++m_runMapIter;
				if (m_runMapIter == m_runMap.end())
					m_runMapIter = m_runMap.begin();
			} while ((retCmd == NULL) && (m_runMapIter != old_runMapIter));

			if (retCmd != NULL)
			{
				pthread_mutex_unlock(&m_mapLock);
				return retCmd;
			}
		}
		pthread_mutex_unlock(&m_mapLock);
		pthread_mutex_lock(&(MM::m_emptyLock));

		if(pthread_cond_timedwait(&(MM::m_emptyCondition), &(MM::m_emptyLock), &tSpec) != 0)
		{
			// Timed out and no CMDs have been added to any queue. Exit
			pthread_mutex_unlock(&(MM::m_emptyLock));
			return NULL;
		}
		pthread_mutex_unlock(&(MM::m_emptyLock));
	}
}

/**
 * Moves the send pointer of each run queue in the MM map to the first element.
 * This would normally be required only during failover,
 * where all the queue elements would need to be
 * sent again over the network.
 */
void MM::resetSendPointer(void)
{
	pthread_mutex_lock(&m_mapLock);

	for (m_runMapIter = m_runMap.begin(); m_runMapIter != m_runMap.end(); ++m_runMapIter)
		((*m_runMapIter).second)->resetSendPointer();
	m_runMapIter = m_runMap.begin();

	pthread_mutex_unlock(&m_mapLock);
}

/**
 * Removes a command from the appropriate run queue in the MM map.
 * @param sequenceNum The sequence number of the command to be dequeued.
 * @param run_no The run number of the command to be dequeued.
 */
struct cmd_header* MM::dequeueCommand(unsigned int sequenceNum,	unsigned int run_no)
{
	struct cmd_header* retCmd;

	pthread_mutex_lock(&m_mapLock);

	if ((retCmd = m_runMap[run_no]->dequeueCommand(sequenceNum, run_no)) == NULL)
		{
		pthread_mutex_unlock(&m_mapLock);
		return NULL;
		}
	if (--m_queueLength == INT_MAX)
		m_queueLength = 0;
	pthread_mutex_unlock(&m_mapLock);
	return retCmd;
}
#endif /* _WIN32 */
