#include "GaudiKernel/IIncidentSvc.h"


// Tsa
#include "TrackCaloMatch.h"
#include "Event/Track.h"
#include "CaloUtils/Calo2Track.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackCaloMatch )
  
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

  
  m_ecalLocation = rootInTES() + CaloIdLocation::EcalE ;
  m_hcalLocation = rootInTES() + CaloIdLocation::HcalE;
  m_prsLocation = rootInTES()+ CaloIdLocation::PrsE;

  incSvc()->addListener( this, IncidentType::BeginEvent );
 
  return StatusCode::SUCCESS; 
}

double TrackCaloMatch::energy(const Track& aTrack) const{

  // get the input - seeds
  if (!m_configured) initEvent();       

  double eEcal = energy(aTrack, m_ecalE) ;
  double eHcal = energy(aTrack, m_hcalE) ;
  double ePrs = energy(aTrack, m_psE);
 
  // known bug - sometimes ps gives -ve energy
  if (ePrs < 0) {
    eEcal = 0;
    ePrs = 0;
  }

  return (m_alpha * ePrs) + (m_beta * eEcal) + (m_gamma * eHcal) ;
  
}

double TrackCaloMatch::energy(const Track& aTrack, const TrackCaloMatch::Table* table ) const{

  Table::Range aRange = table->relations( &aTrack ) ;
  return( !aRange.empty()  ? aRange.front().to() : 0);
}

void TrackCaloMatch::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}
 
void TrackCaloMatch::initEvent() const{

 m_configured = true;

 using namespace LHCb::CaloIdLocation;
 m_ecalE = get<Table>(m_ecalLocation) ;
 m_hcalE = get<Table>(m_hcalLocation) ;
 m_psE = get<Table>(m_prsLocation) ;

}
