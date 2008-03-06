// $Id: IConfigTreeEditor.h,v 1.1 2008-03-06 15:11:30 graven Exp $
#ifndef ICONFIGTREEEDITOR_H 
#define ICONFIGTREEEDITOR_H 1

// Include files
#include <string>
#include <utility>
#include <map>
#include "GaudiKernel/INamedInterface.h"
#include "HltBase/ConfigTreeNode.h"


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


class IConfigTreeEditor : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IConfigTreeEditor();


  // given a configuration tree identified by 'in', print the
  // (hierarchical) list of Algorithms present in this tree
  virtual void printAlgorithms(const ConfigTreeNode::digest_type& in) const = 0;

  // given a configuration tree identified by 'in', print the
  // list of Services present in this tree
  virtual void printServices(const ConfigTreeNode::digest_type& in) const = 0;

  // given a configuration tree identified by 'in', print the
  // list of Tools present in this tree
  virtual void printTools(const ConfigTreeNode::digest_type& in) const = 0;

  virtual void printProperties(const ConfigTreeNode::digest_type& in, const std::string& comps,
                                                                      const std::string& props) const = 0;

  // given a configuration tree identified by 'in', apply the changes 
  // defined by 'updates' (which is a multimap of 'component' -> 'key','value'),
  // and return a new configuration tree
  // Note that the new tree is assumed to have been written to the underlying
  // storage...
  // Failure is indicated by returning an 'invalid' digest.
  virtual ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                                     const std::multimap<std::string,
                                                                 std::pair<std::string,std::string> >& updates) const = 0;

  virtual ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                                     const std::vector<std::string>& updates) const = 0;

};
#endif // ICONFIGTREEEDITOR_H
