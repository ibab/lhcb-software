
//-----------------------------------------------------------------------------
/** @file RichRecTupleToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecTupleToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecTupleToolBase.cpp,v 1.1 2005-10-31 13:30:16 jonrob Exp $
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
template class RichRecBase<RichTupleToolBase> ;
// ============================================================================

// ============================================================================
// Standard constructor
// ============================================================================
RichRecTupleToolBase::RichRecTupleToolBase( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichTupleToolBase ( type, name, parent ),
    RichRecBase<RichTupleToolBase> ( this )
{
}
// ============================================================================

// ============================================================================
// Initialise
// ============================================================================
StatusCode RichRecTupleToolBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichTupleToolBase::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialise RichTupleToolBase", sc );

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
StatusCode RichRecTupleToolBase::finalize()
{
  // Common finalisation
  const StatusCode sc = finaliseRichReco();
  if ( sc.isFailure() ) return Error( "Failed to finalise RichRecBase", sc );

  // base class finalize
  return RichTupleToolBase::finalize();
}
// ============================================================================
