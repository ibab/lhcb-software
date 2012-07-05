/*
 * MM.h
 *
 * Author:  Sai Suman
 * 			Vijay Kartik
 */

#ifndef MM_H
#define MM_H

#define WITHDEBUG 0
#define LOCK_TIMEOUT 5

extern "C"
{
#include "Writer/chunk_headers.h"
#include <sys/types.h>
#include <malloc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
}

#include <list>
#include <map>
#include <utility>

struct list_head
{
	struct cmd_header *cmd;
	struct list_head *next;
};

/*
 * LHCb namespace;
 */
namespace LHCb
{

/** @class MMQueue
 * Performs custom memory management and queueing of commands.
 * There will be one MMQueue per run.
 */
class MMQueue
{
private:

	/// A lock to protect the list.
	pthread_mutex_t m_listLock;

	/// The list head and tail.
	struct list_head *m_head, *m_tail;

	/// The list send pointer (till where all entries have been sent).
	struct list_head * volatile m_sendPointer;

	/// The number of elements in the queue.
	unsigned int m_queueLength;

	/// The maximum size of the queue
	size_t m_maxQueueSize;

public:

	/// Constructor.
	MMQueue(size_t maxQueueSize);

	/// Destructor.
	~MMQueue();

	/// getter
	inline unsigned int getQueueLength() const { return this->m_queueLength; };

	/// Enqueues a command.
	void enqueueCommand(struct cmd_header *cmd);

	/// Dequeues a command.
	struct cmd_header* dequeueCommand(unsigned int sequenceNum,	unsigned int run_no);

	/// Moves the send pointer ahead, returns old value at send pointer.
	struct cmd_header* moveSendPointer(void);

	/// Resets send pointer to the start of the queue.
	void resetSendPointer(void);

	/// Returns the number of commands enqueued in this object.
	int getQueueLength(void)
	{
		return m_queueLength;
	}
};

/** @class MM
 * Manages the memory structures and the command queues on a per-run basis.
 */
class MM
{
private:

	std::map<unsigned int, MMQueue*> m_runMap;
	std::map<unsigned int, MMQueue*>::iterator m_runMapIter;

	/// A lock to protect the map
	pthread_mutex_t m_mapLock;

	/// For convenience - file to write out debug printfs and such.
	/// Activate with the #define at the top.
#if WITHDEBUG
	FILE *fd;
#endif

	///A lock to protect the allocator.
	pthread_mutex_t m_allocLock;

	/// The number of elements in the queue.
	unsigned int m_queueLength;

	/// The maximum size of the queue
	size_t m_maxQueueSize;

	/// The total number of bytes allocated so far by the MM object.
	size_t m_allocByteCount;

	/// The total number of commands allocated so far by the MM object.
	size_t m_allocCmdCount;

public:

	/// Constructor.
	MM(size_t maxQueueSize);

	/// Destructor.
	~MM();

	/// getter
	inline unsigned int getQueueLength() const { return this->m_queueLength; };

	/// Allocates space for a command and makes a copy of it.
	struct cmd_header* allocAndCopyCommand(struct cmd_header *, void *data);

	/// Frees the data allocated for a command and associated data.
	void freeCommand(struct cmd_header *ch);

	/// Returns the total number of bytes allocated for all commands in all run queues.
	int getAllocByteCount(void)	{ return m_allocByteCount; };

	/// Returns the total number of commands allocated so far in all run queues.
	int getAllocCmdCount(void) { return m_allocCmdCount; };

	/// Enqueues a command in the appropriate run queue.
	void enqueueCommand(struct cmd_header *cmd);

	/// Dequeues a command from the appropriate run queue.
	struct cmd_header* dequeueCommand(unsigned int sequenceNum,	unsigned int run_no);

	/// Iterates over current runs and returns the first unsent command in the immediately available run.
	struct cmd_header* moveSendPointer(void);

	/// Resets send pointer to the start of the list of runs.
	void resetSendPointer(void);

	/// Returns the total number of commands enqueued in all runs.
	int getQueueLength(void) { return m_queueLength; };


	/// Note: These two variable below are static members now
	/// to ensure smooth later use/changes. If it is later decided
	/// to change the command enqueuing/moving procedures, then these
	/// variables should be the only ones needed to be handled and monitored
	/// by different independent routines/threads.


	/// A condition variable to enable the sender thread to sleep on.
	/// Condition = The condition of having an empty MM, i.e., no commands to process
	static pthread_cond_t m_emptyCondition;

	/// A lock to protect the condition variable.
	static pthread_mutex_t m_emptyLock;
};
}

#endif /*MM_H*/
