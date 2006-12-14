// $Id: HltAlgorithm.cpp,v 1.4 2006-12-14 11:21:35 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IHltDataStore.h"
#include "HltBase/ParserDescriptor.h"

#include "HltBase/ESequences.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltAlgorithm::HltAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "NoFilterPeriod", m_noFilterPeriod = 0);  
  declareProperty( "HistogramUpdatePeriod" , m_histogramUpdatePeriod = 0 );

  // location of the summary and the summary box name
  declareProperty("SummaryName",
                  m_summaryName = LHCb::HltSummaryLocation::Default);
  declareProperty("SelectionName",
                  m_selectionSummaryName = "");
  declareProperty("IsTrigger", m_isTrigger = false);
  
  // location of the input tracks, primary vertices and vertices
  declareProperty("PatInputTracksName",     m_patInputTracksName = "");
  declareProperty("PatInputTracks2Name",     m_patInputTracks2Name = "");
  declareProperty("PatInputVerticesName",     m_patInputVerticesName = "");

  // location of the input tracks, primary vertices and vertices
  declareProperty("InputTracksName",     m_inputTracksName = "");
  declareProperty("InputTracks2Name",    m_inputTracks2Name = "");
  declareProperty("InputVerticesName",   m_inputVerticesName = "");
  declareProperty("PrimaryVerticesName", m_primaryVerticesName = "");

  // output location for tracks and vertices
  declareProperty("OutputTracksName",    m_outputTracksName = "");
  declareProperty("OutputverticesName",  m_outputVerticesName = "");
  
  // location of the condition DB xml input
  declareProperty("ConditionsName",      m_conditionsName = "");

  declareProperty("HistoDescriptor", m_histoDescriptor);

  m_selectionSummaryID = -1;

}
//=============================================================================
// Destructor
//=============================================================================
HltAlgorithm::~HltAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltAlgorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  initMsg();

  if (m_selectionSummaryName != "") {
    info() << " selection summary Name " << m_selectionSummaryName << endreq;
    m_selectionSummaryID = 
      HltNames::selectionSummaryID(m_selectionSummaryName);
    info() << " selection summary ID " << m_selectionSummaryID << endreq;
  }
  
  bool ok = true;
  ok = initContainers();
  if (!ok) return stop(" problems with Containers ");

  initCounters();

  initHistograms();
  
  initConditions();

  m_debug = (outputLevel() >= MSG::DEBUG);

  debug() << "==> Initialize" << endreq;
  return StatusCode::SUCCESS;

}

void HltAlgorithm::init(Hlt::TrackContainer*& con,
                        IHltDataStore*& store,
                        const std::string& name) {
  con = NULL;
  if (name != "") {
    con = &(store->tracks(name));
    debug() << " initialize container " << name << endreq;
  } 
}

void HltAlgorithm::init(Hlt::VertexContainer*& con,
                        IHltDataStore*& store, const std::string& name) {
  con = NULL;
  if (name != "") { con = &(store->vertices(name));
  debug() << " initialize container " << name << endreq;} 
}

void HltAlgorithm::init(PatTrackContainer*& con,
                        PatDataStore*& store,const std::string& name) {
  con = NULL;
  if (name != "") {con = store->tracks(name);
  debug() << " initialize pat container " << name << endreq;} 
}

void HltAlgorithm::init(PatVertexContainer*& con,
                        PatDataStore*& store, const std::string& name) {
  con = NULL;
  if (name != "") { con = store->vertices(name);
  debug() << " initialize pat container " << name << endreq;} 
}

