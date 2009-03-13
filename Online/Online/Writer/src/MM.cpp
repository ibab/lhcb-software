#ifdef BUILD_WRITER

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include "Writer/MM.h"

using namespace LHCb;

/**
 * A malloc that does not return until it succeeds.
 * It loops over 1-second sleeps and ensures that an
 * out-of-memory situation does not throw an exception.
 */
static inline void* malloc_blocking(size_t size)
{
  void *ret = NULL;
  while((ret = malloc(size)) == NULL) { sleep(1); }
  return ret;
}

/**
 * Constructor.
 * Initialises the list pointers and the semaphore. This
 * semaphore is used to synchronize access to the list
 * between the sender and the MDFWriter threads.
 */
MM::MM(size_t maxQueueSize)
{
  m_head = NULL;
  m_tail = NULL;
  pthread_mutex_init(&m_listLock, NULL);
  pthread_mutex_init(&m_emptyLock, NULL);
  pthread_mutex_init(&m_allocLock, NULL);
  pthread_cond_init(&m_emptyCondition, NULL);
  m_sendPointer = NULL;
  m_queueLength = 0;
  m_queueSize = 0;
  m_maxQueueSize = maxQueueSize;
  m_allocCmdCount = 0;
  m_allocByteCount = 0;
}

/**
 * Destructor.
 */
MM::~MM()
{
  if(m_head)
    free(m_head);
}

/** Allocates space for a command.
 * @param header A command header, a copy of which must be made.
 * @param data   In case the command is a WRITE_DATA command, then
 * this argument is non-NULL. The length of data is determined from
 * the header, and a copy of this is made, too.
 * @return A pointer to the command header. If data is non-NULL,
 * then the copy of the data contiguously follows the command header.
 */
struct cmd_header* MM::allocAndCopyCommand(struct cmd_header *header, void *data)
{
  if(m_queueSize > m_maxQueueSize) {
      return NULL;
  }
  struct cmd_header *newHeader;
  void *newData;
  int dataSize = 0;

  switch(header->cmd) {
    case CMD_OPEN_FILE:
    case CMD_CLOSE_FILE:
      dataSize = 0;
      break;
    case CMD_WRITE_CHUNK:
      dataSize = header->data.chunk_data.size;
      break;
  }

  newHeader = (struct cmd_header*)malloc_blocking(dataSize + sizeof(struct cmd_header));
  newData = ((unsigned char*)newHeader) + sizeof(struct cmd_header);
  
  m_queueSize += dataSize + sizeof(struct cmd_header);
  
  if(newHeader) {
    pthread_mutex_lock(&m_allocLock);
    MM::m_allocByteCount+=dataSize + sizeof(struct cmd_header);
    MM::m_allocCmdCount++;
    pthread_mutex_unlock(&m_allocLock);
    memcpy(newHeader, header, sizeof(struct cmd_header));
    if(dataSize > 0) {
      memcpy(newData, data, dataSize);
    }
  }

  return newHeader;
}

/** Frees a command header and any associated data.
 * @param cmd A pointer to the command header to be freed. If
 * the command is of type WRITE_CHUNK, then any associated
 * data is also freed. NB: cmd should have been dequeued first.
 */
