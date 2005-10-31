
//-----------------------------------------------------------------------------
/** @file RichRecHistoToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecHistoToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecHistoToolBase.cpp,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecHistoToolBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class RichRecBase<RichHistoToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecHistoToolBase::RichRecHistoToolBase( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichHistoToolBase ( type, name, parent ),
    RichRecBase<RichHistoToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecHistoToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichHistoToolBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichHistoToolBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichRecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode RichRecHistoToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // base class finalize
  return RichHistoToolBase::finalize();
}
// ============================================================================
