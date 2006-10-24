// $Id: HltMonitorAlgorithm.cpp,v 1.1 2006-10-24 09:31:20 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "HltBase/HltMonitorAlgorithm.h"
#include "HltBase/ParserDescriptor.h"

#include "HltBase/ESequences.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMonitorAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMonitorAlgorithm::HltMonitorAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{

  declareProperty("SummaryName",
                  m_summaryName = LHCb::HltSummaryLocation::Default);
  declareProperty("SelectionName",
                  m_selectionSummaryName = "");
  
  declareProperty("HistoDescriptor", m_histoDescriptor);

}
//=============================================================================
// Destructor
//=============================================================================
HltMonitorAlgorithm::~HltMonitorAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMonitorAlgorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selectionSummaryID = HltNames::selectionSummaryID(m_selectionSummaryName);

  return StatusCode::SUCCESS;

}

//// Main execution
StatusCode HltMonitorAlgorithm::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  bool ok = beginExecute();
  if (!ok) return sc;
  
  return sc;
}


bool HltMonitorAlgorithm::beginExecute() {
  bool ok = true;

  m_summary = get<HltSummary>(m_summaryName);
  if (m_selectionSummaryID<0) return false;
  
  if (!m_summary->hasSelectionSummary(m_selectionSummaryID))
    return false;

  m_selectionSummary = &(m_summary->selectionSummary(m_selectionSummaryID));

  m_tracks.clear();
  getFromSummary(m_tracks,m_selectionSummaryID);

  m_vertices.clear();
  getFromSummary(m_vertices,m_selectionSummaryID);

  return ok;
}


StatusCode HltMonitorAlgorithm::finalize() {
  return GaudiAlgorithm::finalize();  
}

void HltMonitorAlgorithm::initializeHisto( HltHisto& histo, 
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
  histo  = this->book1D( title, x0, xf, nbins);
  info() << " booking histo  " << title 
         << "( "<< nbins << " , "<< x0 <<" , " << xf << ") " 
         << endreq;
};


