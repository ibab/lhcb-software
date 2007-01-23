// $Id: TrackCloneFinder.cpp,v 1.9 2007-01-23 16:36:39 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h" 

// local
#include "TrackCloneFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCloneFinder
//
// 2005-12-08 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackCloneFinder );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCloneFinder::TrackCloneFinder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackCloneFinder>(this);
 
  declareProperty( "MatchingFraction",      m_matchingFraction      = 0.7   );
  declareProperty( "CompareAtLHCbIDsLevel", m_compareAtLHCbIDsLevel = false );
}

//=============================================================================
// Destructor
//=============================================================================
TrackCloneFinder::~TrackCloneFinder() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackCloneFinder::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
// The corresponding flags are set accordingly.
//=============================================================================
bool TrackCloneFinder::areClones( LHCb::Track& track1,
                                  LHCb::Track& track2,
                                  bool setFlag ) const
{
  if ( m_debugLevel ) {
    debug() << "Looking at tracks " << track1.key() << " in "
            << track1.parent() -> name() << " and "
            << track2.key() << " in "
            << track2.parent() -> name() << endreq;
  }

  bool theyAreClones = clones( track1, track2 );
  unsigned int n1, n2 = 0;
  if ( ! m_compareAtLHCbIDsLevel ) {
    n1 = track1.nMeasurements();
    n2 = track2.nMeasurements();
  }
  else {
    n1 = track1.nLHCbIDs();
    n2 = track2.nLHCbIDs();
  }

  if ( setFlag && theyAreClones ) {
    if ( n1 > n2 ) {
      track2.setFlag( LHCb::Track::Clone, true );
    }
    else {
      track1.setFlag( LHCb::Track::Clone, true );
    }
  }

  if ( m_debugLevel ) debug() << "-> areClones = " << theyAreClones << endreq;

  return theyAreClones;
}

//=============================================================================
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
//=============================================================================
bool TrackCloneFinder::clones( const LHCb::Track& track1,
                               const LHCb::Track& track2 ) const
{
  if ( ! areSettingsConsistent( track1, track2 ) ) return false;
  
  // Determine the number of common Velo hits
  // ----------------------------------------
  unsigned int nVelo1 = 0;
  unsigned int nVelo2 = 0;
  unsigned int nVeloCommon = nCommonHits( track1, track2, LHCb::LHCbID::Velo,
                                          nVelo1, nVelo2 );
  unsigned int nVeloMin = GSL_MIN( nVelo1, nVelo2 );

  // Determine the number of common seed hits. Seed = IT + OT (not TT!)
  // ------------------------------------------------------------------
  unsigned int nIT1, nOT1 = 0;
  unsigned int nIT2, nOT2 = 0;
  unsigned int nSeedCommon =
    nCommonHits( track1, track2, LHCb::LHCbID::IT, nIT1, nIT2 ) +
    nCommonHits( track1, track2, LHCb::LHCbID::OT, nOT1, nOT2 );
  unsigned int nSeedMin = GSL_MIN( nIT1 + nOT1 , nIT2 + nOT2 );
  
  if ( m_debugLevel ) {
    debug() << "nVelo1, nVelo2, nVeloCommon = " << nVelo1 << ", "
            << nVelo2 << ", "
            << nVeloCommon << endreq
            << "nSeed1, nSeed2, nSeedCommon = " << ( nIT1 + nOT1 ) << ", "
            << ( nIT2 + nOT2 ) << ", "
            << nSeedCommon << endreq;
  }

  // Decide whether these tracks are clones
  // --------------------------------------
  if ( ( nVeloMin > 0 ) &&
       ( nVeloCommon < m_matchingFraction * nVeloMin ) )
    return false;  // Not clones !

  if ( ( nSeedMin > 0 ) &&
       ( nSeedCommon < m_matchingFraction * nSeedMin ) )
    return false;  // Not clones !

  // Not clones if there is no common region (Velo or Seed) with clusters
  if ( 0 == nVeloMin && 0 == nSeedMin ) return false;

  // Else they are clones!
  return true;
}

