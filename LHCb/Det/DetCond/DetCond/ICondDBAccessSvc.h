// $Id: ICondDBAccessSvc.h,v 1.4 2005-04-25 10:38:35 marcocle Exp $
#ifndef DETCOND_ICONDDBACCESSSVC_H 
#define DETCOND_ICONDDBACCESSSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include <GaudiKernel/IInterface.h>

// from COOL
#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"

// Forward declarations
class ITime;
class TimePoint;

static const InterfaceID IID_ICondDBAccessSvc ( "ICondDBAccessSvc", 1, 0 );

/** @class ICondDBAccessSvc ICondDBAccessSvc.h DetCond/ICondDBAccessSvc.h
 *  
 *  Class used as interface to LCG COOL library API. It should expose as less as
 *  possible COOL internal details.
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
                                    const ITime &since, const ITime &till) const = 0;

  /// Convert from TimePoint class to cool::ValidityKey.
  virtual cool::ValidityKey timeToValKey(const TimePoint &time) const = 0;
   
  /// Convert from cool::ValidityKey to TimePoint class.
  virtual TimePoint valKeyToTime(const cool::ValidityKey &key) const = 0;
  
  /// Return the currently set TAG to use.
  virtual const std::string &tag() const = 0;
  
  /// Set the TAG to use.
  virtual StatusCode setTag(const std::string &_tag) = 0;

  /// Tag the given folder with the given tag-name. If the requested folder is
  /// a folderset, the tag is applied to all the folders below it. (waiting for HVS)
  virtual StatusCode tagFolder(const std::string &path, const std::string &tagName,
                               const std::string &description = "") = 0;

protected:

private:

};
#endif // DETCOND_ICONDDBACCESSSVC_H
