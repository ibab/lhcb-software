
//-----------------------------------------------------------------------------
/** @file RichToolRegistry.h
 *
 *  Header file for tool : RichToolRegistry
 *
 *  CVS Log :-
 *  $Id: RichToolRegistry.h,v 1.7 2005-02-02 10:12:10 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTOOLREGISTRY_H
#define RICHTOOLS_RICHTOOLREGISTRY_H 1

// STL
#include <map>
#include <string>

// base class and interface
#include "GaudiAlg/GaudiTool.h"
#include "RichKernel/IRichToolRegistry.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Kernel
#include "RichKernel/RichHashMap.h"
#include "RichKernel/StringHashFuncs.h"

//-----------------------------------------------------------------------------
/** @class RichToolRegistry RichToolRegistry.h
 *
 *  Tool providing a mapping between tool "nicknames" and types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

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
  //StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

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
  typedef RichHashMap< std::string, std::string > RichToolMap;
  /// The mapping between the tool name and type
  mutable RichToolMap m_myTools;

};

inline void RichToolRegistry::addEntry( const std::string & nickname,
                                        const std::string & type ) const
{
  debug() << " Tool nickname '" << nickname 
          << "' maps to type '" << type << "'" << endreq;
  m_myTools[nickname] = type;
}

#endif // RICHTOOLS_RICHTOOLREGISTRY_H
