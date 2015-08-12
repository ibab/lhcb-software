
//-----------------------------------------------------------------------------
/** @file RichRecTupleAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecTupleAlgBase.cpp,v 1.2 2007-02-01 17:26:23 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

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
template class Rich::Rec::CommonBase<Rich::TupleAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
Rich::Rec::TupleAlgBase::TupleAlgBase( const std::string& name,
                                       ISvcLocator* pSvcLocator )
  : Rich::TupleAlgBase           ( name, pSvcLocator ),
    Rich::Rec::CommonBase<Rich::TupleAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::TupleAlgBase::initialize()
{
  // Initialise base class
  const StatusCode sc = Rich::TupleAlgBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise Rich::TupleAlgBase", sc );

  // Common initialisation
  return initialiseRichReco();
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode Rich::Rec::TupleAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default Rich::RecTupleAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalize
// ============================================================================
StatusCode Rich::Rec::TupleAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise Rich::RecBase", sc );

  // Finalize base class
  return Rich::TupleAlgBase::finalize();
}
// ============================================================================
