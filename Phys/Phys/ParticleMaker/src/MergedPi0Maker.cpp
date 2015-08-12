// $Id: MergedPi0Maker.cpp,v 1.12 2010-09-01 14:54:00 odescham Exp $
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloDet/DeCalorimeter.h"
// local
#include "MergedPi0Maker.h"

/** @file
 *
 *  Implementation file for class : MergedPi0Maker
 *
 *  @date 2006-08-25
 *  @author Olivier Deschamps odescham@in2p3.fr
 *
 */

// ============================================================================
/** @var MergedPi0MakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( MergedPi0Maker )

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name
 *  @param parent tool parent
 */
// ============================================================================
MergedPi0Maker::MergedPi0Maker( const std::string& name,ISvcLocator* pSvcLocator )
: Pi0MakerBase ( name , pSvcLocator ),
  m_calo       ( NULL )
  , m_clusterMasks     ()
  , m_clBase           ()
  , m_clSwitch         ()
{
  setProperty ( "Input" , LHCb::ProtoParticleLocation::Neutrals ) ;
  //
  declareProperty ( "ConfidenceLevelBase"        , m_clBase   ) ;
  declareProperty ( "ConfidenceLevelSwitch"      , m_clSwitch ) ;
  // Filter
  declareProperty( "ConfLevelCut"      , m_clCut = -99.     ) ; 
  setProperty    ( "PtCut"             , 2000. * Gaudi::Units::MeV );
  declareProperty( "GammaPtCut"        , m_gPtCut = 0. * Gaudi::Units::MeV );
  declareProperty( "GammaGammaDistCut" , m_ggDistCut = 2.8 ); // Unit = cellSize
  declareProperty( "Chi2Cut"           , m_chi2Cut   = 1.  );
  declareProperty( "ClusterCodeMasks"  , m_clusterMasks);

 // Confidence level techniques
  m_clBase.push_back("IsPhoton");
  m_clSwitch.push_back("CaloTrMatch");

  // allowed techniques :
  m_knownCLs.push_back("IsNotH");
  m_knownCLs.push_back("IsNotE");
  m_knownCLs.push_back("IsPhoton");
  m_knownCLs.push_back("PhotonDLL");
  m_knownCLs.push_back("CaloTrMatch");
  m_knownCLs.push_back("ClusterMass");
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
MergedPi0Maker::~MergedPi0Maker() {}
// ============================================================================

StatusCode MergedPi0Maker::initialize    ()
{
  // initialize the base class
  const StatusCode sc = Pi0MakerBase::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize Pi0MakerBase",sc);}

  // DeCalorimeter
  m_calo =  getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;

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

  return sc ;
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode MergedPi0Maker::makeParticles (LHCb::Particle::Vector & particles ){
  // avoid some long names

  if( !particles.empty() )
  {
    Warning( "makeParticles(): extend non-empty vector of Particles" ).ignore() ;
  }

  // locate input data
  const LHCb::ProtoParticle::ConstVector& pps = protos() ;

  unsigned long nPp    = 0 ;
  unsigned long nSelPp = 0 ;
  unsigned long nSkip  = 0 ;
  m_count[0] += 1;


  // .. set origin to first PV when exists
  if( m_setPV ){
    const LHCb::RecVertex::Range&  pvs = this->primaryVertices();
    if( pvs.size() != 0 )setPoint((LHCb::Vertex*) pvs.front() );      
  }

  // Loop over PP
  for( LHCb::ProtoParticle::ConstVector::const_iterator ipp = pps.begin() ;
       pps.end() != ipp ; ++ipp ){

    const LHCb::ProtoParticle* pp = *ipp ;

    // ---- skip invalid and charged
    if ( NULL == pp || NULL != pp->track() )   { continue ; }

    // ---- Check the hypothesis
    const LHCb::CaloHypo*   hypo  = *( (pp->calo()).begin() );
    if(LHCb::CaloHypo::Pi0Merged != hypo->hypothesis() )continue;

    ++nPp;
    //counter("0 - ProtoParticle MergedPi0") += 1;

    // Filters
    LHCb::CaloMomentum pi0Momentum( pp , m_point , m_pointErr);

    // ---- apply mass window
    double mass = pi0Momentum.mass();
    if (m_MassWin < fabs(mass-m_Mass) )continue;
    //counter("1 - pass mass window")+=1;

    // ---- apply Pt(pi0) cut
    if ( m_PtCut  > pi0Momentum.pt()     )continue;
    //counter("2 -  pass momentum cut")+=1;

    // ---- apply chi2(Tr,cluster) cut 
    const double chi2 = pp->info(LHCb::ProtoParticle::CaloTrMatch,+1.e+06);
    if ( m_chi2Cut >= 0 && chi2          < m_chi2Cut                )continue;
    //counter("3 -  pass chi2 cut")+=1;


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
    //counter("4 -  pass mask selection")+=1;
    

    // == extract SplitPhotons hypos
    const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
    const LHCb::CaloHypo* g1 = *(hypos.begin() );
    const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
    LHCb::CaloMomentum g1Momentum( g1 , m_point , m_pointErr );
    LHCb::CaloMomentum g2Momentum( g2 , m_point , m_pointErr );
    //info()  << hypos.size() << "    -> " << g1Momentum.pt() << " " << g2Momentum.pt() << endmsg;


    // ---- Apply SplitPhoton pT cut
    if ( m_gPtCut    > g1Momentum.pt()      ){continue;}
    if ( m_gPtCut    > g2Momentum.pt()      ){continue;}
    //counter("5 -pass pT(g) cut")+=1;


    // Gamma-Gamma Min distance
    // retrieve cellID by position
    // (WARNING USE g1 split photon 'position')
    const LHCb::CaloPosition* hypoPos = g1->position();
    if ( NULL == hypoPos){
      Warning("CaloPosition point to null").ignore();
      return StatusCode::FAILURE ;
    }
    const Gaudi::XYZPoint point( hypoPos->x(), hypoPos->y(), hypoPos->z() );

    LHCb::CaloCellID     cellID = m_calo->Cell( point );
    double CellSize =  m_calo->cellSize( cellID  );
    double zpos     =  m_calo->cellZ   ( cellID ) ;
    double epi0     =  pi0Momentum.e();
    double dmin     =  ( epi0 * CellSize > 0 ) ? zpos * 2. * m_Mass / epi0 / CellSize : +9999.; // rare FPE ( hypo outside Calo acceptance ?)
    if ( m_ggDistCut < dmin                 ){continue;}
    //counter("6 - pass dist(gg) cut")+=1;

    // ---- apply CL cut 
    const double CL = confLevel( pp,false );
    if ( m_clCut >= 0 && CL          < m_clCut                ){continue;}
    counter("Confidence Level") += CL;
    //counter(" pass CL cut")+=1;


    ++nSelPp;

    // === create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( );
    particle -> setParticleID( LHCb::ParticleID(m_Id) );
    particle -> setProto( pp ) ;

    // --- set confidence level
    particle  -> setConfLevel  ( CL    ) ;

    // --- set MergedPi0 parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();
    if( calopart.status() != 0 ){
      delete particle ;
      ++nSkip;
      debug() << "CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      debug() << "Unable to fill Merged " << m_pid << " parameters, skip particle [" << nSkip << "]"<< endmsg;
      --nSelPp;
      continue ;
    }


    //-- set mass and mass uncertainties
    particle -> setMeasuredMass( mass ) ; 
    particle -> setMeasuredMassErr( calopart.emass() ) ;

    //-- counter & debug
    m_count[1] += 1;
    if (msgLevel(MSG::VERBOSE)){
      verbose() << " ---- Merged " << m_pid << " found ["<< nSelPp << "]" <<  endmsg;
      verbose() << "Pt    "  << pi0Momentum.pt() << endmsg;
      verbose() << "CL / Chi2  "  << chi2   << " / "<<pp->info(LHCb::ProtoParticle::CaloTrMatch,+1.e+06) << ")"<<endmsg;
      verbose() << "dist(gg)"<< dmin << endmsg;
    }

    // --- add the particle to the container
    particles.push_back( particle );

  }

  counter("Created merged " + m_pid) += nSelPp;
  if (msgLevel(MSG::DEBUG)){
    debug() << " " << endmsg;
    debug() << "-----------------------" << endmsg;
    debug() << " Filtered and created :" << endmsg;
    debug() << " --> " << nSelPp << " Merged " << m_pid << " (among " << nPp <<")"<< endmsg;
    debug() << " Skipped : " << nSkip << endmsg;
    debug() << "--------------------" << endmsg;
  }

  return StatusCode::SUCCESS ;
}

// ============================================================================
bool MergedPi0Maker::clFind(const std::string technique, bool useSwitch)const {  
  const std::vector<std::string> list = (useSwitch) ? m_clSwitch : m_clBase;
  for(std::vector<std::string>::const_iterator it = list.begin(); list.end() != it ; ++it){
    if( *it == technique)return true;
  }
  return false;
}

double MergedPi0Maker::confLevel( const LHCb::ProtoParticle* pp, bool useSwitch ) const{
  if( 0 == pp ){ Error("confLevel(): ProtoParticle* points to NULL!"); return -1 ; };

  double CL = 1.0 ;
  bool hasCL = true;

  // Cluster Mass
  if( clFind("ClusterMass",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::ClusterMass)){
      // assume exponential distribution (it is wrong!)
      CL *= ( std::exp( -0.5 * pp->info(LHCb::ProtoParticle::ClusterMass,0.)
                        / ( 25 * Gaudi::Units::MeV ) ) );
    }  // Update CL
    else{
      hasCL=false;
      Warning("confLevel(): ClusterMass is not available",StatusCode::SUCCESS,1 ).ignore() ; 
    }    
  }

  // NN-based PhotonID IsNotH
  if( clFind("IsNotH",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotH )){
      double v= pp->info(LHCb::ProtoParticle::IsNotH , 0. );
      if( v > 1.) v=1.;
      if( v < 0.) v=0.;
      CL *= v;
    }else{
      hasCL=false;
      Warning("confLevel(): IsNotH is not available",StatusCode::SUCCESS,1 ).ignore() ;
    }    
  }

  // NN-based PhotonID IsNotE
  if( clFind("IsNotE",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotE )){
      double v= pp->info(LHCb::ProtoParticle::IsNotE , 0. );
      if( v > 1.) v=1.;
      if( v < 0.) v=0.;
      CL *= v;
    }else{
      hasCL=false;
      Warning("confLevel(): IsNotE is not available",StatusCode::SUCCESS,1 ).ignore() ;
    }    
  }

  // IsPhoton
  if( clFind("IsPhoton",useSwitch) ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsPhoton )){
      double v =  pp->info(LHCb::ProtoParticle::IsPhoton,+1.);
      if( v > 1. ) v = 1.;
      if( v < 0. ) v = 0.;
      CL *= (1-v);
    }else{
      hasCL=false;
      Warning("confLevel(): IsPhoton is not available",StatusCode::SUCCESS,1 ).ignore() ;
    }
  }


  //(obsolete) DLL-based photonID
  if( clFind("PhotonDLL",useSwitch) ){
    if ( pp->hasInfo(LHCb::ProtoParticle::PhotonID) ){
      double pid = pp->info(LHCb::ProtoParticle::PhotonID, -1.  ) ;
      CL *= 0.5*(std::tanh(pid)+1);
    }else{
      hasCL=false;
      Warning("confLevel() : PhotonDLL is not available",StatusCode::SUCCESS,1 ).ignore() ;
    }
  }
  
  // track matching
  if( clFind("CaloTrMatch",useSwitch)  ){
    if( pp->hasInfo(LHCb::ProtoParticle::CaloTrMatch) ){
      // assume gaussian distribution (it is wrong!)
      CL *= ( 1.0 - std::exp( -0.5 * pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) )) ;
    }else{
      hasCL=false;
      Warning("confLevel() : CaloTrMatch is not available",StatusCode::SUCCESS,1 ).ignore() ;
    }
  }

  if( !hasCL && !useSwitch ){
    Warning("CL base is not available - switch to alternative CL",StatusCode::SUCCESS,1).ignore();
    counter("Switch to alternative CL") += 1;
    return confLevel(pp,true); // try alternative CL
  }
  if( !hasCL )return -1.;
  

  return CL ;
}


int MergedPi0Maker::ClusterCode( const LHCb::ProtoParticle* pp, std::string type ) const{
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

