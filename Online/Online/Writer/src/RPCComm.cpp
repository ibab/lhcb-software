#ifdef BUILD_WRITER

#include <stdexcept>
#include <iostream>
#include <pthread.h>
#include "Writer/RPCComm.h"

using namespace LHCb;

//Names for the XML-RPC functions.
std::string confirmString("confirmFile");
std::string createString("createNewFile");

RPCComm::RPCComm(std::string serverURL)
{
  m_serverURL = serverURL;
  pthread_mutex_init(&m_rpcLock, NULL);
}

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
  xmlrpc_c::value result;
  int ret;

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

  pthread_mutex_lock(&m_rpcLock);

  try {
    m_clientInstance.call(m_serverURL, confirmString, "sss", &result,
        fileName, adler32String, md5CharString);
    ret = xmlrpc_c::value_int(result);
  } catch(girerr::error& err) {

    pthread_mutex_unlock(&m_rpcLock);
    throw std::runtime_error(err.what());
  }

  pthread_mutex_unlock(&m_rpcLock);

  ret = xmlrpc_c::value_int(result);
  if(ret == RUNDB_SERVICE_FAIL) {
    throw std::runtime_error(
        "Could not call RunDB service for confirm(). Check RunDB logs.");
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
  xmlrpc_c::value result;
  int ret;

  pthread_mutex_lock(&m_rpcLock);

  try {
    char runNumberString[20];
    ::sprintf(runNumberString, "%u", runNumber);
    m_clientInstance.call(m_serverURL, createString, "ss", &result,
        fileName, runNumberString);
    ret = xmlrpc_c::value_int(result);
  } catch(girerr::error& err) {

    pthread_mutex_unlock(&m_rpcLock);
    throw std::runtime_error(err.what());
  }

  pthread_mutex_unlock(&m_rpcLock);

  ret = xmlrpc_c::value_int(result);
  if(ret == RUNDB_SERVICE_FAIL) {
    throw std::runtime_error(
        "Could not call RunDB service for create(). Check RunDB logs.");
  }
  return;
}

#endif /* _WIN32 */
