
//-----------------------------------------------------------------------------
/** @file RichMoniAlgBase.h
 *
 *  Header file for algorithm base class : RichMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichMoniAlgBase.h,v 1.4 2005-02-02 09:59:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   19/12/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMONIALGBASE_H
#define RICHKERNEL_RICHMONIALGBASE_H 1

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/TupleObj.h" // Should be included from above file !!

// Interfaces
#include "RichKernel/IRichToolRegistry.h"

/** @class RichMoniAlgBase RichMoniAlgBase.h RichKernel/RichMoniAlgBase.h
 *
 *  Abstract base class for RICH algorithms providing
 *  some basic functionality (same as RichAlgBase) together with
 *  the histogramming and ntuple functionality provided by GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   19/12/2004
 *
 *  @todo Merge common functionality in RichAlgBase and RichMoniAlgBase
 */

class RichMoniAlgBase : public GaudiTupleAlg {

public:

  /// Standard constructor
  RichMoniAlgBase( const std::string& name,
                   ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichMoniAlgBase() = 0;

  /** Initialization of the algorithm after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** The main event processing method. Called once for each event
   *
   * @return The status of the event processing
   * @retval StatusCode::SUCCESS Event processing was successful
   * @retval StatusCode::FAILURE Event processing failed
   */
  virtual StatusCode execute();

  /** Finalization of the algorithm before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

private: // private methods

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

protected:  // protected methods

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

#endif // RICHKERNEL_RICHMONIALGBASE_H
