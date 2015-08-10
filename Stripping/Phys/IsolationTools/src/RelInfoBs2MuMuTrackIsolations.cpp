// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/Particle.h"
// kernel

#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
#include <iostream>
#include "Kernel/IRelatedPVFinder.h"
#include "Math/Boost.h"

#include "RelInfoBs2MuMuTrackIsolations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoBs2MuMuTrackIsolations
//
// 2014-10-02 : Fatima Soomro
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoBs2MuMuTrackIsolations )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoBs2MuMuTrackIsolations::RelInfoBs2MuMuTrackIsolations( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : GaudiTool( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  declareProperty("angle"     , m_angle  = 0.27     ); // 
  declareProperty("fc"        , m_fc  = 0.60     ); // 
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); // 
  declareProperty("ips"       , m_ips  = 3.0     ); // 
  declareProperty("svdis"     , m_svdis  = -0.15     ); // 
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); // 
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); // 
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); // 
  declareProperty("clone_cut"   , m_clone_cut  = -9999. ); // 
  declareProperty("ghost_cut"   , m_ghost_cut  = 0.3 ); // 
  declareProperty("trchi2_cut"   , m_trchi2_cut  = 3.0 ); // 
  declareProperty("makeTrackCuts", m_makeTrackCuts = false);
  declareProperty("IsoTwoBody", m_IsoTwoBody = false);
  declareProperty("ParticlePath",   m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty("Variables", m_variables, "List of variables to store (store all if empty)");
  declareProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);

  m_keys.clear();

}

//=============================================================================
// Destructor
//=============================================================================
RelInfoBs2MuMuTrackIsolations::~RelInfoBs2MuMuTrackIsolations() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RelInfoBs2MuMuTrackIsolations::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;
  
  if ( msgLevel(MSG::DEBUG) ) debug()<<" ==> Initialize"<<endmsg;

  //configure keys
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKPLUSISO);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKPLUSISOTWO);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYQPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYMASSISOPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYCHI2ISOPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYISO5PLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYISO5PLUS);
  
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKID);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKTOPID);
  
  return sc;
}

//rel infor methods
LHCb::RelatedInfoMap* RelInfoBs2MuMuTrackIsolations::getInfo(void) {
  return &m_map;
}

std::string RelInfoBs2MuMuTrackIsolations::infoPath(void){ // check with Alex
  std::stringstream ss;
  // // this function is redundant (Anton)
  ss << std::string("Particle2VertexIsolationRelations");
  //ss << std::string("Particle2TrackIsolationRelations");
  return ss.str();
}

