// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltSummaryTool.h"
#include "HltBase/HltSummaryHelper.h"
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
  : HltBaseTool ( type, name , parent )
{
  m_summary = 0;
  
  declareInterface<IHltConfSummaryTool>(this);
  declareInterface<IHltSummaryTool>(this);
  
  declareProperty("HltSummaryLocation",
                  m_hltSummaryLocation = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltSummaryTool::~HltSummaryTool() {}

StatusCode HltSummaryTool::initialize() {

  StatusCode sc = HltBaseTool::initialize();
  
  return sc;
} 

void HltSummaryTool::getSummary() {
  m_summary = 0;
  std::string loca = m_hltSummaryLocation;
  m_summary = get<LHCb::HltSummary >(loca);  
  Assert( 0 != m_summary, " getSummary() no summary in TES");
 
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
  info() << " deprecated decision Type " << name << endreq;
  return decision();
}

bool HltSummaryTool::hasSelection(const std::string& name) {
  if (!validHltSelectionName(name))
    error() << " No valid selection name " << name << endreq;
  getSummary();
  int id = hltSelectionID(name);
  return m_summary->hasSelectionSummary(id);
}

bool HltSummaryTool::selectionDecision(const std::string& name) {
  bool ok = checkSelection(name);
  if (!ok) return false;
  int id = hltSelectionID(name);
  return m_summary->selectionSummary(id).decision();
}

bool HltSummaryTool::selectionDecisionType(const std::string& name,
                                           const std::string& type) {
  info() << " deprecated decision type" << name << " " << type << endreq;
  return decision();
}


std::vector<std::string> HltSummaryTool::selections() {
  getSummary();
  std::vector<int> ids = m_summary->selectionSummaryIDs();
  std::vector<std::string> names;
  for (std::vector<int>::iterator it = ids.begin(); it!= ids.end(); ++it) {
    int id = *it;
    std::string name = hltSelectionName(id);
    names.push_back(name);
  }
  return names;  
}

size_t HltSummaryTool::selectionNCandidates(const std::string& name) {
  bool ok = checkSelection(name);
  if (!ok) return 0;
  int id = hltSelectionID(name);
  size_t ncans = m_summary->selectionSummary(id).data().size();
  return ncans;
}

std::vector<std::string> 
HltSummaryTool::selectionFilters(const std::string& name) {
  std::vector<std::string> filters;
  std::string key = name+"/Filters";
  if (hltConf().has_key(key))
    filters = hltConf().retrieve<std::vector<std::string> >(key);
  return filters;
}

std::vector<std::string> 
HltSummaryTool::selectionInputSelections(const std::string& name) {
  std::vector<std::string> inputs;
  std::string key = name+"/InputSelections";
  if (hltConf().has_key(key))
    inputs = hltConf().retrieve<std::vector<std::string> >(key);
  return inputs;
}

std::string 
HltSummaryTool::selectionType(const std::string& name) {
  std::string type = "unknown";
  std::string key = name+"/SelectionType";
  if (hltConf().has_key(key)) type = hltConf().retrieve<std::string >(key);
  return type;
}

std::vector<Track*> 
HltSummaryTool::selectionTracks(const std::string& name) {
  std::vector<Track*> tracks;
  bool ok = checkSelection(name);
  if (!ok) return tracks;
  int id = hltSelectionID(name);
  return HltSummaryHelper::retrieve<Track>(*m_summary,id);
}


std::vector<RecVertex*> 
HltSummaryTool::selectionVertices(const std::string& name) {
  std::vector<RecVertex*> vertices;
  bool ok = checkSelection(name);
  if (!ok) return vertices;
  int id = hltSelectionID(name);
  return HltSummaryHelper::retrieve<RecVertex>(*m_summary,id);
}

std::vector<Particle*> 
HltSummaryTool::selectionParticles(const std::string& name) {
  std::vector<Particle*> particles;
  bool ok = checkSelection(name);
  if (!ok) return particles;
  int id = hltSelectionID(name);
  return HltSummaryHelper::retrieve<Particle>(*m_summary,id);


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

bool HltSummaryTool::isInSelection( const Track& track, int id) {
  std::string name = hltSelectionName(id);
  return isInSelection(name,track);
}


std::vector<std::string> HltSummaryTool::confKeys() {
  return hltConf().keys();
}

int HltSummaryTool::confInt(const std::string& name) {
  int val = -1;
  if (hltConf().has_key(name)) val = hltConf().retrieve<int>(name);
  return val;
}

double HltSummaryTool::confDouble(const std::string& name) {
  double val = 0.;
  if (hltConf().has_key(name)) val = hltConf().retrieve<double>(name);
  return val;
}

std::string HltSummaryTool::confString(const std::string& name) {
  std::string val = "unknown";
  if (hltConf().has_key(name)) val = hltConf().retrieve<std::string>(name);
  return val;
}

std::vector<std::string> HltSummaryTool::confStringVector(const std::string& name) 
{
  std::vector<std::string> val;
  if (hltConf().has_key(name)) 
    val = hltConf().retrieve<std::vector<std::string> >(name);
  return val;
}

bool HltSummaryTool::checkSelection(const std::string& name) {
  if (!validHltSelectionName(name))
    error() << " No valid selection name " << name << endreq;  
  getSummary();
  int id = hltSelectionID(name);
  bool ok =  m_summary->hasSelectionSummary(id);
  if (!ok) debug() << " no selection in summary " << name << endreq;
  return ok;
}
