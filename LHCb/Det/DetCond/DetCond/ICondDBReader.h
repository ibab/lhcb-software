#ifndef DETCOND_ICONDDBREADER_H
#define DETCOND_ICONDDBREADER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Time.h"

// from LHCb
#include "Kernel/ICondDBInfo.h"

// from COOL
#include "CoolKernel/types.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/ChannelId.h"

// Forward declarations
namespace cool {
  class IRecord;
}

/** @class ICondDBReader ICondDBReader.h DetCond/ICondDBReader.h
 *
 *  Interface to retrieve data from the conditions database.
 *
 *  @author Marco Clemencic
 *  @date   2006-07-10
 */
class ICondDBReader : virtual public ICondDBInfo {
public:
  /// InterfaceID
  DeclareInterfaceID(ICondDBReader, 2, 1);

  typedef boost::shared_ptr<const cool::IRecord> DataPtr;

  /// Helper class to easily manage an interval of validity as a pair of Gaudi::Time
  /// instances.
  struct IOV {
    /// Constructor
    IOV(Gaudi::Time _since = Gaudi::Time::epoch(),
        Gaudi::Time _until = Gaudi::Time::max()):
          since(_since), until(_until)
    {}
    /// Boundaries of the interval.
    Gaudi::Time since, until;
    /// Define a simple order between two IOV instances.
    inline bool operator<(const IOV& rhs) const {
      return since < rhs.since;
    }
  };

  // /// List of IOV instances.
  // //typedef std::vector<IOV> IOVList;
  /// List of IOV instances.
  /// Note: it is not recommended to inherit from a STL class because of the
  /// absence of a virtual destructor, but in this case it is save because we do
  /// not add data members.
  class IOVList: public std::vector<IOV> {
  public:
    /// Find the intervals in passed interval that are not covered by this list.
    inline IOVList find_holes(const IOV& iov) const;
  };

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

  /// @{
  /// Return the list of occupied IOVs in the given path and channel, for the given IOV.
  virtual IOVList getIOVs (const std::string &path, const IOV &iov, cool::ChannelId channel = 0) = 0;
  virtual IOVList getIOVs (const std::string &path, const IOV &iov, const std::string &channel) = 0;
  /// @}

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

ICondDBReader::IOVList ICondDBReader::IOVList::find_holes(const IOV& iov) const {
  IOVList result;

  Gaudi::Time last = iov.since; // keep track of the end of coverage
  // loop over covering interval
  for (const_iterator covered = begin(); covered != end(); ++covered) {
    if (covered->since > last) { // hole between the end of coverage and begin of next IOV
      result.push_back(IOV(last, covered->since));
    }
    last = covered->until; // prepare to look for the next hole
  }
  if (last < iov.until) {
    // we didn't get anything to cover until the end of the requested IOV
    result.push_back(IOV(last, iov.until));
  }

  return result;
}

#endif // DETCOND_ICONDDBREADER_H