//=============================================================================
// Fill the related info with track variables
//=============================================================================
StatusCode RelInfoBs2MuMuTrackIsolations::calculateRelatedInfo(const LHCb::Particle* top, const LHCb::Particle *part){
 
  if ( msgLevel(MSG::DEBUG) )  debug() << " part is "<<part->particleID().pid()<<" while top is "<< top->particleID().pid()<< endmsg;

  m_count_mup_Giampi = -1;
  m_count_mup_iso2 = -1;
  m_massisoplus = -1;
  m_chargeplus  = -100;
  m_chi2isoplus = -1;
  m_iso5plus    = -1;
  m_trackID = 0;
  m_topID = 0;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.
  // -- Clear the vector with the particles in the specific decay Probably not needed here
  m_decayParticles.clear();
  m_decayParticles.push_back( top );
  saveDecayParticles( top );
  if ( msgLevel(MSG::DEBUG) ) debug()<<"this should be printed. I am looking at particle "<<part->particleID().pid()<<endmsg;
  
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_TracksPath);
  if ( tracks->empty() )
  {
    if ( msgLevel(MSG::WARNING) ) Warning( std::string("Could not retrieve tracks at location: ")+m_TracksPath+ " .... Skipping" );
    return StatusCode::FAILURE;
  }

  StatusCode testcode ;

  if( !part->isBasicParticle()) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "I received a composite particle!! Skipping" << endmsg;
    m_map.clear(); 
    return StatusCode::SUCCESS;
  }
					   
  if ( msgLevel(MSG::DEBUG) ) debug()<<" will call isotwobody "<<m_IsoTwoBody<<endreq;
  if(m_IsoTwoBody){
    if ( msgLevel(MSG::DEBUG) ) debug()<< "Going to call IsolationTwoBody to compute IsolationTwoBody variables "<<endmsg;
    testcode = IsolationTwoBodyVariables(part, top);
    if ( msgLevel(MSG::DEBUG) ) debug() << "Computed IsolationTwoBody variables "<< m_massisoplus<<" and "<< m_chi2isoplus<<" with statuscode "<<testcode <<endmsg;
    if(!testcode) return StatusCode::FAILURE;
    
  } // if m_IsoTwoBody 
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "Going to call TrackIsolations computation" << endmsg ;
  testcode = TrackIsolations(part, top);
  if ( msgLevel(MSG::DEBUG) ) debug() << "Computed TrackIsolations: "<< m_count_mup_Giampi<<endmsg;
  if(!testcode) return StatusCode::FAILURE;
  
  m_map.clear();
  if ( msgLevel(MSG::DEBUG) )  debug()<<" Map cleared.... "<<endreq;
  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {
    
    double value = 0;

    switch (*ikey) {
      
    case RelatedInfoNamed::BSMUMUTRACKPLUSISO         : value = m_count_mup_Giampi; break;
    case RelatedInfoNamed::BSMUMUTRACKPLUSISOTWO      : value = m_count_mup_iso2; break;
    case RelatedInfoNamed::ISOTWOBODYMASSISOPLUS  : value = m_massisoplus ; break;
    case RelatedInfoNamed::ISOTWOBODYCHI2ISOPLUS  : value = m_chi2isoplus ; break;
    case RelatedInfoNamed::ISOTWOBODYISO5PLUS     : value = m_iso5plus    ; break;
    case RelatedInfoNamed::ISOTWOBODYQPLUS        : value = m_chargeplus    ; break;

    case RelatedInfoNamed::BSMUMUTRACKID        : value = m_trackID ; break;
    case RelatedInfoNamed::BSMUMUTRACKTOPID        : value = m_topID ; break;

    }
    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;
    if ( msgLevel(MSG::DEBUG) )  debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;
    m_map.insert( std::make_pair( *ikey, value) );
  } //iter over keys

  return StatusCode(testcode);

}

//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoBs2MuMuTrackIsolations::saveDecayParticles( const LHCb::Particle *top){

  // -- Fill all the daugthers in m_decayParticles
  for ( const auto& dau : top->daughters() ) {
    // -- If the particle is stable, save it in the vector, or...
    if ( dau->isBasicParticle() ) {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      m_decayParticles.push_back( dau );
    }
    else{
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( dau );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << dau->particleID().pid() << endmsg;
      saveDecayParticles( dau );
    } //else 
  } // for loop

}

bool RelInfoBs2MuMuTrackIsolations::isTrackInDecay(const LHCb::Track* track){
  
  bool isInDecay = false;
  for ( const LHCb::Particle * part : m_decayParticles ) {
    const LHCb::ProtoParticle* proto = part->proto();
    if(proto) {
      const LHCb::Track* myTrack = proto->track();
      if(myTrack) {
	if(myTrack == track)
	  {
	    if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
	    isInDecay = true;
	  } 
      }// if myTrack
    }// if proto 
  } // for loop
  
  return isInDecay;

}


