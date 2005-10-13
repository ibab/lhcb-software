
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecToolBase.cpp,v 1.13 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecToolBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif 
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class RichRecBase<RichToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    RichRecBase<RichToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichToolBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode RichRecToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // base class finalize
  return RichToolBase::finalize();
}
// ============================================================================
