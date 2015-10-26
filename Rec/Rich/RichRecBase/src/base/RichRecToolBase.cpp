
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecToolBase
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
template class Rich::Rec::CommonBase<Rich::ToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
Rich::Rec::ToolBase::ToolBase( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : Rich::ToolBase          ( type, name, parent ),
    Rich::Rec::CommonBase<Rich::ToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode Rich::Rec::ToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise Rich::ToolBase", sc );

  // Common initialisation
  return initialiseRichReco();
}
// ============================================================================

// ============================================================================
// Finalise
// ============================================================================
StatusCode Rich::Rec::ToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise Rich::RecBase", sc );

  // base class finalize
  return Rich::ToolBase::finalize();
}
// ============================================================================
