// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltSummaryTool.h"
#include "Event/HltSummaryFunctor.h"
#include "Event/HltNames.h"
#include "Event/HltEnums.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/HltConfigurationHelper.h"
#include "HltBase/HltSummaryHelper.h"

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

StatusCode HltSummaryTool::initialize() {

 IDataProviderSvc* hltsvc = NULL;
  StatusCode sc = serviceLocator()->service("HltDataSvc/EventDataSvc",hltsvc);
  if (!hltsvc) fatal() << " not able to create Hlt Svc provider " << endreq;

  m_conf = NULL;
 //  Hlt::DataHolder<Hlt::Configuration>* holder = 
//     get<Hlt::DataHolder<Hlt::Configuration> >(hltsvc,"Hlt/Configuration");
//   if (!holder) fatal() << " not able to retrieve configuration " << endreq;

//   m_conf = &(holder->object());
  
  return sc;
} 

void HltSummaryTool::getSummary() {
  m_summary = NULL;
  std::string loca = m_summaryLocation;
  m_summary = get<LHCb::HltSummary >(loca);  
  if (! m_summary) error() << " No summaryy in TES! " << endreq;

  loca = m_summaryLocation+"/Configuration";
  Hlt::DataHolder< Hlt::Configuration >* m_holder = 
    get< Hlt::DataHolder<Hlt::Configuration> >(loca);
  m_conf = &(m_holder->object());
  
}

const HltSummary& HltSummaryTool::summary() {
  getSummary();
  return *m_summary;
}

bool HltSummaryTool::decision() {
  getSummary();
  return m_summary->decision();
}

bool HltSummaryTool::decisionType(const std::string& name) {
  getSummary();
  int id = (unsigned int) HltNames::decisionTypeID(name);
  return m_summary->checkDecisionType(id);
}

bool HltSummaryTool::hasSelection(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  return m_summary->hasSelectionSummary(id);
}

bool HltSummaryTool::selectionDecision(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  bool ok =  m_summary->hasSelectionSummary(id);
  if (!ok) return ok;
  return m_summary->selectionSummary(id).decision();
}

bool HltSummaryTool::selectionDecisionType(const std::string& name,
                                           const std::string& type) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  bool ok =  m_summary->hasSelectionSummary(id);
  if (!ok) return ok;
  HltSelectionSummary& sum = m_summary->selectionSummary(id);
  int itype = (unsigned int) HltNames::decisionTypeID(type);
  return sum.checkDecisionType(itype);
}


std::vector<std::string> HltSummaryTool::selections() {
  getSummary();
  std::vector<int> ids = m_summary->selectionSummaryIDs();
  std::vector<std::string> names;
  for (std::vector<int>::iterator it = ids.begin(); it!= ids.end(); ++it) {
    std::string name = HltConfigurationHelper::getName(*m_conf,"SelectionID",*it);
    names.push_back(name);
  }
  return names;  
}

std::vector<std::string> 
HltSummaryTool::selectionFilters(const std::string& name) {
  getSummary();
  std::vector<std::string> filters;
  std::string key = name+"/Filters";
  if (m_conf->has_key(key))
    filters = m_conf->retrieve<std::vector<std::string> >(key);
  return filters;
}

std::vector<std::string> 
HltSummaryTool::selectionAncestors(const std::string& name) {
  getSummary();
  std::vector<std::string> ancestors;
  info() << " Not implemented jet! " << endreq;
  return ancestors;
}

std::vector<Track*> 
HltSummaryTool::selectionTracks(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  std::vector<Track*> tracks = 
    HltSummaryHelper::retrieve<std::vector<Track*> >(*m_summary,id);
  return tracks;
}

std::vector<RecVertex*> 
HltSummaryTool::selectionVertices(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  std::vector<RecVertex*> vertices = 
    HltSummaryHelper::retrieve<std::vector<RecVertex*> >(*m_summary,id);
  return vertices;
}

bool HltSummaryTool::isInSelection(const std::string& name,
                                   const Track& track) {
  bool ok = false;
  getSummary();
  std::vector<Track*> tracks = selectionTracks(name);
  for (std::vector<Track*>::const_iterator it = tracks.begin(); 
       it != tracks.end(); it++) {
    const Track& otrack = *(*it);
    if (HltUtils::matchIDs(track,otrack)) ok = true;
  }
  return ok;
}

std::vector<std::string> HltSummaryTool::confKeys() {
  return m_conf->keys();
}

std::string HltSummaryTool::confString(const std::string& name) {
  std::string val = "unknown";
  if (m_conf->has_key(name)) val = m_conf->retrieve<std::string>(name);
  return val;
}

std::vector<std::string> HltSummaryTool::confStringVector(const std::string& name) 
{
  std::vector<std::string> val;
  if (m_conf->has_key(name)) 
    val = m_conf->retrieve<std::vector<std::string> >(name);
  return val;
}

