// $Id: 
// ============================================================================
// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDataFunctor.h"
#include "Event/RecVertex.h"
//#include "CaloUtils/CaloHypoFilter.h"
// local
#include "PhotonMaker.h"

/** @file
 *
 *  Implementation file for class : PhotonMaker
 *
 *  @date 2003-01-19
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *
 *  @ modified 2006-08-25 for DC06
 *  @ Olivier Deschamps odescham@in2p3.fr
 */
// ============================================================================
/** @var PhotonMakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_TOOL_FACTORY(PhotonMaker)

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name
 *  @param parent tool parent
 */
// ============================================================================
  PhotonMaker::PhotonMaker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool           ( type, name , parent )
  , m_input            ()
// Origin vertex
  , m_point            ()
  , m_pointErr         ()
// PID techniques
  , m_clBase           ()
  , m_clSwitch         ()
// Filters
  , m_clCut            ()
  , m_ptCut            ()
  , m_converted        ()
  , m_unconverted      ()
  ,m_mas               (0.){
  declareProperty ( "Input"                      , m_input = LHCb::ProtoParticleLocation::Neutrals ) ;
  declareProperty ( "Particle"                   , m_part="gamma");
  //
  declareProperty ( "ConfidenceLevelBase"        , m_clBase   ) ;
  declareProperty ( "ConfidenceLevelSwitch"      , m_clSwitch ) ;
  //
  declareProperty ( "ConfLevelCut"               , m_clCut       = -99.   ) ;
  declareProperty ( "PtCut"                      , m_ptCut       = 150.0  ) ;
  declareProperty ( "ConvertedPhotons"           , m_converted   = true   ) ;
  declareProperty ( "UnconvertedPhotons"         , m_unconverted = true   ) ;
  declareProperty ( "PrsThreshold"               , m_minPrs = -1.   ) ;
  //
  declareProperty ( "AddHcalEnergy"              , m_addHcal = false);
  declareProperty ( "MaxHcalRatio"               , m_maxHcal = -1.);
  declareProperty ( "MinHcalRatio"               , m_minHcal = -1.);
  declareProperty ( "MaxPrsEnergy"               , m_maxPrs  = -1.);
  declareProperty( "ClusterCodeMasks"  , m_clusterMasks);

 
  // Confidence level techniques
  m_clBase.push_back("IsNotH");
  m_clSwitch.push_back("PhotonDLL");

  // allowed techniques :
  m_knownCLs.push_back("IsNotH");
  m_knownCLs.push_back("IsNotE");
  m_knownCLs.push_back("IsPhoton");
  m_knownCLs.push_back("PhotonDLL");
  m_knownCLs.push_back("CaloTrMatch");

  //
  m_point = Gaudi::XYZPoint();
  m_pointErr = Gaudi::SymMatrix3x3();

 // declare new interface
  declareInterface<ICaloParticleMaker> (this);
}
// ============================================================================


// ============================================================================
/// destructor
// ============================================================================
PhotonMaker::~PhotonMaker() {}
// ============================================================================

StatusCode PhotonMaker::initialize    ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize GaudiTool",sc);}

  // ParticleProperty
  LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"   << endmsg;
    return sc;
  }
  const LHCb::ParticleProperty* partProp;
  partProp = ppSvc->find( m_part );
  m_Id = (*partProp).pdgID().pid();
  m_mas = partProp->mass();
  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;

  //
  


  // CL techniques  
  for(std::vector<std::string>::const_iterator b = m_clBase.begin(); m_clBase.end() != b ; ++b){
    bool ok = false;
    for(std::vector<std::string>::const_iterator it = m_knownCLs.begin(); m_knownCLs.end() != it ; ++it){
      if( *b == *it){ok=true;break;}
    }
    if( !ok )return Error("Unknown CL technique is required",StatusCode::FAILURE);
  }
  for(std::vector<std::string>::const_iterator b = m_clSwitch.begin(); m_clSwitch.end() != b ; ++b){
    bool ok = false;
    for(std::vector<std::string>::const_iterator it = m_knownCLs.begin(); m_knownCLs.end() != it ; ++it){
      if( *b == *it){ok=true;break;}
    }
    if( !ok )return Error("Unknown CL switch is required",StatusCode::FAILURE);
  }

  info() << " Following technique(s) will be used for CL evaluation : " << m_clBase << endmsg;
  info() << " Switch to " << m_clSwitch << " if one technique is not available " << endmsg;
  if( m_clBase.empty() )Warning(" No PID techniques are selected for CL evaluation" ) ;

  if( m_converted && m_unconverted){debug()<< "BOTH converted (SPD) and unconverted (no SPD) photons are to be created"<< endmsg;}
  else if ( m_converted ){debug()<< "ONLY converted (SPD) photons are to be created"<< endmsg;}
  else if ( m_unconverted ){debug()<< "ONLY unconverted (no SPD) photons are to be created"<< endmsg;}
  else {warning()<<" BOTH converted and unconverted photon are to be skipped !!!" << endmsg; }

  return StatusCode::SUCCESS ;
}
// ============================================================================

