// $Id: IsolationTwoBody.cpp,v 1.1 2010-03-10 13:21:43 giampi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Event/VertexBase.h"
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include <iostream>
#include <fstream>
#include <iomanip>

// local
#include "IsolationTwoBody.h"

using namespace LHCb ;
using namespace Gaudi::Units;
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
  : GaudiTool ( type, name , parent )
    , m_Geom(0)
    , m_angle(0)
    , m_fc(0)
    , m_doca(0)
    , m_ips(0)
    , m_svdis(0)
    , m_svdis_h(0)
    , m_pvdis(0)
    , m_pvdis_h(0)
    , m_tracktype(0)
    , m_PVInputLocation(LHCb::RecVertexLocation::Primary)
{  
  declareInterface<IIsolation>(this);  
  declareProperty("ParticlePath",
                  m_ParticlePath="/Event/Phys/StdNoPIDsPions/Particles");
  declareProperty("angle"     , m_angle  = 0.27     ); // 
  declareProperty("fc"     , m_fc  = 0.60     ); // 
  declareProperty("doca"     , m_doca  = 0.13     ); // 
  declareProperty("ips"     , m_ips  = 3.0     ); // 
  declareProperty("svdis"     , m_svdis  = -0.15     ); // 
  declareProperty("svdis_h"     , m_svdis_h  = 30.     ); // 
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); // 
  declareProperty("pvdis_h"     , m_pvdis_h  = 40.    ); // 
  declareProperty("tracktype"     , m_tracktype  = 3     ); //
  declareProperty("PVInputLocation",m_PVInputLocation);
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode IsolationTwoBody::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  
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

StatusCode IsolationTwoBody::getIso(LHCb::Particle::ConstVector muons, 
                                   int* iso, int isotype=0,
                                   const LHCb::VertexBase* PV=0, 
                                   const LHCb::VertexBase* SV=0) const 
{  
  
  debug() <<"Now get the Isolation level"<< endreq;
  StatusCode sc = StatusCode::SUCCESS;
  
  LHCb::Particles* parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container "
            <<  m_ParticlePath << endmsg;
    return StatusCode::SUCCESS;
  }
  
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();
  int i = 0;
  ROOT::Math::SVector<int, 2> iso0;
  iso0[0]=0;
  iso0[1]=0; 
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;
  //Loop over muons
  Particle::ConstVector::const_iterator ip_muon;
  for ( ip_muon = muons.begin(); ip_muon != muons.end(); ip_muon++) {    
    const LHCb::ProtoParticle * proto =  (*ip_muon)->proto();
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
  
  LHCb::Particles::const_iterator ip;
  for ( ip = parts->begin(); ip != parts->end() ; ++ip) {
    j++;
    const LHCb::ProtoParticle * proto =  (*ip)->proto();
    const LHCb::Track* track = proto->track();
    const LHCb::Particle*  cand = (*ip);
    Gaudi::XYZPoint o(track->position());
    Gaudi::XYZVector p(track->momentum());
    bool isInList = 0;
    double pt = p.Rho();
    if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {
      isInList = 1;
    }
    
    if (isInList) continue;
    
    int i2=0;
    
    //Loop over muons (2)
    Particle::ConstVector::const_iterator ip_muon2;
    for ( ip_muon2 = muons.begin(); ip_muon2 != muons.end(); ip_muon2++) {
      
      bool hltgood = false;
      double fc = 0.;
      Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
      Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));
      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);
      
      if (track->type()==3) {
        iso0[i2] += hltgood;
        debug()<<"isisold "<< i2 << iso0[i2]<< endreq;
      }
      
      Gaudi::XYZPoint vtx(0.,0.,0.); 
      double doca(-1.);          
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);
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
      
      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
      debug()<<"angle "<<angle<<endreq;
      debug()<<"fc "<<fc <<endreq;
      debug()<<"doca "<<doca <<endreq;
      debug()<<"ips "<<ips <<endreq;
      debug()<<"svdis "<<svdis <<endreq;
      debug()<<"pvdis "<<pvdis <<endreq;
      debug()<<"ishtlgood  "<< hltgood<<endreq;
      
      //      if (angle <0.27 && fc<0.60 && doca<0.13 && ips>3.0 && svdis>-0.15
      //          && svdis<30. && pvdis>0.5 && pvdis<40. && track->type()==3) {
      if (angle <m_angle && fc<m_fc && doca<m_doca && ips>m_ips && 
          svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h 
          && track->type()==m_tracktype) {
        iso5[i2] += 1;
        debug()<<"isiso "<< i2 << iso5[i2]<< endreq;
      }
      
      i2++;
    }
    
    
  }
  debug()<<"NTRACKS "<<j<<endreq;
  
  
  // this is idiotic, but oh well...
  
  if (isotype==0) {
    iso[0] = iso0[0];
    iso[1] = iso0[1]; 
  }
  else if (isotype==5) {
    iso[0] = iso5[0];
    iso[1] = iso5[1];
  }
  return sc;
}

//=============================================================================
// 
//=============================================================================
StatusCode IsolationTwoBody::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                      Gaudi::XYZPoint o_mu,Gaudi::XYZVector 
                                      p_mu, Gaudi::XYZPoint PV, bool& hltgood,
                                      double& fc) const 
{
  StatusCode sc = StatusCode::SUCCESS;
  
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
    hltgood=((DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.));
  }
  return sc;
}


//=============================================================================
// 
//=============================================================================
StatusCode IsolationTwoBody::InCone(Gaudi::XYZPoint o1, 
                                   Gaudi::XYZVector p1,Gaudi::XYZPoint o2, 
                                   Gaudi::XYZVector p2,
                                   Gaudi::XYZPoint& vtx, double& 
                                   doca, double& angle) const
{ 
  StatusCode sc = StatusCode::SUCCESS;
  
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
  return sc;  
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
StatusCode IsolationTwoBody::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                          Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                          p_mu, Gaudi::XYZPoint& close1, 
                                          Gaudi::XYZPoint& close2, 
                                          Gaudi::XYZPoint& vertex, bool& 
                                          fail) const
{
  StatusCode sc = StatusCode::SUCCESS;
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
  
  return sc; 
}

//=============================================================================
// Destructor
//=============================================================================

IsolationTwoBody::~IsolationTwoBody() {} 

//=============================================================================