//-------------------------------------------------------------------------
// isolation variables
//-------------------------------------------------------------------------
StatusCode RelInfoBs2MuMuTrackIsolations::TrackIsolations(const LHCb::Particle *part, const LHCb::Particle *top){

  if ( msgLevel(MSG::DEBUG) ) debug() <<" Entering  TrackIsolations "<<endmsg;
  if ( msgLevel(MSG::DEBUG) )  debug() <<" Entering  TrackIsolations "<<endmsg;
  int isotype = 5;
  if ( msgLevel(MSG::DEBUG) ) debug()<<" calling iso with isotype  "<<isotype<<endreq;

  //calling Giampi's tool. setting isotype=5, the default of Bs2MuMu 2013 analysis
  //double iso5 = getIso( part, isotype, false);
  //double iso2_new = getIso( part, 66132, true);

  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ; 
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  const LHCb::VertexBase *PV = dva->bestVertex(top);
  /// Fatima, this will be the BPV of the jpsi in some cases!
  const LHCb::VertexBase *SV = top->endVertex();

  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  if (!allparts) {
    Error( " Failed to get particles container "+m_ParticlePath);
    return StatusCode::FAILURE;
  }

  StatusCode sc = StatusCode::SUCCESS;
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();

  double iso0 = 0.;
  double iso5 = 0.;
  double isonew2 = 0.;
  double o_mu[3];
  double p_mu[3];

  //Loop over input particles, get their simple kinematics
  const LHCb::Track* parttrack = (part->proto())->track();
  o_mu[0] = parttrack->position().x();
  o_mu[1] = parttrack->position().y();
  o_mu[2] = parttrack->position().z();
  p_mu[0] = parttrack->momentum().x();
  p_mu[1] = parttrack->momentum().y();
  p_mu[2] = parttrack->momentum().z();

  bool hltgood = false;
  double fc = 0.;
  Gaudi::XYZPoint omu(o_mu[0],o_mu[1],o_mu[2]);
  Gaudi::XYZVector pmu(p_mu[0],p_mu[1],p_mu[2]);
  
  //Loop over all particles
  LHCb::Particles::const_iterator ip;
  int ntracks=0;  // for counting ntracks
  for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {
    
    const LHCb::ProtoParticle * proto =  (*ip)->proto();
    const LHCb::Track* track = proto->track();
    const LHCb::Particle*  cand = (*ip);
    Gaudi::XYZPoint o(track->position());
    Gaudi::XYZVector p(track->momentum());
    
    if (track->type()!=m_tracktype)   continue;
    ntracks++;
    
    // skip if other particle is in input list
    bool isInList = isTrackInDecay(track);
    if ( msgLevel(MSG::DEBUG) ) debug() <<" check isInList = "<<isInList<< endreq;    
    if (isInList) continue;
    
    if ( msgLevel(MSG::DEBUG) ) debug() <<"particle inNOTinlist of siblings... will continue with computation"<< endreq;

    IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);
    if(hltgood) iso0 ++;

    // find doca and angle between input and other tracks
    Gaudi::XYZPoint vtx(0.,0.,0.);
    double doca(-1.);
    double angle(-1.);
    InCone(omu,pmu, o, p, vtx, doca, angle);
    
    // find impact parameters, distances from secondary and primary vertex
    double imp = 0.;
    double impchi2 = 0.;
    double ips,pvdis,svdis;
    ips=100000.;
    LHCb::RecVertex::Container::const_iterator iv;
    LHCb::RecVertex::Container* verts = NULL;
    if(exist<LHCb::RecVertex::Container>(m_PVInputLocation))
      verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
    else {
      return StatusCode::FAILURE;
      if ( msgLevel(MSG::DEBUG) )  debug() << "Could not find any vertices "<<endreq;
    }
    for ( iv = verts->begin(); iv != verts->end(); iv++) {
      dva->distanceCalculator()->distance(&(*cand),(*iv),imp,impchi2);
      if (impchi2<ips) ips = impchi2;
    }
    ips=sqrt(ips);
    
    pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
    svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
    
    // non-isolating criteria #5
    if (angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips && svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h ){
      if(m_makeTrackCuts) {
	if (track->ghostProbability() < m_ghost_cut || track->chi2PerDoF() <  m_trchi2_cut)     iso5 += 1;
      }
      else  iso5 += 1;
    }// if

    double  iso_angle=0.27;
    double  iso_fc=0.60;
    double  iso_doca=0.13;
    double  iso_ips=3.0;
    double  iso_svdis=-0.15;
    double  iso_svdis_h=330.;
    double  iso_pvdis=0.5;
    double  iso_pvdis_h=340.;
    double  iso_track_clonedist = 99999.;
    double  iso_track_ghost = 10000000;
    double  iso_track_trchi2 = 5.;
  
    double mytrack_clone = track->info(LHCb::Track::CloneDist, 100);
    double mytrack_gp =  track->ghostProbability();
    double mytrack_trchi2 = track->chi2PerDoF();
    if (angle < iso_angle 
	&& fc < iso_fc 
	&& doca < iso_doca 
	&& ips > iso_ips 
	&& svdis > iso_svdis 
	&& svdis < iso_svdis_h 
	&& pvdis > iso_pvdis 
	&& pvdis < iso_pvdis_h
	&& mytrack_clone <  iso_track_clonedist
	&& mytrack_gp < iso_track_ghost
	&& mytrack_trchi2 < iso_track_trchi2
	) {
      isonew2 += 1;
     }//  if
     
  }// matches allparts

  m_count_mup_iso2 = isonew2;
  m_count_mup_Giampi = iso5;
  m_trackID = part->particleID().pid();
  m_topID = top->particleID().pid();
  
  if ( msgLevel(MSG::DEBUG) )  debug()<<"looped over "<<ntracks<<" tracks \n Returning values "<<m_count_mup_iso2<<"   "<< m_count_mup_Giampi<<endreq;
  if ( msgLevel(MSG::DEBUG) )  debug()<<" trackID "<<m_trackID<<" topID  "<<m_topID<<endreq;

  return StatusCode::SUCCESS;

}//TrackIsolations(part)



