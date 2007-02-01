
//-----------------------------------------------------------------------------
/** @file RichToolRegistry.h
 *
 *  Header file for tool : Rich::ToolRegistry
 *
 *  CVS Log :-
 *  $Id: RichToolRegistry.h,v 1.13 2007-02-01 17:51:11 jonrob Exp $
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

// Kernel
#include "RichKernel/RichHashMap.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
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

  public: // for Gaudi framework

    /// Standard constructor
    ToolRegistry( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

    /// Destructor
    virtual ~ToolRegistry() {}

    // Initialization of the tool after creation
    StatusCode initialize();

    // Finalization of the tool before deletion
    //StatusCode finalize();

  public: // methods (and doxygen comments) inherited from interface

    // Converts a tool nickname into a particular class name
    const std::string & toolType( const std::string & nickname ) const;

    // Converts a tool "nickname" into a particular instance name
    const std::string toolName( const std::string & nickname ) const;

    // Returns the context for the tool registry
    const std::string getContext() const;

  private: // methods

    /// Adds a entry to the map between nicknames and class names
    void addEntry( const std::string & nickname, ///< tool nickname
                   const std::string & type      ///< tool class name
                   ) const;

  private: // data

    /// typedef of container of strings for job options
    typedef std::vector<std::string> ToolList;
    /// Tool data from job options
    ToolList m_names;

    /// typedef for the mapping between nicknames and class names
    typedef Rich::HashMap< std::string, std::string > RichToolMap;
    /// The mapping between the tool name and type
    mutable RichToolMap m_myTools;

  };

}

#endif // RICHTOOLS_RICHTOOLREGISTRY_H
