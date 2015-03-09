
//-----------------------------------------------------------------------------
/** @file RichToolRegistry.h
 *
 *  Header file for tool : Rich::ToolRegistry
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTOOLREGISTRY_H
#define RICHKERNEL_RICHTOOLREGISTRY_H 1

// STL
#include <map>
#include <string>
#include <sstream>

// base class and interface
#include "GaudiAlg/GaudiTool.h"
#include "RichKernel/IRichToolRegistry.h"

// Kernel
#include "RichKernel/RichHashMap.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class ToolRegistry RichToolRegistry.h
   *
   *  Tool providing a mapping between tool "nicknames" and types
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   15/03/2002
   */
  //-----------------------------------------------------------------------------

  class ToolRegistry : public GaudiTool,
                       virtual public IToolRegistry
  {

  private: // definitions

    /// typedef of container of strings for job options
    typedef std::vector<std::string> ToolList;

    /// typedef for the mapping between nicknames and class names
    typedef Rich::HashMap< std::string, std::string > RichToolMap;

  public: // for Gaudi framework

    /// Standard constructor
    ToolRegistry( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

    /// Destructor
    virtual ~ToolRegistry() {}

    // Initialization of the tool after creation
    StatusCode initialize();

  public: // methods (and doxygen comments) inherited from interface

    // Converts a tool nickname into a particular class name
    const std::string & toolType( const std::string & nickname ) const;

    // Converts a tool "nickname" into a particular instance name
    const std::string toolName( const std::string & nickname ) const;

  private: // methods

    /// Set up the tool mappings
    void setUpTools( const ToolList & toolList );

    /// Adds a entry to the map between nicknames and class names
    void addEntry( const std::string & nickname, ///< tool nickname
                   const std::string & type      ///< tool class name
                   ) const;

  private: // data

    /// Tool data from job options
    ToolList m_names;

    /// The mapping between the tool name and type
    mutable RichToolMap m_myTools;

  };

}

#endif // RICHKERNEL_RICHTOOLREGISTRY_H
