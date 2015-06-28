// $Id: SVertexOneSeedTool.cpp,v 1.4 2010-07-02 08:07:43 mgrabalo Exp $
#include "SVertexOneSeedTool.h"
#include "Event/RecVertex.h"

#include "TaggingHelpers.h"

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
DECLARE_TOOL_FACTORY( SVertexOneSeedTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  SVertexOneSeedTool::SVertexOneSeedTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ISecondaryVertexTool>(this);

  declareProperty( "lcs_Long_cut",       m_lcs_Long_cut     = 2.5 );
  declareProperty( "lcs_Upstream_cut",   m_lcs_Upstream_cut = 5.0 );
  declareProperty( "lcs_vtxaddedtracks_cut", m_lcs_vtxaddedtracks_cut = 3.0 );
  declareProperty( "MinSeedPtmin",       m_ptmin = 0.1 );
  declareProperty( "MinSeedIPSmin",      m_ipsmin = 2.5 );
  declareProperty( "MinSeedDphimin",     m_dphimin = 0. );
  declareProperty( "MinSeedProbability", m_maxprobf = 0.42 );
  declareProperty( "noclones",           m_noclones = true );
  declareProperty( "ghostProb_cut",      m_ghostProb_vtx = 0.37);
  declareProperty( "Personality",        m_personality   = "Reco14");
}

StatusCode SVertexOneSeedTool::initialize() 
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  fitter = m_util->VertexFitter();
  //tool<IVertexFit>("OfflineVertexFitter");

 // register (multiple) personalities of tag
  m_buildVertex.registerPersonality("Reco12",this, &SVertexOneSeedTool::buildVertexReco12);
  m_buildVertex.registerPersonality("Reco14",this, &SVertexOneSeedTool::buildVertexReco14);

  // select personality
  // (this throws an exception if the personality chosen by the user is not
  // known)
  try {
      m_buildVertex.setPersonality(m_personality);
  } catch (const std::exception& e) {
      error() << "Caught exception while setting TaggerMuonTool::tag "
          "personality: " << e.what() << endmsg;
      return StatusCode::FAILURE;
  };

  return sc;
}

//=============================================================================
SVertexOneSeedTool::~SVertexOneSeedTool(){}

