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
BTagging::BTagging(const std::string& name,
		   ISvcLocator* pSvcLocator) : DVAlgorithm(name, pSvcLocator){

  declareProperty( "Muon_Pt_cut", m_AXPt_cut_muon = 1.2 );
  declareProperty( "Muon_P_cut",  m_AXP_cut_muon  = 5.0 );

  declareProperty( "Ele_Pt_cut",   m_AXPt_cut_ele = 1.0 );
  declareProperty( "Ele_P_cut",    m_AXP_cut_ele  = 5.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin    = 21  );
  declareProperty( "VeloChargeMax",m_VeloChMax    = 32  );
  declareProperty( "EoverP",       m_EoverP       = 0.8 );

  declareProperty( "Kaon_Pt_cut",  m_AXPt_cut_kaon = 0.4 );
  declareProperty( "Kaon_P_cut",   m_AXP_cut_kaon  = 3.0 );
  declareProperty( "Kaon_IP_cut",  m_IP_cut_kaon   = 3.5 );
  declareProperty( "Kaon_IPPU_cut",m_IPPU_cut_kaon = 3.0 );

  declareProperty( "KaonSame_Pt_cut", m_AXPt_cut_kaonS = 0.4 );
  declareProperty( "KaonSame_P_cut",  m_AXP_cut_kaonS  = 4.0 );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS   = 2.5 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS   = 1.1 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS   = 1.0 );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS    = 1.5 );

  declareProperty( "PionSame_Pt_cut", m_AXPt_cut_pionS = 0.2 );
  declareProperty( "PionSame_P_cut",  m_AXP_cut_pionS  = 2.0 );
  declareProperty( "PionSame_IP_cut", m_IP_cut_pionS   = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS    = 3.0 );
  declareProperty( "PionSame_dQ2_cut",m_dQ2cut_pionS   = 1.0 );

  declareProperty( "TrVeloChargeName",m_veloChargeName = "TrVeloCharge" );
  declareProperty( "SecondaryVertexType", m_SVtype = "none" );
  declareProperty( "CombinationTechnique", m_CombinationTechnique = "none");
  declareProperty( "ProbMin", m_ProbMin = 0.52);
  declareProperty( "VchOmega", m_VchOmega = 0.40);

  declareProperty( "TagOutputLocation", m_TagLocation = "" );
  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );

  m_vtxtool = 0;
  m_nnet = 0;
}

