
//-----------------------------------------------------------------------------
/** @file RichRecTupleAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecTupleAlgBase.cpp,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecTupleAlgBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class RichRecBase<RichTupleAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecTupleAlgBase::RichRecTupleAlgBase( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : RichTupleAlgBase  ( name, pSvcLocator ),
    RichRecBase<RichTupleAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecTupleAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichTupleAlgBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichTupleAlgBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode RichRecTupleAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecTupleAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalize
// ============================================================================
StatusCode RichRecTupleAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // Finalize base class
  return RichTupleAlgBase::finalize();
}
// ============================================================================