StatusCode PhotonMaker::finalize      ()
{
  double f = (m_count[0] == 0 ) ? 0. :  (float) ( m_count[1]+m_count[2])/(float)m_count[0];
  double fc =  (m_count[1]+m_count[2] ==0)? 0 : (float) m_count[2]/(float)(m_count[1] + m_count[2])*100.0;

  info() << name() << " -  Summary -----" << endmsg;
  info() << " Created : " << f << " " << m_part << " per event" << endmsg;
  info() << " " << fc  << "% have SPD hits"<< endmsg;
  info() << " ---------------------------" << endmsg;
  // finalize the base class
  return GaudiTool::finalize ();
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode PhotonMaker::makeParticles (LHCb::Particle::Vector & particles )
{
  // avoid some long names

  if ( !particles.empty() )
  {
    Warning( "makeParticles(): extend non-empty vector of Particles" ).ignore();
  }

  // locate input data
  const LHCb::ProtoParticles* pps = getIfExists<LHCb::ProtoParticles>(evtSvc(),m_input);
  if ( !pps )
  {
    return Warning("No ProtoParticles at " + m_input + " 0 photons created",StatusCode::SUCCESS);
  }

  unsigned long long nConverted = 0 ;
  unsigned long long nUnconverted = 0 ;
  unsigned long long nSelConverted = 0 ;
  unsigned long long nSelUnconverted = 0 ;
  unsigned long long nSkip = 0;
  m_count[0] += 1;

  // Loop over PP
  for ( LHCb::ProtoParticles::const_iterator ipp = pps->begin() ;pps->end() != ipp ; ++ipp ){
    
    const LHCb::ProtoParticle* pp = *ipp ;

    //  ---- skip invalid and charged
    if ( 0 == pp || 0 != pp->track() )   { continue ; }
    const SmartRefVector<LHCb::CaloHypo>& hypos  = pp->calo() ;
    if ( hypos.empty() ) { continue ; }

    //  ---- Check the hypothesis
    const LHCb::CaloHypo*   hypo  = *(hypos.begin());
    if ( 0 == hypo || LHCb::CaloHypo::Photon != hypo->hypothesis() ) { continue ; }
    
    // ---- skip negative energy
    if( hypo->e() <= 0 ) continue;

    // == Evaluate Photon conversion (Spd-based for late conversion after magnet)
    bool cnv = ( pp->info(LHCb::ProtoParticle::CaloDepositID,0.) < 0 ) ;  
    //    bool cnv = (bool) pp->info(LHCb::ProtoParticle::CaloNeutralSpd, 0.) ;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << " Conversion "
                                                  << cnv << " " << pp->info(LHCb::ProtoParticle::CaloDepositID,0.) << endmsg;
    
    // ---- selected (un)converted photons
    if( !cnv && !m_unconverted  ) { continue ; }
    if( cnv  && !m_converted    ) { continue ; }

    // ---- apply Prs filter
    double ePrs =  pp->info(LHCb::ProtoParticle::CaloNeutralPrs,0);
    if( m_minPrs >= 0 && ePrs < m_minPrs )continue;
    if( m_maxPrs >= 0 && ePrs > m_maxPrs )continue;
    cnv ? ++nConverted : ++nUnconverted;

    // ---- apply Hcal filter
    double eHcal=pp->info(LHCb::ProtoParticle::CaloNeutralHcal2Ecal,0);
    if( m_maxHcal >=0 && eHcal/(1.+eHcal) > m_maxHcal)continue;
    if( m_minHcal >=0 && eHcal/(1.+eHcal) < m_minHcal)continue;

    // ---- apply mask on ClusterCode
    bool pass=true;
    if( m_clusterMasks.size() != 0){
      for(std::map<std::string,std::pair<double,double> >::iterator im = m_clusterMasks.begin(); m_clusterMasks.end() != im ; im++){
        std::string type = im->first;
        std::pair<double,double> window = im->second;
        int code = ClusterCode(pp,type);
        if( code == -1)Warning("Unknown ClusterCode mask '"+type+"'",StatusCode::SUCCESS,1.).ignore();
        else if( code < (int)window.first || (int)code > window.second)pass=false;
      }
    }
    if( !pass ) continue;



    // == evaluate kinematical properties
    LHCb::CaloMomentum momentum(pp , m_point , m_pointErr);
    if ( 0 != momentum.status()  ) { continue ; }
    double pT = momentum.pt();
    double E  = momentum.e();
    double px = momentum.px();
    double py = momentum.py();
    double pz = momentum.pz();
    double p  = E;

    // -- apply hcal correction if requested
    if ( m_addHcal || m_mas > 0 ){
      if( m_addHcal ){
        E *= ( 1. + eHcal );
        px *= (1. + eHcal);
        py *= (1. + eHcal);
        pz *= (1. + eHcal);
        pT *= (1. + eHcal);
        p  = E;
      }
      if ( m_mas > 0 ){
        p  = std::sqrt( E*E - m_mas*m_mas );
        px *= (p/E);
        py *= (p/E);
        pz *= (p/E);
        pT *= (p/E);
      }
    }
    if ( pT < m_ptCut ) { continue; }

    // ---- apply CL filter (must be after pT cut to match neutralID definition range)
    const double CL = confLevel( pp ,false);
    if ( CL  < m_clCut ) continue ;
    counter("Confidence Level") += CL;
    cnv ? ++nSelConverted : ++nSelUnconverted;

    // ===== create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( ); // photon constructor
    particle -> setParticleID( LHCb::ParticleID (m_Id) );
    particle -> setProto( pp ) ;

    // === set  mass and mass uncertainties
    particle -> setMeasuredMass( m_mas   ) ;
    particle -> setMeasuredMassErr( 0   ) ; // the mass is EXACT zero!

    // === set confidence level
    particle  -> setConfLevel  ( CL    ) ;

    // === set photon parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();

    // ===  Massive object or Hcal correction
    // Warning : covariant matrix should be modified accordingly -> to be included in CaloParticle ...
    if( m_addHcal || m_mas > 0 )particle->setMomentum( Gaudi::LorentzVector(px,py,pz,E) );

    if ( calopart.status() !=0 ){
      delete particle ;
      ++nSkip;
      warning() << "Unable to fill photon parameters, skip particle [" << nSkip << "]"<< endmsg;
      warning() << " CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      cnv ? --nSelConverted : --nSelUnconverted;
      continue ;
    }

    // === printout
    if (msgLevel(MSG::VERBOSE)){
      verbose() << "----- Single "<<m_part << " found [" << nSelConverted+nSelUnconverted << "]"<<endmsg;
      verbose() << "Pt : " << momentum.pt() << endmsg;
      verbose() << "CL : " << CL << endmsg;
      verbose() << "Chi2 " << pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) << endmsg;
      verbose() << "Converted (SPD digits)? : " << cnv << endmsg;
      verbose() << "CaloDeposit : " <<pp->info(LHCb::ProtoParticle::CaloDepositID,-999.) <<endmsg;
      if( m_addHcal)verbose() << " Hcal factor : " << (1.+eHcal) << endmsg;
      if( m_mas> 0.)verbose() << " Mass : " << m_mas << endmsg;
      verbose() << " "<< endmsg;
    }

    // === push the particle into the container
    particles.push_back( particle );
  }
  m_count[1] += nSelUnconverted ;
  m_count[2] += nSelConverted ;
  counter("Created converted photons")+= nSelConverted;
  counter("Created unConverted photons")+= nSelUnconverted;


  // === debug
  if (msgLevel(MSG::DEBUG) ){
    debug() << " " << endmsg;
    debug() << "-----------------------" << endmsg;
    debug() << " Filtered and created :" << endmsg;
    debug() << " --> " << nSelConverted + nSelUnconverted << " photons (among "
            << nConverted + nUnconverted <<")"<< endmsg;
    debug() << "   -> " << nSelConverted
            << " Converted Photons (among " << nConverted <<")"<< endmsg;
    debug() << "   -> " << nSelUnconverted
            << " Unconverted Photons (among " << nUnconverted <<")"<< endmsg;
    debug() << " Skipped : " << nSkip << endmsg;
    debug() << "-----------------------" << endmsg;
  }
  return StatusCode::SUCCESS ;
}