bool HltAlgorithm::initContainers() {
  
  // HltContainers of tracks and vertices
  //--------------------------------------
  m_hltDataStore     = tool<IHltDataStore>( "HltDataStore" );
  if (!m_hltDataStore) return false;
  
  // get the input tracks and vertices containers 
  init(m_inputTracks,m_hltDataStore,m_inputTracksName);
  init(m_inputTracks2,m_hltDataStore,m_inputTracks2Name);

  init(m_primaryVertices,m_hltDataStore,m_primaryVerticesName);
  init(m_inputVertices,m_hltDataStore,m_inputVerticesName);

  init(m_outputTracks,m_hltDataStore,m_outputTracksName);
  init(m_outputVertices,m_hltDataStore,m_outputVerticesName);

  //  release(m_hltDataStore);  

  m_patDataStore = tool<PatDataStore>("PatDataStore");
  if (!m_hltDataStore) return false;
  
  init(m_patInputTracks,m_patDataStore,m_patInputTracksName);
  init(m_patInputTracks2,m_patDataStore,m_patInputTracks2Name);
  init(m_patInputVertices,m_patDataStore,m_patInputVerticesName);
  
  // release(patstore);

  
  infoContainer(m_patInputTracks," pat input tracks ",m_patInputTracksName);
  infoContainer(m_patInputTracks2," pat input tracks 2",m_patInputTracks2Name);
  infoContainer(m_patInputVertices," pat input vertices ",
                m_patInputVerticesName);
  
  infoContainer(m_inputTracks,   " input tracks ",   m_inputTracksName);
  infoContainer(m_inputTracks2,  " input tracks 2 ", m_inputTracks2Name);
  infoContainer(m_inputVertices, " input vertices ", m_inputVerticesName);
  infoContainer(m_outputTracks,  " output track ",   m_outputTracksName);
  infoContainer(m_outputVertices," output vertices ",m_outputVerticesName);

  return true;
}

void HltAlgorithm::initCounters() 
{

  // Input
  initializeCounter(m_countInput ,        "Input");
  initializeCounter(m_countNonEmpty ,     "NonEmpty");
  initializeCounter(m_countEmptyTracks ,  "EmptyTracks");
  initializeCounter(m_countEmptyTracks2,  "EmptyTracks2");
  initializeCounter(m_countInputVertices, "InputVertices");
  initializeCounter(m_countEmptyVertices, "EmptyVertices");
  initializeCounter(m_countInputPVs,      "InputPVs");
  initializeCounter(m_countEmptyPVs,      "EmptyPVs");
  initializeCounter(m_countInputTracks ,  "InputTracks");
  initializeCounter(m_countInputTracks2,  "Onputtracks2");
  // Output
  initializeCounter(m_countCandidates ,   "Candidates");
  initializeCounter(m_countOverruled ,    "Overruled");
  initializeCounter(m_countAccepted ,     "Accepted");
  initializeCounter(m_countOutputVertices,"OutputVertices");
  initializeCounter(m_countOutputTracks , "OutputTracks");
  initializeCounter(m_countOutputTracks2, "Onputtracks2");

  // new counters
  initializeCounter(m_counterEntries,  "Entries");
  initializeCounter(m_counterInput,    "Input");
  initializeCounter(m_counterPasses,   "Passes");
  
  
}

void HltAlgorithm::initMsg() {

  m_verbose = false;
  m_debug   = false;
  m_info    = false;
  m_warning = false;
  m_error   = false;
  m_fatal   = true;

  if (msgLevel(MSG::ERROR)) {m_error = true;}
  if (msgLevel(MSG::WARNING)) {m_error = true; m_warning = true;}
  if (msgLevel(MSG::INFO)) 
  {m_error = true; m_warning = true; m_info = true;}
  if (msgLevel(MSG::DEBUG)) 
  {m_error = true; m_warning = true; m_info = true; m_debug = true;}
  if (msgLevel(MSG::VERBOSE)) 
  {m_error=true; m_warning = true; m_info = true; m_debug = true; m_verbose=true;}

  info() << " msg level " << m_info << m_debug << m_verbose << endreq;
}


