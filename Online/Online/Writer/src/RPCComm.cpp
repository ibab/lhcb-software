#ifdef BUILD_WRITER

#include <stdexcept>
#include <iostream>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Writer/RPCComm.h"
#include "Writer/Utils.h"

using namespace LHCb;

/**
 * "Confirms" that the file is completely written to and closed.
 * This creates a record in the run database specifying that the
 * file may now be migrated to tape.
 * @param fileName The name of the file which has been closed.
 * @param adlerSum The Adler32 checksum of the entire file.
 * @param md5CSum   The MD5 checksum of the entire file.
 */
void RPCComm::confirmFile(char *fileName, unsigned int adlerSum, const unsigned char *md5CSum)
{
  int ret;
  char headerData[1024];
  char xmlData[1024];
  char response[1024];

  char adler32String[9];
  char md5CharString[33];

  /* We need to send this as a string because it's not very clear how the
   * XMLRPC library handles unsigned values.
   */
  sprintf(md5CharString, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
      md5CSum[0], md5CSum[1], md5CSum[2], md5CSum[3],
      md5CSum[4], md5CSum[5], md5CSum[6], md5CSum[7],
      md5CSum[8], md5CSum[9], md5CSum[10], md5CSum[11],
      md5CSum[12], md5CSum[13], md5CSum[14], md5CSum[15]);

  sprintf(adler32String, "%08X", adlerSum);

  /* Now we fill up templates. */
  snprintf(xmlData, sizeof(xmlData), CONFIRM_TEMPLATE,
  	fileName, adler32String, md5CharString);
  snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE,
  	"WriterHost", strlen(xmlData));

  memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response));

  if(ret < 0)
    throw std::runtime_error("Could not run RPC call for confirm.");
  if(isError(response))
  	throw std::runtime_error(response);

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
  char response[1024];

  snprintf(xmlData, sizeof(xmlData), CREATE_TEMPLATE,
    fileName, runNumber);
  snprintf(headerData, sizeof(headerData), HEADER_TEMPLATE,
    "WriterHost", strlen(xmlData));

  memset(response, 0, sizeof(response));
  ret = requestResponse(headerData, xmlData, response, sizeof(response));

  if(ret < 0)
    throw std::runtime_error("Could not run RPC call for create.");
  if(isError(response))
  	throw std::runtime_error(response);

  return;
}

/**
 * Connects, writes RPC information to an HTTP server, and reads the result.
 * Returns 0 on success, -1 on error.
 */
int RPCComm::requestResponse(char *requestHeader, char *requestData, char *response, int responseLen)
{
	struct sockaddr_in destAddr;
	int sockFd;
	int ret;


  if(Utils::nameLookup(m_serverURL->getHost(), &destAddr, NULL) != 0)
		throw std::runtime_error("Could not resolve name.");
	destAddr.sin_port = htons(m_serverURL->getPort());
	destAddr.sin_family = AF_INET;

  sockFd = Utils::connectToAddress(&destAddr,
    Utils::DEFAULT_BUF_SIZE, Utils::DEFAULT_BUF_SIZE, NULL);

	BIF sendBif1(sockFd, requestHeader, strlen(requestHeader));
	BIF sendBif2(sockFd, requestData, strlen(requestData));
	BIF recvBif(sockFd, response, responseLen);

  if(sockFd < 0)
    throw std::runtime_error("Could not connect to RPC server.");
  ret = sendBif1.nbSendTimeout();
  if(ret == BIF::TIMEDOUT || ret == BIF::DISCONNECTED)
    throw std::runtime_error("Could not send request header.");
  ret = sendBif2.nbSendTimeout();
  if(ret == BIF::TIMEDOUT || ret == BIF::DISCONNECTED)
    throw std::runtime_error("Could not send request data.");
  ret = recvBif.nbRecvTimeout();
  if(recvBif.getBytesRead() <= 0)
    throw std::runtime_error("Could not read response data.");

  close(sockFd);
  return 0;
}

/**
 * Checks the XML RPC response string for an error code.
 * Returns 1 in case of an error, and 0 if not.
 */
int RPCComm::isError(char *response)
{
	std::string responseStr(response);
	std::string::size_type loc;

	/*First check for HTTP response status.*/
	loc = responseStr.find("200 OK");
	if(loc > responseStr.length())
		return 1;

	/*Then check for a fault string.*/
	loc = responseStr.find("faultCode");
	if(loc > responseStr.length())
		return 0;
	return 1;

	/*Dump the whole response if fault found.*/
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

  curr = (char*)url;
  currw = protocol;

  while(*curr != 0 && *curr != ':') {
    *currw = *curr;
    curr++; currw++;
  }
  curr+=3; /*Skip "://" */
  currw = host;
  while(*curr != 0 && *curr != '/' && *curr != ':') {
  	*currw = *curr;
  	curr++; currw++;
  }

  if(*curr == ':') { /*There's a port.*/
  	currw = portStr;
  	curr++;
  	while(*curr != 0 && *curr != '/') {
		  *currw = *curr;
	    currw++; curr++;
	  }
    port = atoi(portStr);
  }
  snprintf(path, sizeof(path), "%s", curr);
}


#endif /* _WIN32 */
