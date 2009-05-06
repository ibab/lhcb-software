// $Id: CaloEFlowAlg.cpp,v 1.3 2009-05-06 16:15:30 odescham Exp $
// Include files 

// from GaudiKernel
#include "GaudiKernel/AlgFactory.h"
// local
#include "CaloEFlowAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloEFlowAlg
//
// 2009-04-08 : Aurelien Martens
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloEFlowAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloEFlowAlg::CaloEFlowAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : CaloMoniAlg ( name, pSvcLocator ){
  
  declareProperty("EnergyFilterMin"    , m_eFilterMin  = 50.*Gaudi::Units::MeV );
  declareProperty("EtFilterMin"        , m_etFilterMin = 50.*Gaudi::Units::MeV );
  declareProperty("EnergyFilterMax"    , m_eFilterMax  = 1.*Gaudi::Units::GeV );
  declareProperty("EtFilterMax"        , m_etFilterMax = 1.*Gaudi::Units::GeV );

  if(detData()     == "Ecal" ){setInputData( LHCb::CaloDigitLocation::Ecal );}
  else if(detData()== "Hcal" ){setInputData( LHCb::CaloDigitLocation::Hcal );}    
  else if(detData()== "Prs"  ){setInputData( LHCb::CaloDigitLocation::Prs  );}
  else if(detData()== "Spd"  ){setInputData( LHCb::CaloDigitLocation::Spd  );}
}
//=============================================================================
// Destructor
//=============================================================================
CaloEFlowAlg::~CaloEFlowAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloEFlowAlg::initialize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;


  if      ( "Ecal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );  }  
  else if ( "Hcal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  }
  else if ( "Prs"  == detData() ){
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs  );  }  
  else if ( "Spd"  == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd  );
  }  
  else{
    return Error( "Unknown detector name "+detData() );
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloEFlowAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  typedef const LHCb::CaloDigit::Container Digits;
  if ( msgLevel(MSG::DEBUG) )debug() << name() << " execute " << endmsg;
  
  // produce histos ?
  if ( msgLevel(MSG::DEBUG) )debug() << " Producing histo " << produceHistos() << endmsg;
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  
  // get input data
  if( !exist<Digits>( inputData() )){
    debug() << "no digit container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  Digits* digits = get<Digits> ( inputData() );
  if ( digits -> empty() ){
    if ( msgLevel(MSG::DEBUG) )debug() << "No digit found in " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  for( Digits::const_iterator digit = digits->begin(); digits->end() != digit ; ++digit ){
    if ( 0 == *digit ) continue;
    
    const LHCb::CaloCellID  id     = (*digit)->cellID();
    
    bool isvalid = m_calo->valid(id) && !m_calo->isPinId( id );
    if( isvalid == false ) continue ;    
    
    const double            e      = (*digit)->e();
    const double            et     = e * m_calo->cellSine( id );

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " before thresholds :  cellID " << id.index() << " e " << e << " et " << et << endmsg;
    
    if( e  < m_eFilterMin )  continue;
    if( e  > m_eFilterMax )  continue;
    if( et < m_etFilterMin ) continue;
    if( et > m_etFilterMax ) continue;

    if ( msgLevel(MSG::VERBOSE) )verbose() << " cellID " << id.index() << " e " << e << " et " << et << endmsg;
    
    
    if(doHisto("1")) fillCalo2D("1", id , 1. , detData() + " digits position 2D view");
    if(doHisto("2")) fillCalo2D("2", id , e  , detData() + " energy weighted - digits position 2D view");
    if(doHisto("3")) fillCalo2D("3", id , et , detData() + " Et weighted - digits position 2D view");
  
  }
 
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloEFlowAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return CaloMoniAlg::finalize();  // must be called after all other actions
}

//=============================================================================
