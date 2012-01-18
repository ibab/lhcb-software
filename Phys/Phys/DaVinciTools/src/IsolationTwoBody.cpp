// $Id: IsolationTwoBody.cpp,v 1.1 2010/03/10 13:21:43 giampi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/INTupleSvc.h"
//#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "LoKi/LoKi.h"  
#include "LoKi/PhysTypes.h"  
#include "LoKi/Functor.h"  
#include "LoKi/CoreCuts.h"  
#include "LoKi/Combiner.h"
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include <iostream>
#include <fstream>
#include <iomanip>

// local
#include "IsolationTwoBody.h"
#include "Kernel/IParticleCombiner.h"

using namespace LHCb ;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : IsolationTwoBody
//
// 2010-02-04 : Giampiero Mancinelli
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(IsolationTwoBody);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IsolationTwoBody::IsolationTwoBody( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTupleTool ( type, name , parent ) 
  , m_Geom(0)
  , m_angle(0)
  , m_fc(0)
  , m_doca(0)
  , m_ips(0)
  , m_svdis(0)
  , m_svdis_h(0)
  , m_pvdis(0)
  , m_pvdis_h(0)
  , m_ghost(0)
  , m_clone(0)
  , m_trchi2(0)
  , m_tracktype(0)
  , m_hltgood(0)
  , m_PVInputLocation(LHCb::RecVertexLocation::Primary)
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
  
{  
  declareInterface<IParticleIsolation>(this);  
  declareProperty("ParticlePath",
                  m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");
  declareProperty("angle"     , m_angle  = 0.27     ); // 
  declareProperty("fc"     , m_fc  = 0.60     ); // 
  declareProperty("doca"     , m_doca  = 0.13     ); // 
  declareProperty("ips"     , m_ips  = 3.0     ); // 
  declareProperty("svdis"     , m_svdis  = -0.15     ); // 
  declareProperty("svdis_h"     , m_svdis_h  = 30.     ); // 
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); // 
  declareProperty("pvdis_h"     , m_pvdis_h  = 40.    ); // 
  declareProperty("ghost"     , m_ghost  = 1000000.    ); // 
  declareProperty("clone"     , m_clone  = -1000000.    ); // 
  declareProperty("trchi2"     , m_trchi2  = 1000000.    ); // 
  declareProperty("tracktype"     , m_tracktype  = 3     ); //
  declareProperty("hltgood"     , m_hltgood  = -10     ); //
  declareProperty("PVInputLocation",m_PVInputLocation);
  declareProperty("Tuple", m_tuple = false); 
  declareProperty("isMC", m_isMC = false); 
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType);
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode IsolationTwoBody::initialize() 
{
  StatusCode sc = GaudiTupleTool::initialize();
  if (!sc) return sc;
  // LoKi::VertexFitter/VertexFitter  
  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, 
                                             this);
  m_combiner = tool<IParticleCombiner>("LoKi::VertexFitter");
  
  m_Geom = tool<IDistanceCalculator> ("LoKi::DistanceCalculator", this);
  if ( ! m_Geom ) {
    fatal() << "DistanceCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  return sc;
}

//=============================================================================
// get the Isolation
//=============================================================================

StatusCode IsolationTwoBody::getIso(LHCb::Particle::ConstVector& parts, 
                                    int* iso, float* massiso, float* chi2iso,
                                    const LHCb::VertexBase* PV=0, 
                                    const LHCb::VertexBase* SV=0) const 
{  
  
  debug() <<"Now get the Isolation level"<< endreq;
  StatusCode sc = StatusCode::SUCCESS;
  
  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  if (!allparts) {
    error() << " Failed to get particles container "
            <<  m_ParticlePath << endmsg;
    return StatusCode::SUCCESS;
  }
  
  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");
  /*  
      Particle2MCLinker*    m_linkLinks(0); 
      if (m_isMC){      
      
      m_linkLinks = new Particle2MCLinker( this,
      //Particle2MCMethod::Chi2,
      Particle2MCMethod::Links,
      m_ParticlePath);
      if( !m_linkLinks ) {
      fatal() << "Unable to retrieve Link Associator tool"<<endreq;
      return StatusCode::FAILURE;
      }
      }
  */
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();
  int i = 0; 
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;
  //Loop over input particles, get their simple kinematics
  Particle::ConstVector::const_iterator ip_part;
  
  debug() <<"=========== 1"<< endreq;
  //    IParticle2MCAssociator* m_p2mcAssoc;
  Particle::ConstVector iso_parts_0;
  Particle::ConstVector iso_parts_1;
  const MCParticle* mclink1 = 0;
  const MCParticle* mclink2 = 0;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {    
    const LHCb::Particle* part = *(ip_part);
    if (m_tuple) {    
      if (m_isMC) {
        if (i==0) mclink1 = m_p2mcAssoc->relatedMCP(part);
        if (i==1) mclink2 = m_p2mcAssoc->relatedMCP(part);
      }  
    }
    if (i==0) iso_parts_0.push_back(part) ;
    if (i==1) iso_parts_1.push_back(part) ;
    const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    const LHCb::Track* track = proto->track();
    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i] = sqrt(pow(p_mu(0,i),2)+pow(p_mu(1,i),2));
    i++;
  }
  int j=0;
  
  float doca2[2];
  doca2[0]=10000.;
  doca2[1]=10000.;
  float bestdoca[2];
  bestdoca[0]=10000.;
  bestdoca[1]=10000.;
  LHCb::Particle* bestpart_0(0) ;
  LHCb::Particle* bestpart_1(0) ;
  
  debug() <<"=========== 2"<< endreq;
  //Loop over all particles  
  LHCb::Particles::const_iterator ip;
  for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {
    j++;
    const LHCb::ProtoParticle * proto =  (*ip)->proto();
    const LHCb::Track* track = proto->track();
    LHCb::Particle*  cand = (*ip);
    Gaudi::XYZPoint o(track->position());
    Gaudi::XYZVector p(track->momentum());
    bool isInList = 0;
    double pt = p.Rho();
    double ptot = p.R();
    
    // skip if other particle is in input list 
    if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {
      isInList = 1;
    }
    
    if (isInList) continue;
    
    int i2=0;
    bool fromB = false;
    bool fromD = false;
    float MCID(0);
    float MCmothID(0);
    const MCParticle* mclink = 0;
    
    debug() <<"=========== 3"<< endreq;
    if (m_tuple) {    
      if (m_isMC) {
        debug() <<"=========== 4"<< endreq;
        debug() <<"=========== 4a"<< endreq;
        mclink = m_p2mcAssoc->relatedMCP(cand);
        if(mclink) { 
          debug() <<"=========== 5"<< endreq;
          fromB = ancestor(mclink)->particleID().hasBottom();
          fromD = ancestor(mclink)->particleID().hasCharm();
          MCID = mclink->particleID().pid();
          debug() <<"=========== 6"<< endreq;
          const MCParticle*mc_mom = mclink->mother();
          if(mc_mom) {
            debug() <<"=========== 7"<< endreq;
            MCmothID = mc_mom->particleID().pid();
          }
        }
        debug() <<"=========== 4b"<< endreq;
      }
    }
    
    //Loop over input particles (2)
    for ( i2 = 0; i2 < 2; i2++) {
      
    bool isolating = false;
      if (m_tuple) {    
        if (m_isMC) {
          if (i2 == 0 && mclink && mclink1) isolating = ancestor(mclink)==ancestor(mclink1);
          if (i2 == 1 && mclink && mclink2) isolating = ancestor(mclink)==ancestor(mclink2);
        }
      }
      
      int hltgood = -1;
      double fc = 0.;
      Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
      Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));
      // find roadmap isolation (only long tracks)
      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);
      debug() <<"=========== 8"<< endreq;
      
      // find doca and angle between input and other tracks
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);          
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);
      // find impact parameters, distances from secondary and primary vertex
      doca2[i2]=doca;
      if (doca2[i2]<bestdoca[i2]) {
        if (i2==0) bestpart_0 = cand;
        if (i2==1) bestpart_1 = cand;
        bestdoca[i2] = doca2[i2];
      }
      double imp = 0.;
      double impchi2 = 0.;
      double ips,pvdis,svdis;
      ips=100000.;
      RecVertex::Container::const_iterator iv;
      RecVertex::Container* verts = NULL;
      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
        verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
      }
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
        m_Geom->distance(&(*cand),(*iv),imp,impchi2);
        if (impchi2<ips) ips = impchi2;
      }      
      ips=sqrt(ips);
      debug() <<"=========== 9"<< endreq;
      
      double deltaZvtx = (vtx.z()-PosPV.z());
      
      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
      
      bool is_iso5 = false;
      
      float clone = proto->track()->info(LHCb::Track::CloneDist,100000);
      float ghost = proto->track()->ghostProbability();
      double trchi2 = proto->track()->chi2()/proto->track()->nDoF();
      // non-isolating criteria #5
      if (angle <m_angle && fc<m_fc && doca<m_doca && ips>m_ips && 
          svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h 
          && clone>m_clone && ghost<m_ghost && track->type()==m_tracktype
          && trchi2<m_trchi2 && hltgood>m_hltgood) {
        is_iso5 = true;
        if (i2==0) iso_parts_0.push_back(cand) ;
        if (i2==1) iso_parts_1.push_back(cand) ;
        iso5[i2] += 1;
        debug()<<"isiso "<< i2 << iso5[i2]<< endreq;
      }
      float dllk =
        proto->info(LHCb::ProtoParticle::CombDLLk,-10000);
      float dlle =
        proto->info(LHCb::ProtoParticle::CombDLLe,-10000);
      float dllmu =
        proto->info(LHCb::ProtoParticle::CombDLLmu,-10000);
      
      
      debug() <<"=========== 10"<< endreq;
      if (m_tuple) {
        Tuple tuple = nTuple( "IsolationTwoBody") ;
        tuple -> column ( "pttot" , ptmu[0]+ptmu[1] ) ;
        tuple -> column ( "index" , i2 ) ;
        tuple -> column ( "pt" , pt ) ;
        tuple -> column ( "ptot" , ptot ) ;
        tuple -> column ( "trchi2" , trchi2 ) ;
        tuple -> column ( "angle" , angle ) ;
        tuple -> column ( "fc" , fc ) ;
        tuple -> column ( "doca" , doca  ) ;
        tuple -> column ( "ips" , ips ) ;
        tuple -> column ( "svdis" , svdis ) ;
        tuple -> column ( "pvdis" , pvdis ) ;
        tuple -> column ( "deltaZvtx" , deltaZvtx ) ;
        tuple -> column ( "ishltgood" , hltgood ) ;
        tuple -> column ( "dllk" , dllk ) ;
        tuple -> column ( "dlle" , dlle ) ;
        tuple -> column ( "dllmu" , dllmu ) ;
        tuple -> column ( "clone" , clone ) ;
        tuple -> column ( "ghost" , ghost ) ;
        tuple -> column ( "fromB" , fromB ) ;
        tuple -> column ( "fromD" , fromD ) ;
        tuple -> column ( "isolating" , isolating ) ;
        tuple -> column ( "MCID" , MCID ) ;
        tuple -> column ( "MCmothID" , MCmothID ) ;
        tuple -> column ( "iso5" , is_iso5 ) ;
        
        tuple -> write();
      }
      
      
    }
    
    
    
    
  }
  
  

  massiso[0]=0.;
  chi2iso[0]=-100.;
  massiso[1]=0.;
  chi2iso[1]=-100.;
  
  LHCb::Vertex   vertex0   ;
  LHCb::Particle mother0  ;
  LHCb::Vertex   vertex1   ;
  LHCb::Particle mother1  ;
  debug() <<"=========== 11"<< endreq;

  if (iso_parts_0.size()>1) {
    
    StatusCode sc0 = StatusCode::SUCCESS;
    sc0 = m_combiner->combine(iso_parts_0,mother0,vertex0 ) ;
    if(sc0==StatusCode::SUCCESS) {   
      massiso[0] = mother0.measuredMass(); 
      chi2iso[0] = mother0.endVertex()->chi2();
      chi2iso[0] = chi2iso[0]/mother0.endVertex()->nDoF();
    }
  }
  
  else {
    iso_parts_0.push_back(bestpart_0);
    StatusCode sc0a = StatusCode::SUCCESS;
    sc0a = m_combiner->combine(iso_parts_0,mother0,vertex0 ) ;
    if(sc0a==StatusCode::SUCCESS) {   
      chi2iso[0] = mother0.endVertex()->chi2();
      chi2iso[0] = chi2iso[0]/mother0.endVertex()->nDoF();
    }
  }
  
  
  if (iso_parts_1.size()>1) {
    StatusCode sc1 = StatusCode::SUCCESS;
     sc1 = m_combiner->combine(iso_parts_1,mother1,vertex1 ) ;
   if(sc1==StatusCode::SUCCESS) {   
    massiso[1] = mother1.measuredMass();
    chi2iso[1] = mother1.endVertex()->chi2();
    chi2iso[1] = chi2iso[1]/mother1.endVertex()->nDoF();
   }
  }
  
  else {
    iso_parts_1.push_back(bestpart_1);
    StatusCode sc1a = StatusCode::SUCCESS;
    sc1a = m_combiner->combine(iso_parts_1,mother1,vertex1 ) ;
    if(sc1a==StatusCode::SUCCESS) {   
    chi2iso[1] = mother1.endVertex()->chi2();
    chi2iso[1] = chi2iso[1]/mother1.endVertex()->nDoF();
    }
  }
  
  debug()<<"NTRACKS "<<j<<endreq;
  
  // this is idiotic, but oh well...

    iso[0] = iso5[0];
    iso[1] = iso5[1];
  
  return sc;
}

