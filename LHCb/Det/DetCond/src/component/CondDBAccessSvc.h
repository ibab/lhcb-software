#ifndef COMPONENT_CONDDBACCESSSVC_H
#define COMPONENT_CONDDBACCESSSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/GaudiException.h"
#include "Kernel/ICondDBInfo.h"
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBReader.h"
#include "DetCond/ICondDBEditor.h"
#include <set>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread_time.hpp>

#include "CoolKernel/IDatabase.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class CondDBCache;
class IRndmGenSvc;
class ICOOLConfSvc;

namespace cool {
  class Application;
  class RecordSpecification;
}

/** @class CondDBAccessSvc CondDBAccessSvc.h
 *
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  @author Marco Clemencic
 *  @date   2005-01-11
 */

class CondDBAccessSvc: public extends3<Service,
                                       ICondDBAccessSvc,
                                       ICondDBReader,
                                       ICondDBEditor> {
public:

  /// Initialize COOL (CondDB) Access Layer Service
  virtual StatusCode initialize();
  /// Finalize Service
  virtual StatusCode finalize();


  // --------- ICondDBReader implementation

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  /// (Version with alphanumeric channel id)
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel);

  /// @{
  /// @see ICondDBReader::getIOVs
  virtual IOVList getIOVs (const std::string &path, const IOV &iov, cool::ChannelId channel = 0);
  virtual IOVList getIOVs (const std::string &path, const IOV &iov, const std::string &channel);
  /// @}

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);

  /// Retrieve the names of the children nodes of a FolderSet divided in folders and foldersets.
  virtual StatusCode getChildNodes (const std::string &path,
                                    std::vector<std::string> &folders,
                                    std::vector<std::string> &foldersets);

  /// Tells if the path is available in the database.
  virtual bool exists(const std::string &path);

  /// Tells if the path (if it exists) is a folder.
  virtual bool isFolder(const std::string &path);

  /// Tells if the path (if it exists) is a folderset.
  virtual bool isFolderSet(const std::string &path);

  /// Disconnect from the database.
  virtual void disconnect();

  // --------- ICondDBInfo implementation

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags ) const;

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

  /// Return the connection string used to connect to the database.
  virtual const std::string &connectionString() const;

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

protected:
  // Properties

  /// Property CondDBAccessSvc.ConnectionString: full connection string to open database access.
  /// Format is: "<BackEnd>://<HostName>;schema=<Schema>;dbname=<Database>;[user=<User>;][password=<Password>;]"
  /// or "<HostAlias>/<Database>".
  std::string m_connectionString;