//=============================================================================
// IsHLTGood method,used by isolation calculation
//=============================================================================

void  RelInfoBs2MuMuTrackIsolations::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                          Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                          p_mu, Gaudi::XYZPoint PV, bool& hltgood,
                                          double& fc) {

  Gaudi::XYZVector rv;
  Gaudi::XYZPoint vtx;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);

  closest_point(o,p,o_mu,p_mu,close,close_mu,vtx,fail);

  if (fail) {
    fc = -1.;
    hltgood = -1;
  }
  else {
    double pete = p.Rho();
    rv = vtx - PV;
    double DOCA_b = (close-close_mu).R();
    double ZZ = rv.z();
    fc = pointer(rv,p,p_mu);
    
    if       ((DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.)) hltgood = 1;
  }
}


//=============================================================================
// Other functions needed by isolation
//=============================================================================

double RelInfoBs2MuMuTrackIsolations::pointer (Gaudi::XYZVector vertex,
                                          Gaudi::XYZVector p, Gaudi::XYZVector
                                          p_mu)  {
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double temp = arcosine(vertex,ptot);
  double  num=ptot.R()*sin(temp);
  double  den=(num+pt);
  double fc = num/den;
  return fc;
}

//=============================================================================

double RelInfoBs2MuMuTrackIsolations::getphi(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  double dphi = vdau1->momentum().Phi() - vdau2->momentum().Phi();
  return dphi;
}

//=============================================================================
double RelInfoBs2MuMuTrackIsolations::gettheta(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){

  double dtheta = vdau1->momentum().Eta() -  vdau2->momentum().Eta();
  return dtheta;
}

//=============================================================================
double RelInfoBs2MuMuTrackIsolations::ratio( double p1, double p2){
  return fabs(p1 -p2)*(1./fabs(p1+p2));
}

//=============================================================================
double RelInfoBs2MuMuTrackIsolations::IsClose(const LHCb::Particle* p1,const LHCb::Particle* p2) {

  double deta = gettheta(p1,p2);
  double dphi = getphi(p1,p2);
  return sqrt(deta*deta+dphi*dphi);
}

