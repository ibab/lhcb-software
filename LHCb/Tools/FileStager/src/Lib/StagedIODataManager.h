#ifndef STAGEDIODATAMANAGER_H
#define STAGEDIODATAMANAGER_H

// C++ include files
#include <map>

// Framework include files
#include <GaudiKernel/Service.h>
#include <GaudiUtils/IIODataManager.h>

class IIncidentSvc;
class IFileStagerSvc;

namespace Gaudi  {

// Forward declarations
class IFileCatalog;

/** @class StagedIODataManager
 * Same functionality implementation with the IODataManager except for
 * connectDataIO() method which checks via the FileStageSvc if a local copy
 * is available in the bookkeeping and uses it instead of the original dsn.
 *
 */
class StagedIODataManager : public extends1< Service, IIODataManager > {
protected:
   typedef const std::string& CSTR;
   struct Entry  {
      std::string      type;
      IoType           ioType;
      IDataConnection* connection;
      bool             keepOpen;
      Entry(CSTR tech,bool k, IoType iot,IDataConnection* con)
         : type(tech), ioType(iot), connection(con), keepOpen(k) {
      }
   };
   typedef std::map<std::string,Entry*>       ConnectionMap;
   typedef std::map<std::string, std::string> FidMap;

   /// Property: Name of the file catalog service
   std::string m_catalogSvcName;
   std::string m_stagerSvcName;

   /// Property: Age limit
   int m_ageLimit;
   /// Property: Flag for auto gfal data access
   bool m_useGFAL;
   /// Property: Flag if unaccessible files should be quarantines in job
   bool m_quarantine;

   /// Map with I/O descriptors
   ConnectionMap m_connectionMap;
   /// Reference to file catalog
   SmartIF<IFileCatalog> m_catalog;
   SmartIF<IFileStagerSvc> m_stager;
   /// Map of FID to PFN
   FidMap               m_fidMap;
   StatusCode connectDataIO(int typ, IoType rw, CSTR fn, CSTR technology, bool keep,Connection* con);
   StatusCode reconnect(Entry* e);
   StatusCode error(CSTR msg, bool rethrow);
   StatusCode establishConnection(Connection* con);

   SmartIF<IIncidentSvc> m_incSvc; ///the incident service

public:

   /** Initializing constructor
    *  @param[in]   nam   Name of the service
    *  @param[in]   loc   Pointer to the service locator object
    *  @return Initialized reference to service object
    */
   StagedIODataManager(CSTR nam, ISvcLocator* loc);

   /// Standard destructor
   virtual ~StagedIODataManager()  {}

   /// IService implementation: initialize the service
   virtual StatusCode initialize();

   /// IService implementation: finalize the service
   virtual StatusCode finalize();

   /// Open data stream in read mode
   virtual StatusCode connectRead(bool keep_open, Connection* ioDesc);
   /// Open data stream in write mode
   virtual StatusCode connectWrite(Connection* con,IoType mode=Connection::CREATE,CSTR doctype="UNKNOWN");
   /// Release data stream
   virtual StatusCode disconnect(Connection* ioDesc);
   /// Retrieve known connection
   virtual Connection* connection(const std::string& dsn) const;
   /// Get connection by owner instance (0=ALL)
   virtual Connections connections(const IInterface* owner) const;
   /// Read raw byte buffer from input stream
   virtual StatusCode read(Connection* ioDesc, void* const data, size_t len);
   /// Write raw byte buffer to output stream
   virtual StatusCode write(Connection* con, const void* data, int len);
   /// Seek on the file described by ioDesc. Arguments as in ::seek()
   virtual long long int seek(Connection* ioDesc, long long int where, int origin);
};
} // End namespace Gaudi
#endif    // STAGEDIODATAMANAGER_H
