// $Id: HltVertexUpgrade.cpp,v 1.23 2010-08-17 08:41:18 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "boost/foreach.hpp"

// local
#include "HltVertexUpgrade.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexUpgrade );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexUpgrade::HltVertexUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
  , m_quality1Histo(0)
  , m_quality2Histo(0)
  , m_quality1HistoBest(0)  
  , m_quality2HistoBest(0)
{
  declareProperty("RecoName", m_recoName = "empty");
  
  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  
  declareProperty( "TESOutputVerticesName",  
                   m_TESOutputVerticesName = "Hlt1/Vertex/VertexUpgradeBank");

  m_selections.declareProperties();
};
//=============================================================================
// Destructor
//=============================================================================
HltVertexUpgrade::~HltVertexUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltVertexUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  
  m_tool = tool<ITrackUpgrade>("HltTrackUpgradeTool",this);
  if (!m_tool) fatal() << " not able to retrieve upgrade track tool " << endmsg;
  sc = m_tool->setReco(m_recoName);
  
 if (produceHistos()){
    m_quality1Histo = initializeHisto(m_recoName+"1Quality",0,50,200);
    m_quality2Histo = initializeHisto(m_recoName+"2Quality",0,50,200);
    m_quality1HistoBest = initializeHisto(m_recoName+"1QualityBest",0,50,200);    
    m_quality2HistoBest = initializeHisto(m_recoName+"2QualityBest",0,50,200);
  }
  
 return sc;

};

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltVertexUpgrade::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  RecVertices* overtices = getOrCreate<RecVertices,RecVertices>(m_TESOutputVerticesName);

  verbose()<<"Trying vertex upgrade "<<m_recoName<<endmsg;

  const Hlt::VertexSelection *input = m_selections.input<1>();
  for (Hlt::VertexSelection::const_iterator it=input->begin(); it!=input->end(); ++it) {
    const RecVertex* vseed = *it;
    const SmartRefVector<Track>& otracks = vseed->tracks();
    const Track* seed1 =  *otracks.begin();     assert(otracks.begin()!=otracks.end());
    const Track* seed2 =  *(otracks.begin()+1); assert((otracks.begin()+1)!=otracks.end());
    debug() << " -- updating vertex -- " << endmsg;
    if (msgLevel(MSG::DEBUG)) {
      printInfo(" seed1 ", *seed1);
      printInfo(" seed2 ", *seed2);
    }
    std::vector<LHCb::Track*> tracks1, tracks2;
    debug() << " calling update for track1" << endmsg;
    sc = m_tool->upgrade(const_cast<Track&>(*seed1),tracks1);
    if (sc.isFailure()) {
        always() << "Failed to update track!" << endmsg;
        return sc;
    }
    debug() << " calling update for track2" << endmsg;
    sc = m_tool->upgrade(const_cast<Track&>(*seed2),tracks2);
    if (sc.isFailure()) { 
        always() << "Failed to update track!" << endmsg;
        return sc;
    }
    if (tracks1.empty() || tracks2.empty() ) continue;
    debug() << " creating a vertex " << endmsg;
   
    for (std::vector<Track*>::iterator t1 = tracks1.begin();
         t1 != tracks1.end(); ++t1) {
      for (std::vector<Track*>::iterator t2 = tracks2.begin();
           t2 != tracks2.end(); ++t2) {
        RecVertex* sv = new RecVertex();
        overtices->insert(sv);
        _makeVertex(**t1,**t2,*sv);
        sv->setExtraInfo(vseed->extraInfo());
        m_selections.output()->push_back(sv);
        debug() << " created vertex " << endmsg;
        if (msgLevel(MSG::DEBUG)) printInfo(" vertex ",*sv);
        // TODO - wait till RecVertex is done
        // if (m_transferExtraInfo)
        //   sv->setExtraInfo( vseed->extraInfo() );
      }
    }

    if( produceHistos() && !tracks1.empty()){
      std::vector<double> vals; 
      vals.reserve(tracks1.size());
      BOOST_FOREACH( LHCb::Track* cand, tracks1) vals.push_back( getTrackQuality( *cand ) );
      double val = *( std::min_element(vals.begin(),vals.end()) );
      BOOST_FOREACH(const double& x,vals  ) fill( m_quality1Histo,x,1. );
      fill( m_quality1HistoBest,val,1. );
    }
    if( produceHistos() && !tracks2.size()){
      std::vector<double> vals; 
      vals.reserve(tracks2.size());
      BOOST_FOREACH( LHCb::Track* cand, tracks2) vals.push_back( getTrackQuality( *cand ) );
      double val = *( std::min_element(vals.begin(),vals.end()) );
      BOOST_FOREACH(const double& x,vals  ) fill( m_quality2Histo,x,1. );
      fill( m_quality2HistoBest,val,1. );
    }

  }

  debug()<<"Upgrade vertices succesful"<<endmsg;
  return sc;
}

//TODO JA: code is duplicated from HltTrackUpgrade --> put in one location
double HltVertexUpgrade::getTrackQuality(const LHCb::Track& tr)
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