//=============================================================================
void RelInfoBs2MuMuTrackIsolations::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                             Gaudi::XYZPoint o_mu,Gaudi::XYZVector p_mu,
                                             Gaudi::XYZPoint& close1,
                                             Gaudi::XYZPoint& close2,
                                             Gaudi::XYZPoint& vertex, bool& fail) {

  Gaudi::XYZVector v0(o - o_mu);
  Gaudi::XYZVector v1(p.unit());
  Gaudi::XYZVector v2(p_mu.unit());
  Gaudi::XYZPoint temp1(0.,0.,0.);
  Gaudi::XYZPoint temp2(0.,0.,0.);
  fail = false;
  double  d02 = v0.Dot(v2);
  double  d21 = v2.Dot(v1);
  double  d01 = v0.Dot(v1);
  double  d22 = v2.Dot(v2);
  double  d11 = v1.Dot(v1);
  double  denom = d11 * d22 - d21 * d21;
  if (fabs(denom) <= 0.) {
    close1 = temp1;
    close2 = temp2;
    fail = true;
  }
  else {
    double numer = d02 * d21 - d01 * d22;
    double mu1 = numer / denom;
    double mu2 = (d02 + d21 * mu1) / d22;
    close1 = o+v1*mu1;
    close2 = o_mu+v2*mu2;
  }
  vertex = (close1+(close2-close1)*0.5);
}


double RelInfoBs2MuMuTrackIsolations::arcosine(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {

  double num    = (p1.Cross(p2)).R();
  double den    = p1.R()*p2.R();
  double seno   = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha  = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;
  }
  return alpha;
}

//=============================================================================
void RelInfoBs2MuMuTrackIsolations::InCone(Gaudi::XYZPoint o1,
                                      Gaudi::XYZVector p1,Gaudi::XYZPoint o2,
                                      Gaudi::XYZVector p2,
                                      Gaudi::XYZPoint& vtx, double&
                                      doca, double& angle){
  Gaudi::XYZPoint rv;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);
  closest_point(o1,p1,o2,p2,close,close_mu,vtx, fail);
  if (fail) {
    doca =-1.;
    angle=-1.;
  }
  else {
    doca = (close-close_mu).R();
    angle = arcosine(p1,p2);
  }
}

