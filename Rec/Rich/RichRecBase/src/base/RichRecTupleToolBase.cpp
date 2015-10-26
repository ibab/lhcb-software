
//-----------------------------------------------------------------------------
/** @file RichRecTupleToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecTupleToolBase
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecTupleToolBase.h"

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================

// ============================================================================
// Force creation of templated class
#include "RichRecBase.icpp"
template class Rich::Rec::CommonBase<Rich::TupleToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
Rich::Rec::TupleToolBase::TupleToolBase( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : Rich::TupleToolBase  ( type, name, parent ),
    Rich::Rec::CommonBase<Rich::TupleToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::TupleToolBase::initialize()
{
  // Initialise base class
  const StatusCode sc = Rich::TupleToolBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise Rich::TupleToolBase", sc );

  // Common initialisation
  return initialiseRichReco();
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode Rich::Rec::TupleToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise Rich::RecBase", sc );

  // base class finalize
  return Rich::TupleToolBase::finalize();
}
// ============================================================================
