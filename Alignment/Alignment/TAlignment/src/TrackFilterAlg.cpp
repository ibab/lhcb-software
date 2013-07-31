// $Id: TrackFilterAlg.cpp,v 1.18 2010-06-04 13:02:57 svecchi Exp $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SystemOfUnits.h"

// from BOOST
#include <boost/assign/list_of.hpp>
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "TrackFilterAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackFilterAlg
//
// 2007-07-11 : Jan Amoraal
// 2009-12-15 : Stefania Vecchi - add filter for muons
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using namespace boost::lambda;
using namespace boost::assign;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackFilterAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackFilterAlg::TrackFilterAlg( const std::string& name,
                                ISvcLocator* pSvcLocator): GaudiAlgorithm ( name , pSvcLocator ),
    m_tracksInputContainer(""),
    m_tracksOutputContainer(""),
    m_trackType(""),
    m_trackSelectorName(""),
    m_trackSelector(0)
{
  /// Map strings to track types
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  m_stringToTrackTypeMap = { {"Velo", Track::Velo},
			     {"VeloR"     , Track::VeloR },
			     {"Long"      , Track::Long  },
			     {"Upstream"  , Track::Upstream },
			     {"Downstream", Track::Downstream },
			     {"Ttrack"    , Track::Ttrack    },
			     {"Muon"      , Track::Muon } } ;
#else
  m_stringToTrackTypeMap = map_list_of("Velo"      , Track::Velo      )
    ("VeloR"     , Track::VeloR     )
    ("Long"      , Track::Long      )
    ("Upstream"  , Track::Upstream  )
    ("Downstream", Track::Downstream)
    ("Ttrack"    , Track::Ttrack    )
    ("Muon"      , Track::Muon      );
#endif

  /// And vice versa. I whish we had Boost::BiMap
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  m_trackTypeToStringMap = { {Track::Velo, "Velo" },
			     {Track::VeloR     , "VeloR"     },
			     {Track::Long      , "Long"      },
			     {Track::Upstream  , "Upstream"  },
			     {Track::Downstream, "Downstream"},
			     {Track::Ttrack    , "Ttrack"    },
			     {Track::Muon      , "Muon"      }};
#else
  m_trackTypeToStringMap = map_list_of(Track::Velo      , "Velo"      )
    (Track::VeloR     , "VeloR"     )
    (Track::Long      , "Long"      )
    (Track::Upstream  , "Upstream"  )
    (Track::Downstream, "Downstream")
    (Track::Ttrack    , "Ttrack"    )
    (Track::Muon      , "Muon"      );
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  m_lhcbDetChecks = { {"Velo", boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isVelo,_1))},
		      {"TT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isTT  ,_1))},
		      {"IT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isIT  ,_1))},
		      {"OT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isOT  ,_1))},
		      {"Muon", boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isMuon  ,_1))} };
#else
  m_lhcbDetChecks = map_list_of("Velo", boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isVelo,_1)))
    ("TT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isTT  ,_1)))
    ("IT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isIT  ,_1)))
    ("OT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isOT  ,_1)))
    ("Muon", boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isMuon  ,_1)));
#endif

  declareProperty("TracksInputContainer"     , m_tracksInputContainer  = TrackLocation::Default    );
  declareProperty("TracksOutputContainer"    , m_tracksOutputContainer = "Alignment/FilteredTracks");
  declareProperty("TrackType"                , m_trackType             = "Long"                    );
  declareProperty("TrackSelector"            , m_trackSelectorName     = "AlignSelTool"            );
  declareProperty("StripUnwantedDetectorHits", m_strip                 = false                     );
  declareProperty("KeepDetectorHits"         , m_detector              = "OT"                      );
  declareProperty("MinNHits"                 , m_nMinHits              = 0u                        );
  declareProperty("Extrapolator"             , m_nExtrapolator         = "TrackLinearExtrapolator" );

  declareProperty("MuonFilter"               , m_muonFilter            = false                     );
  declareProperty("MuonPcut"                 , m_pcut                  = 0.* GeV                   );
  declareProperty("inCaloAcceptance"         , m_calo                  = false                     );
  declareProperty("noOverlap"                , m_noOverlap             = false                     );
  declareProperty("minHitStation"            , m_nStation              = 2                         );
  declareProperty("MuonChisquareCut"         , m_muonChisquareCut      = 0.                        );
  declareProperty("TheRegion"                , m_theR                 = -1                        );
 

}