//=============================================================================
// Compute IsolationTwoBody variables
//=============================================================================
StatusCode RelInfoBs2MuMuTrackIsolations::IsolationTwoBodyVariables(const LHCb::Particle *P,const LHCb::Particle *top){

  if ( msgLevel(MSG::DEBUG) )  debug() <<"Inside IsolationTwoBodyVariables...  "<< endreq;
  if ( !(P->isBasicParticle())) {
    if ( msgLevel(MSG::DEBUG) ) debug() <<"Inside IsolationTwoBodyVariables... P was composite. Returning "<< endreq;
    return StatusCode::SUCCESS;
  }

  double massiso = 0.;
  double chi2iso = -100.;
  
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ; 
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  const LHCb::VertexBase *PV = dva->bestVertex(top); 
  const LHCb::VertexBase *SV = top->endVertex();
  if ( msgLevel(MSG::DEBUG) )  debug()<<" got vertices "<<PV->position()<<" and "<<SV->position()<<endreq;

  LHCb::Particles*  m_allparts = get<LHCb::Particles>(m_ParticlePath);
  
  const Gaudi::XYZPoint& PosPV = PV->position();
  const Gaudi::XYZPoint& PosSV = SV->position();

  double iso5 =  0.;
  double o_mu[3];
  double p_mu[3];
  
  LHCb::Particle::ConstVector iso_parts;

  iso_parts.push_back(P) ; 
  const LHCb::ProtoParticle * proto =  P->proto();
  const LHCb::Track* track = proto->track();
  o_mu[0] = track->position().x();
  o_mu[1] = track->position().y();
  o_mu[2] = track->position().z();
  p_mu[0] = track->momentum().x();
  p_mu[1] = track->momentum().y();
  p_mu[2] = track->momentum().z();

  int ntracks=0; // counts tracks looped over 
  
  double doca2 = 10000.;
  double bestdoca = 10000;
  const LHCb::Particle* bestpart(0) ;
  
  LHCb::Particles::const_iterator ip;
  for ( ip = m_allparts->begin(); ip != m_allparts->end() ; ++ip) {
    
    if ( msgLevel(MSG::DEBUG) ) debug() <<" looping over all parts "<< endreq;
    ntracks++;
    const LHCb::ProtoParticle * proto =  (*ip)->proto();
    const LHCb::Track* track = proto->track();
    LHCb::Particle*  cand = (*ip);
    Gaudi::XYZPoint o(track->position());
    Gaudi::XYZVector p(track->momentum());
    bool isInList = isTrackInDecay(track);
    if ( msgLevel(MSG::DEBUG) ) debug() <<" check isInList = "<<isInList<< endreq;    
    if (!isInList) {
      if ( msgLevel(MSG::DEBUG) ) debug() <<"particle inNOTinlist of siblings... will continue with computation"<< endreq;
      
      bool hltgood = false;
      double fc = 0.;
      Gaudi::XYZPoint omu(o_mu[0],o_mu[1],o_mu[2]);
      Gaudi::XYZVector pmu(p_mu[0],p_mu[1],p_mu[2]);
      // find roadmap isolation (only long tracks)
      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);

      // find doca and angle between input and other tracks
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);
      // find impact parameters, distances from secondary and primary vertex
      doca2=doca;
      if (doca2<bestdoca) {
	bestpart = cand;
	bestdoca = doca2;
      }
      double imp = 0.;
      double impchi2 = 0.;
      double ips,pvdis,svdis;
      ips=100000.;
      LHCb::RecVertex::Container::const_iterator iv;
      LHCb::RecVertex::Container* verts = NULL;
      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
	verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
      }
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
	dva->distanceCalculator()->distance(&(*cand),(*iv),imp,impchi2);
	if (impchi2<ips) ips = impchi2;
      }
      ips=std::sqrt(ips);
      
      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
      
      double clone = proto->track()->info(LHCb::Track::CloneDist,100000);
      double ghost = proto->track()->ghostProbability();
      double trchi2 = proto->track()->chi2()/proto->track()->nDoF();
      // non-isolating criteria #5
      double m_ghost = 10000000;
      double m_trchi2 = 10000000;
      if (angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips && svdis>m_svdis 
	  && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h
          && clone>m_clone_cut && ghost<m_ghost && track->type()==m_tracktype
	  && trchi2<m_trchi2 && hltgood)
	{
	  iso_parts.push_back(cand) ;
          iso5 += 1;
          if ( msgLevel(MSG::DEBUG) ) debug()<<"isiso "<< iso5<< endreq;
        }

    } // (!isInList)
  }// allparts

  if ( msgLevel(MSG::DEBUG) ) debug()<< "[Loop in IsolationTwoBody done] iso5 = "<< iso5<<endreq;

  LHCb::Vertex   vertex_tmp   ;
  LHCb::Particle mother_tmp  ;

  int  iso_parts_size= iso_parts.size();
  if (iso_parts_size <=1) {
    iso_parts.push_back(bestpart);
  }
  if ( msgLevel(MSG::DEBUG) ) debug()<< "[IsolationTwoBody] size of isoparts is "<< iso_parts_size<<endreq;
  
  StatusCode sc0 = StatusCode::SUCCESS;
  sc0  = dva->particleCombiner()->combine(iso_parts,mother_tmp,vertex_tmp ) ;
  if(sc0==StatusCode::SUCCESS) {
    massiso = mother_tmp.measuredMass();
    chi2iso = mother_tmp.endVertex()->chi2();
    chi2iso = chi2iso/mother_tmp.endVertex()->nDoF();
  }
   
  if(iso_parts_size <=1) massiso = -1.;
  
  // // store values in the relevant variables 
  m_massisoplus =  massiso;
  m_chi2isoplus = chi2iso;
  m_iso5plus    = iso5;
  m_trackID = P->particleID().pid();
  m_topID = top->particleID().pid();
  
  if ( msgLevel(MSG::DEBUG) ) debug()<<"NTRACKS "<<ntracks<<". \n trackID "<<m_trackID<<" topID  "<<m_topID<<endreq;
  if (msgLevel(MSG::DEBUG) ) debug()<<"massiso " << m_massisoplus << " chi2iso " << m_chi2isoplus<<" usual iso5 " << m_iso5plus <<endmsg;

  return StatusCode::SUCCESS;

}

