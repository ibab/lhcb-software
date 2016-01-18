//#ifdef BUILD_WRITER

#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Writer/RPCComm.h"
#include "Writer/Utils.h"
#include "Writer/chunk_headers.h"

#include <errno.h>
#include <string.h>

#define RESPONSELEN 1024

using namespace LHCb;

/**
 * "Confirms" that the file is completely written to and closed.
 * This creates a record in the run database specifying that the
 * file may now be migrated to tape.
 * @param fileName The name of the file which has been closed.
 * @param adlerSum The Adler32 checksum of the entire file.
 * @param md5CSum   The MD5 checksum of the entire file.
 */
void RPCComm::confirmFile(char *fileName, //still
    unsigned int adlerSum, //still
    const unsigned char *md5CSum, //fsdlm
    unsigned long size, //still
    unsigned long events, //still
    unsigned long physEvents) //still
{
  int ret;
  char headerData[1024];
  char xmlData[4096];
  char response[RESPONSELEN];

  char adler32String[9];
  char md5CharString[33];

  bzero(md5CharString, 33);
  bzero(adler32String, 9);

  /* We need to send this as a string because it's not very clear how the
  * XMLRPC library handles unsigned values.
  */
  sprintf(md5CharString,
      "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
      md5CSum[0], md5CSum[1], md5CSum[2], md5CSum[3], md5CSum[4], md5CSum[5],
      md5CSum[6], md5CSum[7], md5CSum[8], md5CSum[9], md5CSum[10], md5CSum[11],
      md5CSum[12], md5CSum[13], md5CSum[14], md5CSum[15]);

  sprintf(adler32String, "%08X", adlerSum);

  /* Now we fill up templates. */
  ret = snprintf(xmlData, sizeof(xmlData), CONFIRM_TEMPLATE, fileName,
      adler32String, md5CharString, size, events, physEvents);
  if (ret < 0 || (unsigned int) ret > sizeof(xmlData))
  {
    throw FailureException("Could not format rpc call correctly.");
  }

  snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE, "WriterHost",
      strlen(xmlData));

  memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response) - 1);

  if (ret < 0)
    throw RetryException("Could not run RPC call for confirm.");

  ret = isError(response);
  if (ret == 2)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw DiscardException(response);
  }
  if (ret != 0)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw RetryException(response);
  }

  return;
}

void RPCComm::updateFile(char *fileName, unsigned int *trgEvents,
    unsigned int *statEvents)
{
  int ret;
  char headerData[1024];
  char response[RESPONSELEN];

  size_t msg_size =
      snprintf(NULL, 0,
          "TRG0:%d;TRG1:%d;TRG2:%d;TRG3:%d;TRG4:%d;TRG5:%d;TRG6:%d;TRG7:%d;PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
          trgEvents[0], trgEvents[1], trgEvents[2], trgEvents[3], trgEvents[4],
          trgEvents[5], trgEvents[6], trgEvents[7], statEvents[PHYSIN],
          statEvents[MBIASIN], statEvents[LUMIIN], statEvents[BEAMGASIN],
          statEvents[RANDEX], statEvents[PHYSEX], statEvents[MBIASEX],
          statEvents[LUMIEX], statEvents[BEAMGASEX], statEvents[RANDEX],
          statEvents[LOWLUMI], statEvents[MIDLUMI], statEvents[HLT1IN],
          statEvents[HLT1EX]) + 1;

  char * msg = (char *) malloc(msg_size);

  ret =
      snprintf(msg, msg_size,
          "TRG0:%d;TRG1:%d;TRG2:%d;TRG3:%d;TRG4:%d;TRG5:%d;TRG6:%d;TRG7:%d;PHYSIN:%d;MBIASIN:%d;LUMIIN:%d;BEAMGASIN:%d;RANDIN:%d;PHYSEX:%d;MBIASEX:%d;LUMIEX:%d;BEAMGASEX:%d;RANDEX:%d;LOWLUMI:%d;MIDLUMI:%d;HLT1IN:%d;HLT1EX:%d",
          trgEvents[0], trgEvents[1], trgEvents[2], trgEvents[3], trgEvents[4],
          trgEvents[5], trgEvents[6], trgEvents[7], statEvents[PHYSIN],
          statEvents[MBIASIN], statEvents[LUMIIN], statEvents[BEAMGASIN],
          statEvents[RANDEX], statEvents[PHYSEX], statEvents[MBIASEX],
          statEvents[LUMIEX], statEvents[BEAMGASEX], statEvents[RANDEX],
          statEvents[LOWLUMI], statEvents[MIDLUMI], statEvents[HLT1IN],
          statEvents[HLT1EX]);
  if (ret < 0 || (unsigned int) ret >= msg_size)
  {
    throw FailureException("Could not format stat counters correctly.");
  }
  /* XXX Tricky here, send RANDEX value for RANDIN, as RANDIN should be greater or equal to RANDEX and nothing is counted in it */

  //XXX check if usual trick with 2 snprintf not better here
  /* Now we fill up templates. */

  size_t xml_size = ::snprintf(NULL, 0, UPDATE_TEMPLATE, fileName, msg) + 1;

  char *xmlData = (char *) ::malloc(xml_size);

  ret = ::snprintf(xmlData, xml_size, UPDATE_TEMPLATE, fileName, msg);
  if (ret < 0 || (unsigned int) ret >= xml_size)
  {
    throw FailureException("Could not format rpc call correctly.");
  }

  ::snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE, "WriterHost",
      (long) ::strlen(xmlData));

  ::memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response) - 1);
  ::free(xmlData);
  ::free(msg);
  xmlData = NULL;
  msg = NULL;

  if (ret < 0)
    throw RetryException("Could not run RPC call for confirm.");

  ret = isError(response);
  if (ret == 2)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw DiscardException(response);
  }
  if (ret != 0)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw RetryException(response);
  }

  return;
}

