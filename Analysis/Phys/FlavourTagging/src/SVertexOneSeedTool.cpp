// $Id: SVertexOneSeedTool.cpp,v 1.4 2010-07-02 08:07:43 mgrabalo Exp $
#include "SVertexOneSeedTool.h"
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SVertexOneSeedTool v1.4
//
// 2010-06-1 : Marc Grabalosa       
//
// Secondary inclusive vertexing based on a likelihood function
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( SVertexOneSeedTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexOneSeedTool::SVertexOneSeedTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) 
{ 
  declareInterface<ISecondaryVertexTool>(this);

  declareProperty( "lcs_Long_cut",     m_lcs_Long_cut     = 2.5 );
  declareProperty( "lcs_Upstream_cut", m_lcs_Upstream_cut = 5.0 );
  declareProperty( "lcs_vtxaddedtracks_cut", m_lcs_vtxaddedtracks_cut = 3.0 );
  declareProperty( "maxprobf", m_maxprobf = 0.4 );
  declareProperty( "noclones", m_noclones = true );

}

StatusCode SVertexOneSeedTool::initialize() {

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }

  fitter = tool<IVertexFit>("OfflineVertexFitter");
  if ( !fitter ) {   
    err() << "Unable to Retrieve Default VertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode SVertexOneSeedTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexOneSeedTool::~SVertexOneSeedTool(){}

//=============================================================================
std::vector<Vertex> SVertexOneSeedTool::buildVertex(const RecVertex& RecVert, 
                                                    const Particle::ConstVector& vtags){


  debug()<<"=======SVertexOneSeedTool========"<<endreq;

  debug()<<" Build 2 Seed Particles for vertexing"<<endreq;
  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs; 
  StatusCode sc;
  Vertex Vfit(0);
  Vertex vtx, myseed(0);
  std::vector<Vertex> vtxvect(0);
  const Particle *p1=0, *p2=0;
  Particle::ConstVector::const_iterator jp, kp;
  double maxprobf = m_maxprobf;
  VertexBase::ExtraInfo likeinfo;

  //loop to find seed -----------------------------------
  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    //FIRST seed particle ----
    if( !(*jp)->proto() ) continue;
    const Track* jtrack = (*jp)->proto()->track();
    if( jtrack->type()!= Track::Long && jtrack->type()!= Track::Upstream) continue;
    double lcs = jtrack->chi2PerDoF();
    if(jtrack->type()== Track::Long && lcs > m_lcs_Long_cut) continue;
    else if(jtrack->type()== Track::Upstream && lcs > m_lcs_Upstream_cut) continue;

    //no clones on seed
    if (m_noclones==true){
      double cloneDist = jtrack->info(LHCb::Track::CloneDist, -1.);
      if (cloneDist!=-1) continue;
    }
 
    m_util->calcIP(*jp, &RecVert, ipl, iperrl);
    if( iperrl==0 ) continue;
    if( iperrl > 1.0 ) continue;                                //cut
    if( ipl/iperrl < 2.5 ) continue;                            //cut
    if( ipl/iperrl > 100.0 ) continue;                          //cut

    //SECOND seed particle ---- (remember! harder cuts must be applied on second
    //track, otherwise a nr of combination will be lost.
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {
      if( !(*kp)->proto() ) continue;
      if((*kp)->proto()->track()->type()!= Track::Long ) continue; //cut
      if((*kp)->proto()->track()->chi2PerDoF() > m_lcs_Long_cut) continue;

      //no clones on seed
      if (m_noclones==true){
        double cloneDist2 = (*kp)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
        if (cloneDist2!=-1) continue;
      }

      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      if( iperrs ==0 ) continue;                                
      if( iperrs > 1.0 ) continue;                              //cut
      if( ips/iperrs < 2.5 ) continue;                          //cut
      if( ips/iperrs > 100.0 ) continue;                        //cut
      if( std::max( ipl, ips ) > 3.0 ) continue;                     //cut

      if( std::min((*jp)->pt(), (*kp)->pt()) /GeV < 0.15) continue;  //cut
      if( std::max((*jp)->pt(), (*kp)->pt()) /GeV < 0.3 ) continue;  //cut
 
      double dphi= fabs((*jp)->momentum().phi()-(*kp)->momentum().phi()); 
      if(dphi>3.1416) dphi= 2*3.1416-dphi;
      if(dphi<0.001) continue;                                  //cut

      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() )          continue;
      if( vtx.chi2PerDoF() > 10.0 ) continue;                   //cut

      ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D< double > > 
        SVpoint((vtx.position()-RecVert.position()).x()/mm,
                (vtx.position()-RecVert.position()).y()/mm,
                (vtx.position()-RecVert.position()).z()/mm);
  
      if(SVpoint.z() < 0.0 ) continue;                 //cut
      if(SVpoint.Theta()>.350 || SVpoint.Theta()<.010) continue;//cut

      double rdist = sqrt(  SVpoint.x()*SVpoint.x()
                            +SVpoint.y()*SVpoint.y()
                            +SVpoint.z()*SVpoint.z()*.074*.074 );

      if(rdist < 0.3) continue;                                 //cut
      if(rdist > 10 ) continue;                                 //cut

      //if the couple is compatible with a Ks, drop it          //cut
      double mass = ((*jp)->momentum() + (*kp)->momentum()).M()/GeV;
      if( mass > 0.490 && mass < 0.505 
          &&  (*jp)->particleID().abspid() == 211
          &&  (*kp)->particleID().abspid() == 211
          && ((*jp)->charge()) * ((*kp)->charge())< 0){
        verbose() << "This is a Ks candidate! skip."<<endreq;
        continue;
      }
      debug() << "    seed found: ="<< vtx.position() <<endreq;

      //evaluate likelihood
      //double prob_chi2  = pol(vtx.chi2PerDoF(), 0.615074, -0.081797, 0.00421188);
      double prob_chi2  = pol(vtx.chi2PerDoF(), 0.627107, -0.0561232, 0.00255272);
      //double prob_ptmin = pol(std::min((*jp)->pt(), (*kp)->pt()) /GeV, 0.0662687, 1.10754, -0.350278);
      double prob_ptmin = pol(std::min((*jp)->pt(), (*kp)->pt()) /GeV, -0.058584, 1.52415, -0.795555, 0.138849);
      //double prob_ipmax = pol(std::max(ipl, ips), 0.763837, -0.0822829, -0.0154407);
      double ipmax = std::max(ipl, ips);
      double prob_ipmax = 0;
      if (ipmax>0.3)  prob_ipmax = pol(ipmax, 1.08349, -0.405747, 0.054368);
      else prob_ipmax = pol(ipmax, 0.551005, 0.587514);
      double prob_ipsmin = 0;
      double ipsmin= std::min(ipl/iperrl, ips/iperrs);
      if (ipsmin<2.5) continue;
      //if(ipsmin<6) prob_ipsmin    = pol(ipsmin, -0.642335, 0.356381, -0.0239819);
      //else prob_ipsmin = pol(ipsmin, 0.536929, 0.0254873, -0.000439594);
      if(ipsmin>9)  prob_ipsmin = pol(ipsmin, 0.63096, 0.00377653);
      else prob_ipsmin = pol(ipsmin, -0.155061, 0.185982, -0.0114987);
      //double prob_deltaphi = pol(dphi, 0.699251, -0.19263, 0.00319839);
      double prob_deltaphi = pol(dphi, 0.588014, 0.122737, -0.179973, 0.036799);
      double prob_rdist;
      prob_rdist = pol(rdist, 0.681304, -0.120933, 0.0229825, -0.00147039);
      //if(rdist<1) prob_rdist= pol(rdist, 9.61771e-05, 0.936598, -0.433183);
      //else        prob_rdist= pol(rdist, 0.44296, 0.0956002, -0.0130237);
      
      double probf = combine(prob_chi2,   prob_ptmin,    prob_ipmax,
                             prob_ipsmin, prob_deltaphi, prob_rdist);
      
      debug()<<"      seed formed - probf: "<<probf<<endreq;
      if(probf>=maxprobf) {
        myseed=vtx;
        if (probf>1) probf=1;
        maxprobf=probf;
        p1=(*jp);
        p2=(*kp);
        debug() << "       === pt=" << p1->pt() <<endreq;
        debug() << "       === pt=" << p2->pt() <<endreq;
      }
      
    }//kp
  }//jp
  //save likelihood
  likeinfo.insert(1, maxprobf);
  Vfit.setExtraInfo(likeinfo);

  //add tracks from best seed
  Vfit.addToOutgoingParticles(p1);
  Vfit.addToOutgoingParticles(p2);
  
  verbose()<<"maxprobf="<<maxprobf<<endreq;
  if(maxprobf<=m_maxprobf) return vtxvect; //return empty

  //add iteratively other tracks to the seed ----------------------
  debug()<<" Adding tracks"<<endreq;
  Particle::ConstVector::const_iterator jpp;
  for ( jpp = vtags.begin(); jpp != vtags.end(); jpp++ ) {
    if( (*jpp) == p1 ) continue;
    if( (*jpp) == p2 ) continue;
    //no clones on added trakcs
    if (m_noclones==true){
      double cloneDistadd = (*jpp)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
      if (cloneDistadd!=-1) continue;
    }
    if((*jpp)->proto()->track()->chi2PerDoF() > m_lcs_vtxaddedtracks_cut) continue;
    double ip, ipe;
    m_util->calcIP(*jpp, &RecVert, ip, ipe);
    if( ipe==0 ) continue;
    if( ip/ipe < 3.5 ) continue; //cut
    if( ip < 0.1 ) continue;     //cut
    double ipSV, ipErrSV;
    m_util->calcIP( *jpp, &myseed, ipSV, ipErrSV );
    verbose()<<" ipSV: "<<ipSV<<endreq;
    if( ipSV > 0.7 ) continue;
    double docaSV, docaErrSV;
    m_util->calcDOCAmin( *jpp, p1, p2, docaSV, docaErrSV);
    verbose()<<" doca: "<<docaSV<<endreq;
    if(docaSV > 0.2 ) continue;
    Vfit.addToOutgoingParticles(*jpp);
    debug() << "  === pt=" << (*jpp)->pt() <<endreq;
  }

  vtxvect.push_back(Vfit);
  return vtxvect;
}

//======================================================================
double SVertexOneSeedTool::pol(double x, double a0, double a1, 
                               double a2, double a3, double a4) {
  double res = a0;
  if(a1) res += a1*x;
  if(a2) res += a2*x*x;
  if(a3) res += a3*x*x*x;
  if(a4) res += a4*x*x*x*x;
  return res;
}

//======================================================================
double SVertexOneSeedTool::combine(double p1, double p2, double p3,
                                   double p4, double p5, double p6){
  double probs = p1*p2*p3*p4*p5*p6;
  double probb = (1-p1)*(1-p2)*(1-p3)*(1-p4)*(1-p5)*(1-p6);
  return probs / ( probs + probb );
}
