// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "Event/MCParticle.h"
#include "Event/MCHeader.h"

// local
#include "BcmHitChecker.h"

DECLARE_ALGORITHM_FACTORY( BcmHitChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BcmHitChecker::BcmHitChecker( const std::string& name,
                              ISvcLocator* pSvcLocator):
  GaudiTupleAlg ( name , pSvcLocator ),
  m_bcmDetLocation ( DeBcmLocation::Default ),
  m_detailedMonitor ( false ),
  m_bcmMCHits ( )
{
  declareProperty("BcmDetLocation", m_bcmDetLocation);
  declareProperty("MonitorInDetail", m_detailedMonitor);
  declareProperty("BcmHitsLocation", m_bcmHitsLocation = "MC/Bcm/Hits" );
}

//=============================================================================
// Destructor
//=============================================================================
BcmHitChecker::~BcmHitChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BcmHitChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
 
  m_bcmDet=( getDet<DeBcm>( m_bcmDetLocation ) );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BcmHitChecker::execute() {

  debug() << "==> Execute" << endmsg;

  getData();
  bcmHitMonitor();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BcmHitChecker::finalize() {
  return GaudiAlgorithm::finalize();
}

//=============================================================================
//  GetData
//=============================================================================
StatusCode BcmHitChecker::getData() {
  debug()<< " ==> BcmHitChecker::getData" <<endmsg;

  if(!exist<LHCb::MCHits>(m_bcmHitsLocation)){ // Should be replaced with LHCb::MCHitLocation::Bcm
    error()<< "There is no MCHits at " << m_bcmHitsLocation << " in TES!" 
           << endmsg;
  }else{
    m_bcmMCHits=get<LHCb::MCHits>(m_bcmHitsLocation);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Plot
//=============================================================================
StatusCode BcmHitChecker::bcmHitMonitor(){
  debug()<< " ==> BcmHitChecker::bcmHitMonitor" <<endmsg;

  LHCb::MCHits::iterator It;
  for(It=m_bcmMCHits->begin(); It!=m_bcmMCHits->end(); It++){
    // Check if hit is in the correct bcm station
    if( checkStation(*It) ){
      plot((*It)->energy()/Gaudi::Units::eV, "enDep",
          "Energy deposited in Diamond [eV]", 0., 500000., 100);
      plot2D((*It)->entry().x()/Gaudi::Units::mm,
          (*It)->entry().y()/Gaudi::Units::mm, "entryXY",
          "Particle entry point in Diamond [mm] - XY plane",
          -80., 80., -80., 80., 160, 160);
      // MC particle energy
      const LHCb::MCParticle* myMCParticle=(*It)->mcParticle();
      if(0!=myMCParticle){
        Gaudi::LorentzVector fMom=myMCParticle->momentum();
        plot(fMom.e()/Gaudi::Units::GeV, "eMCPart",
            "Particle energy [GeV]", 0., 50., 100);
      }

      // Tuple
      if( m_detailedMonitor ){
        const LHCb::MCHeader* evt = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
        long nEvt = evt->evtNumber();
        
        Tuple tuple = nTuple ( 100 , "Hit info" ) ;

        // fill N-Tuple with hit data:
        tuple -> column ( "Event", nEvt ) ;
        tuple -> column ( "Sensor", (~(~0<<DeBcmShifts::shiftStationID))&((*It)->sensDetID()) );
        tuple -> column ( "Path", (*It)->pathLength() ) ;
        tuple -> column ( "EnDep", (*It)->energy() ) ;
        tuple -> column ( "xIn", (*It)->entry().x() ) ;
        tuple -> column ( "yIn", (*It)->entry().y() ) ;
        tuple -> column ( "zIn", (*It)->entry().z() ) ;
        tuple -> column ( "rIn", ( sqrt( (*It)->entry().x()*(*It)->entry().x() + (*It)->entry().y()*(*It)->entry().y() ) ) ) ;
        tuple -> column ( "xOut", (*It)->exit().x() ) ;
        tuple -> column ( "yOut", (*It)->exit().y() ) ;
        tuple -> column ( "zOut", (*It)->exit().z() ) ;
        tuple -> column ( "rOut", ( sqrt( (*It)->exit().x()*(*It)->exit().x() + (*It)->exit().y()*(*It)->exit().y() ) ) ) ;
        tuple -> column ( "arTime", (*It)->time() ) ;

        // fill N-Tuple with particle data
        tuple -> column ( "pid", (*It)->mcParticle()->particleID().pid() ) ;
        tuple -> column ( "charge", (*It)->mcParticle()->particleID().threeCharge()/3 ) ;
        tuple -> column ( "px", (*It)->mcParticle()->momentum().px() ) ;
        tuple -> column ( "py", (*It)->mcParticle()->momentum().py() ) ;
        tuple -> column ( "pz", (*It)->mcParticle()->momentum().pz() ) ;
        tuple -> column ( "E", (*It)->mcParticle()->momentum().E() ) ;

        // fill N-tuple with primary vertex data
        tuple -> column ( "primx", (*It)->mcParticle()->primaryVertex()->position().x() );
        tuple -> column ( "primy", (*It)->mcParticle()->primaryVertex()->position().y() );
        tuple -> column ( "primz", (*It)->mcParticle()->primaryVertex()->position().z() );
        tuple -> write () ;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Check if hit is in the monitored BCM station
//=============================================================================
bool BcmHitChecker::checkStation(LHCb::MCHit* Hit){
  int nStation = m_bcmDet->stationNumber();
  int hitSensor = Hit->sensDetID();
  int hitStation = (hitSensor>>DeBcmShifts::shiftStationID);
  return nStation == hitStation;
}






