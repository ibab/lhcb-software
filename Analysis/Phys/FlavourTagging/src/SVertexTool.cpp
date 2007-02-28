// $Id: SVertexTool.cpp,v 1.5 2007-02-28 08:27:58 sposs Exp $
#include "SVertexTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SVertexTool v1.3
//
// Secondary inclusive vertexing based on a likelihood function
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( SVertexTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexTool::SVertexTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ISecondaryVertexTool>(this);

  declareProperty( "IPFitPol0", m_ipfitpol0= -0.0386294 );
  declareProperty( "IPFitPol1", m_ipfitpol1= 0.198011 );
  declareProperty( "IPFitPol2", m_ipfitpol2= -0.0214092 );
  declareProperty( "IPFitPol3", m_ipfitpol3= 0.00103611 );
  declareProperty( "IPFitPol4", m_ipfitpol4= -2.34523e-05 );
  declareProperty( "IPFitPol5", m_ipfitpol5= 1.99983e-07 );

  declareProperty( "PtFitPol0", m_ptfitpol0= 0.0785018 );
  declareProperty( "PtFitPol1", m_ptfitpol1= 1.76885 );
  declareProperty( "PtFitPol2", m_ptfitpol2= -1.50309 );
  declareProperty( "PtFitPol3", m_ptfitpol3= 0.558458 );
  declareProperty( "PtFitPol4", m_ptfitpol4= -0.0749303 );

  declareProperty( "AnglePol0", m_anglepol0= 0.743526 );
  declareProperty( "AnglePol1", m_anglepol1= -2.09646 );
  declareProperty( "AnglePol2", m_anglepol2= 8.60807 );
  
}