bool PhotonMaker::clFind(const std::string technique, bool useSwitch)const {  
  const std::vector<std::string> list = (useSwitch) ? m_clSwitch : m_clBase;
  for(std::vector<std::string>::const_iterator it = list.begin(); list.end() != it ; ++it){
    if( *it == technique)return true;
  }
  return false;
}




// ============================================================================
double PhotonMaker::confLevel( const LHCb::ProtoParticle* pp, bool useSwitch ) const{


  if( 0 == pp ){ Error("confLevel(): ProtoParticle* points to NULL!"); return -1 ; };

  double CL = 1.0 ;
  bool hasCL=true;

  // track matching
  if ( clFind("CaloTrMatch",useSwitch) ){
    if ( pp->hasInfo(LHCb::ProtoParticle::CaloTrMatch) ){
      CL *= ( 1.0 - std::exp( -0.5 * pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) )) ;
    }else{
      Warning("confLevel(): CaloTrMatch is not available",StatusCode::SUCCESS,1).ignore();
      hasCL= false;
    } 
  }

  // NN-based photonID (IsNotH)
  if( clFind("IsNotH",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotH )){
      double v = pp->info(LHCb::ProtoParticle::IsNotH , 0. ); 
      if( v > 1. ) v=1.; 
      if( v < 0. ) v=0.;
      CL *= v;
    }else{
      hasCL= false;
      Warning("confLevel(): IsNotH is not available",StatusCode::SUCCESS,1).ignore();
    }
  }
  
  // NN-based photonID (IsNotE)
  if( clFind("IsNotE",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotE )){
      double v = pp->info(LHCb::ProtoParticle::IsNotE , 0. );
      if( v > 1. ) v=1.;
      if( v < 0. ) v=0.;
      CL *= v;
    }else{
      hasCL=false;
      Warning("confLevel(): IsNotE is not available",StatusCode::SUCCESS,1).ignore();
    }
  }

  // IsPhoton
  if( clFind("IsPhoton",useSwitch )){
    if( pp->hasInfo(LHCb::ProtoParticle::IsPhoton )){
      double v =  pp->info(LHCb::ProtoParticle::IsPhoton,+1.);
      if( v > 1. ) v = 1.;
      if( v < 0. ) v = 0.;
      CL *= (1-v);
    }else{
      hasCL=false;
      Warning("confLevel(): IsPhoton is not available",StatusCode::SUCCESS,1).ignore() ;
    }    
  }

  //(obsolete) DLL-based photonID : 
  if( clFind("PhotonDLL",useSwitch)){
    if ( pp->hasInfo(LHCb::ProtoParticle::PhotonID) ){
      double pid = pp->info(LHCb::ProtoParticle::PhotonID, -1.  ) ;
      CL *= 0.5*(std::tanh(pid)+1);
    }else{
      hasCL=false;
      Warning("confLevel(): PhotonDLL is not available",StatusCode::SUCCESS,1).ignore() ;
    }
  }
  if( !hasCL && !useSwitch ){
    Warning("CL base is not available - switch to alternative CL",StatusCode::SUCCESS,1).ignore();
    counter("Switch to alternative CL") += 1;
    return confLevel(pp,true); // try alternative CL
  }
  if( !hasCL )return -1.;
  // return
  return CL ;
}


int PhotonMaker::ClusterCode( const LHCb::ProtoParticle* pp, std::string type ) const{
  int code  = (int)pp->info(LHCb::ProtoParticle::CaloClusterCode,0.);
  int mult  = abs(code)/10;
  int pos   = abs(code) - mult*10;
  int isol  = (code > 0) ? 1 : 0;
  int conf  = pos % 2;
  if(type == "Size")return mult;
  else if(type == "2ndPosition")return pos;
  else if(type == "Shape")return conf;
  else if(type == "Isolated")return isol;
  return -1;
}

