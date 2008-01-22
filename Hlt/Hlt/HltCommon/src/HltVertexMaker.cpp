// $Id: HltVertexMaker.cpp,v 1.13 2008-01-22 09:56:44 hernando Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltVertexMaker.h"
#include "HltBase/EParser.h"
#include "HltBase/ESequences.h"

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexMaker
//
// 2006-02-21 : Hugo Ruiz
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexMaker );

using namespace LHCb;
using namespace boost::lambda;
// #namespace bl = boost::lambda;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexMaker::HltVertexMaker( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("CheckForOverlaps", m_checkForOverlaps = false );
  declareProperty("FilterDescriptor", m_filterDescriptor);

  m_inputTracks = 0;
  m_inputTracks2 = 0;
  m_outputVertices = 0;

  m_twoContainers = false;
  m_doInitSelections = false;
  m_considerInputs = false;
  m_algoType = "HltVertexMaker";
}



//=============================================================================
// Destructor
//=============================================================================
HltVertexMaker::~HltVertexMaker() {
  std::for_each(m_functions.begin(),m_functions.end(), delete_ptr());
  std::for_each(m_filters.begin(),m_filters.end(), delete_ptr());
};

//=============================================================================
// Initialization
//=============================================================================

StatusCode HltVertexMaker::initialize() {
  StatusCode sc = HltAlgorithm::initialize();

  m_inputTracks = &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));
  
  if (!m_inputSelection2Name.empty()) {
    m_inputTracks2 = 
      &(retrieveTSelection<LHCb::Track>(m_inputSelection2Name));
    m_twoContainers = true;
    debug() << " consider 2 container " << m_inputSelection2Name << endreq; 
  }

  m_outputVertices = 
    &(registerTSelection<LHCb::RecVertex>(m_outputSelectionName));

  IBiTrackFactory* factory = 
    tool<IBiTrackFactory>("HltTrackBiFunctionFactory",this);

  const std::vector<std::string>& hdes = m_filterDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string filtername = *it;
    std::string funname = "";
    std::string mode = "";
    float x0 = -1.e6;
    float xf =  1.e6;
    bool ok = EParser::parseFilter(filtername,funname,mode,x0,xf);
    Assert(ok," initialize() not able to parse filtername "+filtername);
    
    m_filterNames.push_back(funname);

    int id = hltInfoID(funname);
    Hlt::TrackBiFunction* fun = factory->function(funname);
    Assert( 0 !=  fun,  " initialize() function no created"+funname);
    m_functions.push_back(fun);
    m_filterIDs.push_back(id);


    Hlt::Filter* fil = 0;
    if (mode == "<") fil = new zen::less<double>(x0);
    else if (mode == ">") fil = new zen::greater<double>(x0);
    else fil = new zen::in_range<double>(x0,xf);
    Assert( 0 !=  fil,  " initialize() filter no created"+filtername);
    m_filters.push_back(fil);
    
    m_tcounters.push_back(0);   

    if (m_histogramUpdatePeriod>0) {
      Hlt::Histo* histo = initializeHisto(funname,0.,100.,100);
      m_histos.push_back(histo);
    }

    debug() << " filter " << filtername << " " << id << " "
            << mode << x0 << "," << xf << endreq;
  }
  release(factory);
  
  saveConfiguration();

  return sc;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_debug ) debug() << "HltVertexMaker: Execute" << endmsg;

  RecVertices* overtices = new RecVertices();
  put(overtices,"Hlt/Vertex/"+m_outputSelectionName);

  if ((!m_twoContainers && m_inputTracks->size() <2)) {
    debug() << " no enought tracks in container to make vertices " << endreq;
    return sc;
  } 
  if (m_twoContainers && (m_inputTracks->size() + m_inputTracks2->size()<2)) {
    debug() << " no enought tracks in container to make vertices " << endreq;
    return sc;
  }

  m_input2.clear();
  zen::copy(*m_inputTracks,m_input2);
  if (m_twoContainers) zen::copy(*m_inputTracks2,m_input2);

  if (m_debug) {
    printInfo( "tracks [1]", *m_inputTracks);
    printInfo( "tracks [2]",  m_input2);
  }

  // set the iterators
  Hlt::TrackContainer::const_iterator itMEnd = m_inputTracks->end();
  if (m_input2.size() == m_inputTracks->size()) itMEnd--;
  Hlt::TrackContainer::const_iterator itHStart = m_input2.begin();
  
  for (Hlt::TrackContainer::const_iterator itM = m_inputTracks->begin(); 
       itM != itMEnd; itM++) {
    const LHCb::Track& track1 = *(*itM);
    
    verbose() << " track 0 " << track1.key() << track1.slopes() << endreq;

    itHStart++;

    for (Hlt::TrackContainer::const_iterator itH = itHStart; 
         itH != m_input2.end(); itH++) {
      const LHCb::Track& track2 = *(*itH);
      verbose() << " track 1 " << track2.key() << track2.slopes() << endreq;
      
      // can not be the same track
      if ((*itH) == (*itM)) continue;

      increaseCounter(m_counterCombinations);

      // Check for segment overlaps
      bool accepted = true;
      if (m_checkForOverlaps)
        accepted = !(haveOverlaps(track1,track2));
      
      verbose() << " accepted due to overlaps?" << accepted << endreq;

      m_vals.clear();
      for (size_t i = 0; i < m_functions.size(); ++i) {
        verbose() << " filter " << m_filterNames[i] << endreq;
        Hlt::TrackBiFunction& fun = *(m_functions[i]);
        Hlt::Filter& fil = (*m_filters[i]);
        double val = (fun)(track1,track2);
        accepted = (fil)(val);
        verbose() << " value " << val << " accepted? " << accepted << endreq;
        if (m_monitor) fillHisto(*m_histos[i],val,1.);
        if (!accepted) break;
        m_tcounters[i] += 1;
        m_vals.push_back(val);
      }
      
      if (!accepted) continue;

      verbose()<<" pair found [0] "<<track1.key() <<track1.slopes() << endreq;
      verbose()<<" pair found [1] "<<track2.key() <<track2.slopes() << endreq;
      // Write vertex
      //if ( m_outputByVertex ) {
      LHCb::RecVertex* sv = new RecVertex();
      _makeVertex(track1,track2,*sv);
      m_outputVertices->push_back(sv);
      overtices->insert(sv);
      for (size_t i = 0; i < m_filterIDs.size(); ++i) {
        verbose()<< " info " << m_filterIDs[i]<<" = "<<m_vals[i] << endreq;
        sv->addInfo(m_filterIDs[i],m_vals[i]);
      }
      if (m_debug)
        printInfo(" make vertex ",*sv);
    } // loop on tracks2
  } // loop on tracks1
  
  return StatusCode::SUCCESS;  
}

void HltVertexMaker::saveConfiguration() {
  HltAlgorithm::saveConfiguration();
  const std::vector<std::string>& values = m_filterDescriptor.value();
  confregister("Filters",values);
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltVertexMaker::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); 
  
  info() << " N Combinations " << m_counterCombinations << endreq;
  for (size_t i = 0; i < m_filterNames.size(); ++i) {
    std::string title =  " accepted combinations " 
      + m_filterNames[i] + " / total ";
    infoSubsetEvents(m_tcounters[i],m_counterCombinations,title);
  }

  return sc;
  
}

//=============================================================================

bool HltVertexMaker::haveOverlaps( const LHCb::Track& track1, 
                                   const LHCb::Track& track2) {
  return HltUtils::matchIDs(track1,track2);
}


