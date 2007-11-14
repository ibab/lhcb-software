// $Id: HltTrackMaker.cpp,v 1.2 2007-11-14 13:57:03 hernando Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltTrackMaker.h"
#include "Event/HltNames.h"
#include "HltBase/HltUtils.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/EDictionary.h"
#include "HltBase/EParser.h"
#include "HltBase/IHltFunctionFactory.h"
#include "HltBase/HltConfigurationHelper.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackMaker
//
// 2006-02-21 : Hugo Ruiz
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackMaker );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackMaker::HltTrackMaker( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty( "TrackMakerTool",  
                   m_trackMakerName = "empty");

  declareProperty("FilterDescriptor", m_filterDescriptor);
}



//=============================================================================
// Destructor
//=============================================================================
HltTrackMaker::~HltTrackMaker() {
  for (std::vector<Hlt::TrackBiFunction*>::iterator it = m_functions.begin();
       it != m_functions.end(); ++it) delete *it;
  for (std::vector<Hlt::Filter*>::iterator it = m_filters.begin();
       it != m_filters.end(); ++it) delete *it;
};

//=============================================================================
// Initialization
//=============================================================================

StatusCode HltTrackMaker::initialize() {

  // NOTE: input tracks2 needs to be defined to book histos
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorithm
  
  checkInput(m_inputTracks," input tracks ");
  checkInput(m_inputTracks2," input tracks2 ");
  checkInput(m_outputTracks," output tracks ");

  // get track maker
  m_trackMaker = NULL;
  if (m_trackMakerName == "empty")
    fatal() << " No declared track Maker tool " << endreq;
  m_trackMaker = tool<ITrackMatchUpgrade>(m_trackMakerName);    
  if (!m_trackMaker)
    fatal() << " No ITrackMatchUpdate tool" << endreq;
  
  // create filters
  IHltFunctionFactory* factory = 
    tool<IHltFunctionFactory>("HltFunctionFactory",this);

  const std::vector<std::string>& hdes = m_filterDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string filtername = *it;
    std::string funname = "";
    std::string mode = "";
    float x0 = -1.e6;
    float xf =  1.e6;
    if (!EParser::parseFilter(filtername,funname,mode,x0,xf)) continue;

    m_filterNames.push_back(funname);

    int id = HltConfigurationHelper::getID(*m_conf,"InfoID",funname);
    Hlt::TrackBiFunction* fun = factory->trackBiFunction(funname);
    m_functions.push_back(fun);
    m_filterIDs.push_back(id);
    if (!fun) error() << " error crearing function " << filtername 
                      << " " << id << endreq;

    Hlt::Filter* fil = NULL;
    if (mode == "<") fil = new Estd::less<double>(x0);
    else if (mode == ">") fil = new Estd::greater<double>(x0);
    else fil = new Estd::in_range<double>(x0,xf);
    m_filters.push_back(fil);
    if (!fil) error() << " error crearing filter " << filtername 
                      << " " << id << endreq;

    m_tcounters.push_back(0);   

    if (m_histogramUpdatePeriod>0) {
      HltHisto histo = NULL;
      initializeHisto(histo,filtername,0.,100.,100);
      m_histos.push_back(histo);
    }

    debug() << " filter " << filtername << " " << id << " "
            << mode << x0 << "," << xf << endreq;
  }
  release(factory);

  return StatusCode::SUCCESS;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_debug ) debug() << "HltTrackMaker: Execute" << endmsg;
  m_outputTracks->clear();

  Tracks* otracks = new Tracks();
  put(otracks,m_outputTracksName);

  if (m_debug) {
    printInfo( "tracks [1]", *m_inputTracks);
    printInfo( "tracks [2]", *m_inputTracks2);
  }
  
  for (Hlt::TrackContainer::const_iterator itM = m_inputTracks->begin(); 
       itM != m_inputTracks->end(); itM++) {
    const LHCb::Track& track1 = *(*itM);

    verbose() << " track 0 " << track1.key() << track1.slopes() << endreq;

    for (Hlt::TrackContainer::const_iterator itH = m_inputTracks2->begin(); 
         itH != m_inputTracks2->end(); itH++) {
      const LHCb::Track& track2 = *(*itH);
      verbose() << " track 1 " << track2.key() << track2.slopes() << endreq;
      
      m_vals.clear();
      bool accepted = true;
      for (size_t i = 0; i < m_functions.size(); ++i) {
        verbose() << " filter " << m_filterNames[i] << endreq;
        Hlt::TrackBiFunction& fun = *(m_functions[i]);
        Hlt::Filter& fil = (*m_filters[i]);
        double val = (fun)(track1,track2);
        accepted = (fil)(val);
        verbose() << " value " << val << " accepted? " << accepted << endreq;
        if (m_monitor) fillHisto(m_histos[i],val,1.);
        if (!accepted) break;
        m_tcounters[i] += 1;
        m_vals.push_back(val);
      }
      
      if (!accepted) continue;

      verbose()<<" match found [0] "<<track1.key() <<track1.slopes() << endreq;
      verbose()<<" match found [1] "<<track2.key() <<track2.slopes() << endreq;

      Track* otrack = new Track();
      double quality = 0.;
      sc = m_trackMaker->match(track1,track2,*otrack,quality);
      if (sc.isSuccess()) {
        otracks->insert(otrack);
        m_outputTracks->push_back(otrack);
        for (size_t i = 0; i < m_filterIDs.size(); ++i) {
          verbose()<< " info " << m_filterIDs[i]<<" = "<<m_vals[i] << endreq;
          otrack->addInfo(m_filterIDs[i],m_vals[i]);
        }
        if (m_debug)
          printInfo(" make track ",*otrack);
      } else delete otrack;
      
    } // loop on tracks2
  } // loop on tracks1
  
  return StatusCode::SUCCESS;  
}

void HltTrackMaker::saveConfiguration() {
  HltAlgorithm::saveConfiguration();

  std::string type = "HltTrackMaker";
  confregister("Type",type);

  const std::vector<std::string>& filters = m_filterDescriptor.value();
  confregister("Filters",filters);

  confregister("TrackMaker",m_trackMakerName);
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackMaker::finalize() {

  for (size_t i = 0; i < m_filterNames.size(); ++i) {
    std::string title =  " event accepted " + m_filterNames[i] + " / input ";
    infoSubsetEvents(m_tcounters[i],m_counterEntries,title);
  }

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorith  
  
  return StatusCode::SUCCESS;
  
}



