// Include files 
#include "BTagging.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTagging
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BTagging>          s_factory ;
const        IAlgFactory& BTaggingFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTagging::BTagging( const std::string& name,
	    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) {

 declareProperty( "WriteNtpl",   m_WriteNtpl = false );

 declareProperty( "Muon_Pt_cut", m_AXPt_cut_muon = 1.2 );
 declareProperty( "Muon_P_cut",  m_AXP_cut_muon  = 5.0 );
 declareProperty( "Muon_IP_cut", m_IP_cut_muon   = -99 );

 declareProperty( "Ele_Pt_cut",  m_AXPt_cut_ele  = 1.2 );
 declareProperty( "Ele_P_cut",   m_AXP_cut_ele   = 5.0 );
 declareProperty( "Ele_IP_cut",  m_IP_cut_ele    = -99 );

 declareProperty( "Kaon_Pt_cut", m_AXPt_cut_kaon = 0.4 );
 declareProperty( "Kaon_P_cut",  m_AXP_cut_kaon  = 3.0 );
 declareProperty( "Kaon_IP_cut", m_IP_cut_kaon   = 3.7 );

 declareProperty( "KaonSame_Pt_cut", m_AXPt_cut_kaonS = 0.4 );
 declareProperty( "KaonSame_P_cut",  m_AXP_cut_kaonS  = 4.0 );
 declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS   = 2.5 );
 declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS   = 1.1 );
 declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS   = 1.0 );
 declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS    = 1.5 );

}