/**
 * Creates an entry for the file in the Run Database.
 * This call is used to create a record in the run database, indicating
 * that a file of the specified name, and belonging to the specified
 * run number has been created (but not necessarily written fully to).
 * @param fileName The name of the file that has been created.
 * @param runNumber The run number to which the file belongs.
 */
void RPCComm::createFile(char *fileName, unsigned int runNumber)
{
  int ret;

  char headerData[1024];
  char xmlData[1024];
  char response[RESPONSELEN];

  snprintf(xmlData, sizeof(xmlData), OPEN_TEMPLATE, fileName, runNumber);
  snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE, "WriterHost",
      (long) strlen(xmlData));

  memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response) - 1);

  if (ret < 0)
    throw RetryException("Could not run RPC call for create.");

  ret = isError(response);
  if (ret == 2)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw DiscardException(response);
  }
  if (ret != 0)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw RetryException(response);
  }

  return;
}

/**
 * Connects, writes RPC information to an HTTP server, and reads the result.
 * Returns 0 on success, -1 on error.
 */
int RPCComm::requestResponse(char *requestHeader, char *requestData,
    char *response, int responseLen)
{
  struct sockaddr_in destAddr;
  int sockFd;
  int ret;

  if (Utils::nameLookup(m_serverURL->getHost(), &destAddr, NULL) != 0)
    throw FailureException("Could not resolve name.");
  destAddr.sin_port = htons(m_serverURL->getPort());
  destAddr.sin_family = AF_INET;

  sockFd = Utils::connectToAddress(&destAddr, Utils::DEFAULT_BUF_SIZE,
      Utils::DEFAULT_BUF_SIZE, NULL);

  BIF sendBif1(sockFd, requestHeader, strlen(requestHeader));
  BIF sendBif2(sockFd, requestData, strlen(requestData));
  BIF recvBif(sockFd, response, responseLen);

  if (sockFd < 0)
    throw RetryException("Could not connect to RPC server.");
  ret = sendBif1.nbSendTimeout();
  if (ret == BIF::TIMEDOUT || ret == BIF::DISCONNECTED)
  {
    int my_errno = errno;
    close(sockFd);
    char msg[512];
    snprintf(msg, 512, "Could not send request header: errno = %d, %s",
        my_errno, strerror(my_errno));
    throw RetryException(msg);
  }
  ret = sendBif2.nbSendTimeout();
  if (ret == BIF::TIMEDOUT || ret == BIF::DISCONNECTED)
  {
    int my_errno = errno;
    close(sockFd);
    char msg[512];
    snprintf(msg, 512, "Could not send request data: errno = %d, %s", my_errno,
        strerror(my_errno));
    throw RetryException(msg);
  }
  ret = recvBif.nbRecvTimeout();
  if (recvBif.getBytesRead() <= 0)
  {
    int my_errno = errno;
    close(sockFd);
    char msg[512];
    snprintf(msg, 512, "Could not read request data: errno = %d, %s", my_errno,
        strerror(my_errno));
    throw RetryException(msg);
  }

  close(sockFd);
  return 0;
}

/**
 * Checks the XML RPC response string for an error code.
 * @Return 0 if no error
 * @Return 1 in case of an unknown/unmanaged error
 * @Return 2 in case of run closed or worse
 */
