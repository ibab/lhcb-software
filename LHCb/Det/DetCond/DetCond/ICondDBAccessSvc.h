// $Id: ICondDBAccessSvc.h,v 1.11 2006-04-25 17:20:19 marcocle Exp $
#ifndef DETCOND_ICONDDBACCESSSVC_H 
#define DETCOND_ICONDDBACCESSSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include <GaudiKernel/IInterface.h>

// from COOL
#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"

// Forward declarations
namespace Gaudi {
  class Time;
}
namespace coral {
  class AttributeList;
}
namespace cool {
   class ExtendedAttributeListSpecification;
}

static const InterfaceID IID_ICondDBAccessSvc ( "ICondDBAccessSvc", 1, 0 );

/** @class ICondDBAccessSvc ICondDBAccessSvc.h DetCond/ICondDBAccessSvc.h
 *  
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
 *
 *  CondDBAccessSvc can be operated with only an in-memory CondDB, setting both the options NoBD and useCache to true.  The memory
 *  database can be populated using cacheAddFolder and cacheAddObject (or their XML counter parts). The CondDB folders of the
 *  memory db are equivalent to COOL single-version folders (see COOL documentation for details).
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-01-11
 */
class ICondDBAccessSvc : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBAccessSvc; }

  // Utilities:

  /// Used to obtain direct access to the database.
  virtual cool::IDatabasePtr& database() = 0;

  /// Possible recognized node types.
  enum StorageType { FOLDERSET, XML, Native };
  /// Known types of leaf nodes (aka Folders).
  enum VersionMode { SINGLE, MULTI };
  
  /// Create a CondDB node in the hierarchy (Folder or FolderSet)
  virtual StatusCode createFolder(const std::string &path,
                                  const std::string &descr,
                                  StorageType storage = XML,
                                  VersionMode vers = MULTI) const = 0;
 
  /// Utility function that simplifies the storage of an XML string.
  virtual StatusCode storeXMLString(const std::string &path, const std::string &data,
                                    const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const = 0;

  /// Utility function that simplifies the storage of an XML string.
  /// (Useful for Python, the times are in seconds)
  virtual StatusCode storeXMLString(const std::string &path, const std::string &data,
                                    const double since_s, const double until_s, cool::ChannelId channel = 0) const = 0;
  
  /// Convert from Gaudi::Time class to cool::ValidityKey.
  virtual cool::ValidityKey timeToValKey(const Gaudi::Time &time) const = 0;
   
  /// Convert from cool::ValidityKey to Gaudi::Time class.
  virtual Gaudi::Time valKeyToTime(const cool::ValidityKey &key) const = 0;
  
  /// Return the currently set TAG to use.
  virtual const std::string &tag() const = 0;
  
  /// Set the TAG to use.
  virtual StatusCode setTag(const std::string &_tag) = 0;

  /// Tag the given folder with the given tag-name. If the requested folder is
  /// a folderset, the tag is applied to all the folders below it. (waiting for HVS)
  virtual StatusCode tagFolder(const std::string &path, const std::string &tagName,
                               const std::string &description = "", cool::ChannelId channel = 0) = 0;

  /// Retrieve data from the condition database.
  /// Returns a shared pointer to an attribute list, the folder description and the IOV limits.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                boost::shared_ptr<coral::AttributeList> &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0) = 0;

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names) = 0;

  /// Add a folder to the cache (bypass the DB)
  virtual StatusCode cacheAddFolder(const std::string &path, const std::string &descr,
                                    const cool::ExtendedAttributeListSpecification& spec) = 0;
  
  /// Add a folder-set to the cache (bypass the DB)
  virtual StatusCode cacheAddFolderSet(const std::string &path, const std::string &descr) = 0;
  
  /// Add an XML folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path) = 0;

  /// Add an object to the cache (bypass the DB)
  virtual StatusCode cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                    const coral::AttributeList& payload, cool::ChannelId channel = 0) = 0;
  
  /// Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                       const std::string& data, cool::ChannelId channel = 0) = 0;

  /// Dump the cache (debug)
  virtual void dumpCache() const = 0;
  
protected:

private:

};
#endif // DETCOND_ICONDDBACCESSSVC_H