void HltAlgorithm::initHistograms() {

  if (m_histogramUpdatePeriod == 0) return;

  // Input
  if (m_primaryVertices)
    initializeHisto(m_histoInputPVs,      "PrimaryVertices");
  if (m_inputVertices)
    initializeHisto(m_histoInputVertices, "InputVertices");
  if (m_inputTracks)
    initializeHisto(m_histoInputTracks,   "InputTracks");
  if (m_inputTracks2)
    initializeHisto(m_histoInputTracks2,  "InputTracks2");

  if (m_patInputTracks)
    initializeHisto(m_histoPatInputTracks, "PatInputTracks");
  if (m_patInputTracks2)
    initializeHisto(m_histoPatInputTracks2, "PatInputTracks2");
  if (m_patInputVertices)
    initializeHisto(m_histoPatInputVertices, "PatInputVertices");

  // Output
  initializeHisto(m_histoCandidates ,   "Candidates");
  if (m_outputVertices)
    initializeHisto(m_histoOutputVertices,"OutputVertices");
  if (m_outputTracks)
    initializeHisto(m_histoOutputTracks,  "OutputTracks");
}

bool HltAlgorithm::initConditions() 
{  
  // for conditions
  //----------------------------------
  m_conditions=NULL;
  if (m_conditionsName != "") {
    try {
      registerCondition(m_conditionsName, m_conditions, 
                        &HltAlgorithm::i_cacheConditions);
      info() << " searching conditions at " << m_conditionsName << endreq;
      m_conditions = getDet<Condition>(m_conditionsName);
      if (m_conditions == NULL)
        info() << " no condition found!" << endreq;
      else
        i_cacheConditions();
    } catch (GaudiException) {
      fatal() << " Problem with conditions at " << m_conditionsName << endreq;
    }
  }
  return true;
}

//// Main execution

//// Main execution
StatusCode HltAlgorithm::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  bool ok = beginExecute();
  if (!ok) return sc;
  
  endExecute();
  
  return sc;
}


bool HltAlgorithm::beginExecute() {
  bool ok = true;

  increaseCounter( m_counterEntries );

  m_summary = NULL;
  m_selectionSummary = NULL;
  if (m_selectionSummaryID>0) selectionSummary();
  
  m_nCandidates = 0;
  
  m_filter =( m_noFilterPeriod <= 0? true:
              ( !(m_counterEntries % m_noFilterPeriod) == 0 ) );

  m_monitor =( m_histogramUpdatePeriod <= 0? false:
               ( (m_counterEntries % m_histogramUpdatePeriod) == 0 ) );

  if (m_filter) setDecision(false);
    
  ok = size(m_inputTracks,m_nInputTracks,m_histoInputTracks,
            " input tracks ");
  if (!ok) return ok;

  ok = size(m_inputTracks2,m_nInputTracks2,m_histoInputTracks2,
            " input tracks 2 ");
  if (!ok) return ok;

  ok = size(m_patInputTracks,m_nPatInputTracks,m_histoPatInputTracks,
            " pat input tracks ");
  if (!ok) return ok;

  ok = size(m_patInputTracks2,m_nPatInputTracks2,m_histoPatInputTracks2,
            " pat input tracks 2");
  if (!ok) return ok;

  ok = size(m_primaryVertices,m_nPrimaryVertices,m_histoInputPVs,
            " input primary vertices ");
  if (!ok) return ok;

  ok = size(m_inputVertices,m_nInputVertices,m_histoInputVertices,
            " input vertices ");
  if (!ok) return ok;

  ok = size(m_patInputVertices,m_nPatInputVertices,m_histoPatInputVertices,
            " pat input vertices ");
  if (!ok) return ok;

  increaseCounter(m_counterInput );

  // Increase input counters
  increaseCounter( m_countNonEmpty );
  increaseCounter( m_countInputVertices, m_nInputVertices );
  increaseCounter( m_countInputPVs,      m_nPrimaryVertices );
  increaseCounter( m_countInputTracks,   m_nInputTracks );
  increaseCounter( m_countInputTracks2,  m_nInputTracks2 );

  return ok;
}

bool HltAlgorithm::endExecute() {
  bool ok = true;

  // fillHisto(m_histoCandidates, m_nCandidates, 1.);

  ok = size(m_outputTracks,m_nOutputTracks,m_histoOutputTracks,
            " output tracks ");
  if (!ok) return ok;

  ok = size(m_outputVertices,m_nOutputVertices,m_histoOutputVertices,
            " output vertices ");
  if (!ok) return ok;

  setDecision(true);

  increaseCounter(m_counterPasses);

  return ok;
}

