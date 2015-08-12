// Include files 


// from Hlt
#include "Event/HltSelReports.h"
#include "Event/HltDecReports.h"
#include "Event/HltObjectSummary.h"
#include "Kernel/IANNSvc.h"

// local
#include "HltTrackConverter.h"

#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackConverter
//
// 2010-05-03 : Albert Frithjof Bursche
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackConverter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackConverter::HltTrackConverter( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "HltObjectSummary", 
                   m_HltObjectSummaryLocation = LHCb::HltObjectSummaryLocation::Default
		   // LHCb::HltSelReportsLocation::Default
                   , "Location of the HltObjectSummary in TES" );
  declareProperty( "TrackDestination", m_ConvertedTracksDestination = "Rec/Track/ConvertedHltTracks"
                   , "Location in TES the convertet tracks are supposed to end up." );
  //  double CloneOverlapTreshold = 0.01;
  declareProperty( "CloneOverlapTreshold",m_CloneOverlapTreshold = 0.7 
                   , "Remove every track if there is another track with 'CloneOverlapTreshold' overlap. The other track is then kept." );
  declareProperty( "MinimalHits",m_MinimalHits = 3 
                   , "Remove every track having less than this number of hits (in the selected tracking stations)" );
 
 {
    std::vector<std::string> UseHitsFromDefault;
    UseHitsFromDefault.push_back("AllTrackingStations");
    declareProperty( "UseHitsFrom", m_UseHitsFrom = UseHitsFromDefault
		     , "Names of stations of which hits should be used" );
    
    for (int i=0;i<9;i++)
      m_UseHitsFromLookupTable[i] = false;
    for (std::vector<std::string>::iterator it = m_UseHitsFrom.begin();it!=m_UseHitsFrom.end();++it)
      { 
	if (*it == "Velo")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Velo] = true;
	    continue;
	  }
	if (*it == "TT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    continue;
	  }
	if (*it == "IT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    continue;
	  }
	if (*it == "OT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::OT] = true;
	    continue;
	  }
	if (*it == "Muon")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Muon] = true;
	    continue;
	  }
	if (*it == "ST")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    continue;
	  }
	if (*it == "AllTrackingStations")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Velo] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::OT] = true;
	    continue;
	  }
      }
  }
  {
    m_HltLines.push_back("Hlt1Global");// see Thomasz mail for better solution
    declareProperty( "HltLinesToUse", m_HltLines
		     , "Names of Trigger lines of which hits should be used" );
    
  }
  declareProperty( "ReadHltLinesFrom1stEvent", m_HltLinesFrom1stEvent = false
		   , "Names of Trigger lines of which hits should be used" );
  declareProperty( "RequireTrackClassID", m_requireTrackClassID= true
		   , "Require that the HLTObjectSummaryClID is equal to LHCb::Track::classID()" );
  declareProperty( "AddFirstState", m_addFirstState = true
		   , "Add a state to the track." ) ;
  declareProperty( "SelReportsLocation", m_selRepLoc = LHCb::HltSelReportsLocation::Default
		   , "Manually set the SelReports location." ) ;
}

StatusCode HltTrackConverter::initializeTriggerLists()
{
  std::vector<IANSvc::minor_key_type> m_hlt1_init = svc<IANNSvc>("ANNDispatchSvc")->keys(Gaudi::StringKey(std::string("Hlt1SelectionID")));
  std::vector<IANSvc::minor_key_type> m_hlt2_init = svc<IANNSvc>("ANNDispatchSvc")->keys(Gaudi::StringKey(std::string("Hlt2SelectionID")));
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< "Available Hlt1 Triggers\n";
  for (std::vector<IANSvc::minor_key_type>::iterator it = m_hlt1_init.begin();it!=m_hlt1_init.end();++it)
    {
      m_HltLines.push_back(*it);
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< *it << "\n";
    }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << endmsg;
    debug()<< "Available Hlt2 Triggers\n";
  }
  for (std::vector<IANSvc::minor_key_type>::iterator it = m_hlt2_init.begin();it!=m_hlt2_init.end();++it)
    {
      m_HltLines.push_back(*it);
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< *it << "\n";
    }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug () << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackConverter::~HltTrackConverter() {} 

