#ifndef COMPONENT_CONDDBDISPATCHERSVC_H
#define COMPONENT_CONDDBDISPATCHERSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBReader.h"
#include <vector>
#include <map>

template <class TYPE> class SvcFactory;

/** @class CondDBDispatcherSvc CondDBDispatcherSvc.h component/CondDBDispatcherSvc.h
 *
 *
 *
 *  @author Marco Clemencic
 *  @date   2006-07-10
 */
class CondDBDispatcherSvc: public extends1<Service, ICondDBReader> {
public:
  /// Initialize COOL (CondDB) Access Layer Service
  virtual StatusCode initialize();
  /// Finalize Service
  virtual StatusCode finalize();

  /// Disconnect from the database.
  virtual void disconnect();

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
  CondDBDispatcherSvc( const std::string& name, ISvcLocator* svcloc );

  virtual ~CondDBDispatcherSvc( ); ///< Destructor


private:

  ICondDBReader *alternativeFor(const std::string &path) const;

  // -------------------- Data Members

  /// Property CondDBDispatcherSvc.MainAccessSvc: the AccessSvc instance to use to retrieve all the
  /// objects for which an alternative is not specified (default to "CondDBAccessSvc").
  std::string m_mainAccessSvcName;

  /// Property CondDBDispatcherSvc.Alternatives: list of alternative Access Services in the form of
  /// "/path/for/alternative":"ServiceType/ServiceName".
  std::map<std::string,std::string> m_alternativesDeclarationMap;

  /// Pointer to the main access service.
  ICondDBReader* m_mainDB;

  /// Container fo the alternatives.
  std::map<std::string,ICondDBReader*> m_alternatives;

  /// Enable/disable direct mapping from the database structure to the transient
  /// store using XML persistency format (enabled by default).
  bool m_xmlDirectMapping;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBDispatcherSvc>;

};
#endif // COMPONENT_CONDDBDISPATCHERSVC_H
