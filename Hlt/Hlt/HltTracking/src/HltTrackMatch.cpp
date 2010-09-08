// $Id: HltTrackMatch.cpp,v 1.23 2010-09-08 12:55:25 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "boost/foreach.hpp"

// local
#include "HltTrackMatch.h"
#include "HltBase/HltTrackMerge.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackMatch
//
// 2007-12-07 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackMatch );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackMatch::HltTrackMatch( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
  , m_tool(0)
  , m_qualityHisto(0)
  , m_quality2Histo(0)
  , m_qualityHistoBest(0)
  , m_quality2HistoBest(0)
  , m_qualityID(0)
  , m_quality2ID(0)
{
  
  declareProperty("MaxQuality",m_maxQuality = 1e6);
  declareProperty("MaxQuality2",m_maxQuality2 = 1e6);
  declareProperty("Tool", m_toolName);
  declareProperty("RecoID",m_recoID); 
  declareProperty("TESOutput",  m_TESOutput);
  declareProperty("TrackType",  m_trackType);
  declareProperty("TransferIDs",  m_transferIDs);
  declareProperty("TransferAncestor",  m_transferAncestor);
  declareProperty("TransferInfo",  m_transferInfo);
  declareProperty("Quality",  m_qualityName = "<UNKNOWN>");
  declareProperty("Quality2",  m_quality2Name  = "<UNKNOWN>");


  m_selections.declareProperties();

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackMatch::~HltTrackMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  m_qualityID = 0;
  if (!m_qualityName.empty()) m_qualityID = hltInfoID(m_qualityName);
  m_quality2ID = 0;
  if (!m_quality2Name.empty()) m_quality2ID = hltInfoID(m_quality2Name);
  
  info() << " Reco: " << name() 
         << " Tool: " << m_toolName
         << " Output: " << m_TESOutput << endmsg;

  debug() << " reco ID " << m_recoID
          << " transfers IDs " << m_transferIDs 
          << " transfer ancestor " << m_transferAncestor
          << " track type " << m_trackType 
          << " quality [1] " << m_qualityName << " " << m_qualityID
          << " quality [2] " << m_quality2Name << " " << m_quality2ID
          << endmsg;
  
  m_tool = tool<ITrackMatch>(m_toolName,this);
  
  if (produceHistos()){
    m_qualityHisto = initializeHisto(m_qualityName,-50,50,1000);
    m_qualityHistoBest = initializeHisto(m_qualityName+"Best",-50,50,1000);
    if (!m_quality2Name.empty()) {
    m_quality2Histo = initializeHisto(m_quality2Name,-50,50,1000);
    m_quality2HistoBest = initializeHisto(m_quality2Name+"Best",-50,50,1000);
    }
  }

  
  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackMatch::execute() {

  debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;

  //TODO: must move this into a layer of public tool, like is done in TrackUpgrade...
  //      in order to guarantee that there is a single (tool) instance corresponding
  //      to each TES location
  // Fortunately, things are not so bad here, as the TES is not used to cache information,
  // just to dump/transfer ownership into....
  Tracks* otracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_TESOutput);

  BOOST_FOREACH(const LHCb::Track* track1, *m_selections.input<1>()) {
    if (msgLevel(MSG::VERBOSE)) printInfo(" track [1] ",*track1);
    BOOST_FOREACH(const LHCb::Track* track2, *m_selections.input<2>()) {
      if (msgLevel(MSG::VERBOSE)) printInfo(" track [2] ",*track2);

      Track otrack(track1->key());

      double quality = 0.;
      double quality2 = 0.;
      sc = m_tool->match(*track1,*track2,otrack,quality,quality2);
      if (sc.isFailure()) {
        // Warning(" matching failed ",0);
        continue;
      }

      if (produceHistos()) {
	fill(m_qualityHisto,quality,1.);
     if (m_quality2Histo) fill(m_quality2Histo,quality2,1.);
      }
      
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << " track [1+2] quality " 
                  << quality << ", " << quality2 << endmsg;
        printInfo(" track [1+2] ",otrack);
      }   
        
      if ((quality<m_maxQuality) && (quality2 < m_maxQuality2)) {
        if (msgLevel(MSG::VERBOSE)) verbose() << " accepted track [1+2] " << endmsg;
        LHCb::Track* track3 = otrack.clone();
        if (m_qualityID  != 0) track3->addInfo(m_qualityID,quality);
        if (m_quality2ID != 0) track3->addInfo(m_quality2ID,quality2);
        if (m_transferInfo) {
          Hlt::MergeInfo(*track1,*track3);
          Hlt::MergeInfo(*track2,*track3);
        }  
        otracks->insert( track3 );
        m_selections.output()->push_back(track3);
      }

    }
  }
  
  if (produceHistos() && otracks->size() > 0){
    std::vector<double> vals; 
    vals.reserve(otracks->size());
    BOOST_FOREACH( LHCb::Track* cand, *otracks) {
        if (fabs(cand->info(m_qualityID,1e6)) >1e-6) 
            vals.push_back( fabs(cand->info(m_qualityID,1e6)));
    }
    double val = *( std::min_element(vals.begin(),vals.end()) );
    fill(m_qualityHistoBest,val,1.); 
    vals.clear();
    BOOST_FOREACH( LHCb::Track* cand, *otracks) {
        if (fabs(cand->info(m_quality2ID,1e6)) >1e-6)
            vals.push_back( fabs(cand->info(m_quality2ID,1e6)));
    }
    val = *( std::min_element(vals.begin(),vals.end()) );
    fill(m_quality2HistoBest,val,1.); 
  }
  

  if (msgLevel(MSG::DEBUG)) printInfo(" matched tracks ",*m_selections.output());

  return StatusCode::SUCCESS;
}

