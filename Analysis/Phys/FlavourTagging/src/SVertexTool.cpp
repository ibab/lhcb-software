// $Id: SVertexTool.cpp,v 1.15 2010-02-11 18:35:45 musy Exp $
#include "SVertexTool.h"
#include "Event/RecVertex.h"
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
  GaudiTool ( type, name, parent ) 
{ 
  declareInterface<ISecondaryVertexTool>(this);

  declareProperty( "lcs_Long_cut",     m_lcs_Long_cut     = 2.5 );
  declareProperty( "lcs_Upstream_cut", m_lcs_Upstream_cut = 5.0 );

  declareProperty( "IPFitPol0", m_ipfitpol0= -0.535);
  declareProperty( "IPFitPol1", m_ipfitpol1= 0.3351 );
  declareProperty( "IPFitPol2", m_ipfitpol2= -0.03102 );
  declareProperty( "IPFitPol3", m_ipfitpol3= 0.001316 );
  declareProperty( "IPFitPol4", m_ipfitpol4= -0.00002598 );
  declareProperty( "IPFitPol5", m_ipfitpol5= 0.0000001919);

  declareProperty( "PtFitPol0", m_ptfitpol0= 0.04332 );
  declareProperty( "PtFitPol1", m_ptfitpol1= 0.9493 );
  declareProperty( "PtFitPol2", m_ptfitpol2= -0.5283 );
  declareProperty( "PtFitPol3", m_ptfitpol3= 0.1296);
  declareProperty( "PtFitPol4", m_ptfitpol4= -0.01094 );

  declareProperty( "AnglePol0", m_anglepol0= 0.4516 );
  declareProperty( "AnglePol1", m_anglepol1= 1.033 );

}

