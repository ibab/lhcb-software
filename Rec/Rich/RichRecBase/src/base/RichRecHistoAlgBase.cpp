
//-----------------------------------------------------------------------------
/** @file RichRecHistoAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : Rich::Rec::HistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecHistoAlgBase.cpp,v 1.2 2007-02-01 17:26:23 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecHistoAlgBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class Rich::Rec::CommonBase<Rich::HistoAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
Rich::Rec::HistoAlgBase::HistoAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::HistoAlgBase   ( name, pSvcLocator ),
    Rich::Rec::CommonBase<Rich::HistoAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::HistoAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = Rich::HistoAlgBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise Rich::HistoAlgBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() )
    return Error( "Failed to initialise Rich::RecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode Rich::Rec::HistoAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default Rich::RecHistoAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalize
// ============================================================================
StatusCode Rich::Rec::HistoAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise Rich::RecBase", sc );

  // Finalize base class
  return Rich::HistoAlgBase::finalize();
}
// ============================================================================
