// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltSummaryTool.h"
#include "Event/HltSummaryFunctor.h"
#include "HltBase/HltUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSummaryTool
//
// 2006-07-20 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltSummaryTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSummaryTool::HltSummaryTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  m_summary = NULL;
  
  declareInterface<IHltSummaryTool>(this);
  
  declareProperty("SummaryLocation",
                  m_summaryLocation = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltSummaryTool::~HltSummaryTool() {} 

//=============================================================================

bool HltSummaryTool::isInSelection(const Track& track, int id) {
  bool ok = false;
  if (!m_summary) error() 
    << " please load the HltSumamryTool or set the summary " << endreq;
  if (!m_summary->hasSelectionSummary(id)) return false;
  std::vector<Track*> tracks;
  HltSummaryFunctor::retrieve(*m_summary,id,tracks);
  for (std::vector<Track*>::const_iterator it = tracks.begin(); 
       it != tracks.end(); it++) {
    const Track& otrack = *(*it);
    if (HltUtils::matchIDs(otrack,track)) ok = true;
  }
  return ok;
}


