// $Id: ICondDBReader.h,v 1.1 2006-07-11 18:25:16 marcocle Exp $
#ifndef DETCOND_ICONDDBREADER_H 
#define DETCOND_ICONDDBREADER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

// from COOL
#include "CoolKernel/types.h"

// Forward declarations
namespace Gaudi {
  class Time;
}
namespace coral {
  class AttributeList;
}

static const InterfaceID IID_ICondDBReader ( "ICondDBReader", 1, 0 );

/** @class ICondDBReader ICondDBReader.h DetCond/ICondDBReader.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2006-07-10
 */
class ICondDBReader : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBReader; }

  /// Retrieve data from the condition database.
  /// Returns a shared pointer to an attribute list, the folder description and the IOV limits.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                boost::shared_ptr<coral::AttributeList> &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0) = 0;

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names) = 0;

protected:

private:

};
#endif // DETCOND_ICONDDBREADER_H