private:
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

  /// Pointer to the service initializing COOL/CORAL.
  ICOOLConfSvc *m_coolConfSvc;

  /// Shared pointer to the COOL database instance
  cool::IDatabasePtr m_db;

  /// Shared pointer to the COOL database instance
  cool::IFolderSetPtr   m_rootFolderSet;

  /// Pointer to the cache manager
  CondDBCache *m_cache;

  /// Pointer to the random generator service
  IRndmGenSvc *m_rndmSvc;

  /// Lazy connection flag.
  /// If true (the default), the connection to  (lazy = connect only when needed).
  bool m_lazyConnect;

  /// Enable/disable direct mapping from the database structure to the transient
  /// store using XML persistency format (enabled by default).
  bool m_xmlDirectMapping;

  /// Path in the condition database (not in the transient store) to be used as
  /// heart-beat marker.
  /// The latest update of the condition specified give information about when
  /// the replica was last updated: We cannot guarantee that the database is more
  /// "up to date" then the "since" field of the latest object in the heart-beat
  /// condition.
  std::string m_heartBeatCondition;

  /// Latest know update of the database ("since" field of the latest heart-beat condition).
  /// Initialized to 0, if no heart-beat condition is requested, it is set to
  /// cool::ValidityKeyMax, otherwise, during the first access to the DB, the
  /// object valid until ValidityKeyMax is retrieved and its "since" field is
  /// recorded in this variable.
  /// When disconnected from the database, it is reset to 0 to force a re-check.
  cool::ValidityKey m_latestHeartBeat;

  // ----------------------------------------------
  // ---------- Private Member Functions ----------
  // ----------------------------------------------

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_initializeConnection();

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_openConnection();

  /// Internal method to retrieve an object.
  StatusCode i_getObject(const std::string &path, const Gaudi::Time &when,
                         DataPtr &data,
                         std::string &descr, Gaudi::Time &since, Gaudi::Time &until,
                         bool use_numeric_chid,
                         cool::ChannelId channel, const std::string &channelstr);

  /// Internal method to retrieve an object from the database.
  /// If the cache is activated, the result is copied there.
  StatusCode i_getObjectFromDB(const std::string &path, const cool::ValidityKey &when,
                               DataPtr &data,
                               std::string &descr, cool::ValidityKey &since, cool::ValidityKey &until,
                               bool use_numeric_chid,
                               cool::ChannelId channel, const std::string &channelstr);

  /// Internal method to get the list of IOVs in a range.
  /// @see ICondDBReader::getIOVs
  IOVList i_getIOVsFromDB(const std::string &path, const IOV &iov, cool::ChannelId channel = 0);

  void i_generateXMLCatalogFromFolderset(const std::string &path);

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_validateDefaultTag();

  /// Check if the TAG set exists in the DB.
  inline StatusCode i_checkTag() const { return i_checkTag(tag()); }

  /// Check if the given TAG exists in the DB.
  StatusCode i_checkTag(const std::string &tag) const;

  /// Generate a tag name that do not create conflicts in the DB.
  /// Tagnames generated by this method will start with "_auto_".
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


  /// Return the value of m_latestHeartBeat.
  /// The value is retrieved from the database when requested the first time
  /// in the RUNNING state.
  const cool::ValidityKey &i_latestHeartBeat();

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBAccessSvc>;

  /// AttributeListSpecification used to sore XML strings
  static std::unique_ptr<cool::RecordSpecification> s_XMLstorageSpec;

  /// Parameter controlling the granularity of the queries to the conditions database (in time units).
  ///
  /// When different from 0, instead of retrieving the only condition valid for the requested event time,
  /// we will retrieve all the conditions valid for a range around the event time, rounded by the
  /// granularity.
  ///
  /// For example, with granularity set to 1h and the event time 16:18:08, all the conditions valid for the
  /// range 16:00:00 to 17:00:00 will be retrieved.
  ///
  /// \b Note: if the cache is not enabled the parameter has no effect.
  cool::ValidityKey m_queryGranularity;

  // -------------------------------------
  // ---------- Time Out Thread ----------
  // -------------------------------------

  /// Property CondDBAccessSvc.ConnectionTimeOut: How many seconds to keep the connection to the DB open after the
  /// last access (default = 120, 0 means never). The connection will be started again if a new operation is performed.
  int m_connectionTimeOutProp;
  boost::posix_time::time_duration m_connectionTimeOut;

  /// Mutex to avoid conflicts between the main thread and the thread trying to close the connection.
  boost::mutex m_busy;

  /// The time of last access.
  boost::system_time     m_lastAccess;

  /// Mutex to protect the last access time.
  boost::mutex     m_lastAccessMutex;

  /// Pointer to the second thread.
  std::unique_ptr<boost::thread> m_timeOutCheckerThread;

  /// Function to set the last access time to "now".
  inline void touchLastAccess()
  {
    boost::mutex::scoped_lock myLock(m_lastAccessMutex);
    m_lastAccess = boost::get_system_time();
  }

  /// Get the last access time.
  inline const boost::system_time &lastAccess() const
  {
    return m_lastAccess;
  }

  /// Start the timeout checker thread, if requested.
  inline void i_startTimeoutChecker() {
    if ( UNLIKELY( (!m_timeOutCheckerThread)
                   && (!m_connectionTimeOut.is_pos_infinity()) ) ) {
      m_timeOutCheckerThread.reset( new boost::thread(TimeOutChecker{this}) );
    }
  }

  /// Stop the timeout checker thread if running.
  inline void i_stopTimeoutChecker() {
    if ( LIKELY( NULL != m_timeOutCheckerThread.get() ) ) {
      m_timeOutCheckerThread->interrupt(); // tell the thread to stop
      m_timeOutCheckerThread->join(); // wait for it
      m_timeOutCheckerThread.reset(); // delete it
    }
  }

  /// Class executed in a separate thread to disconnect from the database if
  /// a time-out is reached.
  class TimeOutChecker
  {
    /// Pointer to the CondDBAccessSvc, used to acquire operation locks and
    /// access parameters.
    CondDBAccessSvc *m_owner;
    /// Cached MsgStream to report messages.
    MsgStream        log;

  public:
    TimeOutChecker(CondDBAccessSvc *owner):
      m_owner(owner),
      log(m_owner->msgSvc(),m_owner->name()+".TimeOutChecker")
    {
    }

    void operator () ()
    {
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
        log << MSG::VERBOSE << "Starting" << endmsg;

      boost::system_time last_access = m_owner->lastAccess();

      // set initial check time
      boost::system_time next_check = last_access + m_owner->m_connectionTimeOut;

      try {
        // enter infinite loop
        while (true) {
          // Wait until the next check point time is reached.
          // An early exit must be triggered by a call to this->interrupt(), which
          // will produce an exception during the sleep.
          boost::thread::sleep(next_check);
          if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
            log << MSG::VERBOSE << "Time-out reached (" << next_check << ")" << endmsg;

          boost::mutex::scoped_lock busy_lock(m_owner->m_busy);

          if ( last_access == m_owner->lastAccess() ) { // no further accesses

            if ( m_owner->database()->isOpen() ) { // close the database
              log << MSG::INFO << "Disconnect from database after being idle for "
              << m_owner->m_connectionTimeOut.total_seconds()
              << "s (will reconnect if needed)"<< endmsg;
              m_owner->database()->closeDatabase();
              // reset the latest heart beat because it may be different the next time
              // we connect to the DB
              if (!m_owner->m_heartBeatCondition.empty()) m_owner->m_latestHeartBeat = 0;
            }

            // schedule the next check for now + dt (seems a good estimate)
            next_check = boost::get_system_time() + m_owner->m_connectionTimeOut;

          } else {

            if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
              log << MSG::VERBOSE << "Wait more" << endmsg;

            // schedule the next check for last_access + dt
            next_check = last_access = m_owner->lastAccess();
            next_check += m_owner->m_connectionTimeOut;
          }
        }
      }
      // Ignore the exception since it is used only to exit from the loop.
      catch (boost::thread_interrupted&) {}
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
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
      // If the database has not been instantiated yet, we may be using
      // lazy connection and we have to connect now.
      if (!m_owner->m_db) {
        // we have to release the lock because i_initializeConnection
        // needs to lock the DB
        busy_lock.unlock();
        StatusCode sc = m_owner->i_initializeConnection();
        busy_lock.lock();
        if (! sc.isSuccess())
          throw GaudiException("Cannot initialize connection",
                               "DataBaseOperationLock::DataBaseOperationLock",
                               sc);
      }
      if (!m_owner->m_db->isOpen()){
        log << MSG::INFO << "Connecting to database" << endmsg;
        m_owner->m_db->openDatabase(); // ensure that the db is open
        m_owner->i_startTimeoutChecker(); // ensure that the timeout checker is running
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
