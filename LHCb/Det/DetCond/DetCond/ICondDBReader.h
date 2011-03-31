#ifndef DETCOND_ICONDDBREADER_H
#define DETCOND_ICONDDBREADER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

// from LHCb
#include "Kernel/ICondDBInfo.h"

// from COOL
#include "CoolKernel/types.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/ChannelId.h"

// Forward declarations
namespace Gaudi {
  class Time;
}
namespace cool {
  class IRecord;
}

/** @class ICondDBReader ICondDBReader.h DetCond/ICondDBReader.h
 *
 *
 *  @author Marco Clemencic
 *  @date   2006-07-10
 */
class ICondDBReader : virtual public ICondDBInfo {
public:
  /// InterfaceID
  DeclareInterfaceID(ICondDBReader, 2, 0);

  typedef boost::shared_ptr<const cool::IRecord> DataPtr;

  /// Retrieve data from the condition database.
  /// Returns a shared pointer to an attribute list, the folder description and the IOV limits.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0) = 0;

  /// Retrieve data from the condition database.
  /// Returns a shared pointer to an attribute list, the folder description and the IOV limits.
  /// (Version with alphanumeric channel id)
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel) = 0;

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names) = 0;

  /// Retrieve the names of the children nodes of a FolderSet divided in folders and foldersets.
  virtual StatusCode getChildNodes (const std::string &path,
                                    std::vector<std::string> &folders,
                                    std::vector<std::string> &foldersets) = 0;

  /// Tells if the path is available in the database.
  virtual bool exists(const std::string &path) = 0;

  /// Tells if the path (if it exists) is a folder.
  virtual bool isFolder(const std::string &path) = 0;

  /// Tells if the path (if it exists) is a folderset.
  virtual bool isFolderSet(const std::string &path) = 0;

protected:

private:

};
#endif // DETCOND_ICONDDBREADER_H