//=============================================================================
// Main execution
//=============================================================================
LHCb::Track::Types HltTrackConverter::SetTrackType(const LHCb::Track& t) const
{
  //  enum  	Types  {
  //    TypeUnknown = 0, Velo, VeloR, Long,
  //    Upstream, Downstream, Ttrack, Muon,
  //    Calo, TT, VeloPix
  //  }
  const std::vector<LHCb::LHCbID>& lhcbIDs = t.lhcbIDs(); 
  bool hasIT = false;
  bool hasOT = false;
  bool hasVelo = false;
  bool hasVeloPhi = false;
  bool hasTT =  false;
  //  bool hasMuon = false;

  for(std::vector<LHCb::LHCbID>::const_iterator it = lhcbIDs.begin();it!=lhcbIDs.end();++it)
    switch(it->detectorType())
      {
      case LHCb::LHCbID::Velo:
	hasVelo = true;
	if (it->isVeloPhi())
	  hasVeloPhi = true;
	continue;
      case LHCb::LHCbID::TT:
	hasTT = true;
	continue;
      case LHCb::LHCbID::IT:
	hasIT = true;
	continue;
      case LHCb::LHCbID::OT:
	hasOT = true;
	continue;
      case LHCb::LHCbID::Muon:
        //	hasMuon = true;
	continue;
	
      }
  bool hasT = hasIT || hasOT;
  
  if(hasVelo && hasT)
    return LHCb::Track::Long;
  if(hasVelo && hasTT)
    return LHCb::Track::Upstream;
  if(hasTT && hasT)
    return LHCb::Track::Downstream;
  if(hasVelo)
    {
      if(hasVeloPhi)
	return LHCb::Track::Velo;
      else
	return  LHCb::Track::VeloR;
    }
  return LHCb::Track::TypeUnknown;
}

namespace {
  class TrackComperator
  {
  public:
    bool operator()(LHCb::Track* a ,LHCb::Track*b)
    {
      return a->nLHCbIDs() > b->nLHCbIDs();
    }
  } ;
}

