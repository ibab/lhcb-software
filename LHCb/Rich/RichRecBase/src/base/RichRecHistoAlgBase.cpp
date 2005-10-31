
//-----------------------------------------------------------------------------
/** @file RichRecHistoAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecHistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecHistoAlgBase.cpp,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
template class RichRecBase<RichHistoAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecHistoAlgBase::RichRecHistoAlgBase( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : RichHistoAlgBase  ( name, pSvcLocator ),
    RichRecBase<RichHistoAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecHistoAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichHistoAlgBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichHistoAlgBase", sc );

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
StatusCode RichRecHistoAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecHistoAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalize
// ============================================================================
StatusCode RichRecHistoAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // Finalize base class
  return RichHistoAlgBase::finalize();
}
// ============================================================================
