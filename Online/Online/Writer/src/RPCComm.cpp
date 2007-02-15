#include <stdexcept>
#include <iostream>
#include "Writer/RPCComm.h"

using namespace LHCb;

//Names for the XML-RPC functions.
std::string confirmString("confirmFile");
std::string createString("createNewFile");

RPCComm::RPCComm(std::string serverURL)
{
  m_serverURL = serverURL;
}

/**
  * "Confirms" that the file is completely written to and closed.
  * This creates a record in the run database specifying that the
  * file may now be migrated to tape.
  * @param fileName The name of the file which has been closed.
  * @param adlerSum The Adler32 checksum of the entire file.
  * @param md5CSum   The MD5 checksum of the entire file.
  */
void RPCComm::confirmFile(std::string & fileName, unsigned int adlerSum, const unsigned char *md5CSum)
{
  xmlrpc_c::value result;
  int ret;
  char md5CharString[33];
  char adler32String[9];

  /* We need to send this as a string because it's not very clear how the
   * XMLRPC library handles unsigned values.
   */

  sprintf(md5CharString, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
    md5CSum[0], md5CSum[1], md5CSum[2], md5CSum[3], 
    md5CSum[4], md5CSum[5], md5CSum[6], md5CSum[7], 
    md5CSum[8], md5CSum[9], md5CSum[10], md5CSum[11], 
    md5CSum[12], md5CSum[13], md5CSum[14], md5CSum[15]);

  sprintf(adler32String, "%X", adlerSum);

  try {
    m_clientInstance.call(m_serverURL, confirmString, "sss", &result,
      fileName.c_str(), adler32String, md5CharString);
    ret = xmlrpc_c::value_int(result);
  } catch(girerr::error err) {
    std::cout << err.what();
    throw std::runtime_error(
      "Could not initiate connection to Run DB for createFile().");
  }

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
void RPCComm::createFile(std::string & fileName, unsigned int runNumber)
{
  xmlrpc_c::value result;
  int ret;

  try {
    char runNumberString[20];
    ::sprintf(runNumberString, "%u", runNumber);
    m_clientInstance.call(m_serverURL, createString, "ss", &result,
      fileName.c_str(), runNumberString);
    ret = xmlrpc_c::value_int(result);
  } catch(girerr::error err) {
    std::cout << err.what();
    throw std::runtime_error(
      "Could not initiate connection to Run DB for createFile().");
  }

  ret = xmlrpc_c::value_int(result);
  if(ret == RUNDB_SERVICE_FAIL) {
    throw std::runtime_error(
      "Could not call RunDB service for create(). Check RunDB logs.");
  }
  return;
}

int main(int argc, char **argv)
{
  std::string serverUrl("http://lbondiskdev.cern.ch:8080/");
  RPCComm *c = new RPCComm(serverUrl);

  for(int i=0;i<10;i++) {
    unsigned char md5[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};

    char buffer[20];
    unsigned char *fstar = md5;

    sprintf(buffer, "%s_%i", "File18_Random_File_Name_Run_", i);

    std::string fName(buffer);

    //c->createFile(fName, 1);
    c->confirmFile(fName, 0x123456u, fstar);
  }
}

