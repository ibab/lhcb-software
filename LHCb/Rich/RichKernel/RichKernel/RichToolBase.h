// $Id: RichToolBase.h,v 1.1 2004-06-17 12:00:47 cattanem Exp $
#ifndef RICHUTILS_RICHTOOLBASE_H
#define RICHUTILS_RICHTOOLBASE_H 1

// GaudiTools
#include "GaudiTools/GaudiTool.h"

// Interfaces
#include "RichKernel/IRichToolRegistry.h"

/** @class RichToolBase RichToolBase.h RichUtils/RichToolBase.h
 *
 *  Abstract base class for RICH tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */

class RichToolBase : public GaudiTool {

public:

  /// Standard constructor
  RichToolBase( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  /// Destructor
  virtual ~RichToolBase() {};

  /// Initialize method
  virtual StatusCode initialize();

  /// Finalize method
  virtual StatusCode finalize();

protected:   // Protected methods

  /// Returns pointer to RICH tool registry tool
  inline const IRichToolRegistry * toolRegistry() const
  {
    if (!m_toolReg) m_toolReg = tool<IRichToolRegistry>("RichToolRegistry",m_regName);
    return m_toolReg;
  }

  /// Acquires a tool of the correct type from the RichToolRegistry for a given name
  template <typename TOOL> inline
  TOOL* acquireTool( const std::string & tName, 
                     TOOL*& pTool,
                     const IInterface * parent = 0 ) const {
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << " Acquiring tool '" << tName 
              << "' of type '" << toolRegistry()->toolType(tName) << "'" << endreq;
    }
    return pTool = tool<TOOL>( toolRegistry()->toolType(tName),tName,parent );
  }

  /// Release a tool
  template <typename TOOL> inline
  void releaseTool( TOOL *& pTool ) const {
    if ( pTool ) {
      debug() << " Forced release for tool '" << pTool->name() << "'" << endreq;
      release( pTool );
      pTool = NULL;
    }
  }

private:   // Private data

  /// Pointer to tool registry
  mutable IRichToolRegistry * m_toolReg;

  /// Runtime name for RichToolRegistry
  std::string m_regName;

};

#endif // RICHUTILS_RICHTOOLBASE_H
