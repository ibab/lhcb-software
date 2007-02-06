// $Id: HltOffMonitorExample.cpp,v 1.3 2007-02-06 08:15:50 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltOffMonitorExample.h"

#include "Event/HltSummary.h"
#include "Event/HltNames.h"
#include "Event/Track.h"
#include "TrackInterfaces/ITrackCloneFinder.h"

#include "HltBase/HltUtils.h"
#include "HltBase/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltOffMonitorExample
//
// 2006-12-13 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltOffMonitorExample );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltOffMonitorExample::HltOffMonitorExample( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("SummaryName",
                  m_summaryName = LHCb::HltSummaryLocation::Default);
  declareProperty("SelectionName", m_selectionName = "HadPreTriggerSingle");
}
//=============================================================================
// Destructor
//=============================================================================
HltOffMonitorExample::~HltOffMonitorExample() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltOffMonitorExample::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_idSelection = HltNames::selectionSummaryID(m_selectionName);
  debug() << " selection Name " << m_selectionName 
          << " id " << m_idSelection << endreq;

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltOffMonitorExample::execute() {

  debug() << "==> Execute" << endmsg;

  HltSummary* summary = get<HltSummary>(m_summaryName);
  Tracks* tracks = get<Tracks>("Rec/Track/Best");
  
  std::vector<int> sids = summary->selectionSummaryIDs();
  debug() << " number of selections in summary " << sids.size() << endreq;
  for (std::vector<int>::iterator it = sids.begin(); it != sids.end(); ++it)
    debug() << " summary ID " << *it << " " 
            << HltNames::selectionSummaryStr(*it) << endreq;
  
  bool ok = summary->hasSelectionSummary(m_idSelection);
  debug() << " has selection " << m_idSelection << " "
          << HltNames::selectionSummaryStr(m_idSelection) << " ? "
          << ok << endreq;
  if (!ok) return StatusCode::SUCCESS;

 //  std::vector<Track*> otracks;
//   HltSummaryFunctor::retrieve(*summary,m_idSelection,otracks);
//   debug() << " number of summary tracks " << otracks.size() << endreq;
  
//   ITrackCloneFinder* matchtool = tool<ITrackCloneFinder>("TrackCloneFinder");
  
//   int nmatch0 = 0;
//   int nmatch1 = 0;
//   for (std::vector<Track*>::iterator it = otracks.begin();
//        it != otracks.end(); ++it) {
//     Track& ontrack = *(*it);
//     size_t n0 = ontrack.lhcbIDs().size();
//     size_t n1 = 0;
//     for (Tracks::iterator it2 = tracks->begin(); it2 != tracks->end();
//          ++it2) {
//       Track& offtrack = *(*it2);
//       size_t n = ELoop::count(ontrack.lhcbIDs(),offtrack.lhcbIDs());
//       if (HltUtils::matchIDs(ontrack,offtrack)) nmatch0++;
//       if (matchtool->areClones(ontrack,offtrack)) nmatch1++;  
//       if (n1 < n) n1 = n;
//     }
//     verbose() << " count number of ids together " 
//               << n1 << "/" << n0 << endreq;
//   }
//   debug() << "number of matched tracks [utils] " << nmatch0 << endreq;
//   debug() << "number of matched tracks [clone tool] " << nmatch1 << endreq;

  return StatusCode::SUCCESS;
      
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltOffMonitorExample::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
