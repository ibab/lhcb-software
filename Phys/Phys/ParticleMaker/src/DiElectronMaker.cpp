// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "CaloUtils/CaloTrackTool.h"
#include "Kernel/IRelatedPVFinder.h"
// local
#include "DiElectronMaker.h"
#include "CaloUtils/CaloParticle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DiElectronMaker
//
// 2011-02-25 : Olivier Deschamps - Edwige Tournefier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DiElectronMaker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DiElectronMaker::DiElectronMaker( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : ChargedParticleMakerBase ( name , pSvcLocator ),
    m_trSel   ( NULL ){
  declareProperty("ProtoFilter"        , m_toolType = "ProtoParticleCALOFilter");
  declareProperty("ProtoFilterName"    , m_toolName = "Electron"); // Electron PID filter is the default (for ProtoP Input only)
  declareProperty("OppositeSign"       , m_oppSign = true);
  declareProperty("SymetricPair"       , m_symgamma = false);
  declareProperty("ElectronMaxPIDcut"  , m_eidmax=-99999.);
  declareProperty("ElectronPIDcut"     , m_eidmin=-99999.);
  declareProperty("DiElectronPIDcut"   , m_gid=-99999.);
  declareProperty("ElectronCLcut"      , m_ecl = -1.);        //  electron CL lower threshold
  declareProperty("DiElectronCLcut"    , m_gcl = -1.);        //  di-electron CL lower threshold
  declareProperty("DiElectronPtMin"    , m_ptmin = 500.0);   //  min Pt of pair range
  declareProperty("DiElectronMassMax"  , m_mmax = 200.0);    //  Upper mass range
  declareProperty("DiElectronMassMin"  , m_mmin =  0.0);    //  Lower mass range 
  declareProperty("ADAMASSFactor"      , m_aFactor = 1.5); 
  declareProperty("eOpMax"             , m_eOpMax=-1.); 
  declareProperty("eOpMin"             , m_eOpMin=-1.);
  declareProperty("ElectronInputs"     ,  m_eleinputs  );// start from protoParticles if no Std electron input
  declareProperty("BremCorrectionMethod"   , m_method=0);
  declareProperty("DeltaY"                 , m_deltaY=-1);
  declareProperty("DeltaYmax"              , m_deltaYmax=-1);
  declareProperty("UseEcalEnergy"          , m_ecalE=40*Gaudi::Units::GeV);
  declareProperty("VeloCnvThreshold"       , m_vc=1.5);
  declareProperty( "MotherID", m_pid = "gamma", "Mother ID" );
}
//=============================================================================
// Destructor
//=============================================================================
DiElectronMaker::~DiElectronMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DiElectronMaker::initialize() {
  StatusCode sc = ChargedParticleMakerBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg; 

  m_pFilter     = tool<IProtoParticleFilter>( m_toolType,m_toolName, this);
  m_caloElectron= tool<ICaloElectron>("CaloElectron","CaloElectron",this);
  m_trSel       = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );
  m_calo        = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;

  //get zcalo
  Gaudi::Plane3D plane = m_calo->plane(CaloPlane::ShowerMax);
  double Hesse = plane.HesseDistance();
  m_zcalo = -Hesse/plane.Normal().Z();


  // ParticleProperty
  m_ePps  = ppSvc()->find( "e+" ); // daughters = electron pair
  m_gPps  = ppSvc()->find( m_pid );// mother particle - default = (conversion) gamma

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DiElectronMaker::makeParticles (LHCb::Particle::Vector & dielectrons ){

  //------------ select electron input
  LHCb::Particle::Vector electrons;  
  if( m_eleinputs.empty() && exist<LHCb::ProtoParticles>( m_input ) ){
    // Starting from protoparticles
    const LHCb::ProtoParticles* pps = get<LHCb::ProtoParticles>( m_input );
    if (msgLevel(MSG::DEBUG))debug() << " Starting from " << pps->size() << " protoParticles " << endmsg;
    counter("Input protoP from "+m_input) += pps->size();
    for(LHCb::ProtoParticles::const_iterator ipp = pps->begin();pps->end() != ipp ; ++ipp){
      // apply filter and produce electrons particles
      const LHCb::ProtoParticle* proto = *ipp;
      if( m_pFilter->isSatisfied( proto )){
        const int pID = m_ePps->particleID().pid() * (int)(proto->charge());
        LHCb::Particle* electron = new LHCb::Particle(  (LHCb::ParticleID)pID );
        electron->setMeasuredMass(m_ePps->mass());
        electron->setMeasuredMassErr(0.);
        electron->setProto( proto );
        const LHCb::Track* track = proto->track() ;
        if ( !trSel()->accept(*track) ){delete electron; continue;}
        const LHCb::State* state = usedState( track );
        p2s()->state2Particle( *state, *electron ).ignore();// status code        
        double cl = ConfLevel( electron );
        double pid= ePID( electron );
        if( m_ecl > 0 &&  cl  < m_ecl ){delete electron; continue;}
        if( pid < m_eidmin){delete electron; continue;}
        electrons.push_back( electron );  
      }
    }
  }else if( !m_eleinputs.empty() ){
    unsigned int ninputs=0;
    for(std::vector<std::string>::iterator in=m_eleinputs.begin();m_eleinputs.end()!=in;++in){
      std::string eleinput=*in;
      if( !exist<LHCb::Particle::Range>( eleinput ) ){
        Warning("Container " + eleinput + " undefined",StatusCode::SUCCESS).ignore();
        continue;
      }
      // Starting from electron particles  
      LHCb::Particle::Range iElectrons = get<LHCb::Particle::Range>( eleinput );
      ninputs+=iElectrons.size();
      for(LHCb::Particle::Range::iterator ip = iElectrons.begin(); iElectrons.end() != ip ; ++ip){
        LHCb::Particle* p = (*ip)->clone(); // clone input electrons before removing brem
        double cl = ConfLevel( p );
        double pid= ePID( p );
        if( p->proto() == NULL || p->proto()->track() == NULL){
          delete p;
          return Warning("input particle MUST be basic electrons",StatusCode::FAILURE);
        }
        const LHCb::Track* track = p->proto()->track() ;
        if ( !trSel()->accept(*track) ){delete p; continue;}
        //if ( !m_pFilter->isSatisfied( p->proto() )){delete p;continue;} // no proto filtering when using particle as input
        if( cl  < m_ecl ){delete p; continue;} 
        if( pid < m_eidmin){delete p;continue;} 
        if( bremAdder()->removeBrem( p ) ) counter("BremStrahlung removal") += 1.; // remove BremStrahlung if any
        electrons.push_back( p ); 
      }   
      counter("Input electrons from "+ eleinput) += iElectrons.size(); 
    }     
    if (msgLevel(MSG::DEBUG))debug() << " Starting from " << ninputs << " electrons " << endmsg;
    if( m_eleinputs.size() > 1)counter("Total input electrons") += ninputs;
  }  else { 
    return Warning("No valid input container" ,StatusCode::SUCCESS); 
  }
  counter("Selected electrons")+= electrons.size();
  
  double vc1=-2;
  double vc2=-2;
  //-------------- process
  
  //loop over electrons
  for( LHCb::Particle::Vector::iterator ip1 = electrons.begin() ; electrons.end() != ip1 ; ++ip1 ){
    LHCb::Particle* p1 = *ip1;    
    double cl1 = ConfLevel(p1);
    double pid1= ePID(p1);
    if( pid1 < m_eidmin)continue; // both electrons with pid > m_eidmin

    const LHCb::ProtoParticle* proto1 = p1->proto();    
    // e/p selection (only for long/downstream tracks in Ecal acceptance)
    bool ok1=  m_caloElectron->set( *ip1 );
    double eOp1=m_caloElectron->eOverP();
    counter("Electron in Ecal acc.")+=(int) ok1;
    if( (m_eOpMax > 0 || m_eOpMin>0) && !ok1 ){
      counter("Electron not in Ecal acc. rejected (e/p cut)")+=1;
      continue; 
    }
    if((m_eOpMax > 0 || m_eOpMin>0) && !ok1 )continue; // electron not in Ecal acceptance
    if( m_eOpMax > 0 && eOp1 > m_eOpMax) continue; 
    if( m_eOpMin > 0 && eOp1 < m_eOpMin) continue;
    LHCb::Particle* ele1 = p1->clone();

    //get electron ECAL Hypo info
    const LHCb::CaloHypo::Position* epos1 = NULL;
    LHCb::CaloHypo* ce1 = m_caloElectron->electron();
    if( ce1 != NULL)epos1=ce1->position();    
    // get Y from track linear-extrapolation if no associated cluster
    double ye1(0),Ee1(0),sprye1(999),errEcalo1(999);
    if ( epos1 ){
      LHCb::CaloPosition::Covariance cov1=epos1->covariance();;
      LHCb::CaloPosition::Spread poscov1=epos1->spread();
      ye1=epos1->y();
      Ee1=epos1->e();
      sprye1=poscov1(1,1);
      errEcalo1=cov1(2,2);
    } else { 
      std::pair<double,double> ytr=getY(proto1,m_zcalo);
      ye1=ytr.first;
      sprye1=ytr.second;
    }
    
    //get Velo charge
    vc1 = veloCh(ele1);
    
    //loop on 2nd electron
    for( LHCb::Particle::Vector::iterator ip2 = ip1+1 ; electrons.end() != ip2 ; ++ip2 ){
      LHCb::Particle* p2 = *ip2;
      // check sign of particles
      if( m_oppSign && p1->charge() ==  p2->charge())continue;
      else if( !m_oppSign && p1->charge() != p2->charge() )continue;      


      //--- Apply PID
      double pid2= ePID(p2);      
      double gPID = pid1+pid2;
      // check CL & PID
      if( pid1 < m_eidmax && pid2 < m_eidmax)continue; // at least one electron with pid > m_eidmax
      if( pid1 < m_eidmin || pid2 < m_eidmin)continue; // both electrons with pid > m_eidmin
      if( gPID < m_gid)continue;
      // diElectron CL is the product of electrons CL
      double cl2 = ConfLevel(p2);
      double gCL  = cl1*cl2;
      if( m_gcl > 0 && gCL<m_gcl  ) continue;

      
      //--- e/p selection
      const LHCb::ProtoParticle* proto2 = p2->proto();      
      bool ok2=  m_caloElectron->set( *ip2 );
      double eOp2=m_caloElectron->eOverP();
      if((m_eOpMax > 0 || m_eOpMin>0) && !ok2 )continue;
      if( m_eOpMax > 0 && eOp2 > m_eOpMax)continue;
      if( m_eOpMin >=0 && eOp2 < m_eOpMin)continue;	
      LHCb::Particle* ele2 = p2->clone();

      //Velo charge
      vc2 = veloCh(ele2);      
    
      //get electron ECAL Hypo info
      const LHCb::CaloHypo::Position* epos2 = NULL;
      LHCb::CaloHypo* ce2 = m_caloElectron->electron();
      if( ce2 != NULL)epos2=ce2->position();
      double ye2(0),Ee2(0),sprye2(999),errEcalo2(999);
      if (epos2){
        LHCb::CaloPosition::Covariance cov2=epos2->covariance();
        LHCb::CaloPosition::Spread poscov2=epos2->spread();
        ye2=epos2->y();
        Ee2=epos2->e();
        sprye2=poscov2(1,1);
        errEcalo2=cov2(2,2);
      } else{ 
        std::pair<double,double> ytr=getY(proto2,m_zcalo);
        ye2=ytr.first;
        sprye2=ytr.second;
      }
      
      
      // check that the 2 electrons have the same Y within 3 sigma and maximum within 20cm (gamma conversion only)
      if(m_pid == "gamma"){
        if( m_deltaY> 0 && fabs(ye1-ye2) > m_deltaY*sqrt(sprye1+sprye2))continue;
        if( m_deltaYmax > 0 &&  fabs(ye1-ye2)>m_deltaYmax)continue;
      }
      
      // APPLY BremStrahlung correction
      bool b = bremAdder()->addBrem2Pair(ele1,ele2); 
      if(b)counter("Applying Brem-correction to dielectron")+=1;

      // Calo resolution is better at high energies
      if (m_ecalE > 0 && Ee1 > m_ecalE && (Ee1-ele1->p())>2*errEcalo1 )  ele1->setMomentum( ele1->momentum()*Ee1/ele1->p());
      if (m_ecalE > 0 && Ee2 > m_ecalE && (Ee2-ele2->p())>2*errEcalo2 )  ele2->setMomentum( ele2->momentum()*Ee2/ele2->p());
      
      // mass window pre-cut 
      Gaudi::LorentzVector da=ele1->momentum()+ele2->momentum();
      if( da.M() > m_mmax*m_aFactor || da.M() < m_mmin/m_aFactor)continue;
      
      // pt cut
      if ( da.pt() < m_ptmin) continue;
      
      // create local mother
      LHCb::Particle mother ( m_gPps->particleID() );  	
      
      LHCb::Particle::ConstVector epair ;
      epair.push_back( ele1 );
      epair.push_back( ele2 );
      LHCb::Vertex vertex;
      particleCombiner()->combine( epair , mother , vertex);
      double Mgamma_comb=mother.momentum().M();
      mother.setMeasuredMass(Mgamma_comb);
      mother.setConfLevel( gCL );
      
      // in case of gamma conversion - Look at Velo Charge
      if( m_pid == "gamma"){
        LHCb::Particle* part1;
        int icas=0;
        if ( (vc1==vc2 && vc1> m_vc) || (vc2 <0 && vc1 > m_vc) ) {
          part1 = ele1;
          icas = (vc1==vc2) ? 2 : 1;
        } else if ( vc1<0 && vc2 > m_vc) {
          part1=ele2;
          icas=1;
        } else {
          icas=0;
        }
        
        // DL pairs with double Velo charge for L: keep momentum direction of Long track and set dielectron mass to 0
        if ( 1 == icas){    
          Gaudi::LorentzVector& mome = (Gaudi::LorentzVector&) mother.momentum();
          Gaudi::XYZPoint pos;
          Gaudi::SymMatrix3x3 errpos;
          const LHCb::ProtoParticle* proto = part1->proto();
          const LHCb::Track* tr=proto->track();
          tr->position(pos,errpos);
          mother.setReferencePoint(pos);
          mother.setPosCovMatrix(errpos);
          double pm  = mome.P();
          double p1  = part1->p();
          double fac = pm/p1;
          double px = part1->momentum().px()*fac;
          double py = part1->momentum().py()*fac;
          double pz = part1->momentum().pz()*fac;
          double mas = 0;//mome.mass();
          double e  = sqrt( mas*mas + px*px + py*py+ pz*pz);
          mome.SetPxPyPzE( px,py,pz,e);
        }
      }
      
      
      if (mother.p()>0.){
        if (m_symgamma) {
          Gaudi::LorentzVector mom = mother.momentum();
          Gaudi::LorentzVector symmom(-mom.Px(),-mom.Py(),mom.Pz(),mom.E());
          const LHCb::VertexBase* bestPV = this->calculateRelatedPV(&mother);	
          if (bestPV) {
            const Gaudi::XYZPoint PVpos=bestPV->position();
            Gaudi::XYZPoint PoCA=getPoCA(&mother,PVpos);
            mother.setReferencePoint(PoCA);
          }
          mother.setMomentum(symmom);
        }
        
        Gaudi::LorentzVector mothermom = mother.momentum();
        double Mgamma=mothermom.M();
        if( Mgamma > m_mmax || Mgamma < m_mmin)continue;
        
        //--- CREATE THE DIELECTRON
        const LHCb::Particle* dielectron = this->cloneAndMarkTree( &mother );
        dielectrons.push_back( (LHCb::Particle*) dielectron );     
        bremAdder()->bremMomenta(ele1,ele2,"Dielectron"); // for statistics output
      }else{ 
        if (msgLevel(MSG::DEBUG))debug()<<"DiElectron fit failed: "<<da.px()<<" , " 
                                        <<da.py()<<" , "<<da.E()<<" , "<<da.M()<<endmsg; 
      } 
      delete ele2; 
    } 
    delete ele1;
  } 
  
  if (msgLevel(MSG::DEBUG))debug() << "Created " << dielectrons.size() << " " << m_pid << "->ee" << endmsg; 
  counter("Created "+m_pid+"->ee") += dielectrons.size();         
  
  // clean in all case  
  for( LHCb::Particle::Vector::iterator ip = electrons.begin() ; electrons.end() != ip ; ++ip ){ 
    LHCb::Particle* p = *ip; 
    if( NULL == p)continue; 
    delete p;  
  } 
  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS; 
} 



