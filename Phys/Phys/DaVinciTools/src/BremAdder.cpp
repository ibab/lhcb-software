// $Id$
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// from LHCb
#include "CaloUtils/CaloMomentum.h"
#include "GaudiKernel/Point3DTypes.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/StateParameters.h"
//#include "Event/CaloCellID.h"
// local
#include "BremAdder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BremAdder
//
// 2006-10-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BremAdder );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BremAdder::BremAdder( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  ,m_dllBrem()
  ,m_calo(NULL)
  ,m_method(0){
  declareInterface<IBremAdder>(this);

  // Method 1 : Adding the brem candidate found with BremMatch (reconstruction)
  declareProperty("BremDllCut"  , m_dllBrem  = -999999.); // DLL  cut
  declareProperty("BremChi2Cut" , m_chi2Brem = 300.);     // chi2 cut

  // Method 2 :  looking for brem candidate(s) from a photon container  (active if m_bInput is not empty)
  declareProperty("BremInput"   , m_bInput = "/Event/Phys/StdVeryLooseAllPhotons/Particles");
  declareProperty("PhotonID"    , m_bremid = -2);        //  brem photon ID lower threshold
  declareProperty("PhotonPT"    , m_ptg =100.);  
  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty ("PosTolerance" , m_stol =2.);

  // brem correction method : 
  //    - 0  : add 4-momenta
  //    - 1  : add 3-momenta and re-evaluate the energy according to the mass
  //    - 2  : keep the electron direction unchanged
  declareProperty("BremCorrectionMethod", m_method=0);
}
//============================================================================= 
// Destructor 
//=============================================================================
BremAdder::~BremAdder() {}  

