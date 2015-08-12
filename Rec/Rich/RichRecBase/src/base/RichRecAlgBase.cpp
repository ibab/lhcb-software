
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction algorithm base class : RichRecAlgBase
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecAlgBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class Rich::Rec::CommonBase<Rich::AlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
Rich::Rec::AlgBase::AlgBase( const std::string& name,
                             ISvcLocator* pSvcLocator )
  : Rich::AlgBase                        ( name, pSvcLocator ),
    Rich::Rec::CommonBase<Rich::AlgBase> ( this              )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::AlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise Rich::AlgBase", sc );

  // Common initialisation
  return initialiseRichReco();
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode Rich::Rec::AlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default Rich::RecAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode Rich::Rec::AlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // Finalize base class
  return Rich::AlgBase::finalize();
}
// ============================================================================
