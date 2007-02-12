// $Id: HltBaseAlg.cpp,v 1.6 2007-02-12 09:53:20 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "DetDesc/Condition.h"

// local
#include "HltBase/HltBaseAlg.h"
#include "HltBase/ParserDescriptor.h"
#include "HltBase/ESequences.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltBaseAlg
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltBaseAlg::HltBaseAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DefAlgo ( name , pSvcLocator )
{
  declareProperty( "PassPeriod", m_passPeriod = 0);  
  declareProperty( "HistogramUpdatePeriod" , m_histogramUpdatePeriod = 0 );

  
  // location of the condition DB xml input
  declareProperty("ConditionsName",      m_conditionsName = "");

  declareProperty("HistoDescriptor", m_histoDescriptor);

}
//=============================================================================
// Destructor
//=============================================================================
HltBaseAlg::~HltBaseAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltBaseAlg::initialize() {
  StatusCode sc = DefAlgo::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  initMsg();

  // counters
  initializeCounter(m_counterEntries,   "Entries");
  initializeCounter(m_counterOverruled, "Overruled");
  initializeCounter(m_counterPassed,    "Passed");

  // histos

  initConditions();

  debug() << "==> Initialize" << endreq;
  return StatusCode::SUCCESS;

}

void HltBaseAlg::initMsg() {

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

  debug() << " msg level " << m_info << m_debug << m_verbose << endreq;
}

bool HltBaseAlg::initConditions() 
{
  m_conditions=NULL;
  if (m_conditionsName != "") {
    try {
      registerCondition(m_conditionsName, m_conditions, 
                        &HltBaseAlg::i_cacheConditions);
      info() << " searching conditions at " << m_conditionsName << endreq;
      m_conditions = getDet<Condition>(m_conditionsName);
      if (m_conditions == NULL)
        info() << " no condition found!" << endreq;
      // else
      // i_cacheConditions();
    } catch (GaudiException) {
      fatal() << " Problem with conditions at " << m_conditionsName << endreq;
    }
  }
  return true;
}


StatusCode HltBaseAlg::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  bool ok = beginExecute();
  if (!ok) return sc;
  
  endExecute();
  
  return sc;
}


bool HltBaseAlg::beginExecute() {
  bool ok = true;

  increaseCounter( m_counterEntries );

  m_nCandidates = 0;
  
  m_filter =( m_passPeriod <= 0? true:
              ( !(m_counterEntries % m_passPeriod) == 0 ) );
  
  m_monitor =( m_histogramUpdatePeriod <= 0? false:
               ( (m_counterEntries % m_histogramUpdatePeriod) == 0 ) );
  
  if (m_filter) setDecision(false);
  else increaseCounter(m_counterOverruled,1);
    
  return ok;
}

bool HltBaseAlg::endExecute() {
  bool ok = true;

  setDecision(true);
  
  increaseCounter(m_counterPassed);

  return ok;
}

////  Finalize
StatusCode HltBaseAlg::finalize() {

  infoTotalEvents( m_counterEntries );
  infoSubsetEvents( m_counterPassed, m_counterEntries, " passed/entries");
  if (m_passPeriod > 0) {
    infoSubsetEvents( m_counterOverruled, m_counterEntries,
                      " overruled/entries");
    infoSubsetEvents( m_counterOverruled, m_counterPassed,
                      " overruled/passed");
  }
  return DefAlgo::finalize();  // must be called after all other actions
}


StatusCode HltBaseAlg::i_cacheConditions() {
  
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

void HltBaseAlg::infoTotalEvents( int  nTotEvts )
{info() << "N Entries: \t" << nTotEvts << endreq;}

void HltBaseAlg::infoSubsetEvents(  int nEventsInSubset, int  nTotEvts, 
                                    const std::string& subsetName)
{
  double f = (nTotEvts<=0? 0.: float(nEventsInSubset)/float(nTotEvts) );
  info() << "N " <<subsetName << "\t" << nEventsInSubset 
         << " (" << 100.*f << "%)" << endreq;
}

void HltBaseAlg::infoInputObjects(  int nInputObjects, int  nTotEvts, 
                                 const std::string& objectsName)
{
  double f = (nTotEvts<=0? 0.: float(nInputObjects)/float(nTotEvts) );
  info() << "N " << objectsName << "\t" << nInputObjects
         << "\t(" << f << " per event)" << endreq;
}
void HltBaseAlg::infoAcceptedObjects(int nAcceptedObjects, int  nTotEvts,
                                     const std::string& objectsName)
{
  double f = (nTotEvts<=0? 0.: float(nAcceptedObjects)/float(nTotEvts));
  info() << "N " << objectsName << "\t" << nAcceptedObjects
         << "\t(" << f << " per event)\t("  << 100.*f << " %)" << endreq;
}


void HltBaseAlg::initializeHistosFromDescriptor() {

  if (m_histogramUpdatePeriod == 0) return;

  const std::vector<std::string>& hdes = m_histoDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string title = "";
    int n = 100;
    float x0 = 0.;
    float xf = 1.;
    if (ParserDescriptor::parseHisto1D(*it,title,n,x0,xf)) {
      book1D(title,x0,xf,n);
      debug() << " booking histo from descriptor " << title 
              << "( "<< n << " , "<< x0 <<" , " << xf << ") " 
              << endreq;
    } 
  }
}