int RPCComm::isError(char *response)
{
  std::string responseStr(response);
  char failedRPC[] =
      "<?xml version='1.0'?>\n<methodResponse>\n<params>\n<param>\n<value><array><data>\n<value><int>0</int></value>\n<value>";
  char discardRPC[] =
      "<?xml version='1.0'?>\n<methodResponse>\n<params>\n<param>\n<value><array><data>\n<value><int>-42</int></value>\n<value>";
  size_t len;
  len = responseStr.length();

  /* First check for HTTP response status */
  if (responseStr.find("200 OK") > len)
    return 1;

  /* Check for managed errors */
  if (responseStr.find(discardRPC) < len)
    return 2;

  /* Check for the return code from the function */
  if (responseStr.find(failedRPC) < len)
    return 1;
  /* Then check for a fault string */
  if (responseStr.find("faultCode") > len)
    return 0;

  /* By default, cause an unmanage but retryable error and wait for action from the piquet/export. */
  return 1;
  /*Dump the whole response if fault found.*/
}

std::string RPCComm::createNewFile(unsigned int runNumber)
{
  char headerData[1024], xmlData[1024], response[RESPONSELEN];
  int ret;
  std::string file;
  char startStr[] = "<string>";
  char endStr[] = "</string>";
  char failStr[] = "<value><int>0</int></value>"; // RunDB error code is 0 (don't ask why ...)
  size_t start, end, err;

  snprintf(xmlData, sizeof(xmlData), NEWFILE_TEMPLATE, runNumber);
  snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE, "WriterHost",
      (size_t) strlen(xmlData));

  memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response) - 1);

  if (ret < 0)
    throw RetryException("Could not run RPC call for create");

  ret = isError(response);
  if (ret == 2)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw DiscardException(response);
  }
  if (ret != 0)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw RetryException(response);
  }

  std::string res(response);

  //XXX Should be spotted beforehand in isError
  err = res.find(failStr);
  if (err != res.npos)
  {
    throw RetryException("RunDB answer is error");
  }

  start = res.find(startStr);
  if (start == res.npos)
    throw RetryException("Could not find filename in the RunDB answer");
  start += sizeof(startStr) - 1;

  end = res.find(endStr);
  if (end == res.npos)
    throw RetryException("Could not find filename in the RunDB answer");

  file = std::string(response, start, end - start);

  return file;
}

std::string RPCComm::createNewFile(unsigned int runNumber, std::string streamID,
    std::string identifier)
{
  char headerData[1024], xmlData[1024], response[RESPONSELEN];
  int ret;
  std::string file;
  char startStr[] = "<string>";
  char endStr[] = "</string>";
  char failStr[] = "<value><int>0</int></value>";
  size_t start, end, err;

  ::snprintf(xmlData, sizeof(xmlData), NEWFILE_TEMPLATE_STREAM, runNumber,
      streamID.c_str(), identifier.c_str());
  ::snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE, "WriterHost",
      (long) ::strlen(xmlData));

  ::memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response) - 1);

  if (ret < 0)
    throw RetryException("Could not run RPC call for create");
  ret = isError(response);
  if (ret == 2)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw DiscardException(response);
  }
  if (ret != 0)
  {
    // Test if MsgStream segfault comes again. And corrects the fact that after \n, message level is INFO instead of ERROR !
    for (unsigned int i = 0; i < strlen(response); ++i)
    {
      if (response[i] == '\n')
        response[i] = ' ';
    }
    throw RetryException(response);
  }

  std::string res(response);

  //XXX Should be spotted beforehand in isError
  err = res.find(failStr);
  if (err != res.npos)
  {
    throw RetryException("RunDB answer is error");
  }

  start = res.find(startStr);
  if (start == res.npos)
    throw RetryException("Could not find filename in the RunDB answer" + res);
  start = start + sizeof(startStr) - 1;

  end = res.find(endStr);
  if (end == res.npos)
    throw RetryException("Could not find filename in the RunDB answer");

  file = std::string(response, start, end - start);

  return file;
}

URL::URL(const char *url)
{
  char *curr, *currw;
  char portStr[7];

  memset(protocol, 0, sizeof(protocol));
  memset(host, 0, sizeof(host));
  memset(path, 0, sizeof(path));
  memset(portStr, 0, sizeof(portStr));
  port = 80;

  curr = (char*) url;
  currw = protocol;

  while (*curr != 0 && *curr != ':')
  {
    *currw = *curr;
    curr++;
    currw++;
  }
  curr += 3; /*Skip "://" */
  currw = host;
  while (*curr != 0 && *curr != '/' && *curr != ':')
  {
    *currw = *curr;
    curr++;
    currw++;
  }

  if (*curr == ':')
  { /*There's a port.*/
    currw = portStr;
    curr++;
    while (*curr != 0 && *curr != '/')
    {
      *currw = *curr;
      currw++;
      curr++;
    }
    port = atoi(portStr);
  }
  snprintf(path, sizeof(path), "%s", curr);
}

//#endif /* _WIN32 */