//=============================================================================
std::vector<Vertex> 
SVertexOneSeedTool::buildVertex(const RecVertex& RecVert,
                                      const Particle::ConstVector& vtags)
{
  return m_buildVertex(RecVert, vtags);
}
//=============================================================================
std::vector<Vertex> 
SVertexOneSeedTool::buildVertexReco12(const RecVertex& RecVert,
                                      const Particle::ConstVector& vtags)
{
  
  if ( msgLevel(MSG::DEBUG) )
  {
    debug()<<"=======SVertexOneSeedTool========"<<endreq;
    debug()<<" Build 2 Seed Particles for vertexing"<<endreq;
  }

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl(0), iperrl(0), ips(0), iperrs(0);
  StatusCode sc = StatusCode::SUCCESS;
  Vertex Vfit(0), vtx(0), myseed(0);
  std::vector<Vertex> vtxvect(0);
  const Particle *p1=0, *p2=0;
  Particle::ConstVector::const_iterator jp, kp;
  double maxprobf = m_maxprobf;
  VertexBase::ExtraInfo likeinfo;
  Gaudi::XYZPoint SVpos;

  //loop to find seed -----------------------------------
  for ( jp = vtags.begin(); jp != vtags.end(); ++jp ) 
  {

    //FIRST seed particle ----
    if( !(*jp)->proto() )                                                 continue;
    const Track* jtrack = (*jp)->proto()->track();
    if( jtrack->type()!= Track::Long && jtrack->type()!= Track::Upstream) continue;
    const double lcs = jtrack->chi2PerDoF();
    if(jtrack->type()== Track::Long && lcs > m_lcs_Long_cut) continue;
    else if(jtrack->type()== Track::Upstream && lcs > m_lcs_Upstream_cut) continue;

    //no clones on seed
    if (m_noclones)
    {
      const double cloneDist = jtrack->info(LHCb::Track::CloneDist, -1.);
      if (cloneDist!=-1) continue;
    }

    m_util->calcIP(*jp, &RecVert, ipl, iperrl);
    ipl=fabs(ipl);
    if( iperrl==0 ) continue;
    if( iperrl > 1.0 ) continue;                                //cut
    if( ipl/iperrl < 2.5 ) continue;                            //cut
    if( ipl/iperrl > 100.0 ) continue;                          //cut

    //SECOND seed particle ---- (remember! harder cuts must be applied on second
    //track, otherwise a nr of combination will be lost.
    for ( kp = (jp+1) ; kp != vtags.end(); ++kp )
    {
      if( !(*kp)->proto() ) continue;
      if((*kp)->proto()->track()->type()!= Track::Long ) continue; //cut
      if((*kp)->proto()->track()->chi2PerDoF() > m_lcs_Long_cut) continue;

      //no clones on seed
      if (m_noclones)
      {
        const double cloneDist2 = (*kp)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
        if (cloneDist2!=-1) continue;
      }

      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      ips=fabs(ips);
      if( iperrs ==0 ) continue;
      if( iperrs > 1.0 ) continue;                              //cut
      if( ips/iperrs < 2.5 ) continue;                          //cut
      if( ips/iperrs > 100.0 ) continue;                        //cut
      if( std::max( ipl, ips ) > 3.0 ) continue;                     //cut

      if( std::min((*jp)->pt(), (*kp)->pt()) /GeV < 0.15) continue;  //cut
      if( std::max((*jp)->pt(), (*kp)->pt()) /GeV < 0.3 ) continue;  //cut

      const double dphi = std::fabs(TaggingHelpers::dphi((*jp)->momentum().phi(),
                                                         (*kp)->momentum().phi()));
      if(dphi<0.001) continue;                                  //cut

      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() )          continue;
      if( vtx.chi2PerDoF() > 10.0 ) continue;                   //cut

      const ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D< double > >
        SVpoint((vtx.position()-RecVert.position()).x()/mm,
                (vtx.position()-RecVert.position()).y()/mm,
                (vtx.position()-RecVert.position()).z()/mm);

      if(SVpoint.z() < 0.0 ) continue;                 //cut
      if(SVpoint.Theta()>.350 || SVpoint.Theta()<.010) continue;//cut

      const double rdist = std::sqrt( SVpoint.x()*SVpoint.x()
                                      +SVpoint.y()*SVpoint.y()
                                      +SVpoint.z()*SVpoint.z()*.074*.074 );

      if(rdist < 0.3) continue;                                 //cut
      if(rdist > 10 ) continue;                                 //cut

      //if the couple is compatible with a Ks, drop it          //cut
      const double mass = ((*jp)->momentum() + (*kp)->momentum()).M()/GeV;
      if( mass<0.2) continue;
      if( mass > 0.490 && mass < 0.505
          &&  (*jp)->particleID().abspid() == 211
          &&  (*kp)->particleID().abspid() == 211
          && ((*jp)->charge()) * ((*kp)->charge())< 0)
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "This is a Ks candidate! skip."<<endreq;
        continue;
      }
      if ( msgLevel(MSG::DEBUG) )
        debug() << "    seed found: ="<< vtx.position() <<endreq;

      //evaluate likelihood
      const double prob_chi2 = pol(vtx.chi2PerDoF(), 0.632154, -0.0421607, 0.00181837);
      const double ptmin = std::min((*jp)->pt(),(*kp)->pt()) /GeV;
      double prob_ptmin = 0;
      if (ptmin < m_ptmin) continue;
      if (ptmin<1) prob_ptmin = pol(ptmin, 0.0144075, 1.28718, -0.477544);
      else prob_ptmin = pol(ptmin, 0.536686, 0.382045, -0.0945185);
      const double ipmax = std::max(ipl, ips);
      double prob_ipmax = 0;
      if (ipmax<0.4) prob_ipmax = pol(ipmax, 0.490689, 1.21535, -1.87733);
      else prob_ipmax = pol(ipmax, 0.790337, -0.229543, 0.00438416);
      const double ipsmin= std::min(ipl/iperrl, ips/iperrs);
      double prob_ipsmin = 0;
      if (ipsmin < m_ipsmin) continue;
      if (ipsmin<6) prob_ipsmin = pol(ipsmin, -0.442382, 0.326852, -0.0258772);
      else prob_ipsmin = pol(ipsmin, 0.540565, 0.0130981, -0.000204322);
      if(dphi < m_dphimin) continue;
      const double prob_deltaphi = pol(dphi, 0.611887, 0.0165528, -0.0398336);
      const double prob_rdist = pol(rdist, 0.673403, -0.0743514, 0.00646535);
      const double pointtheta = SVpoint.Theta();
      double prob_pointtheta = 0;
      if (pointtheta <0.05) prob_pointtheta = pol(pointtheta, 0.466409, 7.29019, -94.1293);
      else prob_pointtheta = pol(pointtheta, 0.555528, 0.98924, -5.43018);

      if ( msgLevel(MSG::DEBUG) )
        debug() << prob_chi2<< " "<<  prob_ptmin<<  " "<<    prob_ipmax<< " "
                << prob_ipsmin<<  " "<< prob_deltaphi<<  " "<< prob_rdist
                << " "<<prob_pointtheta<<endmsg;

      double probf = combine(prob_chi2,   prob_ptmin,    prob_ipmax,
                             prob_ipsmin, prob_deltaphi, prob_rdist, prob_pointtheta);
      
      if ( msgLevel(MSG::DEBUG) )
        debug()<<"      seed formed - probf: "<<probf<<endreq;

      if(probf>=maxprobf) 
      {
        myseed=vtx;
        if (probf>1) probf=1;
        maxprobf=probf;
        p1=(*jp);
        p2=(*kp);
        if ( msgLevel(MSG::DEBUG) )
          debug() << "       === pt1=" << p1->pt() << "       === pt2=" << p2->pt() <<endreq;
        
        //set seed position
        SVpos.SetX((vtx.position()-RecVert.position()).x()/mm);
        SVpos.SetY((vtx.position()-RecVert.position()).y()/mm);
        SVpos.SetZ((vtx.position()-RecVert.position()).z()/mm);
        if ( msgLevel(MSG::DEBUG) )
          debug()<<"       svpos (SV-RV): "<<SVpos.x()<<", "
                 <<SVpos.y()<<", "<<SVpos.z()<<endreq;
      }

    }//kp
  }//jp
  //save likelihood
  likeinfo.insert(LHCb::Vertex::LastGlobal+1, maxprobf);
  Vfit.setExtraInfo(likeinfo);
  Vfit.setPosition(SVpos);

  //add tracks from best seed
  Vfit.addToOutgoingParticles(p1);
  Vfit.addToOutgoingParticles(p2);

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"maxprobf="<<maxprobf<<endreq;
  if(maxprobf<=m_maxprobf) return vtxvect; //return empty

  //add iteratively other tracks to the seed ----------------------
  if ( msgLevel(MSG::DEBUG) )
    debug()<<" Adding tracks"<<endreq;
  for ( Particle::ConstVector::const_iterator jpp = vtags.begin();
        jpp != vtags.end(); ++jpp )
  {
    if( (*jpp) == p1 ) continue;
    if( (*jpp) == p2 ) continue;
    //no clones on added trakcs
    if (m_noclones)
    {
      const double cloneDistadd = (*jpp)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
      if (cloneDistadd!=-1) continue;
    }
    if((*jpp)->proto()->track()->chi2PerDoF() > m_lcs_vtxaddedtracks_cut) continue;
    double ip(0), ipe(0);
    m_util->calcIP(*jpp, &RecVert, ip, ipe);
    ip=fabs(ip);
    if( ipe==0 ) continue;
    if( ip/ipe < 3.5 ) continue; //cut
    if( ip < 0.1 ) continue;     //cut
    double ipSV(0), ipErrSV(0);
    m_util->calcIP( *jpp, &myseed, ipSV, ipErrSV );
    ipSV=fabs(ipSV);
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<" ipSV: "<<ipSV<<endreq;
    if( ipSV > 0.9 ) continue;
    double docaSV(0), docaErrSV(0);
    m_util->calcDOCAmin( *jpp, p1, p2, docaSV, docaErrSV);
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<" doca: "<<docaSV<<endreq;
    if(docaSV > 0.2 ) continue;
    Vfit.addToOutgoingParticles(*jpp);
    if ( msgLevel(MSG::DEBUG) ) debug() << "  === pt=" << (*jpp)->pt() <<endreq;
  }

  vtxvect.push_back(Vfit);
  return vtxvect;
}
//=============================================================================
std::vector<Vertex> 
SVertexOneSeedTool::buildVertexReco14(const RecVertex& RecVert, 
                                      const Particle::ConstVector& vtags)
{
  
 if ( msgLevel(MSG::DEBUG) )
  {
    debug()<<"=======SVertexOneSeedTool======== "<<endreq;
    debug()<<" Build 2 Seed Particles for vertexing"<<endreq;
  }
 
  //Build Up 2 Seed Particles For Vertexing ------------------------
 double ipl(0), iperrl(0), ips(0), iperrs(0); 
 StatusCode sc = StatusCode::SUCCESS;
 Vertex Vfit(0), vtx(0), myseed(0);
 std::vector<Vertex> vtxvect(0);
 const Particle *p1=0, *p2=0;
 Particle::ConstVector::const_iterator jp, kp;
 double maxprobf = m_maxprobf;
 VertexBase::ExtraInfo likeinfo;
 Gaudi::XYZPoint SVpos;

  //loop to find seed -----------------------------------
  for ( jp = vtags.begin(); jp != vtags.end(); ++jp ) {

    //FIRST seed particle ----
    if( !(*jp)->proto() )                                                 continue;
    const Track* jtrack = (*jp)->proto()->track();

    const double lcs = jtrack->chi2PerDoF();
    if(jtrack->type()== Track::Long          && lcs > m_lcs_Long_cut)     continue;
    else if(jtrack->type()== Track::Upstream && lcs > m_lcs_Upstream_cut) continue;
    if(jtrack->ghostProbability()>m_ghostProb_vtx )                       continue;

 
    m_util->calcIP(*jp, &RecVert, ipl, iperrl);
    ipl=fabs(ipl);
    if( iperrl==0 )          continue;
    if( iperrl > 1.0 )       continue;                          //cut
    if( ipl/iperrl < 2.5 )   continue;                          //cut
    if( ipl/iperrl > 100.0 ) continue;                          //cut

    //SECOND seed particle ---- (remember! harder cuts must be applied on second
    //track, otherwise a nr of combination will be lost.
    for ( kp = (jp+1) ; kp != vtags.end(); ++kp ) {
      if( !(*kp)->proto() )   continue;

      const Track* ktrack = (*kp)->proto()->track();
      const double lcs = ktrack->chi2PerDoF();
      if(ktrack->type() == Track::Long  && lcs > m_lcs_Long_cut)            continue;      
      else if(ktrack->type()== Track::Upstream && lcs > m_lcs_Upstream_cut) continue;
      if(ktrack->type()== Track::Upstream && jtrack->type()== Track::Upstream ) continue;
      if(ktrack->ghostProbability()>m_ghostProb_vtx )                       continue;

      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      ips=fabs(ips);
      if( iperrs ==0 )         continue;                                
      if( iperrs > 1.0 )       continue;                        //cut
      if( ips/iperrs < 2.5 )   continue;                        //cut
      if( ips/iperrs > 100.0 ) continue;                        //cut
      if( std::max( ipl, ips ) > 3.0 ) continue;                //cut

      if( std::min((*jp)->pt(), (*kp)->pt()) /GeV < 0.15) continue;  //cut
      if( std::max((*jp)->pt(), (*kp)->pt()) /GeV < 0.3 ) continue;  //cut
 
      const double dphi = fabs(TaggingHelpers::dphi((*jp)->momentum().phi(),
                                                    (*kp)->momentum().phi()));
      if(dphi<0.001) continue;                                  //cut

      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() )          continue;
      if( vtx.chi2PerDoF() > 10.0 ) continue;                   //cut

      ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D< double > > 
        SVpoint((vtx.position()-RecVert.position()).x()/mm,
                (vtx.position()-RecVert.position()).y()/mm,
                (vtx.position()-RecVert.position()).z()/mm);
  
      if(SVpoint.z() < 0.0 )                           continue;//cut
      if(SVpoint.Theta()>.350 || SVpoint.Theta()<.010) continue;//cut

      double rdist = sqrt(  SVpoint.x()*SVpoint.x()
                            +SVpoint.y()*SVpoint.y()
                            +SVpoint.z()*SVpoint.z()*.074*.074 );

      if(rdist < 0.3) continue;                                 //cut
      if(rdist > 10 ) continue;                                 //cut

      double mass = ((*jp)->momentum() + (*kp)->momentum()).M()/GeV;
      if( mass<0.2)   continue;
      //if the couple is compatible with a Ks, drop it          //cut
      if ((*jp)->charge() * (*kp)->charge() < 0){        
        if ((*jp)->particleID().abspid()==211 ||  (*kp)->particleID().abspid()==211){
          double pi_mass = 139.570;
          
          Gaudi::LorentzVector jp1 = (*jp)->momentum();
          Gaudi::LorentzVector jp2 = (*kp)->momentum();
          jp1.SetE(jp1.P2()+pi_mass*pi_mass);
          jp2.SetE(jp2.P2()+pi_mass*pi_mass);
          
          mass = (jp1+jp2).M()/GeV;
          if( mass > 0.490 && mass < 0.505 ){
            if ( msgLevel(MSG::DEBUG) )
              debug()<<
                " Ks candidate (two pions or a pion + one mis-ID pion) ! "<<mass<<
                " "<<(*kp)->momentum().M()<<endreq;
            continue;
          }
        }        
      }
      if ( msgLevel(MSG::DEBUG) )
        debug() << "    seed found: ="<< vtx.position()<<" "<<*jp<<" "<<*kp<<endreq;

      //evaluate likelihood
      double prob_chi2 = pol(vtx.chi2PerDoF(), 0.632154, -0.0421607, 0.00181837);
      double ptmin = std::min((*jp)->pt(),(*kp)->pt()) /GeV;
      double prob_ptmin = 0;
      if (ptmin < m_ptmin) continue;
      if (ptmin<1) prob_ptmin = pol(ptmin, 0.0144075, 1.28718, -0.477544);
      else prob_ptmin = pol(ptmin, 0.536686, 0.382045, -0.0945185);
      double ipmax = std::max(ipl, ips);
      double prob_ipmax = 0;
      if (ipmax<0.4) prob_ipmax = pol(ipmax, 0.490689, 1.21535, -1.87733);
      else prob_ipmax = pol(ipmax, 0.790337, -0.229543, 0.00438416);
      double ipsmin= std::min(ipl/iperrl, ips/iperrs);
      double prob_ipsmin = 0;
      if (ipsmin < m_ipsmin) continue;
      if (ipsmin<6) prob_ipsmin = pol(ipsmin, -0.442382, 0.326852, -0.0258772);
      else prob_ipsmin = pol(ipsmin, 0.540565, 0.0130981, -0.000204322);
      if(dphi < m_dphimin) continue;
      double prob_deltaphi = pol(dphi, 0.611887, 0.0165528, -0.0398336);
      double prob_rdist = pol(rdist, 0.673403, -0.0743514, 0.00646535);
      double pointtheta = SVpoint.Theta();
      double prob_pointtheta = 0;
      if (pointtheta <0.05) prob_pointtheta = pol(pointtheta, 0.466409, 7.29019, -94.1293);
      else prob_pointtheta = pol(pointtheta, 0.555528, 0.98924, -5.43018);
      
      if ( msgLevel(MSG::DEBUG) )
        debug()<<prob_chi2<< " "<<  prob_ptmin<<  " "<<    prob_ipmax<< " "
               << prob_ipsmin<<  " "<< prob_deltaphi<<  " "<< prob_rdist<< " "<<prob_pointtheta<<endmsg;
      
      double probf = combine(prob_chi2,   prob_ptmin,    prob_ipmax,
                             prob_ipsmin, prob_deltaphi, prob_rdist, prob_pointtheta);
            
      if ( msgLevel(MSG::DEBUG) )
        debug()<<"      seed formed - probf: "<<probf<<endreq;
      if(probf>=maxprobf) {
        myseed=vtx;
        if (probf>1) probf=1;
        maxprobf=probf;
        p1=(*jp);
        p2=(*kp);
        if ( msgLevel(MSG::DEBUG) )
          debug() << "       === pt1=" << p1->pt() << "       === pt2=" << p2->pt() <<endreq;
        //set seed position
        SVpos.SetX((vtx.position()-RecVert.position()).x()/mm);
        SVpos.SetY((vtx.position()-RecVert.position()).y()/mm);
        SVpos.SetZ((vtx.position()-RecVert.position()).z()/mm);
        if ( msgLevel(MSG::DEBUG) )
          debug()<<"       svpos (SV-RV): "<<SVpos.x()<<", "<<SVpos.y()<<", "<<SVpos.z()<<endreq;
      }
      
    }//kp
  }//jp

  if ( msgLevel(MSG::VERBOSE) ) 
    verbose()<<"maxprobf="<<maxprobf<<endreq;

  if(maxprobf<=m_maxprobf) return vtxvect; //return empty
  //save likelihood
  likeinfo.insert(LHCb::Vertex::LastGlobal+1, maxprobf);
  Vfit.setExtraInfo(likeinfo);
  Vfit.setPosition(SVpos);
  if ( msgLevel(MSG::DEBUG) )
    debug()<<"OSVtx  svpos (SV-RV): "<<SVpos.x()<<", "<<SVpos.y()<<", "<<SVpos.z()<<endreq;

  //add tracks from best seed
  Vfit.addToOutgoingParticles(p1);
  Vfit.addToOutgoingParticles(p2);
  

  //add iteratively other tracks to the seed ----------------------
  if ( msgLevel(MSG::DEBUG) )  debug()<<" Adding tracks "<<endreq;

  for ( Particle::ConstVector::const_iterator jpp = vtags.begin(); 
        jpp != vtags.end(); ++jpp ) {
    if( (*jpp) == p1 ) continue;
    if( (*jpp) == p2 ) continue;
    if( (*jpp)->proto()->track()->ghostProbability() > m_ghostProb_vtx)    continue;
    if( (*jpp)->proto()->track()->chi2PerDoF() > m_lcs_vtxaddedtracks_cut) continue;
    double ip(0), ipe(0);
    m_util->calcIP(*jpp, &RecVert, ip, ipe);
    ip=fabs(ip);
    if( ipe==0 )       continue;
    if( ip/ipe < 3.5 ) continue; //cut
    if( ip < 0.1 )     continue; //cut
    double ipSV(0), ipErrSV(0);
    m_util->calcIP( *jpp, &myseed, ipSV, ipErrSV );
    ipSV=fabs(ipSV);
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<" ipSV: "<<ipSV<<endreq;
    if( ipSV > 0.9 )   continue;
    double docaSV, docaErrSV;
    m_util->calcDOCAmin( *jpp, p1, p2, docaSV, docaErrSV);
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<" doca: "<<docaSV<<endreq;
    if(docaSV > 0.2 )  continue;
    Vfit.addToOutgoingParticles(*jpp);
    if ( msgLevel(MSG::DEBUG) ) debug() << "  === pt=" << (*jpp)->pt() <<endreq;
  }
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<" SVertex total tracks "<<Vfit.outgoingParticlesVector().size()<<endreq;
  
  vtxvect.push_back(Vfit);
  return vtxvect;
}

//======================================================================
double SVertexOneSeedTool::pol(double x, double a0, double a1,
                               double a2, double a3, double a4)
{
  double res = a0;
  if(a1) res += a1*x;
  if(a2) res += a2*x*x;
  if(a3) res += a3*x*x*x;
  if(a4) res += a4*x*x*x*x;
  return res;
}

//======================================================================
double SVertexOneSeedTool::combine(double p1, double p2, double p3,
                                   double p4, double p5, double p6, double p7)
{
  const double probs = p1*p2*p3*p4*p5*p6*p7;
  const double probb = (1-p1)*(1-p2)*(1-p3)*(1-p4)*(1-p5)*(1-p6)*(1-p7);
  return probs / ( probs + probb );
}
