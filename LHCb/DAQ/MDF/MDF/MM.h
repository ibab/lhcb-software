#ifndef MM_H
#define MM_H

extern "C" {
#include "MDF/chunk_headers.h"
#include <sys/types.h>
}

#include <list>

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
      /// A list which is used to to queue commands.
      std::list<struct cmd_header*> m_cmdList;

      /// The maximum pending sequence number in the cached list.
      unsigned int m_maxSeqPending;

      /// The minimum pending sequence number in the cached list.
      unsigned int m_minSeqPending;

    public:

      ////////STATIC MEMBERS AND METHODS///////

      /// The total number of bytes allocated so far by all MM objects.
      static size_t m_allocByteCount;
      /// The total number of commands allocated so far by all MM objects.
      static size_t m_allocCmdCount;


      /// Allocates space for a command and makes a copy of it.
      static struct cmd_header* allocAndCopyCommand(struct cmd_header *, void *data);

      /// Frees the data allocated for a command and associated data.
      static void freeCommand(struct cmd_header *ch);

      /// Returns the total number of bytes allocated for all commands.
      static int getAllocByteCount(void) {return m_allocByteCount;};

      /// Returns the total number of commands allocated so far.
      static int getAllocCmdCount(void) {return m_allocCmdCount;};

      /////////INSTANCE MEMBERS AND METHODS///////////

      /// Basic constructor.
      MM()
      {
	m_maxSeqPending = m_minSeqPending = 0;
      }

      /// Enqueues a command.
      void enqueueCommand(struct cmd_header *cmd);

      /// Dequeues a command and frees it.
      void dequeueCommand(std::list<struct cmd_header*>::iterator );

      /// Returns the first command in the list.
      std::list<struct cmd_header*>::iterator getCommandIterator(void)
      {
	return m_cmdList.begin();
      }

      /// Returns the last command in the list.
      std::list<struct cmd_header*>::iterator getLastCommand(void)
      {
	return m_cmdList.end();
      }

      /// Returns the number of commands enqueued in this object.
      int getQueueLength(void)
      {
	return m_cmdList.size();
      }


  };

}

#endif /*MM_H*/
