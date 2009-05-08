// $Id: TrackCloneFinder.cpp,v 1.13 2009-05-08 15:43:18 gkrocker Exp $
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
// Update
// 2009-05-08: Georg Krocker
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
  //In some cases, such as HLT we want to search for clones only if the tracks
  //are close. To do so, set RestrictedSearch to true
  declareProperty( "RestrictedSearch",	    m_restrictedSearch = false);
  //The search windows for which two tracks in the velo are supposed to be
  //close. 
  //TODO: The Value of this parameters is currently determined iteratively
  //and may bot be optimal. This has to be investigated further.
  declareProperty("VeloXSeparation",	    m_xseparationV = 0.5);
  declareProperty("VeloYSeparation",	    m_yseparationV = 0.5);
  declareProperty("VeloTXSeparation",	    m_txseparationV = 4e-3);
  declareProperty("VeloTYSeparation",	    m_tyseparationV = 4e-3);

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

  //If we want to speed up the time for clonesearch we can look only on clones
  //which are physcially close
  bool theyAreClose=true;
  if(m_restrictedSearch) theyAreClose = areTracksClose( track1, track2);

  if(!m_restrictedSearch || theyAreClose) { 
      
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
// In some cases the tracks might not be fitted so we can not use the chi2 as
// criterion. So, check if the track is fitted..
	  } else if(track1.fitStatus() == LHCb::Track::Fitted &&
		  track2.fitStatus() == LHCb::Track::Fitted){
	      const double chi1 = track1.chi2PerDoF();
	      const double chi2 = track2.chi2PerDoF();
	      chi1 < chi2 ? track2.setFlag( LHCb::Track::Clone, true ) :
		  track1.setFlag( LHCb::Track::Clone, true );
	  } else {
	      if ( m_debugLevel) 
		 debug () << "At least one of your tracks is not fitted!" << endreq;
	  }
      }

      if ( m_debugLevel ) debug() << "-> areClones = " << theyAreClones << endreq;
  }      
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
	    hits1[i1]->type() ==  LHCb::Measurement::VeloPhi ||
	    hits1[i1]->type() ==  LHCb::Measurement::VeloLiteR ||
	    hits1[i1]->type() ==  LHCb::Measurement::VeloLitePhi ) )continue;
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
//Look if two tracks are really close to each other 
//=============================================================================
bool TrackCloneFinder::areTracksClose(const LHCb::Track& tr1, 
	const LHCb::Track& tr2) const
{
    //We only check wheter the tracks are close in the velo or not so far
    //before we check if they are ghosts, just to save some time
    
    //first we get the states in the velo
    const LHCb::State* vstate1 = tr1.stateAt(LHCb::State::ClosestToBeam);
    const LHCb::State* vstate2 = tr2.stateAt(LHCb::State::ClosestToBeam);
    
    //and check if they exist
    if(tr1.hasStateAt(LHCb::State::ClosestToBeam) && 
	    tr2.hasStateAt(LHCb::State::ClosestToBeam))
    {
	//We check if one of the states lies outside the velo, then our model
	//of a linear track aproximation is wrong and we want to check the
	//tracks anyway for clones
	if(vstate1->z()>990. || vstate2->z()>990.) return true;

	//As LHCb::State::ClosestToBeam is not always at the same z position,
	//we extrapolate the tracks linear to a z position in the middle of
	//the two states
	double extrapolateTo = vstate1->z()+(vstate2->z() - vstate1->z()/2);

	//if they are not close together in x,y or if the slopes are very
	//different they should not be clones. We first check for the slopes
	if(fabs(vstate1->tx() - vstate2->tx()) > m_txseparationV) return false;
	if(fabs(vstate1->ty() - vstate2->ty()) > m_tyseparationV) return false;
	//Afterwards we extrapolate the tracks
	if(fabs((vstate1->x()-(vstate1->tx()*(vstate1->z()-extrapolateTo))) - 
		(vstate2->x()-(vstate2->tx()*(vstate2->z()-extrapolateTo)))) > m_xseparationV) return false;
	if(fabs((vstate1->y()-(vstate1->ty()*(vstate1->z()-extrapolateTo))) - 
		(vstate2->y()-(vstate2->ty()*(vstate2->z()-extrapolateTo)))) > m_yseparationV) return false;
    }

    //ok, they are really close so they can be clones
    return true;
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
// vim:sw=4:tw=78:ft=cpp
