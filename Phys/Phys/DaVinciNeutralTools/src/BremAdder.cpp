// $Id$
// Include files

// from LHCb
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/CaloParticle.h"
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BremAdder::BremAdder( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  ,m_dllBrem()
  ,m_calo(NULL)
  ,m_method()
  ,m_key(NULL)
  ,m_key2(NULL)
  ,m_list()
  ,m_list2()
  ,m_z0(0){
  declareInterface<IBremAdder>(this);

  // Method 1 : Adding the brem candidate found with BremMatch (reconstruction)
  declareProperty("BremDllCut"  , m_dllBrem  = -999999.); // DLL  cut
  declareProperty("BremChi2Cut" , m_chi2Brem = 300.);     // chi2 cut

  // Method 2 :  looking for brem candidate(s) from a photon container  (active if m_bInput is not empty)
  declareProperty("BremInput"   , m_bInput = "/Event/Phys/StdVeryLooseAllPhotons/Particles");
  declareProperty("PhotonCL"    , m_bremid = 0.1);        //  brem photon ID lower threshold
  declareProperty("PhotonPT"    , m_ptg =75.);
  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty ("PosTolerance" , m_stol =2.);

  // brem correction method :
  //    - 0  : add 4-momenta
  //    - 1  : add 3-momenta and re-evaluate the energy according to the mass
  //    - 2  : keep the electron direction unchanged
  //    - 3  : determine brem origin from the track slope & add 3-momenta : more precise but slower (default)
  declareProperty("BremCorrectionMethod", m_method=3);
}

//=============================================================================
// Destructor
//=============================================================================
BremAdder::~BremAdder() {}



StatusCode BremAdder::finalize(){
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  return GaudiTool::finalize();
}

//=============================================================================
StatusCode BremAdder::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;

  // get extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  m_p2s = tool<IParticle2State>("Particle2State"); // not private

  // get calo position
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  Gaudi::Plane3D plane = m_calo->plane(CaloPlane::ShowerMax);
  double Hesse = plane.HesseDistance();
  m_z0 = -Hesse/plane.Normal().Z();
  if (msgLevel(MSG::DEBUG))debug() << "Z0 " << m_z0 << endmsg;
  if (msgLevel(MSG::DEBUG))  debug() << "BremAdder : correction method = " << m_method << endmsg;

  return sc;
}

//============================================================================= INTERFACED METHODS
// ---- Add brem to particle
bool BremAdder::addBrem   ( LHCb::Particle* particle,bool force )
{
  if( NULL == particle)return false;
  const std::vector<const LHCb::CaloHypo*>& brems = bremList((const LHCb::Particle*)particle,"Add");
  return brem4particle( particle, "add" , brems, force  );
}
// ---- Remove brem from particle
bool BremAdder::removeBrem( LHCb::Particle* particle,bool force ){
  if( NULL == particle)return false;
  const std::vector<const LHCb::CaloHypo*>& brems = bremList((const LHCb::Particle*)particle,"Remove");
  return brem4particle( particle, "remove",brems,force);
}

//---- Add brem(s) to a pair of particles removing overlaps
bool BremAdder::addBrem2Pair( LHCb::Particle* p1, LHCb::Particle* p2 , bool force){
  if( NULL == p1 || NULL == p2)return false;
  const std::pair<std::vector<const LHCb::CaloHypo*>,std::vector<const LHCb::CaloHypo*> >&pair=bremLists(p1,p2,"Add to pair");
  const std::vector<const LHCb::CaloHypo*>& brems1 = pair.first;
  const std::vector<const LHCb::CaloHypo*>& brems2 = pair.second;
  //info() << "---- Pair " << p1<<" , " << p2 << endmsg;
  bool b1= brem4particle( p1, "add" , brems1,force );
  bool b2= brem4particle( p2, "add" , brems2,force );
  //info() << "---- Added -----"<<b1<<" " <<b2<<endmsg;
  return (b1 && b2);
}

