// $Id: RichToolRegistry.h,v 1.2 2004-07-15 15:44:40 jonrob Exp $
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
 *  Tool providing a mapping between tool "nicknames" and types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichToolRegistry : public GaudiTool,
                         virtual public IRichToolRegistry {

public: // for Gaudi framework

  /// Standard constructor
  RichToolRegistry( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichToolRegistry() {}

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods inherited from interface

  // Converts a tool nickname into a particular class name
  const std::string & toolType( const std::string & nickname ) const;

private: // methods

  /** Adds a entry to the map between nicknames and class names
   */
  void addEntry( const std::string & nickname, ///< nickname
                 const std::string & type      ///< class name
                 ) const;

private: // data

  /// typedef of container of strings for job options
  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  /// typedef for the mapping between nicknames and class names
  typedef std::map< std::string, std::string > RichToolMap;
  /// The mapping between the tool name and type
  mutable RichToolMap m_myTools;

};

inline void RichToolRegistry::addEntry( const std::string & nickname,
                                        const std::string & type ) const
{
  debug() << " Tool name '" << nickname 
          << "' maps to type '" << type << "'" << endreq;
  m_myTools[nickname] = type;
}

#endif // RICHUTILS_RICHTOOLREGISTRY_H