//=============================================================================
BTagging::~BTagging() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BTagging::initialize() {

  MsgStream req(msgSvc(), name());
  
  // Load all necessary tools via the base class
  StatusCode sc = loadTools();
  if( !sc ) {
    req << MSG::ERROR << "   Unable to load tools" << endreq;
    return StatusCode::FAILURE;
  }

  sc = toolSvc()->retrieveTool( "VisPrimVertTool", m_visTool, this );
  if( sc.isFailure() ) {
     req << MSG::FATAL << "Unable to retrieve the VisPrimVertTool" << endreq;
     return sc;
  }

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  sc = service("ParticlePropertySvc", ppSvc);
  if( !sc ) {
    req << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
   
  // Now book ntuples
  NTuplePtr nt1(ntupleSvc(), "/NTUPLES/FILE1/70");
  if ( !nt1 ) {
    nt1 = ntupleSvc()->book ("/NTUPLES/FILE1", 70,
			     CLID_ColumnWiseTuple, "BTagging");
    if ( !nt1 ) {
      req << MSG::FATAL << "    Could not book Ntuple" << endreq;
      return StatusCode::FAILURE;
    }
    // Add elements to ntuple definition:
    sc = nt1->addItem ("Run",    m_Run);
    sc = nt1->addItem ("Event",  m_Event);
    sc = nt1->addItem ("Tag",    m_Tag);
    sc = nt1->addItem ("TagCat", m_TagCat);
    sc = nt1->addItem ("trig0",  m_trig0);
    sc = nt1->addItem ("trig1",  m_trig1);
    sc = nt1->addItem ("BID",    m_BID);
    sc = nt1->addItem ("B0mass", m_B0mass);
    sc = nt1->addItem ("B0the",  m_B0the);
    sc = nt1->addItem ("B0phi",  m_B0phi);
    sc = nt1->addItem ("RVz",    m_RVz);
    sc = nt1->addItem ("K",      m_K);
    sc = nt1->addItem ("kType",  m_kType);
    sc = nt1->addItem ("knrec",  m_knrec);

    //tagging candidates
    sc = nt1->addItem ("N",     m_N, 0, 100 ); //limit
    sc = nt1->addItem ("ID",     m_N, m_AXID);
    sc = nt1->addItem ("P",      m_N, m_AXP);
    sc = nt1->addItem ("Pt",     m_N, m_AXPt);
    sc = nt1->addItem ("phi",    m_N, m_AXphi);
    sc = nt1->addItem ("ch",     m_N, m_ch);
    sc = nt1->addItem ("ip",     m_N, m_AXip);
    sc = nt1->addItem ("iperr",  m_N, m_AXiperr);
    sc = nt1->addItem ("ipPU",   m_N, m_IPPU);
    sc = nt1->addItem ("trtyp",  m_N, m_trtyp);
    sc = nt1->addItem ("InvMss", m_N, m_InvMss);
    sc = nt1->addItem ("ThBp",   m_N, m_ThBp);

  } else {
    req << MSG::FATAL << "    The ntuple was already booked" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTagging::execute() {
  
  MsgStream  req( msgSvc(), name() );
  
  NTuplePtr nt1(ntupleSvc(), "/NTUPLES/FILE1/70");
  if ( !nt1 ) {   
    req << MSG::ERROR << "    Unable to book Ntpl" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc = StatusCode::SUCCESS;  
  
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if ( !evt ) {   
    req << MSG::ERROR << "    Unable to Retrieve Event" << endreq;
    return StatusCode::FAILURE;
  }

  SmartDataPtr<Collisions> mcCollisions(eventSvc(), CollisionLocation::Default);
  if ( !mcCollisions ) {   
    req << MSG::ERROR << "    Unable to Retrieve Collisions" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve L0-trigger info
  SmartDataPtr<L0DUReport> l0du( eventSvc(), L0DUReportLocation::Default );
  m_trig0 = 0; if( 0 != l0du ) if( l0du->decision() ) m_trig0 = 1;

  // Retrieve L1-trigger info
  SmartDataPtr<L1Report> L1Report( eventSvc(), L1ReportLocation::Default );
  m_trig1 = 0; if( 0 != L1Report ) if( L1Report->decision() ) m_trig1 = 1;

  // Counter of events processed
  req << MSG::DEBUG << ">>>>>  Processing Event Nr " << evt->evtNum()
      << " Run " << evt->runNum() << "  <<<<<" << endreq;

  m_Run   = evt->runNum();
  m_Event = evt->evtNum();

  //----------------------------------------------------------------------
  // Fill the PhysDeskTop
  StatusCode scDesktop = desktop()->getInput();
  if (scDesktop.isFailure()) {
    req << MSG::ERROR << "    Unable to fill PhysDesktop " << endreq;
    return StatusCode::SUCCESS;
  }
  const ParticleVector&   parts = desktop()->particles();
  const VertexVector& verts_tmp = desktop()->vertices();    
  scDesktop = desktop()->saveDesktop();
  if (scDesktop) {
    req << MSG::DEBUG 
	<< "Nr Vertices: "    << verts_tmp.size() 
	<< "  Nr Particles: " << parts.size() <<endreq;
  } else {
    req << MSG::ERROR << "not able to save desktop in TES" << endreq;
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------
  Vertices::const_iterator iv, jv;
  VertexVector verts;
  verts.clear();
  //check that there are no duplicates of Vertices !!!! (very dirty!)
  //(the second half is sometimes doubled for selected events...)
  bool isDupV = false;
  for( iv=verts_tmp.begin(); iv!=verts_tmp.end(); iv++){
    if( (*iv)->type() != Vertex::Primary ) continue;
    for( jv=iv+1; jv!=verts_tmp.end(); jv++){
      if( (*jv)->type() != Vertex::Primary ) continue;
      double dist = ((*iv)->position()).distance((*jv)->position())/mm;
      if( dist < 0.00001 ) { isDupV = true; break; }
    }
    if(!isDupV) verts.push_back(*iv);
    req << MSG::DEBUG << "Vtx ok: " << (*iv)->position() <<endreq;
  }

  //look for B in official selection
  m_BID = 0;
  ParticleVector::const_iterator ipart, jpart, jp, kp;
  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    int aid = (*ipart)->particleID().abspid();
    if( aid > 500 && aid < 550 ) m_BID = (*ipart)->particleID().pid();
  }

  req << MSG::DEBUG << "-------- Signal:  " << m_BID << endreq;

  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  HepLorentzVector ptot;
  Particle* AXB0 = 0;
  ParticleVector axfigli ;
  axfigli.clear();

  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if( (*ipart)->particleID().abspid() == abs(m_BID)) {
      AXB0 = (*ipart);
      axfigli = FindDaughters(AXB0);
      axfigli.push_back(AXB0);
      req << MSG::DEBUG << "Found Selected B0/Bs! "<< m_BID <<"  sons=" 
	  << axfigli.size()-1 <<endreq;
      //signal b
      ptot = AXB0->momentum();
      m_B0mass = float(ptot.m()/GeV);
      m_B0the  = float(ptot.theta());
      m_B0phi  = float(ptot.phi());

      req << MSG::DEBUG << "Signal B-Mass=" << m_B0mass << endreq;
      break;
    }  
  }
  if( axfigli.size() == 0 ) {
    req << MSG::DEBUG <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  //choose as primary vtx the one with smallest IP wrt B signal
  double kdmin = 1000000;
  RecVert = 0;
  m_knrec = 0;
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ){
      m_knrec++;
      double ip, iperr;
      sc = geomDispCalculator()->calcImpactPar(*AXB0, **iv, ip,
					       iperr, ipVec, errMatrix);
      if ( sc.isSuccess() && fabs(ip/iperr) < kdmin ) {
	kdmin = fabs(ip/iperr);
	RecVert = (*iv);
      }     
    }
  }
  if( !RecVert ) {
   req << MSG::ERROR <<"NO RecVert!! Skip." <<endreq;
   return StatusCode::SUCCESS;
  }    

  //build VertexVector of pileup ----------------------------
  PileUpVtx.clear();
  for(Vertices::const_iterator iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() != Vertex::Primary ) continue;
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    req << MSG::DEBUG <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }
  m_RVz = RecVert->position().z()/mm ;

  //loop over Particles, build vtags vector////////////////////////
  ParticleVector vtags;
  vtags.clear();

  for ( ipart = parts.end()-1; ipart != parts.begin()-1; ipart-- ){
    if( vtags.size() > 99 ) {
      req << MSG::WARNING << "Reached array limit on N" <<endreq;  
      break; 
    }

    if( (*ipart)->charge() == 0 ) continue;                    //preselection
    if( (*ipart)->p()/GeV < 2.0 ) continue;                    //preselection
    if( (*ipart)->momentum().theta() < 0.012 ) continue;       //preselection
 
    if( isinTree( *ipart, axfigli ) )  continue ;              //exclude signal

    //kill upstreams
    ContainedObject* contObj = (*ipart)->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      if(proto->track()->upstream()) continue;                 //preselection
    }

    //eliminate from vtags all parts from a pileup vtx
    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    PileUpIP( *ipart, ippu, ippuerr );
    if( ippuerr ) if( ippu/ippuerr < 3.7 ) continue;           //preselection
  
    //check that there are no duplicates of Particles !!!! (very dirty!)
    //(the second half is usually doubled for selected events...)
    bool isDup = false;
    for( jpart = ipart-1; jpart != parts.begin()-1; jpart--){
       if( (*jpart)->origin() == (*ipart)->origin() ) { 
	 isDup=true; 
	 req << MSG::DEBUG << "Duplication! p="<< (*ipart)->p()/GeV <<endreq;
	 break; 
       }
    }
    ///////////////////////////////////////
    if(!isDup) vtags.push_back(*ipart); //
    /////////////////////////////////////
  }

  //fill info ------//////////////////////////////////////////////
  req << MSG::DEBUG << "-------- Tagging Candidates: "<< vtags.size() << endreq;
  m_N = 0;
  Particle* axp;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    axp = (*ipart);
    m_AXID[m_N]  = axp->particleID().pid(); 
    m_AXP[m_N]   = axp->p()/GeV;
    m_AXPt[m_N]  = axp->pt()/GeV;
    m_ch[m_N]= (int) axp->charge();
    m_AXphi[m_N] = axp->momentum().phi();
    m_InvMss[m_N]= (ptot + axp->momentum()).m()/GeV;
    Hep3Vector vp  = (axp->momentum()).vect() ;
    Hep3Vector vBp = (ptot+axp->momentum()).vect() ;
    m_ThBp[m_N]  = vp.angle(vBp/(vBp.mag()));

    //calculate signed IP wrt RecVert
    m_AXip[m_N]   =-100.0;
    m_AXiperr[m_N]= 1000.0;
    double ip, iperr ; 
    sc = geomDispCalculator()->calcImpactPar(*axp, *RecVert, ip,
                                             iperr, ipVec, errMatrix);
    if( sc ) {
      m_AXip[m_N]   = ipVec.z()>0 ? ip : -ip ; 
      m_AXiperr[m_N]= iperr; 
    }

    //calculate min IP wrt all pileup vtxs 
    PileUpIP( axp, ip, iperr );
    if(iperr) m_IPPU[m_N]=ip/iperr; else m_IPPU[m_N]=10000.0;

    //look for protopart to see what type it is
    ContainedObject* contObj = axp->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      m_trtyp[m_N] = 0;
      if(proto->track()->forward() ) m_trtyp[m_N] = 1;
      if(proto->track()->match()   ) m_trtyp[m_N] = 2;
      if(proto->track()->upstream()) m_trtyp[m_N] = 3;
      if(proto->track()->veloTT()  ) m_trtyp[m_N] = 4;
      if(proto->track()->velo()    ) m_trtyp[m_N] = 5;
      if(proto->track()->seed()    ) m_trtyp[m_N] = 6;
    }
    printInfo(axp);

    m_N++;
  }

  ///----------------------------------------------------------------------
  //BUILD UP 2 SEED PARTICLES FOR VERTEXING

  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  Vertex Vfit, vtx;
  Particle* p1=0, *p2=0;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    if( (*jp)->particleID().abspid()==13 ) continue;  //exclude muons

    //at least one must be long forward 
    ContainedObject* contObj1 = (*jp)->origin();
    if (!contObj1) continue;
    ProtoParticle* proto1 = dynamic_cast<ProtoParticle*>(contObj1);
    if( ! (proto1->track()->forward()) ) continue;               //preselection 

    sc = geomDispCalculator()->calcImpactPar(**jp, *RecVert, ipl,
					     iperrl, ipVec, errMatrix);
    if( sc.isFailure() ) continue;
    if( iperrl > 1.0 ) continue;                                 //preselection 
    if( ipl/iperrl < 2.0 ) continue;                             //preselection 
    if( ipl/iperrl > 100.0 ) continue;                           //preselection 

    //SECOND particle ---------------------------------------
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {

      if( (*kp)->particleID().abspid()==13 ) continue;

      sc = geomDispCalculator()->calcImpactPar(**kp, *RecVert, ips,
					       iperrs, ipVec, errMatrix);
      if( sc.isFailure() ) continue;  
      if( iperrs > 1.0 ) continue;                              //preselection 
      if( ips/iperrs < 2.0 ) continue;                          //preselection 
      if( ips/iperrs > 100.0 ) continue;                        //preselection 
 
      //second particle must also be forward
      ContainedObject* contObj2 = (*kp)->origin();
      if ( !contObj2 ) continue ;
      ProtoParticle* proto2 = dynamic_cast<ProtoParticle*>(contObj2);
      if( ! (proto2->track()->forward()) ) continue;           //preselection 

      sc = vertexFitter()->fitVertex( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      int ndf = 2*2 - 2;                                       //correct NDoF ?
      if( vtx.chi2() / ndf > 10.0 ) continue;                  //preselection
      if((vtx.position().z()/mm - m_RVz) < 1.0 ) continue;     //preselection

      HepLorentzVector sum = (*jp)->momentum() + (*kp)->momentum();

      //if the couple is compatible with a Ks, drop it         //preselection
      if( sum.m()/GeV > 0.490 && sum.m()/GeV < 0.505 
	 &&  (*jp)->particleID().abspid() == 211
	 &&  (*kp)->particleID().abspid() == 211
	 && ((*jp)->particleID().threeCharge())
	  * ((*kp)->particleID().threeCharge()) < 0 ) {
	req <<MSG::DEBUG<< "This is a Ks candidate! skip."<<endreq;
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
      probp1=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x
	-0.01094*pow(x,4);
      if(x > 5.0) probp1=0.65;
      x= (*kp)->pt()/GeV;
      probp2=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x
	-0.01094*pow(x,4);
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
  req <<MSG::DEBUG<< "Seed Particles: -------------------------" <<endreq;
  if( p1 ) if( p2 ) { printInfo(p1); printInfo(p2); }
  req <<MSG::DEBUG<< "MAX probf= " << probf <<endreq;
  req <<MSG::DEBUG<< "---------------------------------------- " <<endreq;

  Vertex VfitTMP;
  ParticleVector Pfit;  
  Pfit.clear();

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
      sc = geomDispCalculator()->calcImpactPar(**jpp, *RecVert, ip,
					       ipe, ipVec, errMatrix);
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
      probp1=0.04332+0.9493*x-0.5283*x*x+0.1296*x*x*x
	-0.01094*pow(x,4);
      if(x > 5.0) probp1=0.65;
      // total
      if(probi1<0) probi1=0;
      if(probp1<0) probp1=0;
      probs=probi1*probp1;
      probb=(1-probi1)*(1-probp1);
      likeb=probs/(probs+probb);
      if( likeb < 0.2 ) continue;                                         //cut

      Pfit.push_back(*jpp);

      sc = vertexFitter()->fitVertex( Pfit, VfitTMP ); /////////FIT///////////
      if( !sc ) { Pfit.pop_back(); continue; }
    
      int ndf = 2 * Pfit.size() - 2;                              //correct NDoF ?

      if( VfitTMP.chi2() / ndf    > 5.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut
      if((VfitTMP.position().z()/mm - m_RVz) < 1.0 ) 
	{ Pfit.pop_back(); continue; }                                   //cut

      //look what is the part which behaves worst
      double ipmax = -1.0;
      int ikpp = 0;
      bool worse_exist = false;
      ParticleVector::iterator kpp, kpp_worse;

      for( kpp=Pfit.begin(); kpp!=Pfit.end(); kpp++, ikpp++ ){
	if( Pfit.size() < 3 ) break;

	ParticleVector tmplist = Pfit;
	tmplist.erase( tmplist.begin() + ikpp );

	sc = vertexFitter()->fitVertex( tmplist, vtx ); 
	if( !sc ) continue;

	sc = geomDispCalculator()->calcImpactPar(**kpp, vtx, ip,
						 ipe, ipVec, errMatrix);
	if( !sc ) continue;
	if( ip/ipe > ipmax ) {
	  ipmax = ip/ipe;
	  kpp_worse = kpp;
	  worse_exist = true;
	}
      }
      //decide if keep it or kill it
      if( worse_exist ) {
	req <<MSG::DEBUG<< "Worse=" << (*kpp_worse)->particleID().pid()
	    << " P=" << (*kpp_worse)->p()/GeV
	    << " ipmax=" << ipmax ;
	if ( ipmax > 3.0 && Pfit.size() > 2 ) {
	  Pfit.erase( kpp_worse );
	  req << " killed." << endreq;	
	} else {
	  req << " included." << endreq;	
	}
      }
    }
    sc = vertexFitter()->fitVertex( Pfit, Vfit ); //RE-FIT//////////////////////
    if( !sc ) Pfit.clear();
  }
  req <<MSG::DEBUG<< "================ Fit Results: " << Pfit.size() <<endreq;
  
  double ach = 0;
  for( ParticleVector::iterator kp=Pfit.begin(); kp != Pfit.end(); kp++ ) {
    printInfo(*kp);
    ach += (*kp)->charge();
  }
  if( Pfit.size()> 1) {
    req <<MSG::DEBUG<< "Vfit chi2=" << Vfit.chi2() 
	<< "  Vertex charge:" << ach << endreq;
  }

  ///----------------------------------------------------------------------
  //DECIDE NOW TAG

  // some inits first
  int imuon = -1;
  int iele  = -1;
  int ikaon = -1;
  int ikaonS= -1;
  double qk = 0;
  double ptmaxm = -99.0;
  double ptmaxe = -99.0;
  double ptmaxk = -99.0;
  double ptmaxkS= -99.0;

  for( int i=0; i < m_N; i++ ) {
    
    if( m_trtyp[i] > 3 ) continue;

    if(abs(m_AXID[i]) == 13) { // selects OS muon tag
      if(m_AXPt[i] > m_AXPt_cut_muon &&
	 m_AXP[i]  > m_AXP_cut_muon  &&
	 fabs(m_AXip[i]/m_AXiperr[i]) > m_IP_cut_muon ) {
	if( m_AXPt[i] > ptmaxm ) { //Pt ordering
	  imuon = i;
	  ptmaxm = m_AXPt[i];
	}
      }
    }
    if(abs(m_AXID[i]) == 11) { // selects OS electron tag
      if(m_AXPt[i] > m_AXPt_cut_ele &&
	 m_AXP[i]  > m_AXP_cut_ele  &&
	 fabs(m_AXip[i]/m_AXiperr[i]) > m_IP_cut_ele ) {
	if( m_AXPt[i] > ptmaxe ) { //Pt ordering
	  iele = i;
	  ptmaxe = m_AXPt[i];
	}
      }
    }
    if(abs(m_AXID[i]) == 321) { // selects OS kaon tag
      if(m_AXPt[i] > m_AXPt_cut_kaon &&
	 m_AXP[i]  > m_AXP_cut_kaon  &&
	 m_IPPU[i] > 3.7 &&
	 fabs(m_AXip[i]/m_AXiperr[i]) > m_IP_cut_kaon ) {
	qk += m_ch[i];
	if( m_AXPt[i] > ptmaxk ) { //Pt ordering
	  ikaon = i;
	  ptmaxk = m_AXPt[i];
	}
      }
    }
    if( abs(m_BID) == 531 && abs(m_AXID[i]) == 321) { // selects SS kaon tag
      if(m_AXPt[i] > m_AXPt_cut_kaonS &&
	 m_AXP[i]  > m_AXP_cut_kaonS  &&
	 fabs(m_AXip[i]/m_AXiperr[i]) < m_IP_cut_kaonS ) {

	double deta= log(tan(m_B0the/2))-log(tan(asin(m_AXPt[i]/m_AXP[i])/2));
	double dphi= fabs(m_AXphi[i]-m_B0phi);
	if(dphi > 3.1416 ) dphi = 6.2832 - dphi;
	double dQ  = m_InvMss[i]-m_B0mass;

	if( fabs(dphi) < m_phicut_kaonS &&
	    fabs(deta) < m_etacut_kaonS &&
	    dQ < m_dQcut_kaonS ) {
	  if(m_AXPt[i] > ptmaxkS ) { //Pt ordering
	    ikaonS = i;
	    ptmaxkS = m_AXPt[i];
	  }
	}
      }
    }
  }

  //--- combine tags:
  //in case ele and mu both exist kill lower Pt
  if( imuon != -1 && iele!= -1 ) {
    if(m_AXPt[imuon] > m_AXPt[iele]) iele= -1; else imuon= -1;
  } 

  int ic=0;
  int ix=0;
  if( imuon  != -1)  ic+=10000;
  if( iele   != -1)  ic+=1000;
  if( ikaon  != -1)  ic+=100;
  if( ikaonS != -1)  ic+=10;
  ic++;

  //////////////////////

  int tagdecision = 0;
  if(ic==10001) {    // mu 
    ix=1;
    tagdecision = -m_ch[imuon];
  }
  if(ic== 1001) {    // el 
    ix=2;
    tagdecision = -m_ch[iele];
  }
  if(ic==  101) {    // K 
    ix=3 ;
    if(qk != 0 ) tagdecision = int(-qk/fabs(qk));
  }
  if(ic==10101) {   // mu-k
    ix=4;
    int chsum = m_ch[imuon]+m_ch[ikaon];
    if(chsum) tagdecision = -chsum/abs(chsum);
  }
  if(ic== 1101) {   // e-k 
    ix=5;
    int chsum = m_ch[iele]+m_ch[ikaon];
    if(chsum) tagdecision = -chsum/abs(chsum);
  }
  if(ic==    1 && Pfit.size() >1 ) { // vertex charge
    ix=6;
    if(ach) tagdecision = int(-ach/fabs(ach));
  }
  if(ic==   11) {    // KS 
    ix=7;
    tagdecision = m_ch[ikaonS];
  }
  if(ic==10011) {   // mu-kS 
    ix=8;
    int chsum = m_ch[imuon]-m_ch[ikaonS];
    if(chsum) tagdecision = -chsum/abs(chsum);   
  }
  if(ic== 1011) {   // e-kS 
    ix=9;
    int chsum = m_ch[iele]-m_ch[ikaonS];
    if(chsum) tagdecision = -chsum/abs(chsum);   
  }
  if(ic==  111) {   // k-kS 
    ix=10;
    int chsum = m_ch[ikaon]-m_ch[ikaonS];
    if(chsum) tagdecision = -chsum/abs(chsum);   
  }
  if(ic==10111) {   // mu-k-kS 
    ix=11;
    int chsum = m_ch[imuon]+m_ch[ikaon]-m_ch[ikaonS];
    if(chsum) tagdecision = -chsum/abs(chsum) ;  
  }
  if(ic== 1111) {   // e-k-kS 
    ix=12;
    int chsum = m_ch[iele]+m_ch[ikaon]-m_ch[ikaonS];
    if(chsum) tagdecision = -chsum/abs(chsum);  
  }

  if(tagdecision==0) ix=0; 

  m_Tag = tagdecision;
  m_TagCat = ix; 

  req << MSG::DEBUG << "====> Tagging: " << tagdecision
      << "   Category=" << ix << "   code:" << ic <<endreq;

  //look into MCtruth to know how many visible collisions are there
  //and what process signal corresponds to (info not used for tagging!)
  m_K = 0;
  m_kType = 0;
  for (Collisions::const_iterator colls = mcCollisions->begin(); 
       colls != mcCollisions->end(); colls++) {
    if( m_visTool->isVisible(*colls) ) {
      m_K++;
      if( (*colls)->isSignal() ) {
	m_kType = (*colls)->processType();
	req << MSG::DEBUG << "Collision process type: " << m_kType 
	    << " at z=" << (*colls)->primVtxPosition().z()/mm << endreq;
      }
    }
  }

  ///OUTPUT ---------------------------------------------------------------
  req << MSG::INFO << "TAGGINGINFO  " << m_Run 
      << " " << m_Event 
      << " " << tagdecision 
      << " " << ix 
      << " " << m_trig0 
      << " " << m_trig1 
      << " " << m_knrec
      << " " << m_K
      << endreq;
  if( m_WriteNtpl ) {
    sc = nt1->write();
    if( !sc ) req << MSG::ERROR << "Cannot fill tagging Ntuple." << endreq;
  }
  ///----------------------------------------------------------------------

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTagging::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTagging::isinTree( Particle* axp, ParticleVector& sons ) {
  MsgStream req( msgSvc(), name() );

  ParticleVector::iterator ip;
  for( ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->origin() == axp->origin() ) {
      req << MSG::DEBUG << "excluding signal part: " 
	  << axp->particleID().pid() << " with p=" << axp->p()/GeV <<endreq;
      return true;
    }
  }
  return false;
}
//=============================================================================
void BTagging::PileUpIP( Particle* axp, double& ip, double& ipe) {

  double ipmin = 100000.0;
  double ipminerr = 0.0;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;

  for(Vertices::const_iterator iv=PileUpVtx.begin(); iv!=PileUpVtx.end(); iv++){

    double ipx, ipex;
    StatusCode sc = geomDispCalculator()->calcImpactPar(*axp, **iv, ipx,
							ipex, ipVec, errMatrix);
    if( sc ) if( ipx < ipmin ) {
      ipmin = ipx;
      ipminerr = ipex;
    }
  }
  ip  = ipmin;
  ipe = ipminerr;

}
//=============================================================================
void BTagging::printInfo( Particle *axchild ) {

  MsgStream req( msgSvc(), name() );

  if( req.level() > (MSG::DEBUG) ) return;

  if( axchild ) { 
    Hep3Vector ipVec;
    HepSymMatrix errMatrix;
    double ip, iperr;
    StatusCode sc=geomDispCalculator()->calcImpactPar(*axchild, *RecVert, ip,
						      iperr, ipVec, errMatrix);
    if( sc ) {
      std::string tipo = "   ";
      ContainedObject* contObj = axchild->origin();
      if (contObj) {
	ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
	if(proto->track()->forward() ) tipo = "fwd";
	if(proto->track()->match()   ) tipo = "mat";
	if(proto->track()->upstream()) tipo = "ups";
	if(proto->track()->veloTT()  ) tipo = "vtt";
	if(proto->track()->velo()    ) tipo = "vel";
	if(proto->track()->seed()    ) tipo = "sed";
      }
      ParticleProperty* ap ;
      ap = ppSvc->findByStdHepID(axchild->particleID().pid());
      req <<MSG::DEBUG<< tipo << " >>> "<< ap->particle() << " <<<"
	  << std::setprecision(3)
	  << "  P=" << axchild->p()/GeV 
	  << " th=" << axchild->momentum().theta()*1000.0
	  << " IP/s=" << ip/iperr
	  << " IP=" << ip/mm
	  << " s=" << iperr/mm
	  << endreq;   
    } 
  }
}
//=============================================================================
// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
ParticleVector BTagging::toStdVector( SmartRefVector<Particle>& refvector ) {
  ParticleVector tvector;
  tvector.clear();
  for( SmartRefVector<Particle>::iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//=============================================================================
ParticleVector BTagging::FindDaughters( Particle* axp ) {

   MsgStream req( msgSvc(), name() );
   ParticleVector apv, apv2, aplist;
   apv.clear();
   aplist.clear();
   apv.push_back(axp);
   Particles::iterator ip, itmp;

   do {
     apv2.clear();
     //fill apv2 with apv daughters
     for( ip=apv.begin(); ip!=apv.end(); ip++ ) {
       if( (*ip)->endVertex() ) {
	 ParticleVector tmp = toStdVector((*ip)->endVertex()->products());
	 for(itmp=tmp.begin(); itmp!=tmp.end(); itmp++) {
	   apv2.push_back(*itmp);
	   aplist.push_back(*itmp);
	   req <<MSG::DEBUG
	       << "Signal Particle: " << (*itmp)->particleID().pid()
	       << "  p=" << (*itmp)->p()/GeV <<endreq;
	 }
       }
     }
     apv = apv2;
   } while ( apv2.size() );
   	    
   return aplist;
}
//=============================================================================
