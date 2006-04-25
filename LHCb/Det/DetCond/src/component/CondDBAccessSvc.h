// $Id: CondDBAccessSvc.h,v 1.13 2006-04-25 17:20:20 marcocle Exp $
#ifndef COMPONENT_CONDDBACCESSSVC_H 
#define COMPONENT_CONDDBACCESSSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBAccessSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class CondDBCache;

/** @class CondDBAccessSvc CondDBAccessSvc.h
 *  
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-01-11
 */

class CondDBAccessSvc: public virtual Service,
                       public virtual ICondDBAccessSvc {
public: 

  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initilize COOL (CondDB) Access Layer Service
  virtual StatusCode initialize();
  /// Finalize Service
  virtual StatusCode finalize();

  // Utilities:
  /// Used to obtain direct access to the database.
  virtual cool::IDatabasePtr& database() { return m_db; }
  
  /// Create a CondDB node in the hierarchy (Folder or FolderSet)
  virtual StatusCode createFolder(const std::string &path,
                                  const std::string &descr,
                                  StorageType storage = XML,
                                  VersionMode vers = MULTI) const;
  
  /// Utility function that simplifies the storage of an XML string.
  virtual StatusCode storeXMLString(const std::string &path, const std::string &data,
                                    const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const;
  
  /// Utility function that simplifies the storage of an XML string.
  /// (Useful for Python, the times are in seconds)
  virtual StatusCode storeXMLString(const std::string &path, const std::string &data,
                                    const double since_s, const double until_s, cool::ChannelId channel = 0) const;
  
  /// Convert from Gaudi::Time class to cool::ValidityKey.
  virtual cool::ValidityKey timeToValKey(const Gaudi::Time &time) const;
   
  /// Convert from cool::ValidityKey to Gaudi::Time class.
  virtual Gaudi::Time valKeyToTime(const cool::ValidityKey &key) const;
 
  /// Return the currently set TAG to use.
  virtual const std::string &tag() const;
  
  /// Set the TAG to use.
  virtual StatusCode setTag(const std::string &_tag);

  /// Tag the given folder with the given tag-name. If the requested folder is
  /// a folderset, the tag is applied to all the folders below it. (waiting for HVS)
  virtual StatusCode tagFolder(const std::string &path, const std::string &tagName,
                               const std::string &description, cool::ChannelId channel = 0);

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                boost::shared_ptr<coral::AttributeList> &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);

  /// Add a folder to the cache (bypass the DB)
  virtual StatusCode cacheAddFolder(const std::string &path, const std::string &descr,
                                    const cool::ExtendedAttributeListSpecification& spec);
  
  /// Add a folder-set to the cache (bypass the DB)
  virtual StatusCode cacheAddFolderSet(const std::string &path, const std::string &descr);
  
  /// Add a folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path);
  
  ///Add an object to the cache (bypass the DB)
  virtual StatusCode cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                    const coral::AttributeList& payload, cool::ChannelId channel = 0);
  
  ///Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                       const std::string& data, cool::ChannelId channel = 0);

  /// Dump the cache (debug)
  virtual void dumpCache() const;

protected:
  /// Standard constructor
  CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~CondDBAccessSvc( ); ///< Destructor

private:
  // Properties

  /// Property CondDBAccessSvc.ConnectionString: full connection string to open database access.
  /// Format is: "<BackEnd>://<HostName>;schema=<Schema>;dbname=<Database>;[user=<User>;][password=<Password>;]"
  /// This will eventually replace the HostName, User, Schema, Database and Password properties.
  std::string m_connectionString;

  /// Property CondDBAccessSvc.HostName: name of the database server
  std::string m_dbHostName;
  
  /** Property CondDBAccessSvc.User: name of the user needed to connect
      to the database server */
  std::string m_dbUser;

  /// Property CondDBAccessSvc.Password: password needed to connect to the DB server
  std::string m_dbPassword;
  
  /// Property CondDBAccessSvc.HidePassword: hide password in log output (default = true)
  bool m_hidePasswd;
  
  /// Property CondDBAccessSvc.Database: name of the database to use
  std::string m_dbName;

  /// Property CondDBAccessSvc.Schema: schema to use
  std::string m_dbSchema;

  /// Property CondDBAccessSvc.DefaultTAG: which tag to use if none is specified
  std::string m_dbTAG;

  /** Property CondDBAccessSvc.BackEnd: which db technology to use (oracle|mysql|sqlite) */
  std::string m_dbBackEnd;

  /** Property CondDBAccessSvc.PurgeDB: Drop and recreate the database during initialization.
      (FIX-ME: There is no way to test if the database is there or in a good shape) */
  bool m_recreateDB;

  /** Property CondDBAccessSvc.RunTest: */
  bool m_test;

  /** Property CondDBAccessSvc.UseCache: store the retrieved informations into a cache for faster
      later access. */
  bool m_useCache;
  
  /// Property CondDBAccessSvc.CacheLowLevel: minimum fill of the cache.
  size_t m_cacheLL;

  /// Property CondDBAccessSvc.CacheHighLevel: maximum fill of the cache.
  size_t m_cacheHL;
  
  /** Property CondDBAccessSvc.NoDB: do not use the database (cache must be on). */
  bool m_noDB;

  /// Shared pointer to the COOL database instance
  cool::IDatabasePtr m_db;

  /// Pointer to the cache manager
  CondDBCache *m_cache;

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_openConnection();

  /// Check if the TAG set exists in the DB.
  inline StatusCode i_checkTag() const { return i_checkTag(tag()); }

  /// Check if the given TAG exists in the DB.
  StatusCode i_checkTag(const std::string &tag) const;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBAccessSvc>;

  /// AttributeListSpecification used to sore XML strings
  static cool::ExtendedAttributeListSpecification *s_XMLstorageAttListSpec;

  /// Counter used to know how many instances of CondDBAccessSvc are around
  /// (and needing the AttributeListSpecification pointers).
  static unsigned long long s_instances;
};
#endif // COMPONENT_CONDDBACCESSSVC_H