double DiElectronMaker::veloCnv(LHCb::Particle* p1, LHCb::Particle* p2){
  const LHCb::ProtoParticle* pp1 = p1->proto();
  const LHCb::ProtoParticle* pp2 = p2->proto();
  if( NULL == pp1 && NULL == pp2 )return -1.;
  double vc1 = pp1->info( LHCb::ProtoParticle::VeloCharge,  0.0 );
  double vc2 = pp2->info( LHCb::ProtoParticle::VeloCharge,  0.0 );
  if(vc1*vc2 == 0.0)return -1.0;
  double eps = 10.e-3;
  return  (fabs(vc1-vc2) > eps) ? -1.0 : vc1 ;
}



double DiElectronMaker::veloCh(LHCb::Particle* p1){
  const LHCb::ProtoParticle* pp1 = p1->proto();
  if( NULL == pp1 )return -1.;
  double vc1 = pp1->info( LHCb::ProtoParticle::VeloCharge,  -1. );
  return  vc1 ;
}


double DiElectronMaker::ePID( LHCb::Particle* electron){
  const LHCb::ProtoParticle* proto = electron->proto();
  if( NULL == proto )return 0.;
  return proto->info( LHCb::ProtoParticle::CombDLLe,  -999.0 );
}


double DiElectronMaker::ConfLevel( LHCb::Particle* electron){
  // à redefinir
  const LHCb::ProtoParticle* proto = electron->proto();
  if( NULL == proto )return 0.;
  const double ve  = proto->info( LHCb::ProtoParticle::CombDLLe,  -999.0 );
  const double vmu = proto->info( LHCb::ProtoParticle::CombDLLmu, -999.0 );
  const double vk  = proto->info( LHCb::ProtoParticle::CombDLLk,  -999.0 );
  const double vp  = proto->info( LHCb::ProtoParticle::CombDLLp,  -999.0 );
  const double estimator = (4*ve-vmu-vk-vp)/2.;
  double confLevel = 0.5*(tanh(estimator)+1); 
  return confLevel;
}