//---- indicate whether the particle has a brem added or not
bool BremAdder::hasBrem(const LHCb::Particle* particle){
  return (particle->info(LHCb::Particle::HasBremAdded,0.) > 0.);
}

//---- get the associate Brem vector
const std::vector<const LHCb::CaloHypo*>  BremAdder::bremList(const LHCb::Particle* particle,std::string flag){
  const std::vector<const LHCb::CaloHypo*>& brems = getBrem(particle);
  if( flag != ""){
    unsigned int nb = brems.size();
    counter(flag + " : " + Gaudi::Utils::toString( nb ) + " #photon(s)")+=1;
    counter(flag + " : " + " photon(s)") += nb;
  }
  return brems;
}

//--- get the associate Brem vectors pair
const std::pair<std::vector<const LHCb::CaloHypo*>,
                std::vector<const LHCb::CaloHypo*> > BremAdder::bremLists(const LHCb::Particle* p1,
                                                                          const LHCb::Particle* p2, std::string flag){
  const std::vector<const LHCb::CaloHypo*>& brems1 = getBrem(p1);
  const std::vector<const LHCb::CaloHypo*>& brems2 = getBrem(p2);
  std::vector<const LHCb::CaloHypo*> bb2;
  // check CaloHypo overlap
  for(std::vector<const LHCb::CaloHypo*>::const_iterator i2=brems2.begin();brems2.end()!=i2;++i2){
    bool ok=true;
    for(std::vector<const LHCb::CaloHypo*>::const_iterator i1=brems1.begin();brems1.end()!=i1;++i1){
      if( *i1 == *i2 ){ ok=false;break;}
    }
    if( ok )bb2.push_back( *i2 );
  }
  if( flag != "" ){
    unsigned int nb = brems1.size() + bb2.size();
    counter(flag + " : " + Gaudi::Utils::toString( nb ) + " #photon(s)")+=1;
    counter(flag + " : " + " photon(s)")+=nb;
    counter(flag + " : " + " #shared  photons(s)")+= (brems2.size() - bb2.size());
  }
  return std::make_pair(brems1,bb2);
}
//---- get the brem as CaloMomentum
const LHCb::CaloMomentum BremAdder::bremMomentum(const LHCb::Particle* particle,std::string flag){
  const std::vector<const LHCb::CaloHypo*>& brems =  bremList(particle, flag);
  LHCb::CaloMomentum m =  LHCb::CaloMomentum( brems );
  m.setReferencePoint(firstOrigin(brems,particle));
  return m;
}

//--- get the brem as CaloMomentum pair
const std::pair<LHCb::CaloMomentum,LHCb::CaloMomentum> BremAdder::bremMomenta(const LHCb::Particle* p1,
                                                                              const LHCb::Particle* p2,
                                                                              std::string flag){

  const std::pair<std::vector<const LHCb::CaloHypo*>,std::vector<const LHCb::CaloHypo*> >& pair=bremLists(p1,p2,flag);
  const std::vector<const LHCb::CaloHypo*>& b1=pair.first;
  const std::vector<const LHCb::CaloHypo*>& b2=pair.second;
  LHCb::CaloMomentum m1 =  LHCb::CaloMomentum( b1 );
  LHCb::CaloMomentum m2 =  LHCb::CaloMomentum( b2 );
  m1.setReferencePoint(firstOrigin(b1,p1));
  m2.setReferencePoint(firstOrigin(b2,p2));
  return std::make_pair(m1,m2);
}



