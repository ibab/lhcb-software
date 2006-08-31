// $Id: ICondDBEditor.h,v 1.3 2006-08-31 13:53:02 marcocle Exp $
#ifndef DETCOND_ICONDDBEDITOR_H 
#define DETCOND_ICONDDBEDITOR_H 1

// Include files
// from STL
#include <string>
#include <map>
#include <set>

// from Gaudi
#include "GaudiKernel/IInterface.h"

// from COOL
#include "CoolKernel/types.h"

// Forward declarations
namespace cool {
   class ExtendedAttributeListSpecification;
}

static const InterfaceID IID_ICondDBEditor ( "ICondDBEditor", 1, 0 );

/** @class ICondDBEditor ICondDBEditor.h DetCond/ICondDBEditor.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2006-07-10
 */
class ICondDBEditor : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBEditor; }

  /// Possible recognized node types.
  enum StorageType { FOLDERSET, XML, Native };
  /// Known types of leaf nodes (aka Folders).
  enum VersionMode { SINGLE, MULTI };

  /// Create a CondDB node in the hierarchy (Folder or FolderSet).
  virtual StatusCode createNode(const std::string &path,
                                const std::string &descr,
                                StorageType storage = XML,
                                VersionMode vers = MULTI) const = 0;
 
  /// Create a CondDB node in the hierarchy (Folder or FolderSet).
  virtual StatusCode createNode(const std::string &path,
                                const std::string &descr,
                                const std::set<std::string> &fields,
                                StorageType storage = XML,
                                VersionMode vers = MULTI) const = 0;
 
  /// Deprecated: use ICondDBEditor::storeXMLData instead.
  inline StatusCode storeXMLString(const std::string &path, const std::string &data,
                                   const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const
  {
    return storeXMLData(path, data, since, until, channel);
  }
  
  /// Utility function that simplifies the storage of an XML string.
  virtual StatusCode storeXMLData(const std::string &path, const std::string &data,
                                  const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const = 0;
  
  /// Utility function that simplifies the storage of a set of XML strings.
  virtual StatusCode storeXMLData(const std::string &path, const std::map<std::string,std::string> &data,
                                  const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel = 0) const = 0;

  /// Tag the given leaf node with the given tag-name.
  virtual StatusCode tagLeafNode(const std::string &path, const std::string &tagName,
                                 const std::string &description = "") = 0;

  /// Tag the given middle node with the given tag-name, recursively tagging the head
  /// of child nodes with automatically generated tag-names.
  virtual StatusCode recursiveTag(const std::string &path, const std::string &tagName,
                                  const std::string &description = "") = 0;
  
protected:

private:

};
#endif // DETCOND_ICONDDBEDITOR_H
