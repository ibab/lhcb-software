// $Id: SVertexTool.cpp,v 1.1 2005-07-04 08:20:06 pkoppenb Exp $
#include "SVertexTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SVertexTool v1.3
//
// Secondary inclusive vertexing based on a likelihood function
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SVertexTool>          s_factory ;
const        IToolFactory& SVertexToolFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexTool::SVertexTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ISecondaryVertexTool>(this);
}

StatusCode SVertexTool::initialize() {

  geom = tool<IGeomDispCalculator>("GeomDispCalculator");
  if ( !geom ) {   
    err() << "Unable to Retrieve GeomDispCalculator" << endreq;
    return StatusCode::FAILURE;
  }
  fitter = tool<IVertexFitter>("UnconstVertexFitter");
  if ( !fitter ) {   
    err() << "Unable to Retrieve UnconstVertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode SVertexTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexTool::~SVertexTool(){}

//=============================================================================
std::vector<Vertex> SVertexTool::buildVertex( const Vertex RecVert, 
					      const ParticleVector vtags ){

  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex Vfit(0);
  Vertex vtx;
  std::vector<Vertex> vtxvect(0);
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  Particle* p1=0, *p2=0;
  ParticleVector::const_iterator jp, kp;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    //FIRST seed particle -----------------------------------
    if( (*jp)->particleID().abspid()==13 ) continue;           //exclude muons
    sc = geom->calcImpactPar(**jp, RecVert, ipl, iperrl, ipVec, errMatrix);
    if( sc.isFailure() ) continue;
    if( iperrl > 1.0 ) continue;                                //preselection
    if( ipl/iperrl < 2.0 ) continue;                            //preselection
    if( ipl/iperrl > 100.0 ) continue;                          //preselection

    double lcs=1000.;
    ContainedObject* contObj = (*jp)->origin();
    ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
    TrStoredTrack* track = proto->track();
    if((track->measurements()).size() > 5)
      lcs = track->lastChiSq()/((track->measurements()).size()-5);
    if( lcs > 2.0 ) continue;                                   //preselection
    //must be long forward 
    if( track->forward() == false ) continue;                   //preselection

    //SECOND seed particle -----------------------------------
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {

      if( (*kp)->particleID().abspid()==13 ) continue;

      sc = geom->calcImpactPar(**kp, RecVert, ips, iperrs, ipVec, errMatrix);
      if( sc.isFailure() ) continue;  
      if( iperrs > 1.0 ) continue;                              //preselection 
      if( ips/iperrs < 2.0 ) continue;                          //preselection 
      if( ips/iperrs > 100.0 ) continue;                        //preselection 

      double lcs=1000.;
      ContainedObject* contObj = (*kp)->origin();
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      TrStoredTrack* track = proto->track();
      if((track->measurements()).size() > 5)
        lcs = track->lastChiSq()/((track->measurements()).size()-5);
      if( lcs > 2.0 ) continue;                                //preselection
      //second particle must also be forward
      if( track->forward() == false ) continue;                //preselection

      //cut on the z position of the seed
      sc = fitter->fitVertex( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //preselection
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //preselection

      //if the couple is compatible with a Ks, drop it         //preselection
      HepLorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.m()/GeV > 0.490 && sum.m()/GeV < 0.505 
	 &&  (*jp)->particleID().abspid() == 211
	 &&  (*kp)->particleID().abspid() == 211
	 && ((*jp)->particleID().threeCharge())
	  * ((*kp)->particleID().threeCharge()) < 0 ) {
	debug() << "This is a Ks candidate! skip."<<endreq;
	//set their energy to 0 so that they're not used afterwards
	HepLorentzVector zero(0.0001,0.0001,0.0001,0.0001);
	(*jp)->setMomentum(zero);
	(*kp)->setMomentum(zero);
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
      proba=aprob(((*jp)->momentum().vect()).angle((*kp)->momentum().vect()));
      // total
      probs=probi1*probi2*probp1*probp2*proba;
      probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      if( probs/(probs+probb) < 0.2 ) continue;                   //preselection
      if( probs/(probs+probb) > probf ) {
	probf = probs/(probs+probb);
	Vfit = vtx;
	p1 = (*jp);
	p2 = (*kp);
      }
    }
  }

  Vertex VfitTMP;
  ParticleVector Pfit(0);

  if( probf>0.32 && p1 != NULL && p2 != NULL ) {

    //add iteratively other tracks to the seed ----------------------
    Pfit.push_back(p1);
    Pfit.push_back(p2);
    ParticleVector::const_iterator jpp;
    for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;
      if( (*jpp)->p()/GeV < 2.0 ) continue;
      if( (*jpp)->particleID().abspid()==13 ) continue;  //exclude muons

      double ip, ipe;
      sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe, ipVec, errMatrix);
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
      if( likeb < 0.2 ) continue;                                         //cut

      Pfit.push_back(*jpp);

      sc = fitter->fitVertex( Pfit, VfitTMP ); /////////FIT///////////
      if( !sc ) { Pfit.pop_back(); continue; }
    
      if( VfitTMP.chi2() / VfitTMP.nDoF()  > 5.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut
      if((VfitTMP.position().z()/mm - RVz) < 1.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut

      //look what is the part which behaves the worst
      double ipmax = -1.0;
      int ikpp = 0;
      bool worse_exist = false;
      ParticleVector::iterator kpp, kpp_worse;

      for( kpp=Pfit.begin(); kpp!=Pfit.end(); kpp++, ikpp++ ){
	if( Pfit.size() < 3 ) break;

	ParticleVector tmplist = Pfit;
	tmplist.erase( tmplist.begin() + ikpp );

	sc = fitter->fitVertex( tmplist, vtx ); 
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
    sc = fitter->fitVertex( Pfit, Vfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }
  debug() << "================ Fit Results: " << Pfit.size() <<endreq;
  Vfit.clearProducts();
  for( jp=Pfit.begin(); jp!=Pfit.end(); jp++ ) Vfit.addToProducts(*jp);

  vtxvect.push_back(Vfit);
  return vtxvect;
}
//=============================================================================
double SVertexTool::ipprob(double x) {
  if( x > 40. ) return 0.6;
  double r = - 0.535 + 0.3351*x - 0.03102*pow(x,2) + 0.001316*pow(x,3)
    - 0.00002598*pow(x,4) + 0.0000001919*pow(x,5);
  if(r<0) r=0;
  return r;
}
double SVertexTool::ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  double r = 0.04332 + 0.9493*x - 0.5283*pow(x,2) + 0.1296*pow(x,3)
    - 0.01094*pow(x,4);
  if(r<0) r=0;
  return r;
}
double SVertexTool::aprob(double x) {
  if( x < 0.02 ) return 0.32;
  return 0.4516 - 1.033*x;
}
//=============================================================================
