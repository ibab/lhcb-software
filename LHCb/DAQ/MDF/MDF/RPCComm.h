#include <xmlrpc-c/girmem.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#define RUNDB_SERVICE_FAIL		0
#define RUNDB_SERVICE_SUCCESS	1

class RPCComm
{
	private:

		/**
		 * An instance of an XML-RPC client. This is reused
		 * for all subsequent calls to the run database service.
		 */
		xmlrpc_c::clientSimple 	clientInstance;
		std::string 			serverURL;

		//////////Functions for talking to the RunDB///////////
		/**
		 * Creates an entry in the Run Database for the specified file,
		 * associated with the specified Run Number and Stream.
		 */
		void createNewFile(std::string fileName, int runNumber, int stream);

	public:
		RPCComm(std::string & serverURL);
		/**
		 * "Confirms" that the file is completely written to, closed,
		 * and ready for migration to tape.
		 */
		void confirmFile(std::string fileName);


};