//============================================================================= PRIVATE METHODS
bool BremAdder::brem4particle( LHCb::Particle* particle,
                               const std::string& what,
                               const std::vector<const LHCb::CaloHypo*>& brems,
                               bool force )
{
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
  if( hasBrem(particle)  && "add" == what && !force){
    if (msgLevel(MSG::DEBUG))debug() << " The bremstrahlung has already been added - nothing to add"<< endmsg;
    return false;
  }
  if( !hasBrem(particle) && "remove" == what && !force ){
    if (msgLevel(MSG::DEBUG))debug() << " No bremstrahlung had been added - nothing to remove "<< endmsg;
    return false;
  }


  // revert the particle momentum  (as in Charged CombinedParticle)
  if( "remove" == what || force ){
    counter("Revert brem correction")+=1;
    const LHCb::State*  state = usedState( proto->track() );
    StatusCode sc = m_p2s->state2Particle( *state, *particle );
    particle->eraseInfo(LHCb::Particle::HasBremAdded);
    particle->addInfo(LHCb::Particle::HasBremAdded,0.); // reset flag
    if( what == "remove")return sc.isFailure() ? false : true;
  }
  if( "add" != what )return Warning("Don't know what to do with brem ! flag must be 'add' or 'remove'",StatusCode::FAILURE);

  // ----- Start bremStrahlung recovery -----
  particle->eraseInfo(LHCb::Particle::HasBremAdded);
  particle->addInfo(LHCb::Particle::HasBremAdded,0.); // reset flag
  if( brems.empty()){
    if (msgLevel(MSG::DEBUG))debug() << "Empty brem list" << endmsg;
    return true;
  }

  // Brems momentum
  LHCb::CaloMomentum bremPhoton = LHCb::CaloMomentum(brems);

  if (msgLevel(MSG::DEBUG)){
    debug() << "Particle Momentum before Brem correction " << particle->momentum()<< endmsg;
    debug() << "Brem correction method = " << m_method << endmsg;
  }


  //----- add brem momentum
  Gaudi::LorentzVector& mome = (Gaudi::LorentzVector&) particle->momentum();
  if( m_method == 0 ){
    mome    += bremPhoton.momentum();
  }else if(m_method == 1){
    // Add 3-momentum and re-evaluate the energy as Sqrt( p² + M²)
    double px = mome.px() + bremPhoton.momentum().px();
    double py = mome.py() + bremPhoton.momentum().py();
    double pz = mome.pz() + bremPhoton.momentum().pz();
    double mas = mome.mass();
    double e  = std::sqrt( mas*mas + px*px + py*py + pz*pz);
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
    double e  = std::sqrt( mas*mas + p*p);
    mome.SetPxPyPzE( px,py,pz,e);
  }else if(m_method == 3){
    // determine brem origin using slope-matching
    bremMatcher(brems,particle);
  }else
    return Error("Unknown bremStrahlung method" + Gaudi::Utils::toString(m_method), StatusCode::FAILURE);


  // add flag
  particle->eraseInfo(LHCb::Particle::HasBremAdded);
  particle->addInfo(LHCb::Particle::HasBremAdded,double(brems.size()) );

  // update covariance matrices
  (Gaudi::SymMatrix4x4&)particle->momCovMatrix() += bremPhoton.momCovMatrix();
  (Gaudi::Matrix4x3&)particle->posMomCovMatrix() += bremPhoton.momPointCovMatrix();


  if (msgLevel(MSG::DEBUG)){
    debug() << "Brem momentum : "<< bremPhoton.momentum() << endmsg;
    debug() << "Particle Momentum after Brem correction " << particle->momentum()<< endmsg;
    debug() << "Particle mass after brem correction " << particle->momentum().M()
            << " , " << particle->measuredMass() << endmsg;
  }
  return true;
}


