
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"

#include "Event/MCParticle.h"
#include "Event/ParticleID.h"

#include "RichReadout/RichSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignal>  s_factory ;
const         IAlgFactory& RichSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichSignal::RichSignal( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  declareProperty( "HitLocation",
                   m_RichHitLocation =  MCHitLocation::RiHits );

  declareProperty( "PrevLocation",
                   m_RichPrevLocation = "Prev/" + MCHitLocation::RiHits );

  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLocation = "PrevPrev/" + MCHitLocation::RiHits );

  declareProperty( "NextLocation",
                   m_RichNextLocation = "Next/" + MCHitLocation::RiHits );

  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );

}

RichSignal::~RichSignal() {};

StatusCode RichSignal::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  StatusCode scFinder = toolSvc()->retrieveTool("PixelFinder" , m_finder );
  if ( scFinder.isFailure() ) {
    log << MSG::FATAL << "    Unable to create PixelFinder tool" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode scRndm = m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.));
  if ( scRndm.isFailure() ) {
    log << MSG::FATAL << "    Unable to create Random generator" << endreq;
    return StatusCode::FAILURE;
  }

  m_BunchSpace = 25.0;

  return StatusCode::SUCCESS;

}

StatusCode RichSignal::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute" << endreq;


  // Form a new container of MCDeposits
  mcSummedDeposits = new MCRichSummedDeposits();

  SmartDataPtr<MCHits> hits( eventSvc(), m_RichHitLocation );
  if ( !hits ) {
    log << MSG::DEBUG << "Cannot locate MCRichHits at " << m_RichHitLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << hits->size()
        << " MCHits at " << m_RichHitLocation << endreq;
    this->SimpleTreatment( hits );
  }

  SmartDataPtr<MCHits> prev( eventSvc(), m_RichPrevLocation );
  if ( !prev ) {
    log << MSG::DEBUG << "Cannot locate MCRichHits at " << m_RichPrevLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << prev->size()
        << " MCHits at " << m_RichPrevLocation << endreq;
    this->SimpleTreatment( prev ) ;
  }

  SmartDataPtr<MCHits> prevprev( eventSvc(), m_RichPrevPrevLocation );
  if ( !prevprev ) {
    log << MSG::DEBUG << "Cannot locate MCRichHits at " << m_RichPrevPrevLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << prevprev->size()
        << " MCHits at " << m_RichPrevPrevLocation << endreq;
    this->SimpleTreatment( prevprev ) ;
  }

  SmartDataPtr<MCHits> next( eventSvc(), m_RichNextLocation );
  if ( !next ) {
    log << MSG::DEBUG << "Cannot locate MCRichHits at " << m_RichNextLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << next->size()
        << " MCHits at " << m_RichNextLocation << endreq;
    this->SimpleTreatment( next ) ;
  }

  // Register new container to Gaudi data store
  if ( !eventSvc()->registerObject(m_RichSummedDepositLocation,mcSummedDeposits) ) {
    log << MSG::ERROR << "Failed to register MCRichSummedDeposits at "
        << m_RichSummedDepositLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully registered " << mcSummedDeposits->size()
        << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
  }

  return StatusCode::SUCCESS;

}

StatusCode RichSignal::SimpleTreatment( SmartDataPtr<MCHits> hits )
{

  MsgStream log(msgSvc(), name());

  for ( MCHits::const_iterator iHit = hits->begin(); 
        iHit != hits->end(); ++iHit ) {

    // local position on HPD window
    HepPoint3D entry = (*iHit)->entry();
    HepPoint3D position( entry.x()/10.0, entry.y()/10.0, entry.z()/10.0  );
    HepPoint3D local = m_finder->localPosition(position);

    // ID of pixel mapping to this position
    RichSmartID id = m_finder->smartID(position);

    // Is hit in active pixel
    if ( 0 != id.Key() && abs(local.z() <= 0.01) ) {

      // Then create a new deposit
      MCRichDeposit* newDeposit = new MCRichDeposit();
      newDeposit->setParentHit( *iHit );
      newDeposit->setEnergy( this->SimpleEnergy() );
      newDeposit->setTime( (*iHit)->timeOfFlight()/pow(10,9) );

      // Add to the set of other deposits in the pixel
      if ( !mcSummedDeposits->object(id) ) {
        MCRichSummedDeposit* newSummedDeposit = new MCRichSummedDeposit();
        newSummedDeposit->addToDeposits(newDeposit);
        mcSummedDeposits->insert( newSummedDeposit, id );
      } else {
        mcSummedDeposits->object(id)->addToDeposits(newDeposit);
      }

    }
  }

  return StatusCode::SUCCESS;

}

StatusCode RichSignal::finalize()
{

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize" << endreq;

  m_rndm.finalize();

  return StatusCode::SUCCESS;
}

double RichSignal::SimpleEnergy(){

  double gain = 15.0; 
  double rand1 = m_rndm(); 
  double rand2 = m_rndm();

  if (rand1 <= 0.2) {
    return rand2*gain;
  } else {
    return gain;
  }

  return 0; // shouldn't happen
}