//=============================================================================
StatusCode BremAdder::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  m_p2s = tool<IParticle2State>("Particle2State"); // not private

  if (msgLevel(MSG::DEBUG))  debug() << "BremAdder : correction method = " << m_method << endmsg;
  return StatusCode::SUCCESS;  
}
//=============================================================================
bool BremAdder::brem4particle( LHCb::Particle* particle, std::string what,bool force ) const{
  // boolean 'force' : force re-adding/re-removing brem 


  if (msgLevel(MSG::DEBUG))  
    debug() << what << " BremStrahlung  to/from particle momentum " << particle->momentum() << endmsg ;

  //-- consistency checks
  const LHCb::ProtoParticle* proto = particle ->proto();
  if( NULL == proto) {
    if (msgLevel(MSG::DEBUG))debug() << " The particle is not basic - no brem to be added" << endmsg;
    return false;
  }
  if( NULL == proto->track() ){
    if (msgLevel(MSG::DEBUG))debug() << " No charged track associated to the particle - no brem to be added" << endmsg;
    return false;
  }

  
  //-- check if bremmStrahlung has already been added
  const LHCb::State*  state = usedState( proto->track() );
  bool bremIsAdded = false;  
  double eps = 1E-4;
  if( fabs( particle->momentum().P() - state->p()) > eps ) bremIsAdded = true ;
  //
  if( bremIsAdded  && "add" == what && !force){
    if (msgLevel(MSG::DEBUG))debug() << " The bremstrahlung has already been added - nothing to add"<< endmsg;
    return false;
  }
  if( !bremIsAdded && "remove" == what && !force ){
    if (msgLevel(MSG::DEBUG))debug() << " No bremstrahlung had been added - nothing to remove "<< endmsg;
    return false;  
  }


  // revert the particle momentum  (as in Charged CombinedParticle)
  if( "remove" == what || force ){
    counter("Revert brem correction")+=1;
    StatusCode sc = m_p2s->state2Particle( *state, *particle );
    return sc.isFailure() ? false : true;
  }  
  if( "add" != what )Warning("Don't know what to do with brem ! flag must be 'add' or 'remove'").ignore();


  // ----- Start bremStrahlung recovery -----

  // construct list of brems
  counter("Brem recovery")+=1;
  const std::vector<const LHCb::CaloHypo*>& brems = bremList(particle);
  if( brems.empty()){ 
    if (msgLevel(MSG::DEBUG))debug() << "Empty brem list" << endmsg;
    return true;
  }
  // Brems momentum
  LHCb::CaloMomentum bremPhoton = LHCb::CaloMomentum();  
  for( std::vector<const LHCb::CaloHypo*>::const_iterator ib = brems.begin();brems.end() !=ib ; ib++){
    bremPhoton.addCaloPosition( *ib );
  }


  if (msgLevel(MSG::DEBUG)){
    debug() << "Particle Momentum before Brem correction " << particle->momentum()<< endmsg;  
    debug() << "Brem correction method = " << m_method << endmsg;
  }
  
  // add brem momentum
  Gaudi::LorentzVector& mome = (Gaudi::LorentzVector&) particle->momentum();


  if( m_method == 0 ){
    mome    += bremPhoton.momentum();
  }else if(m_method == 1){
    // Add 3-momentum and re-evaluate the energy as Sqrt( p² + M²)
    double px = mome.px() + bremPhoton.momentum().px();
    double py = mome.py() + bremPhoton.momentum().py();
    double pz = mome.pz() + bremPhoton.momentum().pz();
    double mas = mome.mass();
    double e  = sqrt( mas*mas + px*px + py*py + pz*pz);
    mome.SetPxPyPzE( px,py,pz,e);
  }else if(m_method == 2){
    // keep momentum direction and particle mass
    double pe  = mome.P();
    double pb  = bremPhoton.momentum().P();
    double p   = pe+pb;
    double fac = p/pe;
    double px = mome.px()*fac;
    double py = mome.py()*fac;
    double pz = mome.pz()*fac;
    double mas = mome.mass();
    double e  = sqrt( mas*mas + p*p);
    mome.SetPxPyPzE( px,py,pz,e);
  }else
    return Error("Unknown bremStrahlung method" + Gaudi::Utils::toString(m_method), StatusCode::FAILURE);
  
  

  // update covariance matrices 
  (Gaudi::SymMatrix4x4&)particle->momCovMatrix() += bremPhoton.momCovMatrix(); 
  (Gaudi::Matrix4x3&)particle->posMomCovMatrix() += bremPhoton.momPointCovMatrix();   
  

  if (msgLevel(MSG::DEBUG)){  
    debug() << "Brem momentum : "<< bremPhoton.momentum() << endmsg;
    debug() << "Particle Momentum after Brem correction " << particle->momentum()<< endmsg;  
    debug() << "Particle mass after brem correction "<< particle->momentum().M()<<" , "<<particle->measuredMass()<<endmsg; 
  }  
  return true;
} 




                                                                                
const std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> BremAdder::getPos(const LHCb::ProtoParticle* proto,LHCb::State::Location lstate
                                                                       ,double zcalo, double def) const{
  const LHCb::Track* tr=proto->track();
  LHCb::State nstate;
  //-->ET get first available State instead of ClosestToBeam
  if (lstate == LHCb::State::ClosestToBeam)  {
    nstate = *(usedState(tr));
  }else{
    const LHCb::State* state = tr->stateAt( lstate );
    if( state == NULL && def>0){
      nstate = LHCb::State();
      StatusCode sc = m_extrapolator->propagate ( *tr,def , nstate ,  LHCb::ParticleID(11)  );
      if (msgLevel(MSG::DEBUG))debug()<<"Extrapolating to: "<<def<<endmsg;
      nstate.setLocation(lstate);
      if(sc.isFailure()){
        Warning("Extrapolator failed").ignore();   
        return std::make_pair(Gaudi::XYZPoint(0,0,-1*Gaudi::Units::km),Gaudi::SymMatrix3x3());
      }    
    }else if(state==NULL){
      Warning("State points to NULL").ignore();
      return std::make_pair(Gaudi::XYZPoint(0,0,-1*Gaudi::Units::km),Gaudi::SymMatrix3x3());
    }
    else
      nstate = *state;
  }

  if (msgLevel(MSG::DEBUG))debug() << "State " << lstate << " " << nstate <<endmsg;
  double tx=nstate.tx();
  double ty=nstate.ty();
  double x=nstate.x()+tx*(zcalo - nstate.z());
  double y=nstate.y()+ty*(zcalo - nstate.z());
  Gaudi::XYZPoint point = Gaudi::XYZPoint( x,y,zcalo);
  Gaudi::SymMatrix3x3 cov   = nstate.errPosition() + (zcalo - nstate.z())*(zcalo - nstate.z())*nstate.errSlopes();
  return std::make_pair(point,cov);
}


