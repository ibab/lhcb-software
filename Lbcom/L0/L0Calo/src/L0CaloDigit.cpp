// $Id: L0CaloDigit.cpp,v 1.1 2002-01-30 15:58:20 ocallot Exp $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

/// CaloEvent
#include "CaloEvent/CaloDigit.h"

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
  , m_nameOfEcalDataContainer   ( "/Event/Raw/Ecal/Digit"    )
  , m_nameOfHcalDataContainer   ( "/Event/Raw/Hcal/Digit"    )
  , m_nameOfPrsDataContainer    ( "/Event/Raw/Prs/Digit"     )
  , m_nameOfSpdDataContainer    ( "/Event/Raw/Spd/Digit"     )
  , m_nameOfOutputRoot          ( "/Event/Trig/L0"   )
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
  declareProperty("OutputRoot"      , m_nameOfOutputRoot         ) ;
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

  SmartDataPtr<DeCalorimeter> detEcal( detDataService() ,
                                       m_nameOfGeometryRoot + "Ecal" ) ;
  if( 0 == detEcal ) {
    log << MSG::ERROR << "Unable to retrieve Detector Element = "
        << m_nameOfGeometryRoot+"Ecal" << endreq;
    return StatusCode::FAILURE ;
  }
  m_ecal = (DeCalorimeter*) detEcal;

  // Retrieve the HCAL detector element

  SmartDataPtr<DeCalorimeter> detHcal( detDataService() ,
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

  // Check the existence of (and create if needed) the output directory
  {
    SmartDataPtr<DataObject> outDir( eventDataService() ,
                                     m_nameOfOutputRoot );
    if( 0 == outDir ) {                        // touch the output directory
      log << MSG::ERROR << "OutputDirectory="
          << m_nameOfOutputRoot << " does not exist" << endreq ;
    }
  }

  // Get the ECAL input container

  SmartDataPtr< ObjectVector<CaloDigit> > ecalDigit ( eventDataService(),
                                          m_nameOfEcalDataContainer );
  if( 0 == ecalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Ecal data container="
        << m_nameOfEcalDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0CaloAdcs* ecalRaw = new L0CaloAdcs();
  std::string outputContainer = m_nameOfOutputRoot+"/EcalRaw" ;
  StatusCode sc = eventDataService()->registerObject( outputContainer,
                                                      ecalRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != ecalRaw ) { delete ecalRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << outputContainer << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = ecalDigit->begin() ;
       ecalDigit->end() != digit ; ++digit ) {
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

  SmartDataPtr<ObjectVector<CaloDigit> > hcalDigit ( eventDataService() ,
                                         m_nameOfHcalDataContainer );
  if( 0 == hcalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Hcal data container="
        << m_nameOfHcalDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0CaloAdcs* hcalRaw = new L0CaloAdcs();

  outputContainer = m_nameOfOutputRoot+"/HcalRaw" ;
  sc = eventDataService()->registerObject( outputContainer,
                                           hcalRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != hcalRaw ) { delete hcalRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << outputContainer << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = hcalDigit->begin() ;
       hcalDigit->end() != digit ; ++digit ) {
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

  SmartDataPtr<ObjectVector<CaloDigit> > prsDigit ( eventDataService() ,
                                                    m_nameOfPrsDataContainer );
  if( 0 == prsDigit ) {
    log << MSG::ERROR << "Unable to retrieve Prs data container="
        << m_nameOfPrsDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0PrsSpdHitVector* prsRaw = new L0PrsSpdHitVector();

  outputContainer = m_nameOfOutputRoot+"/PrsRaw" ;
  sc = eventDataService()->registerObject( outputContainer,
                                           prsRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != prsRaw ) { delete prsRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << outputContainer << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = prsDigit->begin() ;
       prsDigit->end() != digit ; ++digit ) {
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

  SmartDataPtr<ObjectVector<CaloDigit> > spdDigit ( eventDataService() ,
                                                    m_nameOfSpdDataContainer );
  if( 0 == spdDigit ) {
    log << MSG::ERROR << "Unable to retrieve Spd data container="
        << m_nameOfSpdDataContainer << endreq;
    return StatusCode::FAILURE;
  }

  L0PrsSpdHitVector* spdRaw = new L0PrsSpdHitVector();

  outputContainer = m_nameOfOutputRoot+"/SpdRaw" ;
  sc = eventDataService()->registerObject( outputContainer,
                                           spdRaw ) ;
  if( sc.isFailure() ) {
    if( 0 != spdRaw ) { delete spdRaw ; }
    log << MSG::ERROR << "Unable to register the output container="
        << outputContainer << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = spdDigit->begin() ;
       spdDigit->end() != digit ; ++digit ) {
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
