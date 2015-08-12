// ============================================================================
// Include files
// ============================================================================
#include "NeutralProtoPAlg.h"
// ============================================================================
/** @file
 *  Implementation file for class NeutralProtoPAlg
 *  @date 2006-06-09
 *  @author Olivier Deschamps
 */
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NeutralProtoPAlg )
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
  NeutralProtoPAlg::NeutralProtoPAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_hyposLocations    ()
  , m_light_mode        ( false )
  , m_estimator(0)
  , m_mass()
  , m_setMass(false){
  // declare the properties
  declareProperty ( "HyposLocations"        , m_hyposLocations   ) ;
  declareProperty ( "ProtoParticleLocation" ,  m_protoLocation   ) ;
  declareProperty ( "LightMode"      , m_light_mode ,
                    "Use 'light' mode and do not collect all information. Useful for Calibration." ) ;

  // default location from context()
  using namespace LHCb::CaloHypoLocation;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s   ) ) ; // put it 1st to speed-up photon mass retrieval
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Photons      ) ) ;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , SplitPhotons ) ) ;
  //  m_protoLocation = LHCb::CaloAlgUtils::PathFromContext( flag , LHCb::ProtoParticleLocation::Neutrals );
  m_protoLocation = LHCb::ProtoParticleLocation::Neutrals ;
}
// ============================================================================
// Destructor
// ============================================================================
NeutralProtoPAlg::~NeutralProtoPAlg() {}
// ============================================================================
// Initialization
// ============================================================================
StatusCode NeutralProtoPAlg::initialize(){
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc )
    info() << " Hypothesis loaded from " << *loc << endmsg;

  //
  if ( lightMode() )
    info() << "Neutral protoparticles will be created in 'Light' Mode" << endmsg ;


  m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimator",this);
  m_estimator->hypo2Calo()->_setProperty("Seed", "false").ignore();
  m_estimator->hypo2Calo()->_setProperty("PhotonLine", "true").ignore();
  m_estimator->hypo2Calo()->_setProperty("AddNeighbors", "false").ignore();

  m_mass.clear();
  m_setMass=false;
  return sc;
}

