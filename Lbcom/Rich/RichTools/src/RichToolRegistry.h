// $Id: RichToolRegistry.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHUTILS_RICHTOOLREGISTRY_H
#define RICHUTILS_RICHTOOLREGISTRY_H 1

#include <map>
#include <string>

// base class and interface
#include "GaudiTools/GaudiTool.h"
#include "RichKernel/IRichToolRegistry.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"


/** @class RichToolRegistry RichToolRegistry.h
 *
 *  Tool providing a mapping between tool names and types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichToolRegistry : public GaudiTool,
                         virtual public IRichToolRegistry {

public:

  /// Standard constructor
  RichToolRegistry( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichToolRegistry() {}

  StatusCode initialize();  ///< Initialize method
  StatusCode finalize();    ///< Finalize method

  /// Method to return the tool type from the name
  const std::string & toolType( const std::string & name ) const;

private: // methods

  /// Add an entry to the tool list
  void addEntry( const std::string & name, const std::string & type ) const;

private: // data

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  typedef std::map< std::string, std::string > RichToolMap;
  /// The mapping between the tool name and type
  mutable RichToolMap m_myTools;

};

inline void RichToolRegistry::addEntry( const std::string & name,  
                                        const std::string & type ) const
{
  debug() << " Tool name '" << name << "' maps to type '" << type << "'" << endreq;
  m_myTools[name] = type;
}

#endif // RICHUTILS_RICHTOOLREGISTRY_H
