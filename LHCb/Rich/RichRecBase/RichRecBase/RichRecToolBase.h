// $Id: RichRecToolBase.h,v 1.6 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/GaudiException.h"

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

  /// Acquires a tool of the correct type from the RichToolRegistry for a given name
  template <typename TOOL> inline
  TOOL* acquireTool( std::string tName, TOOL*& pTool ) {
    if ( !m_toolReg ) throw GaudiException( "RichToolRegistry not initialised", 
                                            name(), StatusCode::FAILURE );
    if ( !toolSvc()->retrieveTool(m_toolReg->toolType(tName),tName,pTool) ) {
      pTool = NULL;
      throw GaudiException( "Unable to retrieve tool '" + tName +
                            "' of type '" + m_toolReg->toolType(tName) + "'" ,
                            name(), StatusCode::FAILURE );
    }
    return pTool;
  }
  
  /// Release a tool
  template <typename TOOL> inline
  void releaseTool( TOOL *& pTool ) {
    if ( pTool ) { toolSvc()->releaseTool( pTool ); pTool = NULL; }
  }

private:   // Private data

  /// Pointer to tool registry
  IRichToolRegistry * m_toolReg;

  /// Message service printout level
  int m_msgLevel;

  /// Runtime name for RichToolRegistry
  std::string m_regName;

};

inline int RichRecToolBase::msgLevel()
{
  return m_msgLevel;
}

inline bool RichRecToolBase::msgLevel( int mLevel )
{
  return ( m_msgLevel && m_msgLevel <= mLevel );
}

#endif // RICHRECALGS_RICHRECTOOLBASE_H
