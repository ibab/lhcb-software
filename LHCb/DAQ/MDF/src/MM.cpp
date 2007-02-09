extern "C" {
#include <malloc.h>
#include <string.h>
}

#include <list>
#include <iostream>

#include "MDF/MM.h"


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
  if(header) {
    MM::m_allocByteCount+=dataSize + sizeof(struct cmd_header);
    MM::m_allocCmdCount++;
    memcpy(newHeader, header, sizeof(struct cmd_header));
    if((unsigned)dataSize > sizeof(struct cmd_header))
      memcpy(newData, data, header->data.chunk_data.size);
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
  m_maxSeqPending = cmd->data.chunk_data.seq_num;
  if(m_cmdList.size() == 0) {
    m_minSeqPending = m_maxSeqPending;
  }
  m_cmdList.push_back(cmd);
}

/**
  * Removes a command from the queue.
  * @param it An iterator which is  currently at the position
  * where the command to be dequeued is located.
  */
void MM::dequeueCommand(std::list<struct cmd_header*>::iterator it)
{
  m_cmdList.erase(it);
  if(m_cmdList.size() > 0) {
    m_minSeqPending = (*m_cmdList.begin())->data.start_data.seq_num;
  } else {
    m_minSeqPending = 0;
    m_maxSeqPending = 0;
  }
}

