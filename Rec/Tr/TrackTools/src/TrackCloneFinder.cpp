// $Id: TrackCloneFinder.cpp,v 1.11 2008-05-06 09:51:11 aperiean Exp $
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
// Update
// 2005-05-05 : Adrian Perieanu
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
  declareProperty( "MatchingFractionT",     m_matchingFractionT     = 0.50);
  declareProperty( "CompareAtLHCbIDsLevel", m_compareAtLHCbIDsLevel = false);
  declareProperty( "CompareLDT",            m_compareLDT            = false);
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
void TrackCloneFinder::areClones( LHCb::Track& track1,
                                  LHCb::Track& track2,
                                  bool setFlag) const
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
  } else {
    n1 = track1.nLHCbIDs();
    n2 = track2.nLHCbIDs();
  }

  if ( setFlag && theyAreClones ) {
    if ( n1 > n2 ) {
      track2.setFlag( LHCb::Track::Clone, true );
    } else if (n2 > n1) {
      track1.setFlag( LHCb::Track::Clone, true );
    } else{
      const double chi1 = track1.chi2PerDoF();
      const double chi2 = track2.chi2PerDoF();
      chi1 < chi2 ? track2.setFlag( LHCb::Track::Clone, true ) :
                    track1.setFlag( LHCb::Track::Clone, true );
    }
  }

  if ( m_debugLevel ) debug() << "-> areClones = " << theyAreClones << endreq;
}

//=============================================================================
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
//=============================================================================
bool TrackCloneFinder::clones( const LHCb::Track& track1,
                               const LHCb::Track& track2 ) const
{
  if ( ! areSettingsConsistent( track1, track2 ) ) return false;
  
  unsigned int nHitsCommon     = 0;
  getCommonHits( track1, track2, nHitsCommon);

  unsigned int ntrack1= 0;
  unsigned int ntrack2 = 0;
  if( !m_compareAtLHCbIDsLevel){
    ntrack1 = track1.nMeasurements();
    ntrack2 = track2.nMeasurements();
  }else{
    ntrack1 = track1.nLHCbIDs();
    ntrack2 = track2.nLHCbIDs();
  }
  unsigned int nTrackMin = GSL_MIN( ntrack1, ntrack2);

  bool are_clones = false;
  if( nHitsCommon > m_matchingFraction*nTrackMin){
    are_clones = true;
  }
  
  if( !are_clones && m_compareLDT){
    if( (track1.type() == LHCb::Track::Long) &&
	(track2.type()    == LHCb::Track::Downstream ||
	 track2.type()    == LHCb::Track::Ttrack)){
      if( nHitsCommon > m_matchingFractionT*nTrackMin){
	are_clones = true;
      }
    }
  }
 
  
  return are_clones;
}
//=============================================================================
//Calculate the number of common hits between two input Tracks.
//=============================================================================
void TrackCloneFinder::getCommonHits( const LHCb::Track& track1,
				      const LHCb::Track& track2,
				      unsigned int& nCommonHits) const
{
  if( !m_compareAtLHCbIDsLevel){
    const std::vector<LHCb::Measurement*>& hits1 = track1.measurements();
    const std::vector<LHCb::Measurement*>& hits2 = track2.measurements();

    // Calculate the number of common LHCbIDs
    for( unsigned int i1 = 0; i1 < hits1.size(); ++i1){
      // if second track is VeloTT(Upstream) skip the IT&OT hits
      if( track2.type() == LHCb::Track::Upstream &&
	  ( hits1[i1]->type() ==  LHCb::Measurement::IT||
	    hits1[i1]->type() ==  LHCb::Measurement::OT)) break;
      // if second track is Tsa || Downstream skip the VeloR&VeloPhi hits
      if( ( track2.type() == LHCb::Track::Downstream || 
	    track2.type() == LHCb::Track::Ttrack) &&
	  ( hits1[i1]->type() ==  LHCb::Measurement::VeloR ||
	    hits1[i1]->type() ==  LHCb::Measurement::VeloPhi) )continue;
      for( unsigned int i2 = 0; i2 < hits2.size(); ++i2){
	if( hits1[i1]->lhcbID() == hits2[i2]->lhcbID()){
	  ++nCommonHits;
	  break;
	}
      }
    }  
  }else{
    const std::vector<LHCb::LHCbID>& ids1 = track1.lhcbIDs();
    const std::vector<LHCb::LHCbID>& ids2 = track2.lhcbIDs();
    
    // Calculate the number of common LHCbIDs
    for( unsigned int i1 = 0; i1 < ids1.size(); ++i1){
      // if second track is VeloTT(Upstream) skip the IT&OT hits
      if( track2.type() == LHCb::Track::Upstream &&
	  ( ids1[i1].isIT() || ids1[i1].isOT())) break;
      // if second track is Tsa || Downstream skip the VeloR&VeloPhi hits
      if( ( track2.type() == LHCb::Track::Downstream || 
	    track2.type() == LHCb::Track::Ttrack) &&
	  ids1[i1].isVelo() )continue;
      
      for( unsigned int i2 = 0; i2 < ids2.size(); ++i2){
	if( ids1[i1].channelID() == ids2[i2].channelID()){
	  ++nCommonHits;
	  break;
	}
      }
    }  
  }
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
