// $Id: LumiHistoMaker.cpp,v 1.1 2008-08-13 16:03:52 panmanj Exp $
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
  //declareProperty("InputVariables",  m_Variables );
}

LumiHistoMaker::~LumiHistoMaker()
{
  //delete m_outputSelection;
} 


StatusCode LumiHistoMaker::initialize() {

  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  // ------------------------------------------
  //IANNSvc* annSvc = svc<IANNSvc>("LumiANNSvc");

  initCounters();
  const std::string& name="VeloTracks";
  const std::string& name2="PV2D";
  const std::string& name3="PV3D";
  myhisto = initializeHisto(name,0.,200.,20);
  myhisto2 = initializeHisto(name2,0.,10.,10);
  myhisto3 = initializeHisto(name3,0.,10.,10);
  std::string variables[] = {
    "PUMult",
    "RZVelo",
    "RZVeloBW",
    "Velo",
    "Muon",
    "BKMuon",
    "SPD",
    "Calo"
  };
  
  info() << sizeof(variables)<< " variables declared " << endmsg ;
  
  //Generate Histos using known number and name of variables!
  // Ranges should be adapted to each variable!!
  for (int it = 0;  it != 8; it++) {
    m_Histos.push_back(initializeHisto(variables[it],0.,100.,20));
  }
  
  
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

//---------------------------------------------------------

// Lumi variables:
//     "PUMult"     : 0   ,
//     "RZVelo"     : 1   ,
//     "RZVeloBW"   : 2   , 
//     "Velo"       : 3   , 
//     "Muon"       : 4   , 
//     "BKMuon"     : 5   , 
//     "SPD"        : 6   , 
//     "Calo"       : 7   , 
//     "PV2D"       : 10  , 
//     "PV3D"       : 11  , 
//     "PU"         : 13  , 
//     "LastKey"    : 16

StatusCode LumiHistoMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  
  increaseCounter(m_counterEntries,1);

  // get data container
  m_HltLumiSummarys = get<LHCb::HltLumiSummarys>(m_DataName);
  LHCb::HltLumiSummary* hltLS;
  hltLS = *m_HltLumiSummarys->begin();
  info() << m_DataName << " data has been found" << endmsg ;
  info() << hltLS->info(1,0)<< " velo tracks!"<< endmsg;
  info() << hltLS->info(0,0)<< " Pile Up multi!"<< endmsg;
  info() << hltLS->info(6,0)<< " SPD multi!"<< endmsg;
  info() << hltLS->info(7,0)<< " Calo SumEt!"<< endmsg;
  info() << hltLS->info(10,0)<< " PV2D!"<< endmsg;
  info() << hltLS->info(11,0)<< " PV3D!"<< endmsg;
  // Fill histos -> Hard wired so far!!
  fillHisto(*myhisto,hltLS->info(1,0),1.);
  fillHisto(*myhisto2,hltLS->info(10,0),1.);
  fillHisto(*myhisto3,hltLS->info(11,0),1.);
  
  //for (int it = 0;  it < m_Histos.size(); it++) {
  //  fillHisto(*m_Histos[it],hltLS->info(it,-1),1.);    
  //}
  int i=0;
  for (std::vector<Hlt::Histo*>::iterator it = m_Histos.begin(); it != m_Histos.end(); ++it,++i) {
    fillHisto(**it, (double) hltLS->info(i,-1), 1.);
    
  }
    
  
  increaseCounter(m_counterHistoInputs,1);
  bool ok=1;
  setDecision(ok);
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
  //infoCandidates(m_counterCandidates, m_counterAccepted, " passed ");
  return HltBaseAlg::finalize();
}
