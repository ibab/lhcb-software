#include <string>
#include <stdexcept>
#include <iostream>

#include "MDF/RPCComm.h"

//Names for the XML-RPC functions.
std::string confirmString("createAndConfirmFile");

RPCComm::RPCComm(std::string & serverURL)
{
	this->serverURL = serverURL;
}

/**
 * "Confirms" that the file is completely written to, closed, and ready
 * for migration to tape.
 */
void RPCComm::confirmFile(std::string fileName)
{
	xmlrpc_c::value result;
	int ret;

	try {
		clientInstance.call(serverURL, confirmString, "s", &result, fileName.c_str());
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