//=============================================================================
// 
//=============================================================================
void IsolationTwoBody::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                 Gaudi::XYZPoint o_mu,Gaudi::XYZVector 
                                 p_mu, Gaudi::XYZPoint PV, int& hltgood,
                                 double& fc) const 
{
  
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
    if ((DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.)) hltgood = 1;
  }
}


//=============================================================================
// 
//=============================================================================
void IsolationTwoBody::InCone(Gaudi::XYZPoint o1, 
                              Gaudi::XYZVector p1,Gaudi::XYZPoint o2, 
                              Gaudi::XYZVector p2,
                              Gaudi::XYZPoint& vtx, double& 
                              doca, double& angle) const
{
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
// 
//=============================================================================
double IsolationTwoBody::pointer (Gaudi::XYZVector vertex, 
                                  Gaudi::XYZVector p, Gaudi::XYZVector 
                                  p_mu) const 
{  
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double  num=ptot.R()*sin(arcosine(vertex,ptot));
  double  den=(num+pt);  
  double fc = num/den;
  return fc;
}

//=============================================================================
// 
//=============================================================================
double IsolationTwoBody::ratio( double p1, double p2) const
{  
  return fabs(p1 -p2)*(1./fabs(p1+p2)); 
}

//=============================================================================
// 
//=============================================================================

double IsolationTwoBody::arcosine(Gaudi::XYZVector p1,
                                  Gaudi::XYZVector p2) const 
{  
  
  double num=(p1.Cross(p2)).R();
  double den=p1.R()*p2.R();
  double seno = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;  
  }
  return alpha;
}

//=============================================================================
// 
//=============================================================================
void IsolationTwoBody::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                     Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                     p_mu, Gaudi::XYZPoint& close1, 
                                     Gaudi::XYZPoint& close2, 
                                     Gaudi::XYZPoint& vertex, bool& 
                                     fail) const
{
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


//=============================================================================
// 
//=============================================================================
const LHCb::MCParticle* IsolationTwoBody::ancestor(const LHCb::MCParticle*imc) const {
  if (imc->mother()) {    
    const LHCb::MCParticle*mc_mom = imc->mother();
    debug()<<"iso c "<< mc_mom << endreq;
    debug()<<"iso c "<< mc_mom->momentum() << endreq;
    debug()<<"iso c "<< mc_mom->particleID() << endreq;
    debug()<<"iso c "<< mc_mom->mother() << endreq; 
    return ancestor(mc_mom);
  }
  else {    
    return imc;
  }
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode IsolationTwoBody::finalize() {
  return GaudiTupleTool::finalize() ;
} 
//=============================================================================
// Destructor
//=============================================================================


IsolationTwoBody::~IsolationTwoBody() {
} 

//=============================================================================
