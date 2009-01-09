#ifndef MM_H
#define MM_H

#define LOCK_TIMEOUT 5

extern "C" {
#include "Writer/chunk_headers.h"
#include <sys/types.h>
#include <malloc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
}

#include <list>

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

      /// A lock to protect the condition variable.
      pthread_mutex_t m_emptyLock;

      /// The list head and tail.
      struct list_head *m_head, *m_tail;

      /// The list send pointer (till where all entries have been sent).
      struct list_head * volatile m_sendPointer;

      /// The number of elements in the queue.
      unsigned int m_queueLength;

      /// A condition variable to enable the sender thread to sleep on.
      pthread_cond_t m_emptyCondition;

      /// The total number of bytes allocated so far by the MM object.
      size_t m_allocByteCount;
      /// The total number of commands allocated so far by the MM object.
      size_t m_allocCmdCount;

      ///A lock to protect the allocator.
      pthread_mutex_t m_allocLock;

    public:
      /// Constructor.
      MM();

      /// Destructor.
      ~MM();
      
      /// getter
      inline unsigned int getQueueLength() const { return this->m_queueLength; };

      /// Allocates space for a command and makes a copy of it.
      struct cmd_header* allocAndCopyCommand(struct cmd_header *, void *data);

      /// Frees the data allocated for a command and associated data.
      void freeCommand(struct cmd_header *ch);

      /// Returns the total number of bytes allocated for all commands.
      int getAllocByteCount(void) {return m_allocByteCount;};

      /// Returns the total number of commands allocated so far.
      int getAllocCmdCount(void) {return m_allocCmdCount;};

      /// Enqueues a command.
      void enqueueCommand(struct cmd_header *cmd);

      /// Dequeues a command.
      struct cmd_header* dequeueCommand(unsigned int sequenceNum, unsigned int run_no);

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
