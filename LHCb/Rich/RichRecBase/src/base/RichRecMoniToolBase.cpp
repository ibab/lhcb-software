
//-----------------------------------------------------------------------------
/** @file RichRecMoniToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniToolBase.cpp,v 1.4 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecMoniToolBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif 
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class RichRecBase<RichMoniToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecMoniToolBase::RichRecMoniToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichMoniToolBase ( type, name, parent ),
    RichRecBase<RichMoniToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecMoniToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichMoniToolBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichMoniToolBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode RichRecMoniToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // base class finalize
  return RichMoniToolBase::finalize();
}
// ============================================================================
