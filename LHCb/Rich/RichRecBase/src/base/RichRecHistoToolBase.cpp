
//-----------------------------------------------------------------------------
/** @file RichRecHistoToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecHistoToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecHistoToolBase.cpp,v 1.2 2007-02-01 17:26:23 jonrob Exp $
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
template class Rich::Rec::CommonBase<Rich::HistoToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
Rich::Rec::HistoToolBase::HistoToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : Rich::HistoToolBase ( type, name, parent ),
    Rich::Rec::CommonBase<Rich::HistoToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::HistoToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = Rich::HistoToolBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichHistoToolBase", sc );

  // Common initialisation
  sc = initialiseRichReco();
  if ( sc.isFailure() )
    return Error( "Failed to initialise Rich::RecBase", sc );

  return sc;
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode Rich::Rec::HistoToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise Rich::RecBase", sc );

  // base class finalize
  return Rich::HistoToolBase::finalize();
}
// ============================================================================
