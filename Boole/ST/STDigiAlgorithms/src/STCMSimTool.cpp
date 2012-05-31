#include "GaudiKernel/IIncidentSvc.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "STCMSimTool.h"

#include "STDet/DeSTDetector.h"

DECLARE_TOOL_FACTORY(  STCMSimTool )

STCMSimTool::STCMSimTool( const std::string& type, 
                                          const std::string& name,
                                          const IInterface* parent ) :
  ST::ToolBase(type, name, parent), 
  m_isInitialized(false)
{
  // constructer
  declareProperty("forceOptions", m_forceOptions = true);
  declareProperty("cmNoise", m_cmNoise = 2.0); // value to use if forceOptions true

  setForcedInit();
  declareInterface<ISTCMSimTool>(this);
}

STCMSimTool::~STCMSimTool()
{ 
  //destructor
}

StatusCode STCMSimTool::initialize(){

  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Add incident at begin of each event
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return StatusCode::SUCCESS; 
}

void STCMSimTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_isInitialized = false;
}

void STCMSimTool::initEvent() const {

  // make the map
  m_cmValues.clear();

  const std::vector<DeSTSector*>& tSectors = tracker()->sectors(); 
  std::vector<DeSTSector*>::const_iterator iterS = tSectors.begin();
  for ( ; iterS != tSectors.end(); ++iterS ){
    if (m_forceOptions == true){
      m_cmValues[(*iterS)->elementID()] = m_cmNoise * m_gaussDist->shoot();
    }
    else {
      m_cmValues[(*iterS)->elementID()] = (*iterS)->cmSectorNoise() * m_gaussDist->shoot();
    }
  }
 
}

double STCMSimTool::noise(const LHCb::STChannelID& chan) const
{
  
  if (m_isInitialized == false) initEvent();

  return m_cmValues[chan];
}
