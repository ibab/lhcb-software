// $Id: CondDBAccessSvc.h,v 1.26 2007-03-22 16:04:12 marcocle Exp $
#ifndef COMPONENT_CONDDBACCESSSVC_H 
#define COMPONENT_CONDDBACCESSSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBReader.h"
#include "DetCond/ICondDBEditor.h"
#include <set>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>

#include "CoolKernel/IDatabase.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class CondDBCache;
class IRndmGenSvc;

namespace cool {
  class Application;
  class RecordSpecification;
}

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
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);


  // --------- ICondDBEditor implementation

  /// Create a CondDB node in the hierarchy (Folder or FolderSet).
  virtual StatusCode createNode(const std::string &path,
                                const std::string &descr,
                                StorageType storage = XML,
                                VersionMode vers = MULTI) const;

  /// Create a CondDB node in the hierarchy (Folder or FolderSet).
  virtual StatusCode createNode(const std::string &path,
                                const std::string &descr,
                                const std::set<std::string> &fields,
                                StorageType storage = XML,
                                VersionMode vers = MULTI) const;

  /// Utility function that simplifies the storage of an XML string.
  virtual StatusCode storeXMLData(const std::string &path, const std::string &data,
                                  const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const;

  /// Utility function that simplifies the storage of a set of XML strings.
  virtual StatusCode storeXMLData(const std::string &path, const std::map<std::string,std::string> &data,
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
                                    const cool::IRecordSpecification& spec);
  
  /// Add a folder-set to the cache (bypass the DB)
  virtual StatusCode cacheAddFolderSet(const std::string &path, const std::string &descr);
  
  /// Add a folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path);
  
  /// Add an XML folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path, const std::set<std::string> &fields);

  ///Add an object to the cache (bypass the DB)
  virtual StatusCode cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                    const cool::IRecord& payload, cool::ChannelId channel = 0);
  
  ///Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                     const std::string& data, cool::ChannelId channel = 0);

  /// Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                     const std::map<std::string,std::string> &data, cool::ChannelId channel = 0);

  /// Clear the cache
  virtual void clearCache();

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
  static cool::RecordSpecification *s_XMLstorageSpec;

  /// Counter used to know how many instances of CondDBAccessSvc are around
  /// (and needing the AttributeListSpecification pointers).
  static unsigned long long s_instances;

  /// Pointer to a shared instance of the COOL Application
  static std::auto_ptr<cool::Application> s_coolApplication;

  /// Flag to turn off/on the CORAL LFCReplicaService (option UseLFCReplicaSvc, default = false).
  /// Setting this option works only if it is set for the first CondDBAccessSvc initialized
  /// because of a "feature" of CORAL.
  bool m_useLFCReplicaSvc;

  /// Flag to turn off/on the CORAL Automatinc connection clean up
  /// (option EnableCoralConnectionCleanUp, default = false).
  /// Setting this option works only if it is set for the first CondDBAccessSvc initialized.
  bool m_coralConnCleanUp;

  // -------------------------------------
  // ---------- Time Out Thread ----------
  // -------------------------------------

  /// Property CondDBAccessSvc.ConnectionTimeOut: How many seconds to keep the connection to the DB open after the
  /// last access (default = 0, which means never). The connection will be started again if a new operation is performed.
  int m_connectionTimeOut;

  /// Mutex to avoid conflicts between the main thread and the thread trying to close the connection.
  boost::mutex m_busy;

  /// Condition used to synchronize the timeout loop to the main one.
  boost::condition m_stop;

  /// The time of last access.
  boost::xtime     m_lastAccess;

  /// Mutex to protect the last access time.
  boost::mutex     m_lastAccessMutex;

  /// Pointer to the second thread.
  std::auto_ptr<boost::thread> m_timeOutCheckerThread;

  /// Function to set the last access time to "now".
  inline void touchLastAccess()
  {
    boost::mutex::scoped_lock myLock(m_lastAccessMutex);
    boost::xtime_get(&m_lastAccess,boost::TIME_UTC);
  }
  
  /// Get the last access time.
  inline const boost::xtime &lastAccess() const 
  {
    return m_lastAccess;
  }

  class TimeOutChecker 
  {
    CondDBAccessSvc *m_owner;
    MsgStream        log;
    //    boost::mutex     &m_busy;
    //    boost::condition &m_stop;
    //    boost::xtime     &m_lastAccess;
    //    int              m_deltaT
    
  public:
    TimeOutChecker(CondDBAccessSvc *owner):
      m_owner(owner),
      log(m_owner->msgSvc(),m_owner->name()+".TimeOutChecker")
    {
    }
    

    void operator () () 
    {
      log << MSG::VERBOSE << "Starting" << endmsg;

      // prepare lock needed for condition check
      boost::mutex stop_mutex;
      boost::mutex::scoped_lock stop_lock(stop_mutex);

      boost::xtime last_access = m_owner->lastAccess();

      // set initial check time
      boost::xtime next_check = last_access;
      next_check.sec += m_owner->m_connectionTimeOut;

      while ( !m_owner->m_stop.timed_wait(stop_lock,next_check) ) {

        log << MSG::VERBOSE << "Time-out reached (" << next_check.sec << ")" << endmsg;

        boost::mutex::scoped_lock busy_lock(m_owner->m_busy);        

        if ( last_access.sec == m_owner->lastAccess().sec ) { // no further accesses

          if ( m_owner->database()->isOpen() ) { // close the database
            log << MSG::INFO << "Disconnect from database after being idle for "
                << m_owner->m_connectionTimeOut << "s (will reconnect if needed)"<< endmsg;
            m_owner->database()->closeDatabase();
          }

          // schedule the next check for now + dt (seems a good estimate)
          boost::xtime_get(&next_check,boost::TIME_UTC); // current time
          next_check.sec += m_owner->m_connectionTimeOut;

        } else {
          
          log << MSG::VERBOSE << "Wait more" << endmsg;

          // schedule the next check for last_access + dt
          next_check = last_access = m_owner->lastAccess();
          next_check.sec += m_owner->m_connectionTimeOut;
        }        
      }
      log << MSG::VERBOSE << "Stopping" << endmsg;
    }
  };

  class DataBaseOperationLock 
  {
    CondDBAccessSvc    *m_owner;
    MsgStream           log;
    boost::mutex::scoped_lock busy_lock;
  public:
    DataBaseOperationLock(const CondDBAccessSvc *owner):
      m_owner(const_cast<CondDBAccessSvc*>(owner)),
      log(m_owner->msgSvc(),m_owner->name()+".DataBaseOperationLock"),
      busy_lock(m_owner->m_busy) // lock the access to the db
    {
      if (!owner->m_db->isOpen()){
        log << MSG::INFO << "Connecting to database" << endmsg;
        owner->m_db->openDatabase(); // ensure that the db is open
      }
    }
 
    ~DataBaseOperationLock() 
    {
      m_owner->touchLastAccess(); // update last access time
    }
  };  

  friend class TimeOutChecker;
  friend class DataBaseOperationLock;

};
#endif // COMPONENT_CONDDBACCESSSVC_H
