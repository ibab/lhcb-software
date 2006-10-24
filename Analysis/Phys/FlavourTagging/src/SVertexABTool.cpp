// $Id: SVertexABTool.cpp,v 1.4 2006-10-24 10:16:44 jpalac Exp $
#include "SVertexABTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SVertexABTool 
//
// Returns a vector of two inclusive secondary vertices
//
// 2006-01-28 : Marco Musy
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( SVertexABTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexABTool::SVertexABTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ISecondaryVertexTool>(this);
  declareProperty( "SkipTrackPurge", m_SkipTrackPurge = false );
  declareProperty( "cut_chiA", m_cut_chiA = 25.0 );
  declareProperty( "cut_chiB", m_cut_chiB = 25.0 );
  declareProperty( "cut_zA", m_cut_zA     = 0.0 );
  declareProperty( "cut_zB", m_cut_zB     = 0.0 );
}

StatusCode SVertexABTool::initialize() {

  geom = tool<IGeomDispCalculator>("GeomDispCalculator");
  if ( !geom ) {   
    err() << "Unable to Retrieve GeomDispCalculator" << endreq;
    return StatusCode::FAILURE;
  }
  fitter = tool<IVertexFit>("UnconstVertexFitter");
  if ( !fitter ) {   
    err() << "Unable to Retrieve UnconstVertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode SVertexABTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexABTool::~SVertexABTool(){}

//=============================================================================
std::vector<Vertex> SVertexABTool::buildVertex( const RecVertex& RecVert, 
                                                const Particle::ConstVector& vtags ){
  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex vtx;
  std::vector<Vertex> vseeds(0),vtxvect(0);
  std::vector<double> vprobf,vprobf_sorted;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  Particle::ConstVector::const_iterator jp, kp;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    //FIRST seed particle -----------------------------------
    sc = geom->calcImpactPar(**jp, RecVert, ipl, iperrl, ipVec, errMatrix);
    if( sc.isFailure() ) continue;
    if( ipl/iperrl < 2.0 ) continue;                            //cut
    if( (*jp)->charge() == 0 ) continue;                        //cut 

    double lcs=1000.;
    const ProtoParticle* proto = (*jp)->proto();
    if (!proto) continue;
    const Track* track = proto->track();
    lcs = track->chi2PerDoF();
    if( lcs > 2.0 ) continue;                                   //cut
    //must be long forward 
    if( track->type() != Track::Long ) continue;                //cut

    //SECOND seed particle -----------------------------------
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {

      sc = geom->calcImpactPar(**kp, RecVert, ips, iperrs, ipVec, errMatrix);
      if( sc.isFailure() ) continue;  
      if( ips/iperrs < 2.0 ) continue;                          //cut 
      if( (*kp)->charge() == 0 ) continue;                      //cut 

      double lcs=1000.;
      const ProtoParticle* proto = (*kp)->proto();
      const Track* track = proto->track();
      if((track->measurements()).size() > 5)
        lcs = track->chi2()/((track->measurements()).size()-5);
      if( lcs > 2.0 ) continue;                                //cut
      //second particle must also be forward
      if( track->type() != Track::Long ) continue;                //cut

      //cut on the z position of the seed
      sc = fitter->fit( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //cut
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //cut

      //if the couple is compatible with a Ks, drop it         //cut
      Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.M()/GeV > 0.490 && sum.M()/GeV < 0.505 
          &&  (*jp)->particleID().abspid() == 211
          &&  (*kp)->particleID().abspid() == 211
          && ((*jp)->particleID().threeCharge())
          * ((*kp)->particleID().threeCharge()) < 0 ) {
        debug() << "This is a Ks candidate! skip."<<endreq;
        //set their energy to 0 so that they're not used afterwards
        Gaudi::LorentzVector zero(0.0001,0.0001,0.0001,0.0001);
        //(*jp)->setMomentum(zero);
        //(*kp)->setMomentum(zero);//xxx
        continue;
      }

      //build a likelihood that the combination comes from B ---------
      double probi1, probi2, probp1, probp2, proba, probs, probb;
      // impact parameter
      probi1=ipprob(ipl/iperrl);
      probi2=ipprob(ips/iperrs);
      // pt
      probp1=ptprob((*jp)->pt()/GeV); 
      probp2=ptprob((*kp)->pt()/GeV); 
      // angle
      //proba=aprob(((*jp)->momentum()).Angle((*kp)->momentum()));
      proba=0.2;//xxx
      // total
      probs=probi1*probi2*probp1*probp2*proba;
      probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      probf = probs/(probs+probb);
      if( probf < 0.32 ) continue;                             //cut

      vseeds.push_back(vtx);
      vprobf.push_back(probf);
    }
  }
  if(vprobf.size()<2) return vtxvect;

  //find best 2 seeds
  bool VAexist = false ;
  Vertex vfitA(0), vfitB(0);
  vprobf_sorted = vprobf; 
  std::sort(vprobf_sorted.begin(), vprobf_sorted.end()); 
  int ibestA = vprobf_sorted.size() -1;
  for(unsigned int i=0; i!=vprobf.size(); ++i) {
    int j = vprobf.size()-i-1;
    if( vprobf.at(j) == vprobf_sorted.at(ibestA) ) {
      vfitA = vseeds.at(j);
      VAexist = true;
    }
  }
  if(!VAexist) return vtxvect;

  bool VBexist = false ;
  int ibestB = ibestA-1;
  for(unsigned int i=0; i!=vprobf.size(); ++i) {
    int j = vprobf.size()-i-1;
    if( j == ibestA ) continue;

    if(ibestB>-1) if( vprobf.at(j) == vprobf_sorted.at(ibestB) ) {
      vfitB = vseeds.at(j);
      Particle::ConstVector partsB = vfitB.outgoingParticlesVector();
      if(isinVtx(vfitA, partsB.at(0) )) { --ibestB; continue; }
      if(isinVtx(vfitA, partsB.at(1) )) { --ibestB; continue; }
      
      VBexist = true;
    }
  }
  if(!VBexist) return vtxvect;
 
  
  debug()<<"check order at seed time:"<<endreq;
  for(unsigned int i=0; i!=vprobf.size(); ++i) {
    debug()<<"vertex: "<<vseeds.at(i).position().z()
	   <<" probf:" <<vprobf.at(i) <<endreq;
  }
  debug()<<"vfitA ="<<vfitA.position().z()<<endreq;
  debug()<<"vfitB ="<<vfitB.position().z()<<endreq;
  Particle::ConstVector pfitA = vfitA.outgoingParticlesVector();
  Particle::ConstVector pfitB = vfitB.outgoingParticlesVector();

  //-----
  //add iteratively other tracks to the seeds ----------------------
  double chiA, chiB, zA, zB;
  StatusCode scA, scB;
  Particle::ConstVector::const_iterator jpp;
  for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

    if( (*jpp)->p()/GeV < 2.0 ) continue;
    if( (*jpp)->charge() == 0 ) continue;                               //cut 
    double ip, ipe;
    sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe, ipVec, errMatrix);
    if( !sc ) continue;
    if( ip/ipe < 1.8 ) continue;                                        //cut
    if( ip/ipe > 100 ) continue;                                        //cut
    if( ipe    > 1.5 ) continue;                                        //cut

    //likelihood for the particle to come from B ------
    double probi1=ipprob(ip/ipe);
    double probp1=ptprob((*jpp)->pt()/GeV); 
    double probs=probi1*probp1;
    double probb=(1-probi1)*(1-probp1);
    if( probs/(probs+probb) < 0.2 ) continue;                           //cut

    if( isinVtx(vfitA, *jpp) ) continue;
    if( isinVtx(vfitB, *jpp) ) continue;

    //try to put it in A ------------------
    pfitA = vfitA.outgoingParticlesVector(); pfitA.push_back(*jpp);
    scA   = fitter->fit( pfitA, vfitA );
    chiA  = vfitA.chi2()/vfitA.nDoF(); 
    zA    = vfitA.position().z()/mm - RVz;

    //try to put it in B ------------------
    pfitB = vfitB.outgoingParticlesVector(); pfitB.push_back(*jpp);
    scB   = fitter->fit( pfitB, vfitB );
    chiB  = vfitB.chi2()/vfitB.nDoF(); 
    zB    = vfitB.position().z()/mm - RVz;

    //decide where to put it for real
    bool putinA=false, putinB=false; 
    if( scA && zA > m_cut_zA && chiA < m_cut_chiA ) putinA=true;
    if( scB && zB > m_cut_zB && chiB < m_cut_chiB ) putinB=true;
    if( putinA && putinB ) if( chiA < chiB ) putinB=false; else putinA=false; 
    //and make action..
    if( ! putinA ) { pfitA.pop_back(); fitter->fit( pfitA, vfitA ); }
    if( ! putinB ) { pfitB.pop_back(); fitter->fit( pfitB, vfitB ); }

    //debug
    debug() << "particle to test was:" <<(*jpp)->particleID().pid()
	    << "  pt=" << (*jpp)->pt()/GeV <<endreq;
    debug() << " zA=" <<zA<< " chiA="<< chiA << " put?"<<putinA<<endreq;
    debug() << " zB=" <<zB<< " chiB="<< chiB << " put?"<<putinB<<endreq;
    if( putinA || putinB ) {
      Particle::ConstVector::iterator kp;
      debug() << "vtxA: " << pfitA.size()<<endreq;
      for( kp=pfitA.begin(); kp!=pfitA.end(); ++kp){
	debug()<< "     " << (*kp)->particleID().pid()
	       << "   pt="<< (*kp)->pt()/GeV <<endreq;
      }
      debug() << "vtxB: " << pfitB.size()<<endreq;
      for( kp=pfitB.begin(); kp!=pfitB.end(); ++kp){
	debug()<< "     " << (*kp)->particleID().pid()
	       << "   pt="<< (*kp)->pt()/GeV <<endreq;
      }
    }

    //if the part was not added skip the rest
    if( ! putinA ) if( ! putinB ) continue;
    if( m_SkipTrackPurge ) continue;

    //otherwise look what is the part which behaves worse in A
    if(putinA) if( pfitA.size() > 3 ) {
      int ikpp = 0;
      double ipmax = -1.0;
      bool worse_exist = false;
      Particle::ConstVector::iterator kpp, kpp_worse;
      for( kpp=pfitA.begin(); kpp!=pfitA.end(); kpp++, ikpp++ ){
        Particle::ConstVector tmplist = pfitA;
        tmplist.erase( tmplist.begin() + ikpp );
        sc = fitter->fit( tmplist, vtx ); 
        if( !sc ) continue;
        sc = geom->calcImpactPar(**kpp, vtx, ip, ipe, ipVec, errMatrix);
        if( !sc ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        debug()<< "Worse in A=" << (*kpp_worse)->particleID().pid()
               << " pt=" << (*kpp_worse)->pt()/GeV
               << " ipmax=" << ipmax ;
        if ( ipmax > 3.5 ) {
          pfitA.erase( kpp_worse );
          debug() << " killed." << endreq;	
        } else {
          debug() << " accepted." << endreq;	
	}
      }
      sc = fitter->fit( pfitA, vfitA ); //RE-FIT//
      if( !sc ) pfitA.clear();
    }

    //same: look what is the part which behaves worse in B
    if(putinB) if(pfitB.size() > 3 ) {
      int ikpp = 0;
      double ipmax = -1.0;
      bool worse_exist = false;
      Particle::ConstVector::iterator kpp, kpp_worse;
      for( kpp=pfitB.begin(); kpp!=pfitB.end(); kpp++, ikpp++ ){
        Particle::ConstVector tmplist = pfitB;
        tmplist.erase( tmplist.begin() + ikpp );
        sc = fitter->fit( tmplist, vtx ); 
        if( !sc ) continue;
        sc = geom->calcImpactPar(**kpp, vtx, ip, ipe, ipVec, errMatrix);
        if( !sc ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        debug()<< "Worse in B=" << (*kpp_worse)->particleID().pid()
               << " P=" << (*kpp_worse)->pt()/GeV
               << " ipmax=" << ipmax ;
        if ( ipmax > 3.5 ) {
          pfitB.erase( kpp_worse );
          debug() << " killed." << endreq;	
        } else {
          debug() << " accepted." << endreq;	
	}
      }
      sc = fitter->fit( pfitB, vfitB ); //RE-FIT////////////////////
      if( !sc ) pfitB.clear();
    }
  }

  debug() << "=> Fit Results A: " << pfitA.size() <<endreq;
  debug() << "=> Fit Results B: " << pfitB.size() <<endreq;
  vfitA.clearOutgoingParticles();
  vfitB.clearOutgoingParticles();
  for( jp=pfitA.begin(); jp!=pfitA.end(); jp++ ) vfitA.addToOutgoingParticles(*jp);
  for( jp=pfitB.begin(); jp!=pfitB.end(); jp++ ) vfitB.addToOutgoingParticles(*jp);
  if( pfitA.size() ) vtxvect.push_back(vfitA);
  if( pfitB.size() ) vtxvect.push_back(vfitB);

  return vtxvect;
}
//=============================================================================
double SVertexABTool::ipprob(double x) {
  if( x > 40. ) return 0.6;
  double r = - 0.535 + 0.3351*x - 0.03102*pow(x,2) + 0.001316*pow(x,3)
    - 0.00002598*pow(x,4) + 0.0000001919*pow(x,5);
  if(r<0) r=0;
  return r;
}
double SVertexABTool::ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  double r = 0.04332 + 0.9493*x - 0.5283*pow(x,2) + 0.1296*pow(x,3)
    - 0.01094*pow(x,4);
  if(r<0) r=0;
  return r;
}
double SVertexABTool::aprob(double x) {
  if( x < 0.02 ) return 0.32;
  return 0.4516 - 1.033*x;
}
//=============================================================================
bool SVertexABTool::isinVtx( const Vertex vtx, const Particle* p) {  
  Particle::ConstVector pvec = vtx.outgoingParticlesVector();
  for( Particle::ConstVector::iterator kp=pvec.begin(); kp!=pvec.end(); ++kp ){
    if( (*kp) == p ) return true;
  }
  return false;
}

//=============================================================================