std::pair<double,double> DiElectronMaker::getY(const LHCb::ProtoParticle* proto, double m_zcalo) {
  const LHCb::Track* tr=proto->track();
  LHCb::State nstate = tr->firstState();
  double ty=nstate.ty();
  double y=nstate.y()+ty*(m_zcalo - nstate.z());
  Gaudi::SymMatrix3x3 cov   = nstate.errPosition() + (m_zcalo - nstate.z())*(m_zcalo - nstate.z())*nstate.errSlopes();
  double covy=cov(1,1);
  return std::make_pair(y,covy);
}

Gaudi::XYZPoint DiElectronMaker::getPoCA(LHCb::Particle* particle, const Gaudi::XYZPoint PVpos){
  const Gaudi::XYZPoint pref = particle->referencePoint();
  const Gaudi::LorentzVector mom = particle->momentum();
  double mom2=particle->p()*particle->p();
  double scal=((PVpos.X()-pref.X())*mom.px()+(PVpos.Y()-pref.Y())*mom.py()+(PVpos.Z()-pref.Z())*mom.pz())/mom2;
  double x=pref.X()+mom.px()*scal;
  double y=pref.Y()+mom.py()*scal;
  double z=pref.Z()+mom.pz()*scal;
  return Gaudi::XYZPoint(x,y,z);
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode DiElectronMaker::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return ChargedParticleMakerBase::finalize();
}

//=============================================================================