//=============================================================================
BTagging::~BTagging() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BTagging::initialize() {

  m_veloCharge= tool<ITrVeloCharge> (m_veloChargeName, this);
  if ( ! m_veloCharge ) {   
    fatal() << " Unable to retrieve " << m_veloChargeName << endreq;
    return StatusCode::FAILURE;
  }
  m_vtxtool = tool<ISecondaryVertexTool> ("SecondaryVertexTool");
  if(! m_vtxtool) {
    fatal() << "Unable to retrieve SecondaryVertexTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ("NNetTool");
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTagging::execute() {
  
  if( !filterPassed() ) {
    debug() << "Event was not selected. "<< endreq;
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
  debug() << "Event was  selected, proceed.. "<< endreq;
  setFilterPassed( true );
 
  // Retrieve informations about event
  EventHeader* evt = get<EventHeader> (EventHeaderLocation::Default);
  if ( !evt ) {   
    err() << "Unable to Retrieve Event" << endreq;
    return StatusCode::SUCCESS;
  }

  // Retrieve trigger info
  TrgDecision* trg = get<TrgDecision> (TrgDecisionLocation::Default);
  if( 0 == trg ) warning() << "Unable to find Trigger." << endreq;

  // Select on trigger
  debug()<< "Check if passes trigger.."<<endreq;
  if(m_RequireL0)  if(trg) if( trg->L0() == 0 ) return StatusCode::SUCCESS;
  if(m_RequireL1)  if(trg) if( trg->L1() == 0 ) return StatusCode::SUCCESS;
  if(m_RequireHLT) if(trg) if( trg->HLT()== 0 ) return StatusCode::SUCCESS;

  // Retrieve Tampering info
  double tamper = 0;
  TamperingResults* tampres = 
                    get<TamperingResults>(TamperingResultsLocation::Default);
  if( tampres ) {
    tamper = tampres->L1TIS() + tampres->L1TOS()*10 + tampres->L1TOB()*100;
    debug() << " Results read ( TIS, TOS, TOB ): "
	    << tampres->L1TIS() << " "
	    << tampres->L1TOS() << " "
	    << tampres->L1TOB() << endreq;
  } else {
    err() << " TamperingResults not found at "
	  << TamperingResultsLocation::Default << endreq;
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------
  // Counter of events processed
  debug() << ">>>>>  Tagging Run Nr " << evt->runNum()
	  << " Event " << evt->evtNum() << "  <<<<<" << endreq;

  // Fill the PhysDeskTop
  const ParticleVector& parts = desktop()->particles();
  const VertexVector&   verts = desktop()->vertices();    
  StatusCode sc = desktop()->saveDesktop();
  if (sc) {
    debug() << "  Nr Vertices: "  << verts.size() 
	    << "  Nr Particles: " << parts.size() <<endreq;
  } else {
    err() << "Unable to save desktop to TES" << endreq;
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;

  //look for B in selection:
  debug() << "-------- Signal:" << endreq;

  Particle* AXB0 = 0;
  ParticleVector axdaugh(0);
  bool isBd = false;
  bool isBs = false;
  bool isBu = false;
  HepLorentzVector ptot;
  ParticleVector::const_iterator ipart;
  double B0mass=0, B0the=0, B0phi=0, B0p=0;

  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if((*ipart)->particleID().hasBottom()) {
      AXB0 = (*ipart);
      axdaugh = FindDaughters(AXB0);
      axdaugh.push_back(AXB0);
      if( AXB0->particleID().hasDown() )    isBd = true;
      if( AXB0->particleID().hasStrange() ) isBs = true;
      if( AXB0->particleID().hasUp() )      isBu = true;
      debug() << "Found Selected B0/Bs!  daugh=" 
	  << axdaugh.size()-1 <<endreq;
      ptot = AXB0->momentum();
      B0mass = ptot.m()/GeV;
      B0the  = ptot.theta();
      B0phi  = ptot.phi();
      B0p    = ptot.vect().mag()/GeV;

      debug() << "Signal B-Mass=" << B0mass << endreq;
      break;
    }  
  }
  if( axdaugh.size() == 0 ) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  //choose as primary vtx the one with smallest IP wrt B signal
  Vertex* RecVert = 0; 
  double knrec = 0;
  double kdmin = 1000000;
  Vertices::const_iterator iv;
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ){
      knrec++;
      double ip, iperr;
      sc = geomDispCalculator()->calcImpactPar(*AXB0, **iv, ip,
					       iperr, ipVec, errMatrix);
      debug() <<"Vertex IP="<<ip/iperr<<endreq;
      if ( sc.isSuccess() && fabs(ip/iperr) < kdmin ) {
	kdmin = fabs(ip/iperr);
	RecVert = (*iv);
      }     
    }
  }
  if( !RecVert ) {
    err() <<"No Reconstructed Vertex!! Skip." <<endreq;
    return StatusCode::SUCCESS;
  }    

  //build VertexVector of pileup ----------------------------
  PileUpVtx.clear();
  for( iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() != Vertex::Primary ) continue;
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }

  //loop over Particles, build vtags vector////////////////////////
  ParticleVector vtags(0);

  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if( vtags.size() > MAXSIZETAGS-1 ) {
      warning() << "Reached array limit on N" <<endreq;  
      break; 
    }

    if( (*ipart)->charge() == 0 ) continue;                    //preselection
    if( (*ipart)->p()/GeV < 2.0 ) continue;                    //preselection
    if( (*ipart)->momentum().theta() < 0.012 ) continue;       //preselection
 
    if( isinTree( *ipart, axdaugh ) )  continue ;              //exclude signal

    //kill upstreams
    ContainedObject* contObj = (*ipart)->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      if(proto->track()->isDownstream()) continue;             //preselection
    }

    //eliminate from vtags all parts from a pileup vtx
    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    PileUpIP( *ipart, ippu, ippuerr );
    if(ippuerr) if( ippu/ippuerr < m_IPPU_cut_kaon ) continue; //preselection

    ///////////////////////////////////////
    vtags.push_back(*ipart);            //
    /////////////////////////////////////
  }

  //fill info ------//////////////////////////////////////////////
  debug() << "-------- Tagging Candidates: "<< vtags.size() <<endreq;
  long   N = 0;
  long   AXID[MAXSIZETAGS];
  double AXP[MAXSIZETAGS];
  double AXPt[MAXSIZETAGS];
  double AXphi[MAXSIZETAGS];
  long   ch[MAXSIZETAGS];
  double AXip[MAXSIZETAGS];
  double AXiperr[MAXSIZETAGS];
  double IPPU[MAXSIZETAGS];
  long   trtyp[MAXSIZETAGS];
  double InvMss[MAXSIZETAGS];
  double ThBp[MAXSIZETAGS];
  double veloch[MAXSIZETAGS];
  double Emeas[MAXSIZETAGS];
  double lcs[MAXSIZETAGS];
  double IPT[MAXSIZETAGS];

  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    Particle* axp = (*ipart);
    HepLorentzVector axpv = axp->momentum();

    AXID[N]  = axp->particleID().pid(); 
    AXP[N]   = axp->p()/GeV;
    AXPt[N]  = axp->pt()/GeV;
    ch[N]    = (int) axp->charge();
    AXphi[N] = axpv.phi();
    InvMss[N]= (ptot + axpv).m()/GeV;
    Hep3Vector vBp= (ptot+axpv).vect() ;
    ThBp[N]  = (axpv.vect()).angle(vBp);
    veloch[N]= m_veloCharge -> calculate( axp );
    lcs[N]   = 1000.;
    IPT[N]   =-1000.;
    trtyp[N] = 0;
  
    debug() << AXID[N] << " P="<< AXP[N] << " Pt="<< AXPt[N]
	    << " invmss="<< InvMss[N]<<endreq;
    debug() <<std::setprecision(6)<< " B   ="<< ptot<<endreq;
    debug() <<std::setprecision(6)<< " axpv="<< axpv<<endreq;
    debug() <<std::setprecision(6)<< " ax+B="<< axpv+ptot<<endreq;

    //calculate signed IP wrt RecVert
    AXip[N]   =-100.0;
    AXiperr[N]= 1000.0;
    double ip, iperr ; 
    sc = geomDispCalculator()->calcImpactPar(*axp, *RecVert, ip,
                                             iperr, ipVec, errMatrix);
    if( sc ) {
      AXip[N]   = ipVec.z()>0 ? ip : -ip ; 
      AXiperr[N]= iperr; 
    }

    //calculate min IP wrt all pileup vtxs 
    PileUpIP( axp, ip, iperr );
    if(iperr) IPPU[N]=ip/iperr; else IPPU[N]=10000.0;

    //look for protopart to see what type it is
    ContainedObject* contObj = axp->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      TrStoredTrack* track = proto->track();

      SmartRefVector<CaloHypo>& vcalo = proto->calo();
      if(vcalo.size()==1) Emeas[N] = (*(vcalo.begin()))->e()/GeV;
      else Emeas[N] = -1;

      if((track->measurements()).size() > 5)
	lcs[N] = track->lastChiSq()/((track->measurements()).size()-5);

      if     (track->forward()     ) trtyp[N] = 1;
      else if(track->match()       ) trtyp[N] = 2;
      else if(track->isUpstream()  ) trtyp[N] = 3;
      else if(track->isDownstream()) trtyp[N] = 4;
      else if(track->isVelotrack() ) trtyp[N] = 5;
      else if(track->isTtrack()    ) trtyp[N] = 6;
      else if(track->isBackward()  ) trtyp[N] = 7;
      else if(track->follow()      ) trtyp[N] = 8;
      else if(track->ksTrack()     ) trtyp[N] = 9;
    }
    printInfo(axp);

    N++;
  }

  ///--------------------------------------------------------- VertexCharge
  double Vch = 0;  
  Vertex Vfit(0);
  if     (m_SVtype=="TDR")  Vfit = m_vtxtool->SVertex( *RecVert, vtags );
  else if(m_SVtype=="NNet") Vfit = m_vtxtool->SVertexNN( *RecVert, vtags );
  else warning()<< "No Vertex Charge tag will be used! "<<m_SVtype<<endreq;

  ParticleVector Pfit = toStdVector(Vfit.products());
  if( Pfit.size()> 1) {
    debug() << "VTX charge ------" << endreq;
    ParticleVector::iterator kp;
    for( kp = Pfit.begin(); kp != Pfit.end(); kp++ ) {
      printInfo(*kp);
      Vch += (int) (*kp)->charge();
    }
    debug() << "Vfit chi2=" << Vfit.chi2() /Vfit.nDoF()
	    << "  Vertex charge: " << Vch << endreq;
    //calculate IP of all taggers wrt new SV
    int i=0;
    for( kp=vtags.begin(); kp != vtags.end(); kp++, i++ ) {
      double ip, iperr ;
      sc = geomDispCalculator()->calcImpactPar(**kp, Vfit, ip,
                                               iperr, ipVec, errMatrix);
      if( sc ) IPT[i] = ipVec.z()>0 ? ip/iperr : -ip/iperr;
    }
  }

  ///----------------------------------------------------------------------
  //select among tag candidates
  //some inits first
  int imuon = -1;
  int iele  = -1;
  int ikaon = -1;
  int ipionS= -1;
  int ikaonS= -1;
  int iSame = -1;
  double qk = 0;
  double ptmaxm = -99.0;
  double ptmaxe = -99.0;
  double ptmaxk = -99.0;
  double ptmaxkS= -99.0;
  double ptmaxpS= -99.0;
  int chsum=0, ic=0, ix=0, tagdecision=0;

  for( int i=0; i < N; i++ ) {
  
    if( trtyp[i] > 2 ) continue; //keep only long tracks

    double IPsig= fabs(AXip[i]/AXiperr[i]);
    double dQ   = InvMss[i]-B0mass;
    debug() << AXID[i] << " P="<< AXP[i]<<"  IPsig="<< IPsig
	    << " invmss="<< InvMss[i]
	    << " lcs="<< lcs[i]
	    <<endreq;


    switch ( abs(AXID[i]) ) {
    case 13:                           // selects OS muon tag
      if(AXPt[i] > m_AXPt_cut_muon)
	if(AXP[i]  > m_AXP_cut_muon)
	  if( AXPt[i] > ptmaxm ) { //Pt ordering
	    imuon = i;
	    ptmaxm = AXPt[i];
	  }   
      break;

    case 11:                           // selects OS electron tag      
      if(AXPt[i] > m_AXPt_cut_ele)
	if(AXP[i] > m_AXP_cut_ele)
	  if(veloch[i] > m_VeloChMin && veloch[i] < m_VeloChMax)
	    if(Emeas[i]/AXP[i] > m_EoverP || Emeas[i]<0)
	      if(trtyp[i] == 1) 
		if( AXPt[i] > ptmaxe ) { //Pt ordering
		  iele = i;
		  ptmaxe = AXPt[i];
		}
      break;

    case 321:                         // selects OS kaon tag
      if(AXPt[i] > m_AXPt_cut_kaon)
	if(AXP[i]  > m_AXP_cut_kaon)
	  if(IPPU[i] > m_IPPU_cut_kaon)
	    if(Pfit.size()==0 || fabs(IPT[i])<8.0 )
	      if((trtyp[i]==1 && lcs[i]<2.5 && fabs(AXip[i])<2.0) ||
		 (trtyp[i]==2 && lcs[i]<1.4 && fabs(AXip[i])<1.0) ||
		 (trtyp[i]==3 && lcs[i]<2.5 && fabs(AXip[i])<1.0) )
		if(IPsig > m_IP_cut_kaon ) {
		  qk += ch[i];
		  if( AXPt[i] > ptmaxk ) { //Pt ordering
		    ikaon = i;
		    ptmaxk = AXPt[i];
		  }
		}
      if( isBs ) {                // selects SS kaon tag
	double deta= fabs(log(tan(B0the/2)/tan(asin(AXPt[i]/AXP[i])/2)));
	double dphi= fabs(AXphi[i]-B0phi); if(dphi>3.1416) dphi=6.2832-dphi;
	if(AXPt[i] > m_AXPt_cut_kaonS) 
	  if(AXP[i] > m_AXP_cut_kaonS) 
	    if(IPsig < m_IP_cut_kaonS)
	      if(dphi < m_phicut_kaonS)
		if(deta < m_etacut_kaonS)
		  if(dQ < m_dQcut_kaonS)
		    if(trtyp[i]==1 || (trtyp[i]==3 && lcs[i]<2.5))
		      if( AXPt[i] > ptmaxkS ) { //Pt ordering
			ikaonS = i;
			ptmaxkS = AXPt[i];
		      }
      }	
      break;

    case 211:                       // selects SS PION tag
      if( isBd || isBu ) {
	if(AXPt[i] > m_AXPt_cut_pionS)
	  if(AXP[i] > m_AXP_cut_pionS)
	    if(IPsig < m_IP_cut_pionS)
	      if(dQ < m_dQcut_pionS ) 
		if(trtyp[i]==1 || (trtyp[i]==3 && lcs[i]<2.5))
		  if( AXPt[i] > ptmaxpS ) { //Pt ordering
		    ipionS = i;
		    ptmaxpS = AXPt[i];
		  }
      }	
      break;
    }
  }

  //--- Combine tags: TDR approach -----------------------------------------
  int itag[6];
  for( int j=0; j!=6; j++ ) itag[j]=0; 

  if(m_CombinationTechnique == "TDR") {

    //in case ele and mu both exist kill lower Pt
    if( imuon != -1 && iele!= -1 ) {
      if(AXPt[imuon] > AXPt[iele]) iele= -1; else imuon= -1;
    } 
    //extra cut for pions SS candidates
    if(ipionS!=-1) if(InvMss[ipionS]-B0mass > m_dQ2cut_pionS) ipionS=-1;
    //-----------------

    if( imuon != -1 ) { ic+=10000; itag[1]=-ch[imuon];}
    if( iele  != -1 ) { ic+= 1000; itag[2]=-ch[iele]; }
    if( ikaon != -1 ) { ic+=  100; itag[3]=-ch[ikaon];}
    //same side (if pionSS fill only category nr7)
    if((isBd || isBu ) && ic==0 ) iSame=ipionS;
    if( isBs ) iSame=ikaonS;
    if( iSame != -1 ) { ic+=   10; itag[4]=ch[iSame];}
    if( Vch ) itag[5]=Vch>0 ? -1:1 ;

    if(     ic==10000) {    // mu 
      ix=1; tagdecision = -ch[imuon];
    }
    else if(ic== 1000) {    // el 
      ix=2; tagdecision = -ch[iele];
    }
    else if(ic==  100) {    // K 
      ix=3; if(qk) tagdecision = qk>0 ? -1 : 1;
    }
    else if(ic==10100) {   // mu-k
      ix=4; chsum = ch[imuon]+ch[ikaon];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic== 1100) {   // e-k 
      ix=5; chsum = ch[iele]+ch[ikaon];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic==    0 && Vch!=0 ) { // vertex charge
      ix=6; tagdecision = Vch>0 ? -1 : 1;
    }
    else if(ic==   10) {    // Same Side pion or kaon 
      ix=7; tagdecision = ch[iSame];
      if( isBu ) tagdecision= -tagdecision; //swap same-side if Bu
    }
    else if(ic==10010) {   // mu-kS 
      ix=8; chsum = ch[imuon]-ch[iSame];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic== 1010) {   // e-kS 
      ix=9; chsum = ch[iele]-ch[iSame];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic==  110) {   // k-kS 
      ix=10; chsum = ch[ikaon]-ch[iSame];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic==10110) {   // mu-k-kS 
      ix=11; chsum = ch[imuon]+ch[ikaon]-ch[iSame];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
    else if(ic== 1110) {   // e-k-kS 
      ix=12; chsum = ch[iele]+ch[ikaon]-ch[iSame];
      if(chsum) tagdecision = chsum>0 ? -1 : 1;
    }
  }

  //--- Combine tags: NNet approach -----------------------------------------
  else if(m_CombinationTechnique == "NNet") {

    int i;
    double pn[6], pnsum_a, pnsum_b, rnet;
    for( i=0; i!=6; i++ ) pn[i]=0.50; 

    if(imuon>0) {
      i=imuon;
      rnet = m_nnet->MLPm(      
			  min(B0p/600., 1.),      //Bp
			  min(B0the/0.25, 1.),    //Bth
			  N/90.,                  //multiplicity
			  tamper ,                //TAMPER
			  min(AXP[i]/150., 1.),   //P
			  min(AXPt[i]/10., 1.),   //Pt
			  min(max(AXip[i]/AXiperr[i]/100.,-1),1.),//IPPV
			  min(max(IPT[i]/12.,-1),1.),          //IPSV
			  0.  );                              //VFLAG
      itag[1]= ch[imuon]>0 ? -1 : 1;
      pn[1]  = 1.0-pol3(rnet, 1.2939, -2.0406, 0.90781); //1-omega
    }
    if(iele>0) {
      i=iele;
      rnet = m_nnet->MLPe(      
			  min(B0p/600., 1.),      //Bp
			  min(B0the/0.25, 1.),    //Bth
			  N/90.,                  //multiplicity
			  tamper ,                //TAMPER
			  min(AXP[i]/150., 1.),   //P
			  min(AXPt[i]/10., 1.),   //Pt
			  min(max(AXip[i]/AXiperr[i]/100.,-1),1.),//IPPV
			  min(max(IPT[i]/12.,-1),1.),          //IPSV
			  0.  );                              //VFLAG
      itag[2]= ch[iele]>0 ? -1 : 1;
      pn[2]  = 1.0-pol4(rnet, 0.4933, -0.6766, 1.761, -1.587);
    }
    if(ikaon>0) {
      i=ikaon;
      rnet = m_nnet->MLPk(      
			  min(B0p/600., 1.),      //Bp
			  min(B0the/0.25, 1.),    //Bth
			  N/90.,                  //multiplicity
			  tamper ,                //TAMPER
			  min(AXP[i]/150., 1.),   //P
			  min(AXPt[i]/10., 1.),   //Pt
			  min(max(AXip[i]/AXiperr[i]/100.,-1),1.),//IPPV
			  min(max(IPT[i]/12.,-1),1.),          //IPSV
			  0.  );                              //VFLAG
      itag[3]= ch[ikaon]>0 ? -1 : 1;
      pn[3]  = 1.0-pol2(rnet, 0.52144, -0.27136);
      debug()<< "pawmlpk("  
	     <<   min(B0p/600., 1.)   <<", "   
	     <<	  min(B0the/0.25, 1.) <<", "
	     <<	  N/90.               <<", "
	     <<	  tamper                <<", "
	     <<   min(AXP[i]/150., 1.)  <<", "
	     <<	  min(AXPt[i]/10., 1.)  <<", "
	     <<	  min(max(AXip[i]/AXiperr[i]/100.,-1),1.) <<", "
	     <<	  min(max(IPT[i]/12.,-1),1.)         <<", "
	     <<	  0. << ")= "<< rnet <<endreq;
    }
    if(ikaonS>0) {
      i=ikaonS;
      double deta= log(tan(B0the/2.))-log(tan(asin(AXPt[i]/AXP[i])/2.));
      double dphi= min(fabs(AXphi[i]-B0phi), 6.283-fabs(AXphi[i]-B0phi));
      double dQ  = InvMss[i]-B0mass;
      rnet = m_nnet->MLPkS(      
			  min(B0p/600., 1.),      //Bp
			  min(B0the/0.25, 1.),    //Bth
			  N/90.,                  //multiplicity
			  tamper ,                //TAMPER
			  min(AXP[i]/150., 1.),   //P
			  min(AXPt[i]/10., 1.),   //Pt
			  min(max(AXip[i]/AXiperr[i]/100.,-1),1.),//IPPV
			  min(max(IPT[i]/12.,-1),1.),          //IPSV
			  0.,                                 //VFLAG
			  deta/3.5,
			  dphi/3.2,
			  min(dQ/12.0, 1.) );
      itag[4]= ch[ikaonS]>0 ? 1 : -1;
      pn[4]  = 1.0-pol2(rnet, 1.0007, -1.0049);
      debug()<< "pawmlpkS("  
	     <<   min(B0p/600., 1.)   <<", "   
	     <<	  min(B0the/0.25, 1.) <<", "
	     <<	  N/90.               <<", "
	     <<	  tamper                <<", "
	     <<   min(AXP[i]/150., 1.)  <<", "
	     <<	  min(AXPt[i]/10., 1.)  <<", "
	     <<	  min(max(AXip[i]/AXiperr[i],-1)/100.,1.) <<", "
	     <<	  min(max(IPT[i]/12.,-1),1.)         <<", "
	     <<	  0. <<", "
	     <<   deta/3.5 <<", "
	     <<   dphi/3.2 <<", "
	     <<   min(dQ/12.0, 1.) 
	     <<   ")= " << rnet <<endreq;
    }
    if(ipionS>0) {
      i=ipionS;
      double deta= log(tan(B0the/2.))-log(tan(asin(AXPt[i]/AXP[i])/2.));
      double dphi= min(fabs(AXphi[i]-B0phi), 6.283-fabs(AXphi[i]-B0phi));
      double dQ  = InvMss[i]-B0mass;
      rnet = m_nnet->MLPpS(      
			  min(B0p/600., 1.),      //Bp
			  min(B0the/0.25, 1.),    //Bth
			  N/90.,                  //multiplicity
			  tamper ,                //TAMPER
			  min(AXP[i]/150., 1.),   //P
			  min(AXPt[i]/10., 1.),   //Pt
			  min(max(AXip[i]/AXiperr[i]/100.,-1),1.),//IPPV
			  min(max(IPT[i]/12.,-1),1.),          //IPSV
			  0.,                                 //VFLAG
			  deta/3.5,
			  dphi/3.2,
			  min(dQ/12.0, 1.) );
      itag[4]= ch[ipionS]>0 ? 1 : -1;
      pn[4]  = 1.0-pol2(rnet, 1.0772, -1.1632);
    }
    if( Vch ) {
      itag[5]= Vch>0 ? -1 : 1;
      pn[5]  = 1.0-m_VchOmega;
    }

    //make decision
    pnsum_a= 0.50;             //hypothesis truetag=+1
    pnsum_b= 0.50;             //hypothesis truetag=-1

    for( int ik = 1; ik != 6; ik++ ) {
      double mtag = itag[ik];
      pnsum_a *= ((1-mtag)/2+mtag* pn[ik] ); // p
      pnsum_b *= ((1+mtag)/2-mtag* pn[ik] ); //(1-p)
      debug()<<"ik="<<ik<<" itag[ik]="<<mtag<<" pn[ik]="<<pn[ik]<<endreq;
    }      
    if(pnsum_a > pnsum_b) tagdecision = +1;
    if(pnsum_a < pnsum_b) tagdecision = -1;
    double pnsum = max(pnsum_a,pnsum_b)/(pnsum_a + pnsum_b);

    //throw away poorly significant tags
    if(pnsum < m_ProbMin) {
      pnsum=0.50;
      tagdecision = 0;
    }
    debug()<<"pnsum_a=" << pnsum_a <<endreq;
    debug()<<"pnsum_b=" << pnsum_b <<endreq;
    debug()<<"pnsum  =" << pnsum <<endreq;

    //sort into categories
    double dpnsum = pnsum-m_ProbMin;
    if(tagdecision) ix = ( (int)(10.0*dpnsum/fabs(1.-dpnsum)) ) +1;
    if(ix>5) ix=5;
    if(ix<0) ix=0;
  }
  else {
    fatal()<<"Unknown tagger combination: "<<m_CombinationTechnique<<endreq;
    return StatusCode::FAILURE;
  } 
  if(tagdecision == 0) ix=0; 

  ///OUTPUT to Logfile -----------------------------------------------------
  info() << "BTAGGING TAG  " << evt->runNum()
	 << std::setw(4) << evt->evtNum()
	 << std::setw(4) << trg->L0() + trg->L1() *10 + trg->HLT() *100
	 << std::setw(5) << tagdecision
	 << std::setw(3) << ix  
	 << std::setw(5) << itag[1]
	 << std::setw(3) << itag[2]
	 << std::setw(3) << itag[3]
	 << std::setw(3) << itag[4]
	 << std::setw(3) << itag[5] << endreq;

  ///OUTPUT to TES ---------------------------------------------------------
  FlavourTag* theTag = new FlavourTag;

  theTag->setTaggedB( AXB0 );
  if     (tagdecision== 1) theTag->setDecision( FlavourTag::bbar );
  else if(tagdecision==-1) theTag->setDecision( FlavourTag::b );
  else theTag->setDecision( FlavourTag::none );
  theTag->setCategory(ix);

  FlavourTags* tags = new FlavourTags;
  tags->insert(theTag);
  sc = eventSvc()->registerObject( m_TagLocation + "/Tags", tags );
  if( sc.isFailure() )
    err() << "Unable to register the tags"<< endreq;
  else debug() << "Registered tag into "<< m_TagLocation << endreq;

  Particles* tagCands = new Particles;  
  if(imuon !=-1) tagCands->insert( *(vtags.begin()+imuon) );
  if(iele  !=-1) tagCands->insert( *(vtags.begin()+iele ) );
  if(ikaon !=-1) tagCands->insert( *(vtags.begin()+ikaon) );
  if(ikaonS!=-1) tagCands->insert( *(vtags.begin()+ikaonS) );
  if(ipionS!=-1) tagCands->insert( *(vtags.begin()+ipionS) );

  sc= eventSvc()->registerObject( m_TagLocation + "/Taggers", tagCands);
  if( sc.isFailure() ) err() << "Unable to register candidates."<< endreq;

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTagging::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTagging::isinTree( Particle* axp, ParticleVector& sons ) {

  for( ParticleVector::iterator ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->origin() == axp->origin() ) {
      debug() << "excluding signal part: " 
	      << axp->particleID().pid() <<" with p="<<axp->p()/GeV<<endreq;
      return true;
    }
  }
  return false;
}
//=============================================================================
void BTagging::PileUpIP( Particle* axp, double& ip, double& ipe) {

  double ipmin = 100000.0;
  double ipminerr = 0.0;
  Vertices::const_iterator iv;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;

  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
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
    std::string tipo = "   ";
    ContainedObject* contObj = axchild->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      if(proto->track()->isLong()      ) tipo = "lng";
      if(proto->track()->isDownstream()) tipo = "dwn";
      if(proto->track()->isUpstream()  ) tipo = "ups";
      if(proto->track()->isVelotrack() ) tipo = "vel";
      if(proto->track()->isTtrack()    ) tipo = "ttr";
      if(proto->track()->isBackward()  ) tipo = "bak";
    }
    debug()<< tipo << " >>> "<< axchild->particleID().pid() << " <<<"
	   << std::setprecision(3)
	   << "  P=" << axchild->p()/GeV 
	   << " th=" << axchild->momentum().theta()*1000.0
	   << endreq;   
  }
}
//=============================================================================
ParticleVector BTagging::toStdVector( SmartRefVector<Particle>& refvector ) {
  ParticleVector tvector(0);
  for( SmartRefVector<Particle>::iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//=============================================================================
inline double BTagging::min(double x, double y) { return x<y ? x : y; }
inline double BTagging::max(double x, double y) { return x>y ? x : y; }
double BTagging::pol2(double x, double a0, double a1) { return a0+a1*x; }
double BTagging::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}
double BTagging::pol4(double x, double a0, double a1, double a2, double a3) {
  return a0+a1*x+a2*x*x+a3*pow(x,3);
}
//=============================================================================
//return a vector containing all daughters of signal 
ParticleVector BTagging::FindDaughters( Particle* axp ) {

   ParticleVector apv(0), apv2, aplist(0);
   apv.push_back(axp);

   do {
     apv2.clear();
     for( Particles::iterator ip=apv.begin(); ip!=apv.end(); ip++ ) {
       if( (*ip)->endVertex() ) {
	 ParticleVector tmp = toStdVector((*ip)->endVertex()->products());
	 for( Particles::iterator itmp=tmp.begin(); itmp!=tmp.end(); itmp++) {
	   apv2.push_back(*itmp);
	   aplist.push_back(*itmp);
	 }
       }
     }
     apv = apv2;
   } while ( apv2.size() );
   	    
   return aplist;
}
//=============================================================================
