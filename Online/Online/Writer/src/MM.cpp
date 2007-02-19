extern "C" {
#include <malloc.h>
#include <string.h>
}

#include <iostream>
#include <stdexcept>

#include "Writer/MM.h"


using namespace LHCb;

/*
 * To enable C++ compiler to know which compilation object
 * to place these variables in.
 */
size_t MM::m_allocByteCount = 0;
size_t MM::m_allocCmdCount = 0;

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
  //TODO: Replace malloc() with more efficient impl if needed.
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

  newHeader = (struct cmd_header*)malloc(dataSize + sizeof(struct cmd_header));
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

  newElem = (struct list_head *)malloc(sizeof(struct list_head));
  if(!newElem) {
    throw std::runtime_error("Could not malloc for new command.");
  }
  newElem->cmd = cmd;
  newElem->next = NULL;

  pthread_mutex_lock(&m_listLock);
  if(m_head == NULL) {
    m_head = newElem;
    m_tail = newElem;
  } else {
    m_tail->next = newElem;
    m_tail = newElem;
  }
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
	}
      } else {
        prev->next = tmp->next;
      }
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