StatusCode SVertexTool::initialize() {

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
StatusCode SVertexTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexTool::~SVertexTool(){}

//=============================================================================
std::vector<Vertex> SVertexTool::buildVertex(const RecVertex& RecVert, 
                                             const Particle::ConstVector& vtags){
  verbose()<<"=======SVertex Tool========"<<endreq;
//  for(Particle::ConstVector::const_iterator ip = vtags.begin(); 
//       ip != vtags.end(); ip++) {
//     info() <<"B vtag part= "<< (*ip)->pt()<<"  "<<(*ip)->proto()<<" id="<<
//            (*ip)->particleID().pid()<<endreq;
//   }

  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex Vfit(0);
  Vertex vtx;
  std::vector<Vertex> vtxvect(0);
  const Particle *p1=0, *p2=0;
  Particle::ConstVector::const_iterator ip, jp, kp;
  Particle::ConstVector vtags_unique, vtags_toexclude;

  //filter duplicates and apply some pre-cuts to speed up what's next
  verbose()<<"Look for seed particles"<<endreq;
  for ( ip = vtags.begin(); ip != vtags.end(); ip++ ) {
    if( !(*ip)->proto() ) continue;

    bool duplicate=false;
    for ( jp = ip+1; jp != vtags.end(); jp++ ) {
      if( (*ip)->proto() == (*jp)->proto() ) { duplicate=true; break; }
    }
    if( duplicate ) continue; 

    if( (*ip)->p()/GeV < 2.0 ) continue;
    if( (*ip)->proto()->track()->type()!= Track::Long 
        && (*ip)->proto()->track()->type()!= Track::Upstream) continue;//preselection

    double lcs = 0.;
    lcs = (*ip)->proto()->track()->chi2PerDoF();
    if((*ip)->proto()->track()->type()== Track::Long ){
      if(lcs > m_lcs_Long_cut) continue;
    }
    if((*ip)->proto()->track()->type()== Track::Upstream ){
      if(lcs > m_lcs_Upstream_cut) continue;
    }
    
    vtags_unique.push_back(*ip);
  }
  verbose() << "size of tracks for sec vtx="<<vtags_unique.size()<<endreq;

  //loop to find seed -----------------------------------
  for ( jp = vtags_unique.begin(); jp != vtags_unique.end(); jp++ ) {

    //FIRST seed particle ----
    m_util->calcIP(*jp, &RecVert, ipl, iperrl);
    if( iperrl==0 ) continue;
    if( iperrl > 1.0 ) continue;                                //preselection
    if( ipl/iperrl < 2.0 ) continue;                            //preselection
    if( ipl/iperrl > 100.0 ) continue;                          //preselection
    verbose() << "seed1: pt="<< (*jp)->pt()/GeV <<endreq;

    //SECOND seed particle ----
    for ( kp = (jp+1) ; kp != vtags_unique.end(); kp++ ) {
      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      if( iperrs ==0 ) continue;                                //preselection
      if( iperrs > 1.0 ) continue;                                //preselection
      if( ips/iperrs < 2.0 ) continue;                            //preselection
      if( ips/iperrs > 100.0 ) continue;                          //preselection
      //cut on the z position of the seed
      // replaced by V.B., 20 aug 2k+9: sc = fitter->fit( **jp, **kp, vtx );
      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //preselection
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //preselection
      double mass = 0.;
      verbose() << "    try with: pt="<< (*kp)->pt()/GeV <<endreq;
      if((*jp)->particleID().pid()!=310 && (*kp)->particleID().pid()!=310){
        //if the couple is compatible with a Ks, drop it         //preselection
        mass = ((*jp)->momentum() + (*kp)->momentum()).M()/GeV;
        if( mass > 0.490 && mass < 0.505 
            &&  (*jp)->particleID().abspid() == 211
            &&  (*kp)->particleID().abspid() == 211
            && ((*jp)->charge()) * ((*kp)->charge())< 0){
          verbose() << "This is a Ks candidate! skip."<<endreq;
          vtags_toexclude.push_back(*jp);
          vtags_toexclude.push_back(*kp);
          continue;
        }
      }
      if((*jp)->particleID().pid()!=3122 && (*kp)->particleID().pid()!=3122){
        if( mass > 1.11 && mass < 1.12 )
          if( ((*jp)->charge()) * ((*kp)->charge()) < 0 )
            if( ((*jp)->particleID().abspid()    ==  211
                 && (*kp)->particleID().abspid() == 2212) ||
                ((*jp)->particleID().abspid()    == 2212
                 && (*kp)->particleID().abspid() ==  211) ) {
              verbose() << "This is a Lambda0 candidate! skip."<<endreq;
              vtags_toexclude.push_back(*jp);
              vtags_toexclude.push_back(*kp);
              continue;
            }
      }
      verbose() << "    seed2 found: pt="<< (*kp)->pt()/GeV <<endreq;

      //build a likelihood that the combination comes from B ---------
      double probi1, probi2, probp1, probp2, proba, probs, probb;
      // impact parameter
      probi1=ipprob(ipl/iperrl);
      probi2=ipprob(ips/iperrs);
      // pt
      probp1=ptprob((*jp)->pt()/GeV); 
      probp2=ptprob((*kp)->pt()/GeV); 
      // angle
      proba= aprob(angle( (*jp)->momentum(), (*kp)->momentum() ));
      // total
      probs=probi1*probi2*probp1*probp2*proba;
      probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      if( probs/(probs+probb) < 0.2 ) continue;                  //preselection
      if( probs/(probs+probb) > probf ) {
        probf = probs/(probs+probb);
        Vfit = vtx;
        p1 = (*jp);
        p2 = (*kp);
        verbose() << "complete seed formed with probf="<<probf<<endreq;
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
    for(jpp = vtags_unique.begin(); jpp != vtags_unique.end(); jpp++){

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;

      double ip, ipe;
      m_util->calcIP(*jpp, &RecVert, ip, ipe);
      if( ipe==0 ) continue;
      if( ip/ipe < 1.8 ) continue;                                        //cut
      if( ip/ipe > 100 ) continue;                                        //cut
      if( ipe > 1.5    ) continue;                                        //cut
      if( isin(vtags_toexclude, *jpp) ) continue;

      //likelihood for the particle to come from B ------
      double probi1=ipprob(ip/ipe);
      double probp1=ptprob((*jpp)->pt()/GeV); // pt
      // total
      double probs=probi1*probp1;
      double probb=(1-probi1)*(1-probp1);
      double likeb=probs/(probs+probb);
      if( likeb < 0.2 ) continue;                                         //cut

      Pfit.push_back(*jpp);

      // replaced by V.B., 20 aug 2k+9: sc = fitter->fit( Pfit, VfitTMP ); /////////FIT///////////
      sc = fitter->fit( VfitTMP , Pfit ); /////////FIT///////////
      if( !sc ) { Pfit.pop_back(); continue; }
    
      if( VfitTMP.chi2() / VfitTMP.nDoF()  > 5.0 ) 
      { Pfit.pop_back(); continue; }                                   //cut
      if((VfitTMP.position().z()/mm - RVz) < 1.0 ) 
      { Pfit.pop_back(); continue; }                                   //cut

      //look what is the part which behaves the worst
      int ikpp = 0;
      double ipmax = -1.0;
      bool worse_exist = false;
      Particle::ConstVector::iterator kpp, kpp_worse;

      for( kpp=Pfit.begin(); kpp!=Pfit.end(); kpp++, ikpp++ ){
        if( Pfit.size() < 3 ) break;

        Particle::ConstVector tmplist = Pfit;
        tmplist.erase( tmplist.begin() + ikpp );

        sc = fitter->fit( vtx , tmplist ); 
        if( !sc ) continue;

        m_util->calcIP(*kpp, &vtx, ip, ipe);
        if( ipe==0 ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        verbose()<< "Worse=" << (*kpp_worse)->particleID().pid()
               << " P=" << (*kpp_worse)->p()/GeV << " ipmax=" << ipmax ;
        if ( ipmax > 3.0 && Pfit.size() > 2 ) {
          Pfit.erase( kpp_worse );
          verbose() << " killed." << endreq;	
        } else {
          verbose() << " included." << endreq;	
        }
      }
    }
    sc = fitter->fit( Vfit , Pfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }
  verbose() << "================ Fit Results: " << Pfit.size() <<endreq;
  Vfit.clearOutgoingParticles();
  for( jp=Pfit.begin(); jp!=Pfit.end(); jp++ ) {
    Vfit.addToOutgoingParticles(*jp);
    verbose() << "================  pt=" << (*jp)->pt() <<endreq;
  }

  vtxvect.push_back(Vfit);
  return vtxvect;
}
//=============================================================================
bool SVertexTool::isin(Particle::ConstVector& vtags_toexclude, 
                       const Particle* axp){

  Particle::ConstVector::const_iterator i;
  for( i=vtags_toexclude.begin(); i!=vtags_toexclude.end(); i++ ){
    if((*i)->proto() == axp->proto()) return true;
  }
  return false;
} 
//=============================================================================
double SVertexTool::angle( Gaudi::LorentzVector a, Gaudi::LorentzVector b) {
  double ang=0;
  if(a.Vect().Mag2() && b.Vect().Mag2()) {
    ang = acos( (a.Vect().Unit()).Dot(b.Vect().Unit()) );
    if(ang>M_PI) ang=2.*M_PI-ang;
    if(ang<0) ang=-ang;
  } else {
    err()<<"Zero vector(s)! Arguments: "<<a.Vect().Mag2()
         <<" "<<b.Vect().Mag2()<<endreq;
  }
  return ang;
}
double SVertexTool::ipprob(double x) {
  if( x > 40. ) return 0.6;
  double r = m_ipfitpol0 + m_ipfitpol1*x + m_ipfitpol2*pow(x,2)
    + m_ipfitpol3*pow(x,3) + m_ipfitpol4*pow(x,4) + m_ipfitpol5*pow(x,5);
  if(r<0) r=0;
  return r;
}
double SVertexTool::ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  double r = m_ptfitpol0 + m_ptfitpol1*x + m_ptfitpol2*pow(x,2)
    + m_ptfitpol3*pow(x,3) + m_ptfitpol4*pow(x,4);
  if(r<0) r=0;
  return r;
}
double SVertexTool::aprob(double x) {
  if( x < 0.02 ) return 0.32;
  double r = m_anglepol0 + m_anglepol1*x;
  if(r<0) r=0;
  return r;
}
//=============================================================================
