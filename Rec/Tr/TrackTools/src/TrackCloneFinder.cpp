// $Id: TrackCloneFinder.cpp,v 1.2 2006-02-07 11:32:02 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h" 

// from TrackEvent
#include "Event/TrackFunctor.h"

// local
#include "TrackCloneFinder.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCloneFinder
//
// 2005-12-08 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackCloneFinder>          s_factory ;
const        IToolFactory& TrackCloneFinderFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCloneFinder::TrackCloneFinder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackCloneFinder>(this);
 
  declareProperty( "MatchingFraction",      m_matchingFraction = 0.7 );
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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
//=============================================================================
bool TrackCloneFinder::areClones( const Track& track1,
                                  const Track& track2 ) const
{
  // Determine the number of common Velo hits
  // ----------------------------------------
  unsigned int nVelo1, nVelo2 = 0;
  unsigned int nVeloCommon = nCommonHits( track1, track2, LHCbID::Velo,
                                          nVelo1, nVelo2 );
  unsigned int nVeloMin = GSL_MIN( nVelo1, nVelo2 );

  // Determine the number of common seed hits. Seed = IT + OT (not TT!)
  // ------------------------------------------------------------------
  unsigned int nSeed1, nSeed2 = 0;
  unsigned int nSeedCommon =
    nCommonHits( track1, track2, LHCbID::IT, nSeed1, nSeed2 ) + 
    nCommonHits( track1, track2, LHCbID::OT, nSeed1, nSeed2 );
  unsigned int nSeedMin = GSL_MIN( nSeed1, nSeed2 );

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
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
// The corresponding flags are set accordingly.
//=============================================================================
bool TrackCloneFinder::areClones( Track& track1,
                                  Track& track2,
                                  bool setFlag ) const
{
  bool theyAreClones = areClones( track1, track2 );

  if ( setFlag && theyAreClones ) {
    if ( track1.nMeasurements() > track2.nMeasurements() ) {
      track2.setFlag( Track::Unique, false );
    }
    else {
      track1.setFlag( Track::Unique, false );
    }
  }

  return theyAreClones;
}

//=============================================================================
// Calculate the number of common hits of a given LHCb type
// between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const Track& track1,
                                            const Track& track2,
                                            unsigned int lhcbidType,
                                            unsigned int& nHits1,
                                            unsigned int& nHits2 ) const
{
  const std::vector<LHCbID>& ids1 = lhcbIDsOfType( track1, lhcbidType );
  const std::vector<LHCbID>& ids2 = lhcbIDsOfType( track2, lhcbidType );

  nHits1 = ids1.size();
  nHits2 = ids2.size();

  return nCommonLHCbIDs( ids1, ids2 ); 
}

//=============================================================================
//  Calculate the number of common hits between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const Track& track1,
                                            const Track& track2,
                                            unsigned int& nHits1,
                                            unsigned int& nHits2 ) const
{
  return nCommonHits( track1, track2, 0, nHits1, nHits2 );
}

//=============================================================================
// Calculate the number of common hits between two input Tracks.
//=============================================================================
unsigned int TrackCloneFinder::nCommonHits( const Track& track1,
                                            const Track& track2 ) const
{
  unsigned int nHits1, nHits2 = 0;

  return nCommonHits( track1, track2, 0, nHits1, nHits2 );
}

//=============================================================================
// 
//=============================================================================
unsigned int TrackCloneFinder::nCommonLHCbIDs( const std::vector<LHCbID>& ids1,
                                               const std::vector<LHCbID>& ids2 ) const
{
  unsigned int nCommon = 0;

  // Calculate the number of common LHCbIDs
  for ( unsigned int it1 = 0; it1 < ids1.size(); ++it1 ) {
    for ( unsigned int it2 = 0; it2 < ids2.size(); ++it2 ) {
      if ( ids1[it1].channelID() == ids2[it2].channelID() ) {
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
const std::vector<LHCbID>
TrackCloneFinder::lhcbIDsOfType ( const Track& track,
                                  unsigned int lhcbidType ) const
{
  std::vector<LHCbID> ids;

  if ( ! m_compareAtLHCbIDsLevel ) {
    const std::vector<Measurement*>& allmeas = track.measurements();
    for ( unsigned int it = 0; it < allmeas.size(); ++it ) {
      LHCbID id = allmeas[it] -> lhcbID();
      if ( lhcbidType == 0 )
        ids.push_back( id );
      else if ( id.checkDetectorType( lhcbidType ) )
        ids.push_back( id );
    }
  }
  else {
    if ( lhcbidType == 0 ) return track.lhcbIDs();
    const std::vector<LHCbID>& allids = track.lhcbIDs();
    for ( unsigned int it = 0; it < allids.size(); ++it ) {
      if ( allids[it].checkDetectorType( lhcbidType ) ) ids.push_back( allids[it] );
    }
  }

  return ids;
}

//=============================================================================
