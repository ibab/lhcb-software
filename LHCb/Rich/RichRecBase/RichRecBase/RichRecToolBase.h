// $Id: RichRecToolBase.h,v 1.2 2003-06-30 15:11:57 jonrob Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

#include <map>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// Interfaces
#include "RichRecBase/IRichToolRegistry.h"

/** @class RichRecToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */

class RichRecToolBase : public AlgTool {

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase() {};

  /// Initialize method
  virtual StatusCode initialize();

  /// Finalize method
  virtual StatusCode finalize();

protected:   // Protected methods

  /// Test printout level
  bool msgLevel( int mLevel ); 

  /// Return message level setting
  int msgLevel();

  /// Acquire a tool from the RichToolRegistry
  template <typename TOOL> inline
  TOOL* acquireTool( std::string name, TOOL*& pTool) {
    m_toolList[name] = true;
    return (pTool=dynamic_cast<TOOL*>(m_toolReg->acquireTool(name)));
  }
  
  /// Release a tool
  void releaseTool( std::string name );
  
private:   // Private data

  /// Pointer to tool registry
  IRichToolRegistry * m_toolReg;

  /// Message service printout level
  int m_msgLevel;

  /// Vector of tool names currently in use
  typedef std::map<std::string,bool> ToolList;
  ToolList m_toolList;

};

inline int RichRecToolBase::msgLevel()
{
  return m_msgLevel;
}

inline bool RichRecToolBase::msgLevel( int mLevel ) 
{ 
  return ( m_msgLevel && m_msgLevel <= mLevel ); 
}

inline void RichRecToolBase::releaseTool( std::string name ) {
  m_toolList[name] = false;
  m_toolReg->releaseTool(name);
}

#endif // RICHRECALGS_RICHRECTOOLBASE_H