void  HltTrackConverter::RemoveClones(LHCb::Track::Vector& tracks) const
{
  std::vector<LHCb::Track*> alltracks(tracks.begin(), tracks.end()) ;
  std::sort(alltracks.begin(),alltracks.end(), TrackComperator() );
  tracks.clear() ;
  for( std::vector<LHCb::Track*>::iterator it = alltracks.begin() ;
       it != alltracks.end(); ++it ) {
    bool found = false;
    for(LHCb::Tracks::iterator jt = tracks.begin(); 
	jt!=tracks.end() && !found ;++jt) {
      size_t nOverlap = (*jt)->nCommonLhcbIDs(**it) ;
      size_t minN     = std::min( (*jt)->lhcbIDs().size(),  (*it)->lhcbIDs().size() ) ;
      found = nOverlap >= minN * m_CloneOverlapTreshold ;
    }
    if(found) {
      delete *it ;
      *it = 0 ;
    } else {
      tracks.push_back(*it) ;
    }
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "CloneRemoval kept " << tracks.size() << " out of " << alltracks.size() << " tracks." << endmsg ;
}

StatusCode HltTrackConverter::execute() 
{
  if(m_HltLinesFrom1stEvent)
    if(initializeTriggerLists()) // side effects 
      m_HltLinesFrom1stEvent = false;
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "==> Execute" << endmsg;
  LHCb::Track::Vector tracks ;
  const LHCb::HltSelReports* selReports = get<LHCb::HltSelReports>(m_selRepLoc);
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Retrieved HltSelReports with size: " << selReports->size() << endmsg ;
  for (std::vector<std::string>::const_iterator s = m_HltLines.begin();s!= m_HltLines.end();++s) {
    const LHCb::HltObjectSummary*  selReport =  selReports->selReport(*s);
    if (selReport)
      executeRecursive(tracks,*selReport);
  } 
  
  RemoveClones( tracks ) ;
  LHCb::Tracks* trackcontainer = new LHCb::Tracks() ;
  for( LHCb::Track::Vector::iterator it = tracks.begin() ;
       it != tracks.end(); ++it) 
    trackcontainer->insert( *it ) ;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Inserting track collection with "  << trackcontainer->size()
            <<" tracks to " << m_ConvertedTracksDestination << endmsg;
  put(trackcontainer,m_ConvertedTracksDestination );
  return StatusCode::SUCCESS;
}

namespace {
  template<class T>
  struct StringWithSetter
  {
    StringWithSetter( const std::string& aname, void (T::*afunction)(double) ) 
      : name(aname), function(afunction) {}
    std::string name ;
    void (T::*function)(double) ;
  } ;
}


void HltTrackConverter::executeRecursive(LHCb::Track::Vector& tracks, const LHCb::HltObjectSummary& SelRep) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "called executeRecursive(" <<  SelRep << ");" << endmsg;
  SmartRefVector <LHCb::HltObjectSummary> substructure = SelRep.substructure();
  for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator child = substructure.begin();child!=substructure.end();++child)
    if( *child ) executeRecursive(tracks, **child);

  if (SelRep.summarizedObjectCLID() ==  LHCb::Track::classID() ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "found track" << endmsg; //never prints anything
  } else
    if (m_requireTrackClassID) return; 

  // collect the LHCbIDs
  std::vector<LHCb::LHCbID> LhcbIDs = SelRep.lhcbIDs();
  LHCb::Track::LHCbIDContainer acceptedlhcbids ;
  for (std::vector<LHCb::LHCbID>::iterator it = LhcbIDs.begin();it!=LhcbIDs.end();++it)
    if (m_UseHitsFromLookupTable[it->detectorType()]) {
      if( it->isOT() && it->otID().straw()==0 ) 
	warning() << "Skipping invalid LHCbID: " << *it << endmsg ;
      else {
	acceptedlhcbids.push_back(*it) ;
	if ( msgLevel(MSG::DEBUG) ) 
	  debug() << "added" << *it << " to track." << endmsg; 
      }
    }
  
  if( acceptedlhcbids.size() >= m_MinimalHits ) {
    
    // create a state from the info fields
    bool stateIsValid = false ;
    LHCb::State state;
    if (m_addFirstState ) {
      
      // we could also do this in initialize. but then, who cares.
      std::vector< StringWithSetter<LHCb::State> > trackStateFields ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("0#Track.firstState.z",&LHCb::State::setZ) ) ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("1#Track.firstState.x",&LHCb::State::setX) ) ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("2#Track.firstState.y",&LHCb::State::setY) ) ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("3#Track.firstState.tx",&LHCb::State::setTx) ) ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("4#Track.firstState.ty",&LHCb::State::setTy) ) ;
      trackStateFields.push_back( StringWithSetter<LHCb::State>("5#Track.firstState.qOverP",&LHCb::State::setQOverP) ) ;

      //decode first state from numericalInfo() and add it to track
      stateIsValid = true ;
      const LHCb::HltObjectSummary::Info& numericalInfo = SelRep.numericalInfo();
      for( std::vector< StringWithSetter<LHCb::State> >::const_iterator ifield = trackStateFields.begin() ;
	   ifield != trackStateFields.end(); ++ifield) {
	LHCb::HltObjectSummary::Info::const_iterator aninfo = numericalInfo.find( ifield->name ) ;
	if( aninfo != numericalInfo.end() ) 
	  (state.*(ifield->function))( aninfo->second ); 
	else {
	  warning() << "Cannot find numerical info for field " << ifield->name << endmsg ;
	  stateIsValid = false ;
	}
      }
    }
    
    if( stateIsValid || !m_addFirstState ) {
      // create the track, fill it and add to track list
      LHCb::Track * t = new LHCb::Track();
      t->setLhcbIDs( acceptedlhcbids ) ;
      t->setType(SetTrackType(*t));
      t->setHistory(LHCb::Track::HLTImportedTrack);
      if( stateIsValid ) t->addToStates(state);
      tracks.push_back(t);
    } 
  }
}

//=============================================================================
