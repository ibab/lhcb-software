// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "HltBase/EParser.h"
// #include "GaudiKernel/IIncidentListener.h"


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
  , m_summary(0)
{
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
  StatusCode status = HltBaseTool::initialize();
  if ( !status.isSuccess() ) return status;

  IIncidentSvc*                incidentSvc(0);     ///<
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->release();

  return status;
}

void HltSummaryTool::handle(const Incident& /*incident*/) {
    m_summary=0;
}

const LHCb::HltSummary& HltSummaryTool::getSummary() {
  if (m_summary==0) m_summary = get<LHCb::HltSummary >(m_hltSummaryLocation);
  Assert( 0 != m_summary, " getSummary() no summary in TES");
  return *m_summary;
}

const HltSummary& HltSummaryTool::summary() {
  return getSummary();
}

bool HltSummaryTool::decision() {
  return getSummary().decision();
}

bool HltSummaryTool::decisionType(const std::string& name) {
  info() << " deprecated decision Type " << name << endreq;
  return decision();
}

bool HltSummaryTool::hasSelection(const std::string& name) {
  if (!validHltSelectionName(name))
    error() << " No valid selection name " << name << endreq;
  return getSummary().hasSelectionSummary(name);
}

bool HltSummaryTool::selectionDecision(const std::string& name) {
  return checkSelection(name) 
         ? getSummary().selectionSummary(name).decision()
         : false;
}

bool HltSummaryTool::selectionDecisionType(const std::string& name,
                                           const std::string& type) {
  info() << " deprecated decision type" << name << " " << type << endreq;
  return decision();
}


std::vector<std::string> HltSummaryTool::selections() {
  return getSummary().selectionSummaryIDs();
}

size_t HltSummaryTool::selectionNCandidates(const std::string& name) {
  return checkSelection(name)
            ? getSummary().selectionSummary(name).data().size()
            : size_t(0);
}

std::vector<std::string> 
HltSummaryTool::selectionFilters(const std::string& name) {
  std::string key = name+"/Filters";
  return hltConf().has_key(key)
            ? hltConf().retrieve<std::vector<std::string> >(key)
            : std::vector<std::string>();
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
  std::string key = name+"/SelectionType";
  std::string type = "unknown";
  if (hltConf().has_key(key)) type = hltConf().retrieve<std::string >(key);
  return type;
}

std::vector<Track*> 
HltSummaryTool::selectionTracks(const std::string& name) {
  getSummary();
  return checkSelection(name)
         ? Hlt::SummaryHelper::retrieve<Track>(summary(),name)
         : std::vector<Track*>();
}


std::vector<RecVertex*> 
HltSummaryTool::selectionVertices(const std::string& name) {
  return checkSelection(name)
         ? Hlt::SummaryHelper::retrieve<RecVertex>(summary(),name)
         : std::vector<RecVertex*>();
}

Particle::ConstVector HltSummaryTool::selectionParticles(const std::string& name) {
  std::vector<const Particle*> particles;
  bool ok = checkSelection(name);
  if (msgLevel(MSG::VERBOSE)) verbose () << "selectionParticles " << name << " OK: " << ok << endmsg ;
  if (!ok) return particles;
  /// @todo why can't the helper do this ?
  const SmartRefVector<LHCb::Particle>& p = summary().selectionSummary(name).particles();
  for ( SmartRefVector<LHCb::Particle>::const_iterator ip = p.begin() ; ip!=p.end(); ++ip) {
    if (msgLevel(MSG::VERBOSE)) verbose () << "Part " << (*ip)->particleID().pid() << endmsg ; 
    particles.push_back(*ip);
  }
  return particles;

}

bool HltSummaryTool::isInSelection(const std::string& name,
                                   const Track& track) {
  std::vector<Track*> tracks = selectionTracks(name);
  bool ok = false;
  for (std::vector<Track*>::const_iterator it = tracks.begin(); 
       it != tracks.end() && !ok; ++it) {
    ok = HltUtils::matchIDs(track,**it);
  }
  return ok;
}

bool HltSummaryTool::checkSelection(const std::string& name) {
  if (!validHltSelectionName(name))
    error() << " No valid selection name " << name << endreq;  
  bool ok =  getSummary().hasSelectionSummary(name);
  if (!ok && msgLevel(MSG::DEBUG)) debug() << " no selection in summary " << name << endreq;
  return ok;
}

std::vector<std::string> HltSummaryTool::confKeys() {
  return hltConf().keys();
}

template <typename T> 
T confVal(const std::string& name, Hlt::Configuration& conf, T unknownValue = T())
{ 
  // std::cout << " requested " << name << " has? " << conf.has_key(name) << std::endl;
  return conf.has_key(name) ? conf.retrieve<T>(name) : T(unknownValue) ;
}


int HltSummaryTool::confInt(const std::string& name) {
  if (hltConf().has_key(name)) return confVal(name,hltConf(),-1);
  std::vector<std::string> cromos = EParser::parse(name,"/");
  if (cromos.size() != 2) return -1;
  boost::optional<IANNSvc::minor_value_type> i =  annSvc().value(cromos[0],cromos[1]);
  int val = i->second;
  return val;
}

double HltSummaryTool::confDouble(const std::string& name) {
  return confVal<double>(name, hltConf(),0);
}

std::string HltSummaryTool::confString(const std::string& name) {
  return confVal<std::string>(name, hltConf(),"unknown");
}

std::vector<std::string> HltSummaryTool::confStringVector(const std::string& name) 
{
  return confVal<std::vector<std::string> >(name,hltConf());
}
