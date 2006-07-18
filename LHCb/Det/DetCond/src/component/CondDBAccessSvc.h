// $Id: CondDBAccessSvc.h,v 1.20 2006-07-18 13:14:26 marcocle Exp $
#ifndef COMPONENT_CONDDBACCESSSVC_H 
#define COMPONENT_CONDDBACCESSSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBReader.h"
#include "DetCond/ICondDBEditor.h"
#include <set>

// Forward declarations
template <class TYPE> class SvcFactory;

class CondDBCache;
class IRndmGenSvc;

/** @class CondDBAccessSvc CondDBAccessSvc.h
 *  
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-01-11
 */

class CondDBAccessSvc: public virtual Service,
                       public virtual ICondDBAccessSvc,
                       public virtual ICondDBReader,
                       public virtual ICondDBEditor {
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


  // --------- ICondDBReader implementation

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                boost::shared_ptr<coral::AttributeList> &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);


  // --------- ICondDBEditor implementation

  /// Create a CondDB node in the hierarchy (Folder or FolderSet).
  virtual StatusCode createNode(const std::string &path,
                                const std::string &descr,
                                StorageType storage = XML,
                                VersionMode vers = MULTI) const;

  /// Utility function that simplifies the storage of an XML string.
  virtual StatusCode storeXMLString(const std::string &path, const std::string &data,
                                    const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const;

  /// Tag the given leaf node with the given tag-name.
  virtual StatusCode tagLeafNode(const std::string &path, const std::string &tagName,
                                 const std::string &description = "");

  /// Tag the given inner node with the given tag-name, recursively tagging the head
  /// of child nodes with automatically generated tag-names.
  virtual StatusCode recursiveTag(const std::string &path, const std::string &tagName,
                                  const std::string &description = "");

  // --------- ICondDBAccessSvc implementation

  /// Used to obtain direct access to the database.
  virtual cool::IDatabasePtr& database() { return m_db; }
  
  /// Convert from Gaudi::Time class to cool::ValidityKey.
  virtual cool::ValidityKey timeToValKey(const Gaudi::Time &time) const;
   
  /// Convert from cool::ValidityKey to Gaudi::Time class.
  virtual Gaudi::Time valKeyToTime(const cool::ValidityKey &key) const;
 
  /// Return the currently set TAG to use.
  virtual const std::string &tag() const;
  
  /// Set the TAG to use.
  virtual StatusCode setTag(const std::string &_tag);

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
  /// or "<HostAlias>/<Database>".
  std::string m_connectionString;

  /// Property CondDBAccessSvc.DefaultTAG: which tag to use if none is specified
  std::string m_dbTAG;

  /** Property CondDBAccessSvc.UseCache: store the retrieved informations into a cache for faster
      later access. */
  bool m_useCache;
  
  /// Property CondDBAccessSvc.CacheLowLevel: minimum fill of the cache.
  size_t m_cacheLL;

  /// Property CondDBAccessSvc.CacheHighLevel: maximum fill of the cache.
  size_t m_cacheHL;
  
  /// Property CondDBAccessSvc.NoDB: do not use the database (cache must be on).
  bool m_noDB;

  /// Property CondDBAccessSvc.ReadOnly: open the database as read-only (default: true).
  bool m_readonly;

  /// Property CondDBAccessSvc.CheckTAGTrials: Number of times to retry the check on the tag (default = 1).
  int m_checkTagTrials;

  /// Property CondDBAccessSvc.CheckTAGTimeOut: Seconds to sleep between one trial and the following (default = 60).
  int m_checkTagTimeOut;

  /// Shared pointer to the COOL database instance
  cool::IDatabasePtr m_db;

  /// Shared pointer to the COOL database instance
  cool::IFolderSetPtr   m_rootFolderSet;

  /// Pointer to the cache manager
  CondDBCache *m_cache;

  /// Pointer to the random generator service
  IRndmGenSvc *m_rndmSvc;

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_openConnection();

  /// Check if the TAG set exists in the DB.
  inline StatusCode i_checkTag() const { return i_checkTag(tag()); }

  /// Check if the given TAG exists in the DB.
  StatusCode i_checkTag(const std::string &tag) const;

  /// Generate a tag name that do not create conflicts in the DB.
  /// Tagnames generated by this mthod will start with "_auto_".
  /// If base value is passed to the method, the result will have
  /// a "_auto_`base`-" prefix.
  std::string generateUniqueTagName(const std::string &base,
                                    const std::set<std::string> &reserved) const;

  /// Function used by recursiveTag to do the real work.
  StatusCode i_recursiveTag(const std::string &path,
                            const std::string &base,
                            const std::string &description,
                            const std::string &tagName,
                            std::set<std::string> &reserved);

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBAccessSvc>;

  /// AttributeListSpecification used to sore XML strings
  static cool::ExtendedAttributeListSpecification *s_XMLstorageAttListSpec;

  /// Counter used to know how many instances of CondDBAccessSvc are around
  /// (and needing the AttributeListSpecification pointers).
  static unsigned long long s_instances;

};
#endif // COMPONENT_CONDDBACCESSSVC_H