//=============================================================================
// Calculate the number of common hits of a given LHCb type
// between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const LHCb::Track& track1,
                                            const LHCb::Track& track2,
                                            unsigned int lhcbidType,
                                            unsigned int& nHits1,
                                            unsigned int& nHits2 ) const
{
  const std::vector<LHCb::LHCbID> ids1 = lhcbIDsOfType( track1, lhcbidType );
  const std::vector<LHCb::LHCbID> ids2 = lhcbIDsOfType( track2, lhcbidType );

  nHits1 = ids1.size();
  nHits2 = ids2.size();

  if ( nHits1 == 0 || nHits2 == 0 ) return 0;

  return nCommonLHCbIDs( ids1, ids2 ); 
}

//=============================================================================
//  Calculate the number of common hits between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const LHCb::Track& track1,
                                            const LHCb::Track& track2,
                                            unsigned int& nHits1,
                                            unsigned int& nHits2 ) const
{
  return nCommonHits( track1, track2, 0, nHits1, nHits2 );
}

//=============================================================================
// Calculate the number of common hits between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const LHCb::Track& track1,
                                            const LHCb::Track& track2 ) const
{
  unsigned int nHits1, nHits2 = 0;

  return nCommonHits( track1, track2, 0, nHits1, nHits2 );
}

//=============================================================================
// 
//=============================================================================
unsigned int
TrackCloneFinder::nCommonLHCbIDs( const std::vector<LHCb::LHCbID>& ids1,
                                  const std::vector<LHCb::LHCbID>& ids2 ) const
{
  if ( m_debugLevel ) {
    debug() << "nLHCbIDs of type " << ids1[0].detectorType()
            << " for track1, track2 = "
            << ids1.size() << " , " << ids2.size() << endreq;
    debug() << "  LHCbIDs track1: ";
    unsigned int it;
    for ( it = 0; it < ids1.size()-1; ++it ) {
      debug() << ids1[it].channelID() << ", ";
    }
    debug() << ids1[it].channelID() << endreq
            << "  LHCbIDs track2: ";
    for ( it = 0; it < ids2.size()-1; ++it ) {
      debug() << ids2[it].channelID() << ", ";
    }
    debug() << ids2[it].channelID() << endreq;
  }

  unsigned int nCommon = 0;
  
  // Calculate the number of common LHCbIDs
  for ( unsigned int i1 = 0; i1 < ids1.size(); ++i1 ) {
    for ( unsigned int i2 = 0; i2 < ids2.size(); ++i2 ) {
      if ( ids1[i1].channelID() == ids2[i2].channelID() ) {
        ++nCommon;
        break;
      }
    }
  }  

  return nCommon;
}

//=============================================================================
// Return a const vector of a Track's LHCbIDs of a given LHCbID type
//=============================================================================
const std::vector<LHCb::LHCbID>
TrackCloneFinder::lhcbIDsOfType ( const LHCb::Track& track,
                                  unsigned int lhcbidType ) const
{
  std::vector<LHCb::LHCbID> ids;
  
  if ( ! m_compareAtLHCbIDsLevel ) {
    const std::vector<LHCb::Measurement*>& allmeas = track.measurements();
    for ( unsigned int it = 0; it < allmeas.size(); ++it ) {
      LHCb::LHCbID id = allmeas[it] -> lhcbID();
      if ( lhcbidType == 0 )
        ids.push_back( id );
      else if ( id.checkDetectorType( lhcbidType ) )
        ids.push_back( id );
    }
  }
  else {
    if ( lhcbidType == 0 ) return track.lhcbIDs();
    const std::vector<LHCb::LHCbID>& allids = track.lhcbIDs();
    for ( unsigned int it = 0; it < allids.size(); ++it ) {
      if ( allids[it].checkDetectorType( lhcbidType ) ) ids.push_back( allids[it] );
    }
  }

  return ids;
}

//=============================================================================
// 
//=============================================================================
bool TrackCloneFinder::areSettingsConsistent( const LHCb::Track& track1,
                                              const LHCb::Track& track2 ) const
{
  if ( m_compareAtLHCbIDsLevel ) return true;

  bool yesNo = true;

  if ( track1.checkPatRecStatus( LHCb::Track::PatRecIDs ) ) yesNo = false;
  
  if ( track2.checkPatRecStatus( LHCb::Track::PatRecIDs ) ) yesNo = false;
  
  if ( ! yesNo )
    error() << "Settings are not consistent. Check what you are doing!"
            << endreq;

  return yesNo;
}

//=============================================================================
