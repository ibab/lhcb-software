// $Id: RichToolRegistry.h,v 1.3 2003-08-06 11:08:14 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTOOLREGISTRY_H
#define RICHRECTOOLS_RICHTOOLREGISTRY_H 1

#include <map>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Interfaces
#include "RichRecBase/IRichToolRegistry.h"

/** @class RichToolRegistry RichToolRegistry.h
 *
 *  Tool providing a mapping between tool names and types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichToolRegistry : public AlgTool,
                         virtual public IRichToolRegistry {

public:

  /// Standard constructor
  RichToolRegistry( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichToolRegistry(){}

  StatusCode initialize();  ///< Initialize method
  StatusCode finalize();    ///< Finalize method

  /// Method to return the tool type from the name
  std::string toolType( std::string name );

private: // data

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  typedef std::map< std::string, std::string > RichToolMap;
  /// The mapping between the tool name and type
  RichToolMap m_tools;

};

#endif // RICHRECTOOLS_RICHTOOLREGISTRY_H
