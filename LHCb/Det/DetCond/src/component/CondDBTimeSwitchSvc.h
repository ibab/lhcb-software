#ifndef COMPONENT_CONDDBTIMESWITCHSVC_H
#define COMPONENT_CONDDBTIMESWITCHSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Map.h"
#include "DetCond/ICondDBReader.h"
#include <vector>
#include <map>

template <class TYPE> class SvcFactory;
class IDetDataSvc;

/** @class CondDBTimeSwitchSvc CondDBTimeSwitchSvc.h component/CondDBTimeSwitchSvc.h
 *
 *
 *
 *  @author Marco Clemencic
 *  @date   2006-07-10
 */
class CondDBTimeSwitchSvc: public extends1<Service, ICondDBReader> {
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

  // --------- ICondDBInfo implementation

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const;


protected:
  /// Standard constructor
  CondDBTimeSwitchSvc( const std::string& name, ISvcLocator* svcloc );

  virtual ~CondDBTimeSwitchSvc( ); ///< Destructor


private:

  /// Internal class to record the readers
  struct ReaderInfo {
    /// CondDBReader instance name ("type/name")
    std::string name;
    /// Boundaries of the Interval Of Validity
    Gaudi::Time since, until;
    /// Default Constructor
    ReaderInfo(const std::string &_n,
        Gaudi::Time _s = Gaudi::Time::epoch(),
        Gaudi::Time _u = Gaudi::Time::max()):
      name(_n),
      since(_s),
      until(_u),
      m_reader(0)
    {}
    /// Default Constructor
    ReaderInfo(const std::string &_n, Gaudi::Time::ValueType _s, Gaudi::Time::ValueType _u):
      name(_n),
      since(_s),
      until(_u),
      m_reader(0)
    {}
    /// Copy constructor (for a correct reference counting).
    ReaderInfo(const ReaderInfo &_ri):
      name(_ri.name),
      since(_ri.since),
      until(_ri.until),
      m_reader(_ri.m_reader)
    {
      if (m_reader) m_reader->addRef();
    }
    /// Destructor (releases the CondDBReader service).
    ~ReaderInfo(){
      if (m_reader) m_reader->release();
    }
    /// Shortcut to check the validity of the reader
    bool isValidAt(const Gaudi::Time &when) const {
      return since <= when && until > when;
    }
    /// Shortcut to retrieve the pointer to the reader service.
    ICondDBReader *reader(ISvcLocator *svcloc) const {
      if (!m_reader) {
        if (!svcloc)
          throw GaudiException("ServiceLocator pointer is NULL",
              "CondDBTimeSwitchSvc::ReaderInfo::get",StatusCode::FAILURE);
        StatusCode sc = svcloc->service(name,m_reader,true);
        if (sc.isFailure())
          throw GaudiException("Cannot get ICondDBReader '"+name+"'",
              "CondDBTimeSwitchSvc::ReaderInfo::get",sc);
      }
      return m_reader;
    }
    /// Shortcut to restrict and IOV to the boundaries defined for the reader
    void cutIOV(Gaudi::Time &_since, Gaudi::Time &_until) const {
      if ( since > _since ) _since = since;
      if ( until < _until ) _until = until;
    }
    /// convert to a string
    std::string toString() const;
  private:
    /// Pointer to the CondDBReader instance
    mutable ICondDBReader *m_reader;
  };

  /// Get the the CondDBReader valid for a given point in time.
  /// Returns 0 if no service is available.
  /// The boolean flag is used to avoid messages (during initialization).
  ReaderInfo *readerFor(const Gaudi::Time &when, bool quiet = false);

  /// Get the the CondDBReader valid for a given point in time.
  /// Returns 0 if no service is available
  ReaderInfo *currentReader();

  /// Get current event time from the detector data svc.
  Gaudi::Time getTime();

  // -------------------- Data Members
  typedef std::vector<std::string> ReadersDeclatationsType;
  typedef GaudiUtils::Map<Gaudi::Time,ReaderInfo> ReadersType;

  /// Property CondDBTimeSwitchSvc.Readers: list of ICondDBReaders to be used
  /// for given intervals of validity. The format is "'Reader': (since, until)",
  /// where since and until are doubles defining the time in standard units.
  ReadersDeclatationsType m_readersDeclatations;

  /// Container for the alternatives. The ReaderInfo objects are indexed by
  /// "until" to allow efficient search with map::upper_bound.
  ReadersType m_readers;

  /// Pointer used to cache the latest requested reader.
  /// It allows to avoid the search.
  ReaderInfo *m_latestReaderRequested;

  /// Pointer to the detector data service, used to get the event time in
  /// the methods that do not require it as argument.
  /// It used only if there was not a previous request with the time.
  IDetDataSvc *m_dds;

  /// Enable/disable direct mapping from the database structure to the transient
  /// store using XML persistency format (enabled by default).
  bool m_xmlDirectMapping;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBTimeSwitchSvc>;

  /// Internal implementation helper to generalize the channel type.
  template <typename Channel>
  IOVList i_getIOVs (const std::string &path, const IOV &iov, const Channel &channel);
};
#endif // COMPONENT_CONDDBDISPATCHERSVC_H
