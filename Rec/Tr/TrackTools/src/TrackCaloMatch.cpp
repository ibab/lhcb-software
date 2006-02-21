
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"


// Tsa
#include "TrackCaloMatch.h"
#include "Event/Track.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"
 
using namespace LHCb;

static const ToolFactory<TrackCaloMatch>  s_factory;
const IToolFactory& TrackCaloMatchFactory = s_factory;
  
TrackCaloMatch::TrackCaloMatch(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
GaudiTool(type, name, parent),
m_ecalE(0),
m_hcalE(0),
m_psE(0),
m_configured(false)
{
  declareInterface<ITrackCaloMatch>(this);

  declareProperty("alpha", m_alpha = 8. );
  declareProperty("beta", m_beta = 1. );
  declareProperty("gamma", m_gamma = 1. );
                                                                                              
}

TrackCaloMatch::~TrackCaloMatch(){
  // destructer
}


StatusCode TrackCaloMatch::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  incSvc()->addListener( this, IncidentType::BeginEvent );
 
  return StatusCode::SUCCESS; 
};

double TrackCaloMatch::energy(const Track& aTrack) const{

  // get the input - seeds
  if (!m_configured) initEvent();       

  double eEcal = 0 ;
  double eHcal = 0 ;
  double ePrs = 0 ;

  Table::Range rEcal = m_ecalE->relations( &aTrack ) ;
  if ( !rEcal.empty() ) { eEcal += rEcal.front().to() ; }
                                                                                               
  Table::Range rHcal = m_hcalE->relations( &aTrack ) ;
  if ( !rHcal.empty() ) { eHcal += rHcal.front().to() ; }
                                                                                               
  Table::Range rPs = m_psE->relations( &aTrack ) ;
  if ( !rPs.empty() ) { ePrs += rPs.front().to() ; }
 
  // known bug - sometimes ps gives -ve energy
  if (ePrs < 0) {
    eEcal = 0;
    ePrs = 0;
  }

  double energy = (m_alpha * ePrs) + (m_beta * eEcal) + (m_gamma * eHcal) ;
  
  return energy;                                                                                            
}

void TrackCaloMatch::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}
 
void TrackCaloMatch::initEvent() const{

 m_configured = true;
 m_ecalE = get<Table>("Rec/Calo/EcalE") ;
 m_hcalE = get<Table>("Rec/Calo/HcalE") ;
 m_psE = get<Table>("Rec/Calo/PrsE" ) ;

}
