// $Id: L0CaloDigit.cpp,v 1.4 2002-04-04 06:04:13 ocallot Exp $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

/// CaloEvent
#include "Event/CaloDigit.h"

/// Event/L0Event
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"

// local
#include "L0CaloDigit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloDigit
//
// 23/08/2001 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<L0CaloDigit>          Factory ;
const        IAlgFactory& L0CaloDigitFactory = Factory ;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
L0CaloDigit::L0CaloDigit( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_nameOfEcalDataContainer   ( CaloDigitLocation::Ecal )
  , m_nameOfHcalDataContainer   ( CaloDigitLocation::Hcal )
  , m_nameOfPrsDataContainer    ( CaloDigitLocation::Prs  )
  , m_nameOfSpdDataContainer    ( CaloDigitLocation::Spd  )
  , m_nameOfEcalOutput          ( L0CaloAdcLocation::Ecal     )
  , m_nameOfHcalOutput          ( L0CaloAdcLocation::Hcal     )
  , m_nameOfPrsOutput           ( L0PrsSpdHitLocation::Prs    )
  , m_nameOfSpdOutput           ( L0PrsSpdHitLocation::Spd    )
  , m_nameOfGeometryRoot        ( "/dd/Structure/LHCb/" )
  , m_etScale                   ( 20. * MeV )
  , m_prsThreshold              ( 10. * MeV )

{
  declareProperty("EcalData"        , m_nameOfEcalDataContainer  ) ;
  declareProperty("HcalData"        , m_nameOfHcalDataContainer  ) ;
  declareProperty("PrsData"         , m_nameOfPrsDataContainer   ) ;
  declareProperty("SpdData"         , m_nameOfSpdDataContainer   ) ;
  declareProperty("GeometryRoot"    , m_nameOfGeometryRoot       ) ;
  declareProperty("EtScale"         , m_etScale                  ) ;
  declareProperty("PrsThreshold"    , m_prsThreshold             ) ;
  declareProperty("EcalOutput"      , m_nameOfEcalOutput         ) ;
  declareProperty("HcalOutput"      , m_nameOfHcalOutput         ) ;
  declareProperty("PrsOutput"       , m_nameOfPrsOutput          ) ;
  declareProperty("SpdOutput"       , m_nameOfSpdOutput          ) ;
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloDigit::~L0CaloDigit() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloDigit::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve the ECAL detector element

  SmartDataPtr<DeCalorimeter> detEcal( detSvc() ,
                                       m_nameOfGeometryRoot + "Ecal" ) ;
  if( 0 == detEcal ) {
    log << MSG::ERROR << "Unable to retrieve Detector Element = "
        << m_nameOfGeometryRoot+"Ecal" << endreq;
    return StatusCode::FAILURE ;
  }
  m_ecal = (DeCalorimeter*) detEcal;

  // Retrieve the HCAL detector element

  SmartDataPtr<DeCalorimeter> detHcal( detSvc() ,
                                       m_nameOfGeometryRoot + "Hcal" ) ;
  if( 0 == detHcal ) {
    log << MSG::ERROR << "Unable to retrieve Detector Element = "
        << m_nameOfGeometryRoot + "Hcal" << endreq;
    return StatusCode::FAILURE ;
  }
  m_hcal = (DeCalorimeter*) detHcal;

  // Initialize the gain correction for leakage in the 2x2 area

  m_gainCorrEcal[0] = 1.00 ;
  m_gainCorrEcal[1] = 1.04 ;
  m_gainCorrEcal[2] = 1.08 ;

  m_gainCorrHcal[0] = 1.00 ;
  m_gainCorrHcal[1] = 1.05 ;

  log << MSG::INFO << "Et scale " << m_etScale/MeV << " MeV/count."
      << " Preshower threshold " << m_prsThreshold/MeV << " MeV." << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloDigit::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Get the ECAL input container

  SmartDataPtr< ObjectVector<CaloDigit> > ecalDigit ( eventSvc(),
                                          m_nameOfEcalDataContainer );
  if( 0 == ecalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Ecal data container="
        << m_nameOfEcalDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0CaloAdcs* ecalRaw = new L0CaloAdcs();
  StatusCode sc = eventSvc()->registerObject( m_nameOfEcalOutput, ecalRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != ecalRaw ) { delete ecalRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << m_nameOfEcalOutput << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  ObjectVector<CaloDigit>::const_iterator digit;  
  for( digit = ecalDigit->begin() ; ecalDigit->end() != digit ; ++digit ) {
    CaloCellID ID  = (*digit)->cellID();
    double  energy = (*digit)->e() * m_gainCorrEcal[ID.area()];
    double  eT     = energy * m_ecal->cellSine(ID) ;
    int     digEt  = (int) floor( eT/m_etScale + .5 );  // Round to nearest int
    if ( 255 < digEt ) { digEt = 255 ; }

    if ( 0 < digEt ) {
      L0CaloAdc* anAdc = new L0CaloAdc( ID, digEt);
      ecalRaw->add( anAdc );
    }
  }
  log << MSG::DEBUG << "ECAL Digit size " << ecalDigit->size() << " Trigger "
      << ecalRaw->size() << endreq;


  // Get the HCAL input container

  SmartDataPtr<ObjectVector<CaloDigit> > hcalDigit ( eventSvc() ,
                                         m_nameOfHcalDataContainer );
  if( 0 == hcalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Hcal data container="
        << m_nameOfHcalDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0CaloAdcs* hcalRaw = new L0CaloAdcs();

  sc = eventSvc()->registerObject( m_nameOfHcalOutput, hcalRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != hcalRaw ) { delete hcalRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << m_nameOfHcalOutput << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( digit = hcalDigit->begin() ; hcalDigit->end() != digit ; ++digit ) {
    CaloCellID ID = (*digit)->cellID();
    double energy = (*digit)->e() * m_gainCorrHcal[ID.area()];
    double eT     = energy * m_hcal->cellSine(ID) ;
    int    digEt  = (int) floor( eT/m_etScale + .5 ) ; // Round to nearest int.
    if ( 255 < digEt ) { digEt = 255 ; }

    //================================================================
    //  Special test: Remove the first/last 6 row of HCAL....
    //  HCAL will now be from row 9 to 22 instead of 3 to 28
    //================================================================
    //int area = ID.area();
    //int row  = ID.row();
    //if ( 0 == area ) {
    //  if ( ( 9 > row) || ( 22 < row ) ) {
    //    digEt = 0;
    //  }
    //}
    //================================================================

    if ( 0 < digEt ) {
      L0CaloAdc* anAdc = new L0CaloAdc( ID, digEt);
      hcalRaw->add( anAdc );
    }
  }
  log << MSG::DEBUG << "HCAL Digit size " << hcalDigit->size() << " Trigger "
      << hcalRaw->size() << endreq;

  // PrsShower

  SmartDataPtr<ObjectVector<CaloDigit> > prsDigit ( eventSvc() ,
                                                    m_nameOfPrsDataContainer );
  if( 0 == prsDigit ) {
    log << MSG::ERROR << "Unable to retrieve Prs data container="
        << m_nameOfPrsDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0PrsSpdHitVector* prsRaw = new L0PrsSpdHitVector();

  sc = eventSvc()->registerObject( m_nameOfPrsOutput, prsRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != prsRaw ) { delete prsRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << m_nameOfPrsOutput << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( digit = prsDigit->begin() ; prsDigit->end() != digit ; ++digit ) {
    CaloCellID ID     = (*digit)->cellID();
    double     energy = (*digit)->e();

    if ( m_prsThreshold < energy ) {
      L0PrsSpdHit* prs = new  L0PrsSpdHit( ID );
      prsRaw->push_back( prs );
    }
  }
  log << MSG::DEBUG
      << "PRS Digit size " << prsDigit->size()
      << " Trigger " << prsRaw->size() << endreq;

  // SPD

  SmartDataPtr<ObjectVector<CaloDigit> > spdDigit ( eventSvc() ,
                                                    m_nameOfSpdDataContainer );
  if( 0 == spdDigit ) {
    log << MSG::ERROR << "Unable to retrieve Spd data container="
        << m_nameOfSpdDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0PrsSpdHitVector* spdRaw = new L0PrsSpdHitVector();

  sc = eventSvc()->registerObject( m_nameOfSpdOutput, spdRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != spdRaw ) { delete spdRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << m_nameOfSpdOutput << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( digit = spdDigit->begin() ; spdDigit->end() != digit ; ++digit ) {
    CaloCellID ID     = (*digit)->cellID();
    double     energy = (*digit)->e();

    if ( 0.1 < energy ) {        // Threshold necessary when reading back DST !
      L0PrsSpdHit* spd = new  L0PrsSpdHit( ID );
      spdRaw->push_back( spd );
    }
  }

  log << MSG::DEBUG
      << "SPD Digit size " << spdDigit->size()
      << " Trigger " << spdRaw->size() << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloDigit::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
