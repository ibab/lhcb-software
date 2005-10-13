
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.cpp,v 1.23 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
template class RichRecBase<RichAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ),
    RichRecBase<RichAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichAlgBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode RichRecAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode RichRecAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // Finalize base class
  return RichAlgBase::finalize();
}
// ============================================================================
