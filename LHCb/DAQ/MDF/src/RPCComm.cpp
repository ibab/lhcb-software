#include <string>
#include <stdexcept>
#include <iostream>

#include "MDF/RPCComm.h"

using namespace LHCb;

//Names for the XML-RPC functions.
std::string confirmString("confirmFile");
std::string createString("createNewFile");

RPCComm::RPCComm(std::string & serverURL)
{
	this->serverURL = serverURL;
}

/**
  * "Confirms" that the file is completely written to, closed, and ready
  * for migration to tape.
  */
void RPCComm::confirmFile(std::string fileName, unsigned int adlerSum, unsigned char md5Sum[16])
{
	xmlrpc_c::value result;
	int ret;

	char md5String[33];
	char adler32String[8];
	// First we need to convert the values into strings.

	sprintf(md5String, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
		md5Sum[0],md5Sum[1],md5Sum[2],md5Sum[3],
		md5Sum[4],md5Sum[5],md5Sum[6],md5Sum[7],
		md5Sum[8],md5Sum[9],md5Sum[10],md5Sum[11],
		md5Sum[12],md5Sum[13],md5Sum[14],md5Sum[15]);

	sprintf(adler32String, "%X", adlerSum);

	try {
		clientInstance.call(serverURL, confirmString, "sss", &result,
			fileName.c_str(), adler32String, md5String);
		ret = xmlrpc_c::value_int(result);
	} catch(girerr::error err) {
		std::cout << err.what();
		throw std::runtime_error(
			"Could not initiate connection to Run DB for createAndConfirmFile().");
	}

	ret = xmlrpc_c::value_int(result);
	if(ret == RUNDB_SERVICE_FAIL) {
		throw std::runtime_error(
			"Could not call RunDB service for confirm(). Check RunDB logs.");
	}
	return;
}

/**
  * Creates an entry for a file in the run database.
  */
void RPCComm::createFile(std::string fileName, int runNumber)
{
	xmlrpc_c::value result;
	int ret;

	try {
		clientInstance.call(serverURL, createString, "si", &result,
			fileName.c_str(), runNumber);
		ret = xmlrpc_c::value_int(result);
	} catch(girerr::error err) {
		std::cout << err.what();
		throw std::runtime_error(
			"Could not initiate connection to Run DB for confirmFile().");
	}

	ret = xmlrpc_c::value_int(result);
	if(ret == RUNDB_SERVICE_FAIL) {
		throw std::runtime_error(
			"Could not call RunDB service for confirm(). Check RunDB logs.");
	}
	return;
}

