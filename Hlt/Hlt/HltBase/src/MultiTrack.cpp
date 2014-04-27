// $Id: MultiTrack.cpp,v 1.1 2010-08-05 10:58:37 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// $URL$
// ============================================================================
// Incldue files
// ============================================================================
// STD * STL
// ============================================================================
#include <algorithm>
// ============================================================================
// HltBase
// ============================================================================
#include "Event/HltMultiTrack.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class Hlt::MultiTrack
 *  @see Hlt::MultiTrack
 *
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-05
 *
 *  $Revision: 1.1 $
 *  Last modification $Date: 2010-08-05 10:58:37 $
 *                 by $Author: ibelyaev $
 */
// ============================================================================
namespace Hlt
{
// ==========================================================================
/// Class ID definition
const CLID CLID_MultiTrack = 7564;
// ==========================================================================
}
// ============================================================================
// virtual descructor
// ============================================================================
Hlt::MultiTrack::~MultiTrack()
{
}
// ============================================================================
std::ostream& Hlt::MultiTrack::fillStream( std::ostream& s ) const
{
    return s << "Hlt:MultiTrack(" << m_tracks.size() << ")";
}
// ============================================================================
const CLID& Hlt::MultiTrack::clID() const
{
    return Hlt::MultiTrack::classID();
}
// ============================================================================
const CLID& Hlt::MultiTrack::classID()
{
    return Hlt::CLID_MultiTrack;
}
// ============================================================================
// Update  References to tarcks
// ============================================================================
void Hlt::MultiTrack::setTracks( const Hlt::MultiTrack::Tracks& tracks )
{
    m_tracks = tracks;
    removeFromTracks( nullptr );
}
// ============================================================================
// Add to (pointer) reference to the stages
// ============================================================================
void Hlt::MultiTrack::addToTracks( const LHCb::Track* track )
{
    if ( track ) m_tracks.push_back( track );
}
// ============================================================================
// Remove from  reference to the tarcks
// ============================================================================
void Hlt::MultiTrack::removeFromTracks( const LHCb::Track* track )
{
    auto it = std::remove( m_tracks.begin(), m_tracks.end(),
                           SmartRef<LHCb::Track>{track} );
    m_tracks.erase( it, m_tracks.end() );
}
// ============================================================================
// Has track ?
// ============================================================================
bool Hlt::MultiTrack::hasTrack( const LHCb::Track* track ) const
{
    return track && m_tracks.end() != std::find( m_tracks.begin(), m_tracks.end(),
                                                 SmartRef<LHCb::Track>{track} );
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
