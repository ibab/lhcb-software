#ifndef MM_H
#define MM_H

extern "C" {
#include "Writer/chunk_headers.h"
#include <sys/types.h>
#include <malloc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
}

#include <list>

#define LIST_SEM_KEY 0x61243
struct list_head {
  struct cmd_header *cmd;
  struct list_head *next;
};

/*
 * LHCb namespace;
 */
namespace LHCb {


  /** @class MM
   * Performs custom memory management and queueing of commands.
   */
  class MM
  {
    private:
      /// A lock to protect the list.
      pthread_mutex_t m_listLock;

      /// The list head and tail.
      struct list_head *m_head, *m_tail;

      /// The list send pointer (till where all entries have been sent).
      struct list_head *m_sendPointer;

      /// The number of elements in the queue.
      unsigned int m_queueLength;

      /// A semaphore to synchronize the enqueueing and the moving of the
      /// Send pointer.
      int m_queueSem;

      /// The total number of bytes allocated so far by all MM objects.
      static size_t m_allocByteCount;
      /// The total number of commands allocated so far by all MM objects.
      static size_t m_allocCmdCount;

    public:
    	/// Constructor.
    	MM();

    	/// Destructor.
    	~MM();

      /// Allocates space for a command and makes a copy of it.
      static struct cmd_header* allocAndCopyCommand(struct cmd_header *, void *data);

      /// Frees the data allocated for a command and associated data.
      static void freeCommand(struct cmd_header *ch);

      /// Returns the total number of bytes allocated for all commands.
      static int getAllocByteCount(void) {return m_allocByteCount;};

      /// Returns the total number of commands allocated so far.
      static int getAllocCmdCount(void) {return m_allocCmdCount;};

      /// Enqueues a command.
      void enqueueCommand(struct cmd_header *cmd);

      /// Dequeues a command.
      struct cmd_header* dequeueCommand(unsigned int sequenceNum);

      /// Moves the send pointer ahead, returns old value at send pointer.
      struct cmd_header* moveSendPointer(void);

      /// Resets send pointer to the start of the list.
      void resetSendPointer(void);

      /// Returns the number of commands enqueued in this object.
      int getQueueLength(void)
      {
        return m_queueLength;
      }
  };
}

#endif /*MM_H*/