//----
const std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3>
BremAdder::getPos(const LHCb::ProtoParticle* proto,
                  const LHCb::State::Location& lstate,
                  double zcalo, double def) const
{
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
const std::vector<const LHCb::CaloHypo*> BremAdder::getBrem(const LHCb::Particle* particle){
  //if bremList is already evaluated for the particle (cache 2 last requests to speed up diElectronMaker)
  if(m_key && m_key == particle ){
    return m_list;
  }
  if(m_key2 && m_key2 == particle){
    //switch 1<->2
    std::vector<const LHCb::CaloHypo*> tempv=m_list;
    m_key2=m_key;
    m_key=particle;
    m_list=m_list2;
    m_list2=tempv;
    return m_list;
  }

  //---- else re-evaluate the brems & cache it
  m_key2=m_key;
  m_list2=m_list;
  m_key=particle;
  m_list.clear();

  // consistency checks
  std::vector<const LHCb::CaloHypo*> bhypos;
  if( particle==NULL)return bhypos;
  const LHCb::ProtoParticle* proto=particle->proto();
  if( proto==NULL)return bhypos;
  const SmartRefVector<LHCb::CaloHypo>& hypos = proto->calo();


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

    //-->ET get extrapolation from firststate instead of FirstMeasurement (ClosestToBeam is replaced by firststate in getPos)
    std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> fromV= getPos( proto, LHCb::State::ClosestToBeam,m_z0);
    std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> fromT= getPos( proto, LHCb::State::AtTT,m_z0,(double) StateParameters::ZEndTT);
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
      //-->ET add cut on photon CL for brem
      // double photID=phproto->info( LHCb::ProtoParticle::PhotonID,-999.); // obsolete DLL based photonID 
      if (ph->confLevel() < m_bremid ) continue;


      const SmartRefVector<LHCb::CaloHypo>& calovect=phproto->calo();
      if ( calovect.empty())continue;
      if ( 1 != calovect.size() )Warning("Several CaloHypo linked to single photon").ignore();
      SmartRefVector<LHCb::CaloHypo>::const_iterator icalo=calovect.begin();
      const LHCb::CaloHypo* caloph = (*icalo);
      const LHCb::CaloHypo::Position* gpos = caloph->position();
      if ( gpos ==NULL)continue;

      double ypos=gpos->y();
      double xpos=gpos->x();

      const LHCb::CaloPosition::Spread& poscov = gpos->spread();

      double sprx=poscov(LHCb::CaloPosition::X,LHCb::CaloPosition::X);
      double errV=std::sqrt( sprx + cfromVelo(0,0));
      double errT=std::sqrt( sprx + cfromTT(0,0));

      // select X-FOI
      double dx1= xpos-std::min(pfromVelo.X()-m_stol*errV,pfromTT.X()-m_stol*errT);
      double dx2=std::max(pfromVelo.X()+m_stol*errV,pfromTT.X()+m_stol*errT)-xpos;
      if( dx1 < 0 || dx2<0)continue;

      // select Y-FOI
      double sprye=cfromTT(1,1);
      double spry=poscov(LHCb::CaloPosition::Y,LHCb::CaloPosition::Y);
      double ye=pfromTT.y();
      if( fabs(ypos-ye)/std::sqrt(spry+sprye)>m_stol  )continue;

      if (msgLevel(MSG::DEBUG)){
        debug()  << " xpos(g)= " << xpos
                 << " VeloX= " << pfromVelo.X() <<" +- " << errV
                 << " TTX= " << pfromTT.X() <<" +- " << errT
                 << " ypos " << ypos
                 << " ye  " << ye
                 << " spreadY " << std::sqrt(spry+sprye)
                 << " VeloY= " << pfromVelo.Y()
                 << " TTY= "   << pfromTT.Y()
                 <<" CL "<< ph->confLevel()
                 <<endmsg;
      }
      nb++;
      bhypos.push_back( caloph );
    }
    counter("added NewBrem (meth="+Gaudi::Utils::toString(m_method)+")")+=nb;
  }else{
    // ======= STANDARD (OLD) BREM RECOVERY : use brem candidate from BremMatch
    if( NULL == photonHypo){
      if (msgLevel(MSG::DEBUG))debug() << " No photon associated to the particle - nothing to add "
                                       << hypos.size() << endmsg;
      return bhypos;
    }

    // Reject if the brem candidate is also the electron cluster candidate
    const LHCb::CaloPosition* photonPos   = photonHypo->position();
    if(NULL != photonPos ){
      const Gaudi::XYZPoint photonPoint( photonPos->x(),photonPos->y(),photonPos->z() );
      const LHCb::CaloCellID& photonCell = m_calo->Cell( photonPoint );
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
    if (pt >m_ptg)bhypos.push_back( photonHypo );
    counter("added StdBrem (meth="+Gaudi::Utils::toString(m_method)+")")+=1;
  }
  m_list=bhypos;
  return bhypos;
}


