// $Id: HltTrackUpgrade.cpp,v 1.18 2010-08-17 08:41:18 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "boost/foreach.hpp"

// local
#include "HltTrackUpgrade.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackUpgrade );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackUpgrade::HltTrackUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
  , m_qualityHisto(0)
  , m_qualityHistoBest(0)
{
  declareProperty("RecoName", m_recoName = "<UNKNOWN>");  
  m_selections.declareProperties();
};
//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgrade::~HltTrackUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_selections.retrieveSelections();
  // ownership: see TrackUpgradeTool: it always assumes ownership,
  // and then transfers it to the TES...
  m_selections.registerSelection();

  m_tool = tool<ITrackUpgrade>("HltTrackUpgradeTool",this);
  sc = m_tool->setReco(m_recoName);
  if (sc.isFailure()) return sc;
  
  if (produceHistos()){
    m_qualityHisto = initializeHisto(m_recoName+"Quality",0,50,200);
    m_qualityHistoBest = initializeHisto(m_recoName+"QualityBest",0,50,200);
  }
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackUpgrade::execute() {

  //@TODO: adapt interface to be more neutral: input a vector<Track*>::const_iterator begin,end, and a back_inserter
  //       don't rely on tool doing pt ordering...
  std::vector<const LHCb::Track*> in(m_selections.input<1>()->begin(),m_selections.input<1>()->end());
  assert(m_selections.output()->empty());
  std::vector<LHCb::Track*> out;
  StatusCode sc = m_tool->upgrade(in,out);
  m_selections.output()->insert(m_selections.output()->end(),out.begin(),out.end());
  
  if (msgLevel(MSG::DEBUG)) printInfo(" upgraded tracks ",*m_selections.output());
  
  if( produceHistos() && out.size() > 0){
    std::vector<double> vals; 
    vals.reserve(out.size());
    BOOST_FOREACH(const LHCb::Track* cand, out) vals.push_back( getTrackQuality( *cand ) );
    double val = *( std::min_element(vals.begin(),vals.end()) );
    BOOST_FOREACH(const double& x,vals  ) fill( m_qualityHisto,x,1. );
    fill( m_qualityHistoBest,val,1. );
  }
  return sc;
}

//=============================================================================

double HltTrackUpgrade::getTrackQuality(const LHCb::Track& tr)
{
  double q = 1e6;
  
  if( m_recoName == "Forward" 
      || m_recoName == "GuidedForward" ){
    q = tr.info(LHCb::Track::PatQuality,1e5);
  }
  else if( m_recoName == "TMuonConf"
	   || m_recoName == "THadronConf"
	   || m_recoName == "TEleConf"    ) {
    //later chi2 available
    q = tr.chi2PerDoF();
  }
  else if( m_recoName == "Velo" 
	   || m_recoName == "FitTrack" 
	   || m_recoName == "VeloTT"    ){
  
    q = tr.chi2PerDoF();
  }
  else{
  // RadCor
    debug()<<"no track quality available for type "<<m_recoName <<endmsg;
  }
  return q;
}
