// $Id: LumiHistoMaker.cpp,v 1.3 2008-08-26 14:03:14 panmanj Exp $
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "HltBase/ANNSvc.h"

#include "LumiHistoMaker.h"

#include "HltBase/ESequences.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiHistoMaker
//
// 2008-07-24 : Antonio Perez-Calero
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiHistoMaker );



LumiHistoMaker::LumiHistoMaker( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : HltBaseAlg ( name , pSvcLocator )

{
  
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 1 );
  declareProperty("InputDataContainer",  m_DataName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty("InputVariables",  m_Variables );
}

LumiHistoMaker::~LumiHistoMaker()
{
  //delete m_outputSelection;
} 


StatusCode LumiHistoMaker::initialize() {

  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  IANNSvc* annSvc = svc<IANNSvc>("LumiANNSvc");
  initCounters();

  // book a histogram for each recognized counter name
  m_Histos.clear();
  m_names.clear();
  m_keys.clear();
  
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar ){
    boost::optional<IANNSvc::minor_value_type> x = annSvc->value("LumiCounters", *ivar);
    if (!x) {
      warning() << "LumiCounter not found with name: " << *ivar <<  endmsg;
    } else {
      int counter = x->second;
      //Generate Histos using known number and name of variables
      info() << "Variable: " << *ivar << " at key: " << counter << " will be histogrammed." <<endreq;
      // Ranges should be adapted to each variable!!
      m_Histos.push_back(initializeHisto(*ivar, -0.5, 400.-0.5, 400));  
      m_names.push_back(*ivar);
      m_keys.push_back(counter);
    }
  }
  info() << m_keys.size()<< " variables used " << endmsg ;
  info() << "Initialised Lumi Histo Maker" << endmsg ;
  return sc;

}

//---------------------------------------------------------
void LumiHistoMaker::initCounters() 
{
   // counters
  initializeCounter(m_counterEntries,    "Entries");
  initializeCounter(m_counterHistoInputs,     "HistoInputs");
  info()<< "Counters initialised!"<<endmsg;
  
}

StatusCode LumiHistoMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  increaseCounter(m_counterEntries,1);

  // get data container
  m_HltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // fill histos
  for ( unsigned int i = 0; i < m_Histos.size(); ++i) {
    std::string cname = m_names[i];
    int counter = m_keys[i];
    Hlt::Histo *histo = m_Histos[i];
    int ivalue = m_HltLumiSummary->info(counter,-1);
    fillHisto(*histo, (double) ivalue, 1.);
    debug() << "histo:" << cname << " filled with " << ivalue << endmsg;
  }
  
  increaseCounter(m_counterHistoInputs,1);
  setDecision(true);
  return sc;

}

void LumiHistoMaker::setDecision(bool ok) {
  setFilterPassed(ok);
}

////  Finalize
StatusCode LumiHistoMaker::finalize() {
  debug() << "LumiHistoMaker::finalize()" << endmsg;
  infoTotalEvents(m_counterEntries);
  infoSubsetEvents(m_counterEntries,m_counterHistoInputs, " events to Histos ");

  return HltBaseAlg::finalize();
}
