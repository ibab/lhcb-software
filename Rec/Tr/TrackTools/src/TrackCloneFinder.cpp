// Include files 
// -------------
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
// Update
// 2009-09-10: Kostyantyn Holubyev
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackCloneFinder )

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
  declareProperty( "CompareLDT",            m_compareLDT            = false);
  //In some cases, such as HLT we want to search for clones only if the tracks
  //are close. To do so, set RestrictedSearch to true
  declareProperty( "RestrictedSearch",	    m_restrictedSearch = false);
  //The search windows for which two tracks in the velo are supposed to be
  //close. 
  //TODO: The Value of this parameters is currently determined iteratively
  //and may bot be optimal. This has to be investigated further.
  declareProperty("VeloXSeparation",	    m_xseparationV = 100);
  declareProperty("VeloYSeparation",	    m_yseparationV = 100);
  declareProperty("VeloTXSeparation",	    m_txseparationV = 3e-2);
  declareProperty("VeloTYSeparation",	    m_tyseparationV = 2e-2);

}

//=============================================================================
// Destructor
//=============================================================================
TrackCloneFinder::~TrackCloneFinder() {}

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
bool TrackCloneFinder::flagClones( LHCb::Track& track1,
				   LHCb::Track& track2 ) const
{
  bool theyAreClones = TrackCloneFinder::areClones( track1, track2 ) ;
  if ( theyAreClones ) {  
    size_t n1 = track1.nLHCbIDs();
    size_t n2 = track2.nLHCbIDs();
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
        if ( m_debugLevel ) 
            debug() << "At least one of the tracks is not fitted, selecting a clone randomly" << endmsg; 
        // for fast reco sequence the clone killer is run before the fit,
        // so chi2 is not available. However, in most cases the decision is
        // made using the number n of LHCb IDs, so for the rare case when n1==n2
        // we simply select the track to mark as a clone randomly
        track2.setFlag( LHCb::Track::Clone, true );
    }
  }
  return theyAreClones ;
}
    

//=============================================================================
// Compare two input Tracks and find whether one is a clone
// of the other based on some "overlap criteria".
// The corresponding flags are set accordingly.
//=============================================================================
bool TrackCloneFinder::areClones( const LHCb::Track& track1,
                                  const LHCb::Track& track2 ) const
{
  if ( m_debugLevel ) {
    debug() << "Looking at tracks " << track1.key() << " in "
            << track1.parent() -> name() << " and "
            << track2.key() << " in "
            << track2.parent() -> name() << endmsg;
  }
  
  //If we want to speed up the time for clonesearch we can look only on clones
  //which are physcially close
  bool theyAreClones(false) ;
  if(!m_restrictedSearch || areTracksClose( track1, track2)) { 
    
    size_t nHitsCommon = track1.nCommonLhcbIDs( track2 ) ;
    size_t n1 = track1.nLHCbIDs();
    size_t n2 = track2.nLHCbIDs();
    size_t nTrackMin = std::min(n1,n2);
    theyAreClones = nHitsCommon > m_matchingFraction*nTrackMin ;
    
    if( !theyAreClones && m_compareLDT &&
	(track1.type() == LHCb::Track::Long) &&
	(track2.type()    == LHCb::Track::Downstream ||
	 track2.type()    == LHCb::Track::Ttrack) )  {
      theyAreClones = nHitsCommon > m_matchingFractionT*nTrackMin ;
    }
    
    if ( m_debugLevel ) debug() << "-> areClones = " << theyAreClones << endmsg;
  }
  return theyAreClones ;
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
// vim:sw=4:tw=78:ft=cpp
