
//-----------------------------------------------------------------------------
/** @file RichRecMoniAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniAlgBase.cpp,v 1.5 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecMoniAlgBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif 
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class RichRecBase<RichMoniAlgBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecMoniAlgBase::RichRecMoniAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : RichMoniAlgBase  ( name, pSvcLocator ),
    RichRecBase<RichMoniAlgBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecMoniAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichMoniAlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichMoniAlgBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Main execute method
// ============================================================================
StatusCode RichRecMoniAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecMoniAlgBase::execute() called !!" );
}
// ============================================================================

// ============================================================================
// Finalize
// ============================================================================
StatusCode RichRecMoniAlgBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // Finalize base class
  return RichMoniAlgBase::finalize();
}
// ============================================================================