//===========
const std::vector<const LHCb::CaloHypo*> BremAdder::bremList(const LHCb::Particle* particle)const {

  // consistency checks
  std::vector<const LHCb::CaloHypo*> bhypos;
  if( particle==NULL)return bhypos;
  const LHCb::ProtoParticle* proto=particle->proto();
  if( proto==NULL)return bhypos;
  const SmartRefVector<LHCb::CaloHypo> hypos = proto->calo();


  // look for CaloHypo linked to PP
  const LHCb::CaloHypo* electronHypo = NULL;
  const LHCb::CaloHypo* photonHypo   = NULL;    
  for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihyp = hypos.begin();ihyp!=hypos.end();++ihyp){
    const LHCb::CaloHypo* hypo = *ihyp;
    if (NULL != hypo){
      if( hypo->hypothesis() == LHCb::CaloHypo::Photon    ) photonHypo   =  hypo;
      if( hypo->hypothesis() == LHCb::CaloHypo::EmCharged ) electronHypo =  hypo;
      if (msgLevel(MSG::DEBUG))debug() << " Hypo " << hypo->hypothesis() << " " << hypo << endmsg;
    }
  }
  

  
  if (msgLevel(MSG::DEBUG))debug() << " Input = " << m_bInput << endmsg;

  if( m_bInput != "" && exist<LHCb::Particle::Range>( m_bInput )){
    // ======= NEW BREM RECOVERY
    if (msgLevel(MSG::DEBUG))debug() << "New brem recovery is processing" <<endmsg;
    Gaudi::Plane3D plane = m_calo->plane(CaloPlane::ShowerMax);
    double Hesse = plane.HesseDistance();
    double z0 = -Hesse/plane.Normal().Z();
    if (msgLevel(MSG::DEBUG))debug() << "Z0 " << z0 << endmsg;

    //-->ET get extrapolation from firststate instead of FirstMeasurement (ClosestToBeam is replaced by firststate in getPos)
    std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> fromV= getPos( proto, LHCb::State::ClosestToBeam,z0) ;
    //    std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> fromFM= getPos( proto, LHCb::State::FirstMeasurement,z0) ;
    std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> fromT= getPos( proto, LHCb::State::AtTT,z0,(double) StateParameters::ZEndTT);

    if( fromV.first.z() == -1*Gaudi::Units::km || fromT.first.z() == -1*Gaudi::Units::km )return bhypos;
    
    Gaudi::XYZPoint pfromVelo = fromV.first;
    Gaudi::XYZPoint pfromTT = fromT.first;
    Gaudi::SymMatrix3x3 cfromVelo = fromV.second;
    Gaudi::SymMatrix3x3 cfromTT = fromT.second;
    
    
    // Starting from photon particles 
    LHCb::Particle::Range photons = get<LHCb::Particle::Range>( m_bInput );    
    int nb=0;
    for( LHCb::Particle::Range::const_iterator ip = photons.begin() ; photons.end() != ip ; ++ip ){
      const LHCb::Particle* ph =  *ip;
      if ( ph->proto() == NULL )continue;      
      //-->ET add cut on PT for brem
      if ( ph->pt() < m_ptg) continue;

      if( NULL != electronHypo &&  
          proto->info(LHCb::ProtoParticle::CaloChargedID , 0) == ph->proto()->info( LHCb::ProtoParticle::CaloNeutralID, 0)){
        if (msgLevel(MSG::DEBUG))debug() << " Electron and Brem candidates are the same calo - nothing to add "<< endmsg;
        continue;      
      }
      
      
      const LHCb::ProtoParticle* phproto = ph->proto();
      //-->ET add cut on photon ID for brem
      double photID=phproto->info( LHCb::ProtoParticle::PhotonID,-999.);
      if (photID < m_bremid ) continue;


      const SmartRefVector<LHCb::CaloHypo>  calovect=phproto->calo();
      if ( calovect.empty())continue;
      if ( 1 != calovect.size() )Warning("Several CaloHypo linked to single photon").ignore();
      SmartRefVector<LHCb::CaloHypo>::const_iterator icalo=calovect.begin();
      const LHCb::CaloHypo* caloph = (*icalo);
      const LHCb::CaloHypo::Position* gpos = caloph->position();
      if ( gpos ==NULL)continue;
      
      double ypos=gpos->y();
      double xpos=gpos->x();


      const LHCb::CaloPosition::Spread poscov=gpos->spread();
      const LHCb::CaloPosition::Covariance cov=gpos->covariance();

      double sprx=poscov(LHCb::CaloPosition::X,LHCb::CaloPosition::X);
      double errV=sqrt( sprx + cfromVelo(0,0));
      double errT=sqrt( sprx + cfromTT(0,0));

      // select X-FOI
      double dx1= xpos-std::min(pfromVelo.X()-m_stol*errV,pfromTT.X()-m_stol*errT);
      double dx2=std::max(pfromVelo.X()+m_stol*errV,pfromTT.X()+m_stol*errT)-xpos;
      if( dx1 < 0 || dx2<0)continue;

      // select Y-FOI
      double sprye=cfromTT(1,1);
      double spry=poscov(LHCb::CaloPosition::Y,LHCb::CaloPosition::Y);
      double ye=pfromTT.y();
      if( fabs(ypos-ye)/sqrt(spry+sprye)>m_stol  )continue;

      if (msgLevel(MSG::DEBUG)){
        debug()  << " xpos(g)= " << xpos
                 << " VeloX= " << pfromVelo.X() <<" +- " << errV
                 << " TTX= " << pfromTT.X() <<" +- " << errT
                 << " ypos " << ypos
                 << " ye  " << ye
                 << " spreadY " << sqrt(spry+sprye)
                 << " VeloY= " << pfromVelo.Y() 
                 << " TTY= "   << pfromTT.Y() 
                 <<" ID "<<photID	
                 <<endmsg;
      }
      nb++;
      bhypos.push_back( caloph );
    }
    counter("added NewBrem ("+Gaudi::Utils::toString(m_method)+")")+=nb;    
  }else{    
    // ======= STANDARD (OLD) BREM RECOVERY : use brem candidate from BremMatch
    if( NULL == photonHypo){
      if (msgLevel(MSG::DEBUG))debug() << " No photon associated to the particle - nothing to add "<< hypos.size() <<endmsg;
      return bhypos;
    }
    
    // Reject if the brem candidate is also the electron cluster candidate    
    const LHCb::CaloPosition* photonPos   = photonHypo->position();
    if(NULL != photonPos ){
      const Gaudi::XYZPoint photonPoint( photonPos->x(),photonPos->y(),photonPos->z() );
      const LHCb::CaloCellID photonCell   = m_calo->Cell( photonPoint );
      if( NULL != electronHypo &&  proto->info(LHCb::ProtoParticle::CaloChargedID , 0) == photonCell.all() ){
        debug() << " Electron and Brem candidates are the same calo - nothing to add "<< endmsg;
        return bhypos;      
      }
    }    

    // Add here the criteria for the identified  brem candidate to be actually added
    if( proto->info(LHCb::ProtoParticle::InAccBrem,0.) == 0.)return bhypos; // no Brem in Acceptance
    if( m_dllBrem  > proto->info(LHCb::ProtoParticle::BremPIDe,-9999. ) )return bhypos;
    if( m_chi2Brem < proto->info(LHCb::ProtoParticle::CaloBremMatch,9999. ) )return bhypos;
    
    const LHCb::CaloHypo::Position* gpos = photonHypo->position();
    if ( gpos ==NULL) return bhypos;
    double xp=gpos->x();
    double yp=gpos->y();
    //-->ET compute PT of photon
    LHCb::CaloMomentum brem = LHCb::CaloMomentum( photonHypo );
    double pt = brem.pt();
    if (msgLevel(MSG::DEBUG)){
      debug() <<"Std brem:  xpos= "<<xp
              <<" ypos= "<<yp
              <<" eph = "<<gpos->e()
              <<" pt = "<<pt
              <<endmsg;
    }    
    //-->ET add PT cut for brem
    if (pt <m_ptg) return bhypos;
    bhypos.push_back( photonHypo );  
    counter("added StdBrem ("+Gaudi::Utils::toString(m_method)+")")+=1;
  } 
  return bhypos;
} 

// ============================================================================
/// Select the appropriate state (as in ChargedParticleMakerBase)
// ============================================================================
const LHCb::State* BremAdder::usedState( const LHCb::Track* track) const{
  if (!track) Exception("NULL track");
  const LHCb::State* uState = 0 ;
  if (msgLevel(MSG::VERBOSE))
  { verbose() << "ChargedParticleMakerBase::usedState :: Looking for State" << endmsg ; }
  // default: closest to the beam:
  if ( ! uState ) { uState = track->stateAt( LHCb::State::ClosestToBeam    ) ; }
  // if not availabel: first measurementr
  if ( ! uState ) { uState = track->stateAt( LHCb::State::FirstMeasurement ) ; }
  // backup
  if ( ! uState ){
    Warning("No state closest to beam or at first measurement for track. Using first state instead",
            10,StatusCode::SUCCESS).ignore() ;
    uState = &track->firstState() ;
  }
  if (msgLevel(MSG::VERBOSE))
  { verbose() << "Using '" << uState->location() << "' state at " << uState->position() << endmsg ; }
  return uState ;
}

