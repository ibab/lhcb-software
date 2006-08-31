// $Id: ICondDBAccessSvc.h,v 1.14 2006-08-31 13:53:02 marcocle Exp $
#ifndef DETCOND_ICONDDBACCESSSVC_H 
#define DETCOND_ICONDDBACCESSSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <set>
#include <map>

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

  /// Used to obtain direct access to the database.
  virtual cool::IDatabasePtr& database() = 0;
  
  /// Convert from Gaudi::Time class to cool::ValidityKey.
  virtual cool::ValidityKey timeToValKey(const Gaudi::Time &time) const = 0;
   
  /// Convert from cool::ValidityKey to Gaudi::Time class.
  virtual Gaudi::Time valKeyToTime(const cool::ValidityKey &key) const = 0;
  
  /// Return the currently set TAG to use.
  virtual const std::string &tag() const = 0;
  
  /// Set the TAG to use.
  virtual StatusCode setTag(const std::string &_tag) = 0;

  /// Add a folder to the cache (bypass the DB)
  virtual StatusCode cacheAddFolder(const std::string &path, const std::string &descr,
                                    const cool::ExtendedAttributeListSpecification& spec) = 0;
  
  /// Add a folder-set to the cache (bypass the DB)
  virtual StatusCode cacheAddFolderSet(const std::string &path, const std::string &descr) = 0;
  
  /// Add an XML folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path) = 0;

  /// Add an XML folder to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLFolder(const std::string &path, const std::set<std::string> &fields) = 0;

  /// Add an object to the cache (bypass the DB)
  virtual StatusCode cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                    const coral::AttributeList &payload, cool::ChannelId channel = 0) = 0;
  
  /// Deprecated: use ICondDBAccessSvc::cacheAddXMLData instead
  inline StatusCode cacheAddXMLObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                      const std::string &data, cool::ChannelId channel = 0)
  {
    return cacheAddXMLData(path, since, until, data, channel);
  }
  

  /// Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                     const std::string &data, cool::ChannelId channel = 0) = 0;

  /// Add an XML object to the cache (bypass the DB)
  virtual StatusCode cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                     const std::map<std::string,std::string> &data, cool::ChannelId channel = 0) = 0;

  /// Dump the cache (debug)
  virtual void dumpCache() const = 0;

protected:

private:

};
#endif // DETCOND_ICONDDBACCESSSVC_H