double NeutralProtoPAlg::getMass(int cellCode ){
  if( !m_setMass ){
    Warning("You should process MergedPi0 protoparticles first to speed up retrieval of photon 'mass'",StatusCode::SUCCESS,1).ignore();
    // === storing masses
    m_setMass = true;
    using namespace LHCb::CaloHypoLocation;
    std::string loc =  LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s   );
    const LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos>( loc );
    if( hypos == NULL)return 0.;
    for ( LHCb::CaloHypos::const_iterator ihypo = hypos->begin() ; hypos->end() != ihypo  ; ++ihypo ){
      const LHCb::CaloHypo* hypo = *ihypo ;
      if( hypo == NULL)continue;
      using namespace CaloDataType;
      int cellCode = int(m_estimator->data(hypo, CellID ));
      m_mass[cellCode]=m_estimator->data(hypo, HypoM );
    } 
  }  
  std::map<int,double>::iterator it = m_mass.find(cellCode);
  if( it == m_mass.end() )return 0.;
  return it->second;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode NeutralProtoPAlg::execute(){

  // create and register the output container
  LHCb::ProtoParticles* protos = NULL;
  if ( !lightMode() && exist<LHCb::ProtoParticles>(m_protoLocation) ){
    Warning( "Existing ProtoParticle container at " +m_protoLocation+ " found -> Will replace", StatusCode::SUCCESS, 1).ignore();
    counter("Replaced Proto")+=1;
    protos = get<LHCb::ProtoParticles>(m_protoLocation);
    protos->clear();
  }
  else{
    protos = new LHCb::ProtoParticles();
    put( protos , m_protoLocation ) ;
  }

  if( NULL == protos )return Warning("NeutralProto container points to NULL ",StatusCode::SUCCESS);

  // -- reset mass storage
  m_mass.clear();
  m_setMass=false;
  //------ loop over all caloHypo containers
  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc ) {

    //==  Load the CaloHypo container
    const LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos>( *loc );
    if ( NULL == hypos ){
      if( msgLevel(MSG::DEBUG) ) debug()<< "No CaloHypo at '" << (*loc) + "'"<<endmsg ;
      counter("No " + (*loc) + " container") += 1;
      continue;
    }

    // no cluster mass storage when no MergedPi0
    if( LHCb::CaloAlgUtils::toUpper(*loc).find("MERGED") != std::string::npos && hypos->size() == 0)m_setMass=true;
    
    if ( msgLevel(MSG::DEBUG) )debug() << "CaloHypo loaded at " << *loc << " (# " << hypos->size()<<")"<< endmsg;
    int count = 0 ;

    // == Loop over CaloHypos
    for ( LHCb::CaloHypos::const_iterator ihypo = hypos->begin() ; hypos->end() != ihypo  ; ++ihypo ){
      const LHCb::CaloHypo* hypo = *ihypo ;
      if( 0 == hypo ) { continue ; }
      count++;

      // == create and store the corresponding ProtoParticle 
      LHCb::ProtoParticle* proto = new LHCb::ProtoParticle() ;
      protos->insert( proto ) ;

      // == link CaloHypo to ProtoP
      using namespace CaloDataType;
      proto-> addToCalo( hypo ) ;

      // ===== add data to protoparticle 
      if ( !lightMode() ){
        std::ostringstream type("");
        type << hypo->hypothesis();
        std::string hypothesis = type.str();
        // extra data :
        pushData(proto, LHCb::ProtoParticle::CaloNeutralID  ,hypo, CellID   );   // seed cellID
        // retrieve HypoM for photon
        int cellCode = int(m_estimator->data(hypo, CellID ));
        if( hypo -> hypothesis() == LHCb::CaloHypo::Photon ){
          double mass  = getMass( cellCode );
          if( mass > 0. ){
            proto->addInfo( LHCb::ProtoParticle::ClusterMass, mass );
            counter("ClusterMass for Photon") += mass;
          }
        }else if( hypo -> hypothesis() == LHCb::CaloHypo::Pi0Merged ){
          pushData(proto, LHCb::ProtoParticle::ClusterMass, hypo, HypoM);
          m_setMass=true;
          m_mass[cellCode] = m_estimator->data(hypo, HypoM );
        }

        pushData(proto, LHCb::ProtoParticle::CaloNeutralPrs, hypo, HypoPrsE );
        pushData(proto, LHCb::ProtoParticle::CaloNeutralEcal,hypo, ClusterE );

        pushData(proto, LHCb::ProtoParticle::CaloTrMatch, hypo, ClusterMatch , +1.e+06 );  // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::ShowerShape, hypo, Spread);         // ** input to neutralID  && isPhoton (as Fr2)
        pushData(proto, LHCb::ProtoParticle::CaloNeutralSpd, hypo, HypoSpdM );   // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::CaloNeutralHcal2Ecal, hypo, Hcal2Ecal); // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::CaloClusterCode, hypo , ClusterCode);
        pushData(proto, LHCb::ProtoParticle::CaloClusterFrac, hypo , ClusterFrac,1);        

        double dep = (m_estimator->data(hypo,  ToSpdM ) > 0) ? -1. : +1.;
        dep *= m_estimator->data(hypo,  ToPrsE );
        proto -> addInfo ( LHCb::ProtoParticle::CaloDepositID   , dep ) ;     // ** input to neutralID toPrsE=|caloDepositID|

        // DLL-based neutralID (to be obsolete)  :
        pushData(proto, LHCb::ProtoParticle::PhotonID , hypo, NeutralID , -1. ,true    ); // old DLL-based neutral-ID // FORCE
                
        // isNotX  inputs :
        pushData(proto, LHCb::ProtoParticle::CaloNeutralE49   , hypo, E49 );
        pushData(proto, LHCb::ProtoParticle::CaloNeutralE19   , hypo, E19 );      // ** input to neutralID 
        pushData(proto, LHCb::ProtoParticle::CaloPrsNeutralE49, hypo, PrsE49  );  // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::CaloPrsNeutralE19, hypo, PrsE19  );  // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::CaloPrsNeutralE4max,hypo, PrsE4Max); // ** input to neutralID
        pushData(proto, LHCb::ProtoParticle::CaloNeutralPrsM  , hypo, HypoPrsM);  // ** input to neutralID
        // isNotX output :
        pushData(proto, LHCb::ProtoParticle::IsNotH   , hypo, isNotH  , -1. , true     ); // new NN-based neutral-ID (anti-H) // FORCE
        pushData(proto, LHCb::ProtoParticle::IsNotE   , hypo, isNotE  , -1. , true     ); // new NN-based neutral-ID (anti-E) // FORCE

        // isPhoton inputs (photon & mergedPi0 only)
        if( hypo -> hypothesis() != LHCb::CaloHypo::PhotonFromMergedPi0 ){
          pushData(proto, LHCb::ProtoParticle::CaloShapeFr2r4     ,hypo, isPhotonFr2r4);  // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloShapeAsym      ,hypo, isPhotonAsym);   // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloShapeKappa     ,hypo, isPhotonKappa);  // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloShapeE1        ,hypo, isPhotonEseed);     // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloShapeE2        ,hypo, isPhotonE2);     // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsShapeE2     ,hypo, isPhotonPrsE2);     // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsShapeEmax   ,hypo, isPhotonPrsEmax);   // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsShapeFr2    ,hypo, isPhotonPrsFr2);    // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsShapeAsym   ,hypo, isPhotonPrsAsym);   // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsM           ,hypo, isPhotonPrsM);      // -- input to isPhoton  
          pushData(proto, LHCb::ProtoParticle::CaloPrsM15         ,hypo, isPhotonPrsM15);    // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsM30         ,hypo, isPhotonPrsM30);    // -- input to isPhoton
          pushData(proto, LHCb::ProtoParticle::CaloPrsM45         ,hypo, isPhotonPrsM45);    // -- input to isPhoton
          // isPhoton output :
          pushData(proto, LHCb::ProtoParticle::IsPhoton , hypo, isPhoton , +1. , true    ); // NN-based neutral-ID (anti-pi0) // FORCE to +1 when missing (i.e. PT < 2 GeV)
        }        
      }// lightmode      
    } // loop over CaloHypos
    counter( *loc + "=>" + m_protoLocation) += count;
  } // loop over HyposLocations


  if ( msgLevel(MSG::DEBUG) )debug() << "# Neutral ProtoParticles created : " << protos -> size() << endmsg;
  counter ("#protos in " + m_protoLocation) += protos->size() ;
  return StatusCode::SUCCESS;
}
// ============================================================================
//  Finalize
// ============================================================================

StatusCode NeutralProtoPAlg::finalize(){
  //
  if ( lightMode() )
  { info() << "Neutral protoparticles have been created in 'Light' Mode" << endmsg ; }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}



void NeutralProtoPAlg::pushData(LHCb::ProtoParticle* proto ,LHCb::ProtoParticle::additionalInfo pflag ,
                                const LHCb::CaloHypo* hypo ,CaloDataType::DataType hflag , double def,bool force){
  double data =  m_estimator->data(hypo, hflag , def );
  if( data != def || force ){
    proto->addInfo( pflag,data ); // only store when different from default

    // counters
    std::ostringstream flag("");
    flag << pflag;
    std::ostringstream type("");
    type << hypo->hypothesis();
    counter(flag.str()+" for "+ type.str()) += data;
    //info() << " ---- Stored info : '" << flag.str()<<"' for "<< type.str() << " : " << data << endmsg;
  }
}


