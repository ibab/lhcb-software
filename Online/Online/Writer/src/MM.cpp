#ifndef _WIN32

extern "C" {
#include <malloc.h>
#include <string.h>
#include <time.h>
}

#include <iostream>
#include <stdexcept>

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

/*
 * To enable C++ compiler to know which compilation object
 * to place these variables in.
 */
size_t MM::m_allocByteCount = 0;
size_t MM::m_allocCmdCount = 0;

/**
 * Constructor.
 * Initialises the list pointers and the semaphore. This
 * semaphore is used to synchronize access to the list
 * between the sender and the MDFWriter threads.
 */
MM::MM()
{
  m_head = NULL;
  m_tail = NULL;
  pthread_mutex_init(&m_listLock, NULL);
  m_sendPointer = NULL;

  m_queueSem = semget(LIST_SEM_KEY, 1, IPC_CREAT | 0666);
  if(m_queueSem < 0) {
  		throw std::runtime_error("Could not create semaphore.");
  }

  //Let's init the semaphore to zero.
  union semun {
  	int val;
    struct semid_ds *buf;
    unsigned short  *array;
	} arg;

	arg.val = 0;
	if(semctl(m_queueSem, 0, SETVAL, arg) != 0) {
		throw std::runtime_error("Could not init semaphore to zero.");
	}
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
  if(newHeader) {
    MM::m_allocByteCount+=dataSize + sizeof(struct cmd_header);
    MM::m_allocCmdCount++;
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

  m_allocByteCount -= totLen;
  m_allocCmdCount--;
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
    //In case everything in the queue has already been sent.
    if(m_sendPointer == NULL)
    	m_sendPointer = newElem;
  }

  pthread_mutex_unlock(&m_listLock);
  m_queueLength++;

  /*
   * Every time a new element it added to the queue, we up
   * the semaphore (the sender thread will down it).
   */
  struct sembuf sbuf[1];
  sbuf[0].sem_num = 0;
  sbuf[0].sem_op = 1;
  sbuf[0].sem_flg = 0;

  while(semop(m_queueSem, sbuf, 1) != 0) {
    sleep(1);
  }

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
   * The queue could possibly even be empty. That's what
   * the semaphore is for. If you can't down it, just wait.
   */
  struct sembuf sbuf[1];
  sbuf[0].sem_num = 0;
  sbuf[0].sem_op = -1;
  sbuf[0].sem_flg = 0;

  struct timespec tspec;
  tspec.tv_sec = 5;
  tspec.tv_nsec = 0;

  ret = semtimedop(m_queueSem, sbuf, 1, &tspec);
  if(ret != 0 && errno != EINTR && errno != EAGAIN) {
    throw std::runtime_error("FATAL:Could not down the semaphore to move send ptr.");
  } else if(ret != 0) {
    return NULL;
  }

  pthread_mutex_lock(&m_listLock);
  if(m_sendPointer == NULL) {
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
struct cmd_header* MM::dequeueCommand(unsigned int sequenceNum)
{
  struct list_head *tmp, *prev;
  struct cmd_header *retCmd;

  prev = NULL;

  pthread_mutex_lock(&m_listLock);
  tmp = m_head;
  while(tmp != NULL) {
    if(tmp->cmd->data.chunk_data.seq_num == sequenceNum) {
      if(!prev) {
        m_head = tmp->next;
        if(!m_head) {
          m_tail = NULL;
          m_sendPointer = NULL;
        }
      } else {
        prev->next = tmp->next;
      }
      pthread_mutex_unlock(&m_listLock);
      retCmd = tmp->cmd;
      free(tmp);
      m_queueLength--;
      return retCmd;
    }
    prev = tmp;
    tmp = tmp->next;
  }

  pthread_mutex_unlock(&m_listLock);
  m_queueLength--;
  return NULL;
}

#endif /* _WIN32 */