void HltBaseAlg::initializeHisto( HltHisto& histo, 
                                  const std::string& title,
                                  float x0, float xf, int nbins) {
  
  if (m_histogramUpdatePeriod == 0) return;

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
  histo = this->book1D( title, x0, xf, nbins);
  debug() << " booking histo  " << title 
          << "( "<< nbins << " , "<< x0 <<" , " << xf << ") " 
          << endreq;
};

void HltBaseAlg::fillHisto( HltHisto& histo, float x, float weight) {
  if (!m_monitor) return;
  this->fill( histo , x, weight);
}


void HltBaseAlg::initializeCounter ( HltCounter& counter, 
                                     const std::string& inputString) { 
  counter.m_name = inputString;
  counter.m_histo = NULL;
  if (m_histogramUpdatePeriod >0)
    initializeHisto(counter.m_histo,inputString,0.,1.,1);
  counter.m_counter = 0;
};

void HltBaseAlg::increaseCounter( HltCounter& count, int increase) {
  count.m_counter += increase;
  if (count.m_histo)
    this->fill( count.m_histo, 0.5, 1.*increase);  
  return;
}


//=============================================================================
  

// void HltBaseAlg::fillHistoCol( HistoCol& histoCol, 
//                                  const std::vector<bool>& inputBools,
//                                  const std::vector<float>& inputVariables, 
//                                  float weight) {
  
//   if ( ! histoCol.m_initialized ) {
//     fatal() << "Histo collection not initialized:" << endreq;
//     return;
//   }  
  
//   if ( m_counterEntries % this->m_histogramUpdatePeriod  == 0 ) {

//     if (inputBools.size() != histoCol.m_nCases ) {
//       fatal() << "Size of bool vector does not match number of cases of histogram collection!" << endreq;
//       return;
//     }
    
//     if (inputVariables.size() != histoCol.m_nVariables ) {
//       fatal() << "Size of vector of variables does not match number of variables of histogram collection!" << endreq;
//       return;
//     }
    
//     std::vector<AIDA::IHistogram1D*>::const_iterator iHisto = histoCol.m_histos.begin();
//     for ( std::vector<float>:: const_iterator iVar = inputVariables.begin();
//           iVar != inputVariables.end(); iVar++ ) {
//       for ( std::vector<bool>:: const_iterator iBool = inputBools.begin();
//             iBool != inputBools.end(); iBool++ ) {
//         if (*iBool)  this->fill( (*iHisto), (*iVar), weight);
//         iHisto++;
//       }
//     }
    
//   }
// }

// void HltBaseAlg::initializeHistoCol( HistoCol& histoCol, 
//                                        const std::string& inputString,
//                                        const std::vector<std::string>& inputVariables,
//                                        const std::vector<std::string>& inputCases ) { 
  
//   histoCol.m_initialized = true;
//   histoCol.m_name = inputString;
//   histoCol.m_nCases = inputCases.size();
//   histoCol.m_nVariables = inputVariables.size();
  
//   for ( std::vector<std::string>::const_iterator iVariable = inputVariables.begin();
//         iVariable != inputVariables.end(); iVariable++ ) {
    
//     for ( std::vector<std::string>::const_iterator iCase = inputCases.begin();
//           iCase != inputCases.end(); iCase++ ) {
//       histoCol.m_histos.push_back( this->book1D( inputString + (*iVariable) + (*iCase), 0., 100., 100 ) );
//     }
//   }
// };
