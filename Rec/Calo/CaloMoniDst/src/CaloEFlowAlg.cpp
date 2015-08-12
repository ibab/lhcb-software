// Include files 

// local
#include "CaloEFlowAlg.h"
#include "Event/MCCaloDigit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/RecHeader.h"
#include "Event/ODIN.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CaloEFlowAlg
//
// 2009-04-08 : Aurelien Martens
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloEFlowAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloEFlowAlg::CaloEFlowAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : CaloMoniAlg ( name, pSvcLocator )
  , m_calo(NULL)
  , m_pidKplus (321)
  , m_pidKminus (-321)
  , m_pidPiplus (211) 
  , m_pidPiminus (-211) 
{ 
  declareProperty("EnergyFilterMin"    , m_eFilterMin  = -999 );
  declareProperty("EtFilterMin"        , m_etFilterMin = -999 );
  declareProperty("ADCFilterMin"       , m_ADCFilterMin  = -999 );  
  declareProperty("EnergyFilterMax"    , m_eFilterMax  = -999 );
  declareProperty("EtFilterMax"        , m_etFilterMax = -999 );
  declareProperty("ADCFilterMax"       , m_ADCFilterMax  = -999 );
  declareProperty("MCTruth"            , m_mctruth = false );
  declareProperty("Simulation"         , m_simulation = true );
  declareProperty("IgnoreTAE"          , m_ignoreTAE   = true );
  declareProperty("IgnoreNonBeamCrossing"          , m_ignoreNonBeamCrossing   = true );
  declareProperty("IgnoreNonPhysicsTrigger"          , m_ignoreNonPhysicsTrigger   = true );
  declareProperty("Slot"               , m_slot = "") ;//pile up evetn '' or Prev/ or Next/

  if(detData()     == "Ecal" ){setInputData( LHCb::CaloDigitLocation::Ecal );}
  else if(detData()== "Hcal" ){setInputData( LHCb::CaloDigitLocation::Hcal );}    
  else if(detData()== "Prs"  ){setInputData( LHCb::CaloDigitLocation::Prs  );}
  else if(detData()== "Spd"  ){setInputData( LHCb::CaloDigitLocation::Spd  );}

  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(index,3) == "Spd" ) m_detectorName = "Spd";

  if (!m_simulation) { m_mctruth=false;}
}
//=============================================================================
// Destructor
//=============================================================================
CaloEFlowAlg::~CaloEFlowAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloEFlowAlg::initialize() {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;


  if      ( "Ecal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );  }  
  else if ( "Hcal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  }
  else if ( "Prs"  == detData() ){
    m_calo = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Prs  );  }  
  else if ( "Spd"  == detData() ) {
    m_calo = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Spd  );
  }  
  else{
    return Error( "Unknown detector name "+detData() );
  }
  if (!m_calo) return Error("No DeCalorimeter for "+detData());

  hBook1( "4", detData() + " : # of Digits"   ,  m_calo->numberOfCells(),  0, m_calo->numberOfCells()   );
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()  << " initialized" << endmsg;
  


  return  StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloEFlowAlg::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  if (m_simulation==false) {
    LHCb::ODIN* evt = get<LHCb::ODIN>( LHCb::ODINLocation::Default );
    if (evt==0) {
      warning() << " NO ODIN BANK IN THIS EVENT" << endmsg;
      return StatusCode::SUCCESS;
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Event: " << evt->eventNumber()
              << " Run: " << evt->runNumber() << endmsg;
    
    const LHCb::ODIN::BXTypes bxtype =  evt->bunchCrossingType();
    unsigned int tae = evt->timeAlignmentEventWindow();
    unsigned int trigger = evt->triggerType();

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << " TAE " << tae
              << " BXType " << bxtype
              << " trigger type " << trigger
              << endmsg;
    
    if (tae!=0 && m_ignoreTAE) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " TAE WINDOW SET TO " << tae << " WILL SKIP THE EVENT " << endmsg;
      return StatusCode::SUCCESS;
    }
    if (bxtype!=3 && m_ignoreNonBeamCrossing) {
      //3 =  BeamCrossing
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " BEAM CROSSING TYPE IS " << bxtype << " WILL SKIP THE EVENT " << endmsg;
      return StatusCode::SUCCESS;
    }
    if (trigger!=1 && m_ignoreNonPhysicsTrigger){
      //1 = PhysicsTrigger
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " TRIGGER TYPE IS " << trigger << " WILL SKIP THE EVENT " << endmsg;
      return StatusCode::SUCCESS;
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << " TAE " << tae
              << " BXType " << bxtype
              << " trigger type " << trigger
              << endmsg;
  }
  

  typedef const LHCb::CaloDigit::Container Digits;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << name() << " execute " << endmsg;
  
  // produce histos ?
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Producing histo " << produceHistos() << endmsg;
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  
  // get input data
  if( !exist<Digits>( inputData() )){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no digit container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  Digits* digits = get<Digits> ( inputData() );
  if ( digits -> empty() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No digit found in " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  initCounters();
  
  for( Digits::const_iterator digit = digits->begin(); digits->end() != digit ; ++digit ){
    if ( 0 == *digit ) continue;
    
    const LHCb::CaloCellID  id     = (*digit)->cellID();
    
    bool isvalid = m_calo->valid(id) && !m_calo->isPinId( id );
    if( isvalid == false ) continue ;    
    
    double            e      = (*digit)->e();
    double            et     = e * m_calo->cellSine( id );

    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
      verbose() << " before thresholds :  cellID " << id.index() << " e " << e << " et " << et << endmsg;

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "thresholds are EMin " << m_eFilterMin << " EMax " <<m_eFilterMax
              << " EtMin " << m_etFilterMin << " EtMax " << m_etFilterMax << endmsg;
    
    
    double pedShift = m_calo->pedestalShift();
    double gain       = m_calo->cellGain(id);
    int adc = (gain !=0.) ? (int) floor((e + pedShift) / gain) : (int) 0;
 
    
    if( adc  < m_ADCFilterMin && m_ADCFilterMin!=-999) continue;
    if( adc  > m_ADCFilterMax && m_ADCFilterMax!=-999)  continue;
    if( e  < m_eFilterMin && m_eFilterMin!=-999) continue;
    if( e  > m_eFilterMax && m_eFilterMax!=-999)  continue;
    if( et < m_etFilterMin && m_etFilterMin!=-999) continue;
    if( et > m_etFilterMax && m_etFilterMax!=-999) continue;

    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
      verbose() << " cellID " << id.index() << " e " << e << " et " << et << endmsg;
    
    count( id );    
    if(doHisto("1")) fillCalo2D("1", id , 1. , detData() + " digits position 2D view");
    if(doHisto("2")) fillCalo2D("2", id , e  , detData() + " energy weighted - digits position 2D view");
    if(doHisto("3")) fillCalo2D("3", id , et , detData() + " Et weighted - digits position 2D view");
  
  }

  fillCounters("4");  
 

  if (m_mctruth) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "LOOKING FOR MCTRUTH" << endmsg;
 //   std::string nameOfMCDigits = "MC/" + m_detectorName + "/Digits";
//    typedef const LHCb::MCCaloDigit::Container MCDigits;
    // Get the MCDigits
//    MCDigits* digits = get<MCDigits> (eventSvc(),LHCb::MCCaloDigitLocation::Prs);
 //   if( 0 == digits ) {
  //    error() << "Cannot locate MCCaloHits in " << m_calo << endmsg;
  //    return StatusCode::FAILURE ;
   // }
    std::string nameOfMCHits = m_slot + "MC/" + m_detectorName + "/Hits";
    typedef const LHCb::MCCaloHit::Container MCHits;
    // Get the MCHits
    MCHits* hits = get<MCHits> (eventSvc(),nameOfMCHits);
    if( 0 == hits ) {
      error() << "Cannot locate MCCaloHits in " << m_calo << endmsg;
      return StatusCode::FAILURE ;
    }
    typedef const LHCb::MCParticle::Container MCParticles;
    // Get the MCParticles
    MCParticles* mcparts = get<MCParticles> (eventSvc(), LHCb::MCParticleLocation::Default );//"Event/MC/Particles");//
    if( 0 == mcparts ) {
      error() << "Cannot locate mcParts in "<< m_calo << endmsg;
      return StatusCode::FAILURE ;
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "hits " << hits << endmsg;

   // for ( MCDigits::const_iterator digit = digits->begin(); digits->end() != digit ; ++digits ){
    //  if (0==*digit) continue;
     // const SmartRefVector<LHCb::MCCaloHit> mchits = (*digit)->hits();
      //debug() << " activeE in digit " << (*digit)->activeE() << endmsg;
      // if (0==*mchits) continue;
      //for( SmartRefVector<LHCb::MCCaloHit>::const_iterator hit = mchits.begin(); mchits.end() != hit ; ++hit ){
    for( MCHits::const_iterator hit = hits->begin();hits->end()!=hit;++hit) {
      if ( 0 == *hit ) continue;
      
      const LHCb::CaloCellID  id     = (*hit)->cellID();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "*hit " << *hit << " id " << id << endmsg;
      bool isvalid = m_calo->valid(id) && !m_calo->isPinId( id );
      if( isvalid == false ) continue ;
      
      double e = (*hit)->activeE();
      
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << " cellID " << id.index() << " e " << e  << endmsg;        
      
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
        debug() << " part " << (*hit)->particle() << endmsg;
        debug() << " partID " << (*hit)->particle()->particleID().pid()
                << " origin vtx " << (*hit)->particle()->originVertex()->position() << endmsg;
      }
      
      int pid = (*hit)->particle()->particleID().pid();
      
      if(pid==m_pidKplus) {
        if(doHisto("5")) fillCalo2D("5", id , 1. , detData() + " K+ digits position 2D view");
        if(doHisto("6")) fillCalo2D("6", id , e  , detData() + " K+ energy weighted - digits position 2D view");
      }
      else if(pid==m_pidKminus) {
        if(doHisto("7")) fillCalo2D("7", id , 1. , detData() + " K- digits position 2D view");
        if(doHisto("8")) fillCalo2D("8", id , e  , detData() + " K- energy weighted - digits position 2D view");
      }
      else if(pid==m_pidPiplus) {
        if(doHisto("9")) fillCalo2D("9", id , 1. , detData() + " Pi+ digits position 2D view");
        if(doHisto("10")) fillCalo2D("10", id , e  , detData() + " Pi+ energy weighted - digits position 2D view");
      }
      else if(pid==m_pidPiminus) {
        if(doHisto("11")) fillCalo2D("11", id , 1. , detData() + " Pi- digits position 2D view");
        if(doHisto("12")) fillCalo2D("12", id , e  , detData() + " Pi- energy weighted - digits position 2D view");
      }
      
      // }
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