void MM::freeCommand(struct cmd_header *cmd)
{
  int totLen = 0;
  switch(cmd->cmd) {
    case CMD_WRITE_CHUNK:
      totLen += cmd->data.chunk_data.size;
    case CMD_OPEN_FILE:
    case CMD_CLOSE_FILE:
      totLen += sizeof(struct cmd_header);
      break;
    default:
      return;
      break;
  }
  free(cmd);
  pthread_mutex_lock(&m_allocLock);
  m_allocByteCount -= totLen;
  m_allocCmdCount--;
  pthread_mutex_unlock(&m_allocLock);

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
void MM::enqueueCommand(struct cmd_header *cmd)
{
  if(cmd == NULL) {
      return; // should only be the case when allocAndCopyCommand
              // return NULL because the m_queueSize was too big
  }
  struct list_head *newElem;

  newElem = (struct list_head *)malloc_blocking(sizeof(struct list_head));
  newElem->cmd = cmd;
  newElem->next = NULL;

  pthread_mutex_lock(&m_listLock);
  if(m_head == NULL) {
    m_head = newElem;
    m_tail = newElem;
    m_sendPointer = m_head;
  } else {
    m_tail->next = newElem;
    m_tail = newElem;
    /*In case everything in the queue has already been sent.*/
    if(m_sendPointer == NULL)
      m_sendPointer = newElem;
  }

  m_queueLength++;
  pthread_mutex_unlock(&m_listLock);

  /*Let the sender thread know that the list isn't empty.*/
  pthread_cond_broadcast(&m_emptyCondition);
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
struct cmd_header* MM::moveSendPointer(void)
{
  struct list_head *oldSendPointer;
  int ret;
  /*
   * The queue could possibly even be empty. If this happens, we want
   * this thread to be put to sleep, waiting for something to come in.
   * A semaphore would have been great for this, but we don't
   * use a semaphore here is because semaphores cannot be "upped"
   * beyond 32768 on i386. So if we have more than those many
   * elements in the queue, the whole semaphore thing breaks down.
   * That's why we have this somewhat complicated mix of pthread mutexes
   * and condwaits. The gist is that there's m_listLock for protecting
   * the list, and m_emptyCondition for sleeping till something is inserted
   * in the list. m_emptyLock is just for protecting m_emptyCondition.
   */

  struct timespec tSpec;
  tSpec.tv_nsec = 0;

  while(1) {
    /*Let's try to get the list lock and see if the list is empty.*/
    pthread_mutex_lock(&m_listLock);
    if(m_sendPointer != NULL)
      break;  /* We have the list lock, let's move the pointer forward. */

    /* There's nothing else for us to send, we now sleep and wait. */
    pthread_mutex_unlock(&m_listLock);
    pthread_mutex_lock(&m_emptyLock);
    tSpec.tv_sec = time(NULL) + LOCK_TIMEOUT;

    ret = pthread_cond_timedwait(&m_emptyCondition, &m_emptyLock, &tSpec);
    if(ret != 0) {
      /* Timed out. Nothing in the list yet. Just exit.*/
      pthread_mutex_unlock(&m_emptyLock);
      return NULL;
    }
    pthread_mutex_unlock(&m_emptyLock);

    /*
     * Condition changed, meaning we have something in the list.
     * So just go over and get the listLock again, this time the sendpointer
     * will be fine.
     */
  }

  /* At this point we have the list lock. */
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
void MM::resetSendPointer(void)
{
  pthread_mutex_lock(&m_listLock);
  m_sendPointer = m_head;
  pthread_mutex_unlock(&m_listLock);
}

/**
 * Removes a command from the queue.
 * @param sequenceNum The sequence number of the command to be
 * dequeued.
 */
struct cmd_header* MM::dequeueCommand(unsigned int sequenceNum, unsigned int run_no)
{
  struct list_head *tmp, *prev;
  struct cmd_header *retCmd;

  prev = NULL;

  pthread_mutex_lock(&m_listLock);
  tmp = m_head;
  while(tmp != NULL) {
    if(tmp->cmd->data.chunk_data.seq_num == sequenceNum &&
       tmp->cmd->run_no == run_no) {
      if(!prev) {
        m_head = tmp->next;
        if(!m_head) {
          m_tail = NULL;
          m_sendPointer = NULL;
        }
      } else {
        prev->next = tmp->next;
      }
      m_queueLength--;
      m_queueSize = m_queueSize - tmp->cmd->data.chunk_data.size - sizeof(struct cmd_header);
      pthread_mutex_unlock(&m_listLock);
      retCmd = tmp->cmd;
      free(tmp);
      return retCmd;
    }
    prev = tmp;
    tmp = tmp->next;
  }

  pthread_mutex_unlock(&m_listLock);
  return NULL;
}

#endif /* _WIN32 */
