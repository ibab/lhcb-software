// $Id: RichToolRegistry.h,v 1.1 2003-06-30 15:47:06 jonrob Exp $
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
 *  Tool providing a sigle point of definition for all tool
 *  names and types.
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

  /// Method returning a pointer to  a particular tool
  IAlgTool * acquireTool( std::string name );

  /// Method to inform that a tool is no longer required
  void releaseTool( std::string name );

private: // private methods

  void releaseTool( IAlgTool *& tool );

private: // private data

  // Tool data from job options
  typedef std::vector<std::string> ToolList;
  ToolList m_names;

  typedef std::pair< std::string, IAlgTool* > RichToolPair;
  typedef std::map< std::string, RichToolPair > RichToolMap;
  RichToolMap m_tools;

  typedef std::map< std::string, int > ToolCount;
  ToolCount m_refC;

};

inline void RichToolRegistry::releaseTool( IAlgTool *& tool )
{
  if (tool) { toolSvc()->releaseTool(tool); tool=NULL; }
}

#endif // RICHRECTOOLS_RICHTOOLREGISTRY_H
