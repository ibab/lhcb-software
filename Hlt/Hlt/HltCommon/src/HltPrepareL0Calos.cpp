// $Id: HltPrepareL0Calos.cpp,v 1.1 2007-06-20 12:17:38 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltPrepareL0Calos.h"
#include "Event/HltEnums.h"
//#include "Event/HltSummaryFunctor.h"
#include "HltBase/HltSequences.h"
#include "HltBase/HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltPrepareL0Calos
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltPrepareL0Calos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltPrepareL0Calos::HltPrepareL0Calos( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  // declareCondition("MinPt",m_ptMin = 0.);
  
  declareProperty("MinEt", m_etMin = 3500.);
  declareProperty("CaloType", m_caloType = 2);

  declareProperty("CaloCandidatesLocation", m_caloCandidatesLocation = 
                  L0CaloCandidateLocation::Full);
  
  declareProperty("OutputL0CaloCandidates", m_outputL0CaloCandidatesName = 
                  "Hlt/L0CaloCandidate/Had");
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareL0Calos::~HltPrepareL0Calos() {
  delete _typeFilter;
  delete _etFun;
  delete _etFilter;
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltPrepareL0Calos::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  _etFun = new Hlt::MemberFunction<L0CaloCandidate>(&L0CaloCandidate::et);
  _etFilter = (( *_etFun > m_etMin).clone());
  _typeFilter = new checkType(m_caloType);

  initializeHisto(m_histoEt,"Et",0.,6000.,100);
  initializeHisto(m_histoEt1,"Et1",0.,6000.,100);

  debug() << " calo candidates location " 
          << m_caloCandidatesLocation << endreq;

  doregister(m_calos,m_outputL0CaloCandidatesName);

//   put(new Tracks(),"Hlt/Track/Caca");

//   put(new Hlt::L0CaloCandidateDataSelection(m_calos),m_outputL0CaloCandidatesName);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltPrepareL0Calos::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  // get calo candidates
  L0CaloCandidates* calos = get<L0CaloCandidates>(m_caloCandidatesLocation);

  // select calo by type 
  m_ocalos.clear();
  Hlt::select(*calos,*_typeFilter,m_ocalos);
  debug() << " number of calos " << m_ocalos.size() << endreq;
  if (m_ocalos.size() <= 0) return stop(" No calos ");

  // monitor the et of the calos
  if (m_debug || m_monitor) {
    std::vector<double> ets;
    ELoop::map(m_ocalos,*_etFun,ets);
    std::sort(ets.begin(),ets.end());
    fillHisto(m_histoEt,ets,1.);
    double et1 = ets.back();
    fillHisto(m_histoEt1,et1,1.);
    if (m_debug) print(ets.begin(),ets.end(), " ets ");
    debug() << " et1 " << et1 << endreq;
  }
  
  // select the calos above an et cut
  m_calos->clear();
  Hlt::select(m_ocalos,*_etFilter,*m_calos);
  int ncan = m_calos->size();
  debug() << " number of calos above et " << ncan << endreq;
  candidateFound(ncan);

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltPrepareL0Calos::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
