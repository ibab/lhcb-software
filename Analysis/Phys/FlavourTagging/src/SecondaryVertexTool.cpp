// $Id: SecondaryVertexTool.cpp,v 1.3 2005-03-01 11:21:26 musy Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "SecondaryVertexTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SecondaryVertexTool v1.3
//
// 2004-03-18 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SecondaryVertexTool>          s_factory ;
const        IToolFactory& SecondaryVertexToolFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SecondaryVertexTool::SecondaryVertexTool( const std::string& type,
 					  const std::string& name,
 					  const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ISecondaryVertexTool>(this);
}

StatusCode SecondaryVertexTool::initialize() {

  debug()<< "==> SecondaryVertexTool initialize" << endreq;  

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
StatusCode SecondaryVertexTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SecondaryVertexTool::~SecondaryVertexTool(){}

//=============================================================================
Vertex SecondaryVertexTool::SVertex( const Vertex RecVert, 
				     const ParticleVector vtags ){

  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex Vfit(0);
  Vertex vtx;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  Particle* p1=0, *p2=0;
  ParticleVector::const_iterator jp, kp;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    if( (*jp)->particleID().abspid()==13 ) continue;           //exclude muons

    //at least one must be long forward 
    ContainedObject* contObj1 = (*jp)->origin();
    if (!contObj1) continue;
    ProtoParticle* proto1 = dynamic_cast<ProtoParticle*>(contObj1);
    if( ! (proto1->track()->forward()) ) continue;              //preselection

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
    if( track->forward() == false ) continue;                   //preselection

    //SECOND particle ---------------------------------------
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
      if( track->forward() == false ) continue;                //preselection

      //second particle must also be forward
      ContainedObject* contObj2 = (*kp)->origin();
      if ( !contObj2 ) continue ;
      ProtoParticle* proto2 = dynamic_cast<ProtoParticle*>(contObj2);
      if( ! (proto2->track()->forward()) ) continue;           //preselection 

      sc = fitter->fitVertex( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //preselection
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //preselection

      HepLorentzVector sum = (*jp)->momentum() + (*kp)->momentum();

      //if the couple is compatible with a Ks, drop it         //preselection
      if( sum.m()/GeV > 0.490 && sum.m()/GeV < 0.505 
	 &&  (*jp)->particleID().abspid() == 211
	 &&  (*kp)->particleID().abspid() == 211
	 && ((*jp)->particleID().threeCharge())
	  * ((*kp)->particleID().threeCharge()) < 0 ) {
	debug() << "This is a Ks candidate! skip."<<endreq;
	//set their energy to 0
	HepLorentzVector zero(0.0001,0.0001,0.0001,0.0001);
	(*jp)->setMomentum(zero);
	(*kp)->setMomentum(zero);
	continue;
      }

      //build a likelihood that the combination comes from B ---------
      double x, probi1, probi2, probp1, probp2, proba, probs, probb;
      // impact parameter
      x= ipl/iperrl;
      probi1=-0.535+0.3351*x-0.03102*x*x+0.001316*x*x*x
	-0.00002598*pow(x,4)+0.0000001919*pow(x,5);
      if(x > 40.0) probi1=0.6;
      x= ips/iperrs;
      probi2=-0.535+0.3351*x-0.03102*x*x+0.001316*x*x*x
	-0.00002598*pow(x,4)+0.0000001919*pow(x,5);
      if(x > 40.0) probi2=0.6;
      // pt
      x= (*jp)->pt()/GeV;
      probp1=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x-0.01094*pow(x,4);
      if(x > 5.0) probp1=0.65;
      x= (*kp)->pt()/GeV;
      probp2=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x-0.01094*pow(x,4);
      if(x > 5.0) probp2=0.65;
      // angle
      x= ((*jp)->momentum().vect()).angle((*kp)->momentum().vect());
      proba=0.4516-1.033*x;
      if(x < 0.02) proba=0.32;

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
  //printout
  debug()<< "MAX probf= " << probf <<endreq;

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
      double x, probi1, probp1, probs, probb, likeb;
      sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe, ipVec, errMatrix);
      if( !sc ) continue;
      if( ip/ipe < 1.8 ) continue;                                        //cut
      if( ip/ipe > 100 ) continue;                                        //cut
      if( ipe > 1.5    ) continue;                                        //cut

      //likelihood for the particle comes from B ------
      x= ip/ipe;
      probi1=-0.535+0.3351*x-0.03102*x*x+0.001316*x*x*x
	-0.00002598*pow(x,4)+0.0000001919*pow(x,5);
      if(x > 40.0) probi1=0.6;
      // pt
      x= (*jpp)->pt()/GeV;
      probp1=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x-0.01094*pow(x,4);
      if(x > 5.0) probp1=0.65;
      // total
      if(probi1<0) probi1=0;
      if(probp1<0) probp1=0;
      probs=probi1*probp1;
      probb=(1-probi1)*(1-probp1);
      likeb=probs/(probs+probb);
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

  return Vfit;
}

//=============================================================================
Vertex SecondaryVertexTool::SVertexNN( const Vertex RecVert, 
				       const ParticleVector vtags ){

  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  StatusCode sc;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  Vertex Vfit, vtx;
  Particle* p1=0, *p2=0;
  ParticleVector::const_iterator jp, kp, jpp;
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 

  long iijp = 0;
  for ( jp = vtags.begin(); jp != vtags.end()-1; jp++, iijp++ ) {

    int jpid = (*jp)->particleID().abspid();
    if( jpid == 2212 ) continue;                                //preselection 

    sc = geom->calcImpactPar(**jp, RecVert, ipl, iperrl, ipVec, errMatrix);
    if( sc.isFailure() ) continue;
    if( fabs(ipl/iperrl) < 2.0 ) continue;                      //preselection 
    if( iperrl > 0.15 ) continue;                               //preselection 
    if( fabs(ipl) > 4.0 ) continue;                             //preselection 
    if( jpid==11 && fabs(ipl) > 2.5 ) continue;                 //preselection 

    double lcs=1000.;
    ContainedObject* contObj = (*jp)->origin();
    ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
    TrStoredTrack* track = proto->track();
    if((track->measurements()).size() > 5)
      lcs = track->lastChiSq()/((track->measurements()).size()-5);
    if( lcs > 2.0 ) continue;                                   //preselection
    if( track->forward() == false ) continue;                   //preselection

    //SECOND particle ---------------------------------------
    long iikp = iijp+1;
    for ( kp = (jp+1) ; kp != vtags.end(); kp++, iikp++ ) {

      int kpid = (*kp)->particleID().abspid();
      if( kpid == 2212 ) continue;                             //preselection 

      sc = geom->calcImpactPar(**kp, RecVert, ips,iperrs, ipVec, errMatrix);
      if( sc.isFailure() ) continue;  
      if( fabs(ips/iperrs) < 2.0 ) continue;                   //preselection 
      if( iperrs > 0.15 ) continue;                            //preselection 
      if( fabs(ips) > 4.0 ) continue;                          //preselection 
      if( kpid==11 && fabs(ips) > 2.5 ) continue;              //preselection 

      double lcs=1000.;
      ContainedObject* contObj = (*kp)->origin();
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      TrStoredTrack* track = proto->track();
      if((track->measurements()).size() > 5)
	lcs = track->lastChiSq()/((track->measurements()).size()-5);
      if( lcs > 2.0 ) continue;                                //preselection
      if( track->forward() == false ) continue;                //preselection

      sc = fitter->fitVertex( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if((vtx.position().z()/mm - RVz) < -10.0 ) continue;    //preselection

      //if the couple is compatible with a Ks, drop it          //preselection
      HepLorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.m()/GeV > 0.490 && sum.m()/GeV < 0.505 
	 &&  jpid == 211 &&  kpid == 211
	 && ((*jp)->particleID().threeCharge())
	  * ((*kp)->particleID().threeCharge()) < 0 ) {
	debug() << "This is a Ks candidate! skip."<<endreq;
	//set their energy to 0
	HepLorentzVector zero(0.0001,0.0001,0.0001,0.0001);
	(*jp)->setMomentum(zero);
	(*kp)->setMomentum(zero);
	continue;
      }

      //build a probability that the combination comes from B ---------
      //P1,P2,PT1,PT2,B0PT,IP1,IP2,JVZ,JCHI
      double NNoutput = NNseeding( (*jp)->p()/GeV,
				   (*kp)->p()/GeV,
				   (*jp)->pt()/GeV,
				   (*kp)->pt()/GeV,
				   7.0,
				   ipl/iperrl,
				   ips/iperrs,
				   vtx.position().z()/mm - RVz,
				   vtx.chi2()/vtx.nDoF() )/2.0+0.5;
      if( NNoutput < 0.2 ) continue;                           //preselection
      if( NNoutput > probf ) {
	probf = NNoutput;
	Vfit = vtx;
	p1 = (*jp);
	p2 = (*kp);
      }
    }
  }
  debug()<< "MAX probf= " << probf <<endreq;

  Vertex VfitTMP;
  ParticleVector Pfit(0);  

  if( probf > 0.8 ) {

    //add iteratively other tracks to the seed ----------------------
    Pfit.push_back(p1);
    Pfit.push_back(p2);
    for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;

      double ip, ipe;
      sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe, ipVec, errMatrix);
      if( !sc ) continue;
      if( ip/ipe < 2.0 ) continue;                                       //cut
      if( ip/ipe > 100 ) continue;                                       //cut
      if( ipe > 1.5    ) continue;                                       //cut

      //likelihood for the particle comes from B ------
      double x, probi1, probp1, probs, probb;
      x= ip/ipe;
      probi1=-0.535+0.3351*x-0.03102*x*x+0.001316*x*x*x
        -0.00002598*pow(x,4)+0.0000001919*pow(x,5);
      if(x > 40.0) probi1=0.6;
      // pt
      x= (*jpp)->pt()/GeV;
      probp1=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x-0.01094*pow(x,4);
      if(x > 5.0) probp1=0.65;
      // total
      if(probi1<0) probi1=0;
      if(probp1<0) probp1=0;
      probs=probi1*probp1;
      probb=(1-probi1)*(1-probp1);
      if( probs/(probs+probb) < 0.2 ) continue;                          //cut

      sc = fitter->fitVertex( Pfit, VfitTMP ); /////////FIT before
      if( !sc ) continue; 
      sc = geom->calcImpactPar(**jpp, VfitTMP, ip, ipe, ipVec, errMatrix);
      if( !sc ) continue; 
      if( ip/ipe > 5.0 ) continue;                                       //cut

      //Add track to the fit
      Pfit.push_back(*jpp);

      sc = fitter->fitVertex( Pfit, VfitTMP ); /////////FIT after
      if( !sc ) { Pfit.pop_back(); continue; }
    
      if( VfitTMP.chi2() / VfitTMP.nDoF() > 5.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut
      if( VfitTMP.position().z()/mm - RVz < 1.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut

      debug()<<"---> chi="<< VfitTMP.chi2() / VfitTMP.nDoF()
	     <<" Vz-RVz="<< VfitTMP.position().z()/mm - RVz
	     <<" prob="<< probs/(probs+probb)
	     <<" IPbef="<< ip/ipe
	     <<endreq;

      //look what is the part which behaves worse
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
      if( worse_exist && ipmax > 3.0) Pfit.erase( kpp_worse );
    }

    sc = fitter->fitVertex( Pfit, Vfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }
  debug() << "================ Fit Results: " << Pfit.size() <<endreq;
  Vfit.clearProducts();
  for( jp=Pfit.begin(); jp!=Pfit.end(); jp++ ) Vfit.addToProducts(*jp);

  return Vfit;
}

//=============================================================================
double SecondaryVertexTool::NNseeding(double P1,
				      double P2,
				      double PT1,
				      double PT2,
				      double B0PT,
				      double IP1,
				      double IP2,
				      double JVZ,
				      double JCHI ) {
  //normalise inputs
  double OUT1 = min(P1/ 80., 1.);
  double OUT2 = min(P2/ 80., 1.);
  double OUT3 = min(PT1/ 8., 1.);
  double OUT4 = min(PT2/ 8., 1.);
  double OUT5 = min(B0PT/25., 1.);
  double OUT6 = min(fabs(IP1/40.)-.05, 1.);
  double OUT7 = min(fabs(IP2/40.)-.05, 1.);
  double OUT8 = min(JVZ/30., 1.);
  double OUT9 = log(JCHI)/10.;

  double RIN1 = 9.391623e-01
    +(-5.951788e+00) * OUT1
    +(-3.643498e+00) * OUT2
    +(-1.364269e+00) * OUT3
    +(1.414424e+00) * OUT4
    +(4.470808e-01) * OUT5
    +(-6.285307e+00) * OUT6
    +(-5.931499e+00) * OUT7
    +(-3.205423e+00) * OUT8
    +(-1.104598e+00) * OUT9;
  double RIN2 = -1.747574e-01
    +(3.133887e-01) * OUT1
    +(4.566571e-01) * OUT2
    +(-2.023576e+00) * OUT3
    +(1.525844e+00) * OUT4
    +(-6.491805e-01) * OUT5
    +(-1.473083e+00) * OUT6
    +(-5.245672e+00) * OUT7
    +(-6.187496e+00) * OUT8
    +(9.767284e-02) * OUT9;
  double RIN3 = 8.948249e-01
    +(7.731287e-01) * OUT1
    +(1.597335e+00) * OUT2
    +(5.308616e-01) * OUT3
    +(-2.925036e+00) * OUT4
    +(3.082963e+00) * OUT5
    +(6.905531e+00) * OUT6
    +(4.739557e+00) * OUT7
    +(-5.694037e-01) * OUT8
    +(4.910773e-01) * OUT9;
  double RIN4 = 4.027563e-01
    +(-4.363552e-01) * OUT1
    +(-2.553686e-01) * OUT2
    +(3.182271e-01) * OUT3
    +(1.288926e-02) * OUT4
    +(6.456403e-01) * OUT5
    +(1.528162e+01) * OUT6
    +(1.210068e+01) * OUT7
    +(-8.270981e-01) * OUT8
    +(2.113075e-01) * OUT9;
  double RIN5 = 2.674934e+00
    +(-6.622229e-01) * OUT1
    +(-1.498590e+00) * OUT2
    +(7.153726e+00) * OUT3
    +(1.092053e+01) * OUT4
    +(-1.866176e-01) * OUT5
    +(5.295907e-01) * OUT6
    +(1.764385e-01) * OUT7
    +(-4.817290e-01) * OUT8
    +(-4.241088e+00) * OUT9;

  OUT1 = SIGMOID(RIN1);
  OUT2 = SIGMOID(RIN2);
  OUT3 = SIGMOID(RIN3);
  OUT4 = SIGMOID(RIN4);
  OUT5 = SIGMOID(RIN5);

  return -2.873225e+00
    +(-8.990243e-01) * OUT1
    +(-6.287556e-01) * OUT2
    +(-4.041671e+00) * OUT3
    +(4.500425e+00) * OUT4
    +(3.393500e+00) * OUT5;
}

//=============================================================================
double SecondaryVertexTool::SIGMOID(double x){
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}

//=============================================================================
inline double SecondaryVertexTool::min(double x, double y) 
{ return x<=y ? x : y; }

