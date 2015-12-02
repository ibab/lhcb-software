#ifndef ICONFIGTREEEDITOR_H 
#define ICONFIGTREEEDITOR_H 1

// Include files
#include <string>
#include <utility>
#include <map>
#include "GaudiKernel/INamedInterface.h"
#include "Kernel/ConfigTreeNode.h"


/** @class IConfigTreeEditor IConfigTreeEditor.h
 *  
 *  functionality:
 *         Interface for obtaining an
 *         individual configuration description
 *         or (an element of) the graph structure 
 *         given its ID
 *
 *         The ID corresponds to the MD5 digest of the contents
 *         of the configration description
 *
 *  @author Gerhard Raven
 *  @date   2007-12-12
 */


class IConfigTreeEditor : virtual public extend_interfaces<INamedInterface>  {
public:
  DeclareInterfaceID(IConfigTreeEditor,4,0);
  virtual ~IConfigTreeEditor() = default;

  // given a configuration tree identified by 'in', apply the changes 
  // defined by 'updates' (which is a multimap of 'component' -> 'key','value'),
  // and return a new configuration tree
  // Note that the new tree is assumed to have been written to the underlying
  // storage...
  // Failure is indicated by returning an 'invalid' digest.
  virtual ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                                     const std::multimap<std::string,
                                                                 std::pair<std::string,std::string> >& updates,
                                                     const std::string& label) const = 0;

};
#endif // ICONFIGTREEEDITOR_H
