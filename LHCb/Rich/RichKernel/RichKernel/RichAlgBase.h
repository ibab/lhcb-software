// $Id: RichAlgBase.h,v 1.1 2004-06-17 12:00:47 cattanem Exp $
#ifndef RICHUTILS_RICHALGBASE_H
#define RICHUTILS_RICHALGBASE_H 1

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// Interfaces
#include "RichKernel/IRichToolRegistry.h"

/** @class RichAlgBase RichAlgBase.h RichUtils/RichAlgBase.h
 *
 *  Abstract base class for RICH algorithms providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

class RichAlgBase : public GaudiAlgorithm {

public:

  /// Standard constructor
  RichAlgBase( const std::string& name,
               ISvcLocator* pSvcLocator );

  virtual ~RichAlgBase() = 0;   ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode finalize  (); ///< Algorithm finalization

protected:  // Protected methods

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

#endif // RICHUTILS_RICHALGBASE_H