////  Finalize
StatusCode HltAlgorithm::finalize() {

  infoTotalEvents   ( m_counterEntries );
  infoSubsetEvents  ( m_counterInput , m_counterEntries, " input/entries ");
  infoSubsetEvents  ( m_counterPasses , m_counterInput,  " passed/inputs ");

  std::vector<HltCounter> counters;
  counters.push_back(m_counterEntries);
  counters.push_back(m_counterInput );
  counters.push_back(m_counterPasses);

  int offset = m_counterEntries;
  for (std::vector<HltCounter>::iterator it = counters.begin();
       it != counters.end(); ++it) {
    const HltCounter& counter = (*it);
    const std::string& name = counter.m_name;
    plot1D(1.,name,0.,2.,2,counter);
    plot1D(0.,name,0.,2.,2,offset-counter);
    offset = counter;
  }

  debug() << "==> Finalize" << endreq;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


StatusCode HltAlgorithm::i_cacheConditions() {
  
  // m_conditions = getDet<Condition>(m_conditionsName);
  if (m_conditions == NULL)
    error() <<" Not posible to condition conditions " << endreq;

  // cacheConditions(m_iconditions,*m_condition);
  // cacheConditions(m_dconditions,*m_condition);
  // cacheConditions(m_dvconditions,*m_condition);
  
  for (std::map<std::string, int*>::iterator it = m_iconditions.begin();
       it != m_iconditions.end(); it++) {
    const std::string& name = it->first;
    *(it->second) = m_conditions->param<int>(name);
    info() << " condition " << name << " = \t" << *(it->second) << endreq;
    
  }
  
  for (std::map<std::string, double*>::iterator it = m_dconditions.begin();
       it != m_dconditions.end(); it++) {
    const std::string& name = it->first;
    *(it->second) = m_conditions->param<double>(name);
    info() << " condition " << name << " = \t" << *(it->second) << endreq;
  }

  for (std::map<std::string, std::vector<double>*>::iterator it = 
         m_dvconditions.begin(); it != m_dvconditions.end(); it++) {
    const std::string& name = it->first;
    *(it->second) = m_conditions->param< std::vector<double> >(name);
    info() << " condition " << name << " = \t" << *(it->second) << endreq;
  }


  return StatusCode::SUCCESS;
}


//=====================================================================

void HltAlgorithm::infoTotalEvents( int  nTotEvts )
{info() << "N Entries:\t" << nTotEvts << endreq;}

void HltAlgorithm::infoSubsetEvents(  int nEventsInSubset, int  nTotEvts, 
                                 const std::string& subsetName)
{
  info() << "N " <<subsetName << "\t" << nEventsInSubset 
         << " (" << 100.*float(nEventsInSubset)/float(nTotEvts)
         << "%)" << endreq;
}

void HltAlgorithm::infoInputObjects(  int nInputObjects, int  nTotEvts, 
                                 const std::string& objectsName)
{
  info() << "N " << objectsName << "\t" << nInputObjects
         << "\t(" << float(nInputObjects)/float(nTotEvts)
         << " per event)" << endreq;
}
void HltAlgorithm::infoAcceptedObjects(  int nAcceptedObjects, int nInputObjects, 
                                    int  nTotEvts,
                                    const std::string& objectsName)
{
  info() << "N " << objectsName << "\t" << nAcceptedObjects
         << "\t(" << float(nAcceptedObjects)/float(nTotEvts)
         << " per event)\t("  << 100.*float(nAcceptedObjects)/float(nInputObjects)
         << " %)" << endreq;
}


void HltAlgorithm::increaseCounter( HltCounter& count, int increase) {
  if ( ! count.m_initialized ) {
    fatal() << "Counter not initialized:" << endreq;
    return;
  }
  
  count.m_counter += increase;
  return;
}


//=============================================================================
void HltAlgorithm::fillHisto( HltHisto& histo, float x, float weight) {
  // if ( ! histo.m_initialized ) {
//     fatal() << "Histo not initialized:" << endreq;
//     return;
//   }
  
//   int period;
  
//   if ( histo.m_updatePeriod > 0 ) {
//     period = histo.m_updatePeriod;
//   }
//   else {
//     period = this->m_histogramUpdatePeriod;
//   }
  
  
//   if ( m_countInput % period == 0 ) {
  if (!m_monitor) return;
  this->fill( histo.m_histo , x, weight);
}
  

void HltAlgorithm::fillHistoCol( HistoCol& histoCol, 
                                 const std::vector<bool>& inputBools,
                                 const std::vector<float>& inputVariables, 
                                 float weight) {
  
  if ( ! histoCol.m_initialized ) {
    fatal() << "Histo collection not initialized:" << endreq;
    return;
  }  
  
  if ( m_countInput % this->m_histogramUpdatePeriod  == 0 ) {

    if (inputBools.size() != histoCol.m_nCases ) {
      fatal() << "Size of bool vector does not match number of cases of histogram collection!" << endreq;
      return;
    }
    
    if (inputVariables.size() != histoCol.m_nVariables ) {
      fatal() << "Size of vector of variables does not match number of variables of histogram collection!" << endreq;
      return;
    }
    
    std::vector<AIDA::IHistogram1D*>::const_iterator iHisto = histoCol.m_histos.begin();
    for ( std::vector<float>:: const_iterator iVar = inputVariables.begin();
          iVar != inputVariables.end(); iVar++ ) {
      for ( std::vector<bool>:: const_iterator iBool = inputBools.begin();
            iBool != inputBools.end(); iBool++ ) {
        if (*iBool)  this->fill( (*iHisto), (*iVar), weight);
        iHisto++;
      }
    }
    
  }
}

void HltAlgorithm::initializeHisto( HltHisto& histo, 
                                    const std::string& title ) {
  int nbins = 100;
  float x0 = 0.;
  float xf = 100.;
  initializeHisto(histo,title,x0,xf,nbins);
};


void HltAlgorithm::initializeHistoCol( HistoCol& histoCol, 
                                       const std::string& inputString,
                                       const std::vector<std::string>& inputVariables,
                                       const std::vector<std::string>& inputCases ) { 
  
  histoCol.m_initialized = true;
  histoCol.m_name = inputString;
  histoCol.m_nCases = inputCases.size();
  histoCol.m_nVariables = inputVariables.size();
  
  for ( std::vector<std::string>::const_iterator iVariable = inputVariables.begin();
        iVariable != inputVariables.end(); iVariable++ ) {
    
    for ( std::vector<std::string>::const_iterator iCase = inputCases.begin();
          iCase != inputCases.end(); iCase++ ) {
      histoCol.m_histos.push_back( this->book1D( inputString + (*iVariable) + (*iCase), 0., 100., 100 ) );
    }
  }
};


void HltAlgorithm::initializeHisto( HltHisto& histo, 
                                    const std::string& title,
                                    float x0, float xf, int nbins) {
  
  const std::vector<std::string> values = m_histoDescriptor.value();
  for (std::vector<std::string >::const_iterator it = values.begin();
       it != values.end(); ++it) {
    const std::string& des = *it;
    std::string xtitle = "";
    int n = 100;
    float y0 = 0.;
    float yf = 100.;
    bool ok =  ParserDescriptor::parseHisto1D(des,xtitle,n,y0,yf);
    if (ok && xtitle == title) { nbins = n; x0 = y0; xf = yf;}
  }
  histo.m_initialized = true;
  histo.m_name = title;
  histo.m_histo = this->book1D( title, x0, xf, nbins);
  info() << " booking histo  " << title 
         << "( "<< nbins << " , "<< x0 <<" , " << xf << ") " 
         << endreq;
};


void HltAlgorithm::initializeCounter ( HltCounter& counter, 
                                       const std::string& inputString) { 
  
  counter.m_initialized = true;
  counter.m_name = inputString;
  counter.m_counter = 0;
};