//=============================================================================
// Destructor
//=============================================================================
TrackFilterAlg::~TrackFilterAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFilterAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set output container if not specified
  if ( m_tracksOutputContainer == "" ) return Error("==> Please specify an output location for the filtered tracks",
                                                    StatusCode::FAILURE);

  /// Get track selector tool
  m_trackSelector = tool<ITrackSelector>(m_trackSelectorName, "Selector", this);
  if (!m_trackSelector) return Error("==> Failed to retrieve track selector tool", StatusCode::FAILURE);

  m_elementsToBeAligned = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_elementsToBeAligned) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  m_extrapolator   = tool<ITrackExtrapolator>(   m_nExtrapolator  , "Extrapolator"  , this );  
  info()
    << " " << endmsg
    << "=========== " << name() << " Settings ============"
    << endmsg
    << "  Tracks input container   : " << m_tracksInputContainer  << endmsg
    << "  Tracks output container  : " << m_tracksOutputContainer << endmsg
    << "=================================================="
    << endmsg
    << " " << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackFilterAlg::execute() {

  /// Get tracks
  Tracks* tracks = get<Tracks>(m_tracksInputContainer);

  filterTracks(tracks);

  return StatusCode::SUCCESS;
}

//=============================================================================
void TrackFilterAlg::filterTrack(LHCb::Track* track, LHCb::Tracks* outputContainer) {
  //=============================================================================
  debug()<<" filtering tracks with filterTrack "<<endmsg;
  
  if ((track->checkType(m_stringToTrackTypeMap[m_trackType])) && (m_trackSelector->accept(*track))) {
    /// Clone track. It's mine
    std::auto_ptr<Track> clonedTrack( track->cloneWithKey() );
    /// let's strip unwanted nodes. This should be a seperate algorithm
    /// Get my own copy of nodes
    LHCBIDS ids = clonedTrack->lhcbIDs();
    if (printDebug()) {
      debug() << "==> Got " << ids.size() << " ids " <<endmsg;
    }
    if (m_strip) {
      debug() << "==> Stripping unwanted ids" << endmsg;
      for_each(ids.begin(), ids.end(), bind(&TrackFilterAlg::strip, this, _1, clonedTrack.get()));
    }
    if (printDebug()) {
      const LHCBIDS& wantedIds = clonedTrack->lhcbIDs();
      debug() << wantedIds.size() << " ids left after stripping" << endmsg;
      int nID = 0;
      for (LHCBIDS::const_iterator i = wantedIds.begin(), iEnd = wantedIds.end(); i != iEnd; ++nID, ++i) {
        debug() << "==> " << nID << ": ID is " << (*i) << endmsg;
      }
    }

    unsigned nHits = 0u;
    const LHCBIDS& wantedIds = clonedTrack->lhcbIDs();
    for (LHCBIDS::const_iterator i = wantedIds.begin(), iEnd = wantedIds.end(); i != iEnd; ++i) {
      const AlignmentElement* elem = m_elementsToBeAligned->findElement((*i));
      if (!elem) {
        if (printDebug()) debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
      ++nHits;
    }

    if ( nHits >= m_nMinHits )  {
      if (printDebug()) {
        debug() << "Found track of type " << m_trackTypeToStringMap[track->type()] << " with a chi2/dof: " << 
          track->chi2() << " / " << track->nDoF() << 
          " and " << nHits << " hits of type " << m_detector << endmsg;
      }
      outputContainer->add(clonedTrack.release());
    }
  }
}

//=============================================================================
void TrackFilterAlg::filterMuonTrack(LHCb::Track* track, LHCb::Tracks* outputContainer) {
//=============================================================================
  debug()<<" filtering with filterMuonTrack"<<endmsg;
  
  bool select=true;  
  if (m_pcut && track->p() < m_pcut ) {
    debug()<< " Discard the track due to the low momentum"<<track->p()<<endmsg;    
    select=false;        //select high momentum tracks
  }
  
  bool ASide = false;
  bool CSide = false;
  int MA[5] = {0,0,0,0,0}; 
  int MC[5] = {0,0,0,0,0};
  int MR[4] = {0,0,0,0};
  
  
  const std::vector<LHCb::LHCbID>& lhcbids = track->lhcbIDs();
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id) {
    if( id->isMuon() ) {
      int iS = id->muonID().station();
      int iR = id->muonID().region();
      int iQ = id->muonID().quarter();
      if ( iQ < 2 ) MA[iS]=1;   // A-side
      else MC[iS]=1;
      MR[iR]++;
    }
  }    
  
  int MAside = MA[0]+MA[1]+MA[2]+MA[3]+MA[4];
  int MCside = MC[0]+MC[1]+MC[2]+MC[3]+MC[4];      

  if(m_theR>-1 && m_theR<4){
    for(int iR=0; iR<4; iR++){
      if(iR!=m_theR && MR[iR]!=0) {
        debug()<<" Descard the track since it hits unwanted Region"<<iR<<endmsg;
        select = false;
      }    
    }
    if(MR[m_theR]==0) {
      debug()<<" Descard the track since it doesn't hit Region"<<m_theR<<endmsg;
      select = false;
    }
  }else if(m_theR>9 && m_theR<40){
    for(int iR=0; iR<4; iR++){
      if(iR>int(m_theR/10) && MR[iR]!=0) {
        debug()<<" Descard the track since it hits unwanted Region"<<iR<<endmsg;
        select = false;
      }
    }
  }else if(m_theR==10) {
    int iMR=0;
    for(int iR=0; iR<4; iR++){
      if( MR[iR]!=0) iMR++;
    }
    if(iMR>1) {
      debug()<<" Descard the track since it hits more than one region"<<iMR<<endmsg;
      select = false;
    }
  }
  
  
  if ( MAside != 0 && MCside == 0) ASide = true;
  else if ( MAside == 0 && MCside != 0 ) CSide = true;
  
  if( MAside + MCside < m_nStation ) {
    debug()<< " Discard the track due to the low number of hit station "<<MAside+MCside<<endmsg;    
    select = false;  /// requirese at least some hits somewhere
  }
  
  if( m_noOverlap && 
     !(( ASide || CSide) && (MAside > m_nStation ||  MCside > m_nStation)) ) {
    debug()<< " Discard the track since overlaps hit station Cside "<< MCside<<" Aside "<<MAside<<endmsg;    
    select = false;
  }else{
    debug()<< " Track hit station Cside "<< MCside<<" Aside "<<MAside<<endmsg;    
  }
  
  if(m_calo){      
    State stateAtCALO ;  
    StatusCode sc = m_extrapolator->propagate( *track, 12750, stateAtCALO ) ;
    if ( fabs( stateAtCALO.x() ) > 3900 && fabs( stateAtCALO.y() ) > 3150 ) {
      debug()<< " Discard the track since falls off the CALO acceptance x"<< 
        fabs( stateAtCALO.x() )<<" y "<< stateAtCALO.y()<<endmsg;
      select = false; //out the calo acceptance
    }
    
  }
  

  if ( m_muonChisquareCut > 0 && track->chi2PerDoF() > m_muonChisquareCut) {
    debug()<< " Discard the track due to the chisquare "<<track->chi2PerDoF()<<endmsg;
    select = false;
	}
  
  if(select) {         
    debug()<<" Track selected "<<endmsg;
    std::auto_ptr<Track> clonedTrack( track->cloneWithKey() );
    LHCBIDS ids = clonedTrack->lhcbIDs();
    outputContainer->add(clonedTrack.release()); 
  }
  debug() << "outputContainer.size " << outputContainer->size() << endmsg;
} 



//=============================================================================
void TrackFilterAlg::filterTracks(const Tracks* tracks) {
//=============================================================================
  Tracks* output = new Tracks();
  if(!m_muonFilter ) for_each(tracks->begin(), tracks->end(), 
                              bind(&TrackFilterAlg::filterTrack, this, _1, output));
  else if(m_muonFilter ) for_each(tracks->begin(), tracks->end(), 
                                  bind(&TrackFilterAlg::filterMuonTrack, this, _1, output));
  /// put filtered track container in TES
  debug()<<" going to save the filtered tracks in the "<<m_tracksOutputContainer<<" container "<<endmsg;
  put(output, m_tracksOutputContainer);
}