StatusCode SVertexTool::initialize() {

  geom = tool<IGeomDispCalculator>("GeomDispCalculator");
  if ( !geom ) {   
    err() << "Unable to Retrieve GeomDispCalculator" << endreq;
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
StatusCode SVertexTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexTool::~SVertexTool(){}

//=============================================================================
std::vector<Vertex> SVertexTool::buildVertex( const RecVertex& RecVert, 
                                              const Particle::ConstVector& vtags ){

  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex Vfit(0);
  Vertex vtx;
  std::vector<Vertex> vtxvect(0);
  const Particle* p1=0, *p2=0;
  Particle::ConstVector::const_iterator jp, kp;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    //FIRST seed particle -----------------------------------
    //if( (*jp)->particleID().abspid()==13 
    //&& (*jp)->pt()/GeV >1.2 && (*jp)->p()/GeV >5.0 ) continue;//exclude mu
    sc = geom->calcImpactPar(**jp, RecVert, ipl, iperrl);
    if( sc.isFailure() ) continue;
    if( iperrl > 1.0 ) continue;                                //preselection
    if( ipl/iperrl < 2.0 ) continue;                            //preselection
    if( ipl/iperrl > 100.0 ) continue;                          //preselection

    double lcs=1000.;
    const ProtoParticle* proto = (*jp)->proto();
    if (!proto) continue ;
    const Track* track = proto->track();
    lcs = track->chi2PerDoF();
    //if( lcs > 2.0 ) continue; //xxx                           //preselection
    //must be long  
    if( track->type() != Track::Long ) continue;                //preselection

    //SECOND seed particle -----------------------------------
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {

      //if( (*kp)->particleID().abspid()==13 
      //&& (*kp)->pt()/GeV >1.2 && (*kp)->p()/GeV >5.0 ) continue;

      sc = geom->calcImpactPar(**kp, RecVert, ips, iperrs);
      if( sc.isFailure() ) continue;  
      if( iperrs > 1.0 ) continue;                              //preselection 
      if( ips/iperrs < 2.0 ) continue;                          //preselection 
      if( ips/iperrs > 100.0 ) continue;                        //preselection 

      double lcs=1000.;
      const ProtoParticle* proto = (*kp)->proto();
      if( !proto ) continue;    
      const Track* track = proto->track();
      lcs = track->chi2PerDoF();
      //if( lcs > 2.0 ) continue;   //xxx                        //preselection
      //second particle must also be long
      if( track->type() != Track::Long ) continue;             //preselection

      //cut on the z position of the seed
      sc = fitter->fit( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //preselection
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //preselection

      //if the couple is compatible with a Ks, drop it         //preselection
      Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.M()/GeV > 0.490 && sum.M()/GeV < 0.505 
          &&  (*jp)->particleID().abspid() == 211
          &&  (*kp)->particleID().abspid() == 211
          && ((*jp)->charge())
          * ((*kp)->charge()) < 0 ) {
        debug() << "This is a Ks candidate! skip."<<endreq;
        //set their energy to 0 so that they're not used afterwards
        Gaudi::LorentzVector zero(0.0001,0.0001,0.0001,0.0001);
        //(*jp)->setMomentum(zero);
        //(*kp)->setMomentum(zero);//xxx
        continue;
      }

      if( sum.M()/GeV > 1.05 && sum.M()/GeV < 1.2
          && (*jp)->particleID().abspid() == 211
          && (*kp)->particleID().abspid() == 2212
          && ((*jp)->charge())
          * ((*kp)->charge()) < 0 ){
        debug() << "This is a Lambda0 candidate! skip."<<endreq;
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
      proba= angle( (*jp)->momentum(), (*kp)->momentum() );
      // total
      probs=probi1*probi2*probp1*probp2*proba;
      probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      if( probs/(probs+probb) < 0.2 ) continue;                  //preselection
      if( probs/(probs+probb) > probf ) {
        probf = probs/(probs+probb);
        Vfit = vtx;
        p1 = (*jp);
        p2 = (*kp);
      }
    }
  }

  Vertex VfitTMP;
  Particle::ConstVector Pfit(0);

  if( probf>0.32 && p1 != NULL && p2 != NULL ) {

    //add iteratively other tracks to the seed ----------------------
    Pfit.push_back(p1);
    Pfit.push_back(p2);
    Particle::ConstVector::const_iterator jpp;
    for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;
      if( (*jpp)->p()/GeV < 2.0 ) continue;
      if( (*jpp)->particleID().abspid()==13 
	  && (*jpp)->pt()/GeV >1.2 && (*jpp)->p()/GeV >5.0 ) continue;

      double ip, ipe;
      sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe);
      if( !sc ) continue;
      if( ip/ipe < 1.8 ) continue;                                        //cut
      if( ip/ipe > 100 ) continue;                                        //cut
      if( ipe > 1.5    ) continue;                                        //cut

      //likelihood for the particle to come from B ------
      double probi1=ipprob(ip/ipe);
      double probp1=ptprob((*jpp)->pt()/GeV); // pt
      // total
      double probs=probi1*probp1;
      double probb=(1-probi1)*(1-probp1);
      double likeb=probs/(probs+probb);
      if( likeb < 0.2 ) continue;                                       //cut

      Pfit.push_back(*jpp);

      sc = fitter->fit( Pfit, VfitTMP ); /////////FIT///////////
      if( !sc ) { Pfit.pop_back(); continue; }
    
      if( VfitTMP.chi2() / VfitTMP.nDoF()  > 5.0 ) 
      { Pfit.pop_back(); continue; }                                   //cut
      if((VfitTMP.position().z()/mm - RVz) < 1.0 ) 
      { Pfit.pop_back(); continue; }                                   //cut

      //look what is the part which behaves the worst
      double ipmax = -1.0;
      int ikpp = 0;
      bool worse_exist = false;
      Particle::ConstVector::iterator kpp, kpp_worse;

      for( kpp=Pfit.begin(); kpp!=Pfit.end(); kpp++, ikpp++ ){
        if( Pfit.size() < 3 ) break;

        Particle::ConstVector tmplist = Pfit;
        tmplist.erase( tmplist.begin() + ikpp );

        sc = fitter->fit( tmplist, vtx ); 
        if( !sc ) continue;

        sc = geom->calcImpactPar(**kpp, vtx, ip, ipe);
        if( !sc ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        debug()<< "Worse=" << (*kpp_worse)->particleID().pid()
               << " P=" << (*kpp_worse)->p()/GeV
               << " ipmax=" << ipmax ;
        if ( ipmax > 3.0 && Pfit.size() > 2 ) {
          Pfit.erase( kpp_worse );
          debug() << " killed." << endreq;	
        } else {
          debug() << " included." << endreq;	
        }
      }
    }
    sc = fitter->fit( Pfit, Vfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }
  debug() << "================ Fit Results: " << Pfit.size() <<endreq;
  Vfit.clearOutgoingParticles();
  for( jp=Pfit.begin(); jp!=Pfit.end(); jp++ ) 
    Vfit.addToOutgoingParticles(*jp);

  vtxvect.push_back(Vfit);
  return vtxvect;
}
//=============================================================================
double SVertexTool::angle( Gaudi::LorentzVector a, Gaudi::LorentzVector b) {
  double ang=0;
  if(a.Vect().Mag2()) if(b.Vect().Mag2()) {
    ang = acos( (a.Vect().Unit()).Dot(b.Vect().Unit()) );
    if(ang>3.1416) ang=6.2832-ang;
    if(ang<0) ang=-ang;
  } else {
    err()<<"Zero vector! Arguments: "<<a.Vect().Mag2()
	 <<" "<<b.Vect().Mag2()<<endreq;
  }
  return ang;
}
double SVertexTool::ipprob(double x) {
  if( x > 40. ) return 0.6;
  //double r = - 0.535 + 0.3351*x - 0.03102*pow(x,2) + 0.001316*pow(x,3)
  //- 0.00002598*pow(x,4) + 0.0000001919*pow(x,5);
  double r = m_ipfitpol0 + m_ipfitpol1*x + m_ipfitpol2*pow(x,2) 
    + m_ipfitpol3*pow(x,3) + m_ipfitpol4*pow(x,4) + m_ipfitpol5*pow(x,5);
  if(r<0) r=0;
  return r;
}
double SVertexTool::ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  //double r = 0.04332 + 0.9493*x - 0.5283*pow(x,2) + 0.1296*pow(x,3)
  //  - 0.01094*pow(x,4);
  double r = m_ptfitpol0 + m_ptfitpol1*x + m_ptfitpol2*pow(x,2) 
    + m_ptfitpol3*pow(x,3) + m_ptfitpol4*pow(x,4);
  if(r<0) r=0;
  return r;
}
double SVertexTool::aprob(double x) {
  if( x > 0.25 ) return 0.7;
  double r = m_anglepol0 + m_anglepol1*x + m_anglepol2*x*x;
  if(r<0) r=0;
  return r;
}
//=============================================================================
