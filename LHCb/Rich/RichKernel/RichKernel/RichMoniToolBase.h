
//============================================================================
/** @file RichMoniToolBase.h
 *
 *  Header file for Tool base class : RichMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichMoniToolBase.h,v 1.3 2005-02-02 09:59:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//============================================================================

#ifndef RICHKERNEL_RICHMONITOOLBASE_H
#define RICHKERNEL_RICHMONITOOLBASE_H 1

// GaudiTools
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiAlg/TupleObj.h" // Should be included from above file !!

// Interfaces
#include "RichKernel/IRichToolRegistry.h"

/** @class RichMoniToolBase RichMoniToolBase.h RichKernel/RichMoniToolBase.h
 *
 *  Abstract base class for RICH tools providing
 *  some basic functionality (same as RichToolBase) together with
 *  the histogramming and ntuple functionality provided by GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-13
 */

class RichMoniToolBase : public GaudiTupleTool {

public:

  /// Standard constructor
  RichMoniToolBase( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichMoniToolBase() {};

  /** Initialization of the tool after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** Finalization of the tool before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

private:   // private methods

  /** Returns pointer to RICH tool registry tool
   *  Used internally by base class to convert tool nicknames
   *  in the appropriate class name
   *
   *  @return Pointer to the IRichToolRegistry interface
   */
  inline const IRichToolRegistry * toolRegistry() const
  {
    if (!m_toolReg) m_toolReg = tool<IRichToolRegistry>("RichToolRegistry",m_regName);
    return m_toolReg;
  }

protected:   // Protected methods

  /** Returns a pointer to the tool associated to a given nickname
   *  Uses the RichToolRegistry tool to convert tool nicknames
   *  in the appropriate class name
   *
   *  @param tName   The nickname of the requested tool
   *  @param pTool   Returned pointer to the requested tool
   *  @param parent  Pointer to parent (used to access private tools)
   *
   *  @return Pointer to the tool associated to the given nickname
   */
  template <typename TOOL> inline
  TOOL* acquireTool( const std::string & tName,
                     TOOL*& pTool,
                     const IInterface * parent = 0 ) const
  {
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << " Acquiring tool '" << tName
              << "' of type '" << toolRegistry()->toolType(tName) << "'" << endreq;
    }
    return pTool = tool<TOOL>( toolRegistry()->toolType(tName),tName,parent );
  }

  /** Forced release of a particular tool
   *  Tools are automatically released during finalisation, so this method
   *  only need be used to release a tool early, before finalisation.
   *
   *  @param pTool  Pointer to the tool to be released
   */
  template <typename TOOL> inline
  void releaseTool( TOOL *& pTool ) const
  {
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

#endif // RICHKERNEL_RICHMONITOOLBASE_H