//=== bremMatcher ==================================================================================================
void BremAdder::bremMatcher(const std::vector<const LHCb::CaloHypo*>& brems,
                            const LHCb::Particle* particle){

  const LHCb::Track* track = particle->proto()->track();
  const LHCb::State* uState=usedState(track);
  double mass=particle->momentum().mass();
  double p = track->momentum().R();
  for(std::vector<const LHCb::CaloHypo*>::const_iterator ib=brems.begin();brems.end()!=ib;++ib){
    const LHCb::CaloHypo* brem = *ib;
    const Gaudi::XYZPoint& origin = bremOrigin(brem,track);
    LHCb::State pState = LHCb::State();
    (m_extrapolator->propagate ( *track, origin.Z() , pState ,  LHCb::ParticleID(11))).ignore();
    LHCb::CaloMomentum b = LHCb::CaloMomentum( brem ,pState.position() ) ;
    p= ( p * pState.slopes()/pState.slopes().R()  + b.momentum().Vect() ).R();
  }

  // cast & update the particle momentum
  Gaudi::XYZVector newp= p * uState->slopes()/uState->slopes().R();
  Gaudi::LorentzVector& momentum = (Gaudi::LorentzVector&) particle->momentum();
  momentum.SetPx( newp.X() );
  momentum.SetPy( newp.Y() );
  momentum.SetPz( newp.Z() );
  momentum.SetE( std::sqrt(newp.R()*newp.R() + mass*mass));
  return;
}


//=== bremOrigin ==================================================================================================
const Gaudi::XYZPoint BremAdder::bremOrigin(const LHCb::CaloHypo* brem,
                                            const LHCb::Track*    track){

  Gaudi::XYZPoint mPoint = Gaudi::XYZPoint();
  // scan the trajectory
  double fromZ = ( LHCb::Track::Downstream == track->type())?  (double)StateParameters::ZEndVelo : usedState(track)->z()-100;
  double toZ   = (double) StateParameters::ZEndTT;
  double step = 50.;
  double z=fromZ;
  double deltamin=999.;
  std::vector<std::pair<double,double> > curve;
  while( z < toZ ){
    LHCb::State pState = LHCb::State();
    if( m_extrapolator->propagate ( *track, z , pState ,  LHCb::ParticleID(11)  ).isFailure()){
      counter("extrapolation failure")+=1;
      return mPoint;
    }
    double ttx=pState.tx();
    LHCb::CaloMomentum b = LHCb::CaloMomentum( brem ,pState.position() );
    double btx=b.px()/b.pz();
    double delta = fabs(btx-ttx);
    curve.push_back(std::make_pair(z,delta));
    if( delta < deltamin){
      deltamin = delta;
      mPoint=pState.position();
    }
    z+=step;
  }
  return mPoint;
}

const Gaudi::XYZPoint BremAdder::firstOrigin( const std::vector<const LHCb::CaloHypo*>& brems ,const LHCb::Particle* particle){
  const LHCb::Track* track = particle->proto()->track();
  Gaudi::XYZPoint firstPoint = Gaudi::XYZPoint(0.,0.,12000);
  for(std::vector<const LHCb::CaloHypo*>::const_iterator ib=brems.begin();brems.end()!=ib;++ib){
    const LHCb::CaloHypo* brem = *ib;
    const Gaudi::XYZPoint origin = bremOrigin(brem,track);
    if( origin.Z() < firstPoint.Z() )firstPoint=origin;
  }
  return firstPoint;
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BremAdder )
