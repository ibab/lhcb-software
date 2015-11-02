#ifndef COMPONENT_CONDDBLOGGER_H
#define COMPONENT_CONDDBLOGGER_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBReader.h"

template <class TYPE> class SvcFactory;

/** @class CondDBLogger CondDBLogger.h component/CondDBLogger.h
 *
 *  Logger of acesses to CondDB.
 *
 *  CondDBLogger is a simple class that allow to store in a file all the
 *  requests made to a ICondDBReader instance.  It has to be used as a front-end
 *  to the instance we want to monitor.
 *
 *  Given the following option snippet
 *  @code
 *  MyCondDBUser.Reader = "ACondDBReader";
 *  @endcode
 *  the CondDBLogger can be enabled with
 *  @code
 *  CondDBLogger.LoggedReader = "ACondDBReader";
 *  MyCondDBUser.Reader = "CondDBLogger";
 *  @endcode
 *  or in python options
 *  @code
 *  user = MyCondDBUser()
 *  user.Reader = CondDBLogger(LoggedReader = user.Reader)
 *  @endcode
 *
 *  The format of the log file is very simple.  Each line starts with an
 *  operation code then the time of the operation in ns (as returned by
 *  Gaudi::Time::ns()).  The rest of the line depend on the operation:
 *  - "INI:"
 *    - Initialization of the logger
 *  - "TAG:"
 *    - Request of the used tag
 *  - "GCH:"
 *    - Retrieve the child nodes of a folderset.
 *  - "GET:"
 *    - Request of an object from the database, the format is<br>
 *      &lt;path&gt; &lt;channel&nbsp;id&gt; &lt;path&gt; &lt;evt.time&gt; &lt;status&gt;
 *  - "GCN:"
 *    - Request of an object from the database using the channel name, the format is<br>
 *      &lt;path&gt; &lt;channel&nbsp;name&gt; &lt;path&gt; &lt;evt.time&gt; &lt;status&gt;
 *  - "FIN:"
 *    - Finalization of the logger
 *  - "IOV:"
 *    - Request list of IOVs (using numeric channel)
 *  - "ICN:"
 *    - Request list of IOVs (using channel name)
 *
 *  @param LoggedReader
 *         Fully qualified name of the ICondDBReader to which the calls have to
 *         be forwarded.
 *  @param LogFile
 *         Path to the log file (it is overwritten if it exists).  If not
 *         specified or set to empty, the file name is set from the name of the
 *         instance plus '.log'.
 *
 *  @author Marco CLEMENCIC
 *  @date   2008-01-24
 */
class CondDBLogger: public extends1<Service, ICondDBReader> {
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

  /// Disconnect from the database.
  virtual void disconnect();

  // --------- ICondDBInfo implementation

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const;

protected:

  /// Standard constructor
  CondDBLogger( const std::string& name, ISvcLocator* svcloc );

  virtual ~CondDBLogger( ); ///< Destructor

private:

  // -------------------- Data Members

  /// Pointer to the CondDBReader whose activity has to be logged.
  ICondDBReader *m_loggedReader;

  /// Name of the CondDBReader whose activity has to be logged.
  std::string m_loggedReaderName;

  /// Path to the file that will contain the log.
  std::unique_ptr<std::ostream> m_logFile;

  /// Path to the file that will contain the log.
  std::string m_logFileName;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBLogger>;

};
#endif // COMPONENT_CONDDBLAYERINGSVC_H
