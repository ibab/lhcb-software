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

  declareProperty( "UseVertexCharge", m_UseVertexCharge = true );
  declareProperty( "WriteToTES",      m_WriteToTES = true );

  declareProperty( "Muon_Pt_cut", m_AXPt_cut_muon = 1.2 );
  declareProperty( "Muon_P_cut",  m_AXP_cut_muon  = 5.0 );
  declareProperty( "Muon_IP_cut", m_IP_cut_muon   = -99 );

  declareProperty( "Ele_Pt_cut",  m_AXPt_cut_ele  = 1.2 );
  declareProperty( "Ele_P_cut",   m_AXP_cut_ele   = 5.0 );
  declareProperty( "Ele_IP_cut",  m_IP_cut_ele    = -99 );

  declareProperty( "Kaon_Pt_cut", m_AXPt_cut_kaon = 0.4 );
  declareProperty( "Kaon_P_cut",  m_AXP_cut_kaon  = 3.0 );
  declareProperty( "Kaon_IP_cut", m_IP_cut_kaon   = 3.7 );
  declareProperty( "Kaon_IPPU_cut",m_IPPU_cut_kaon= 3.7 );

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

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; }
}

//=============================================================================
BTagging::~BTagging() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BTagging::initialize() {

  MsgStream req(msgSvc(), name());
  
  StatusCode sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    req << MSG::FATAL << "Unable to locate Particle Property Service"<<endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTagging::execute() {
  
  MsgStream  req( msgSvc(), name() );
  StatusCode sc = StatusCode::SUCCESS;  

  if( !filterPassed() ) {
    req << MSG::DEBUG << "Event was not selected. "<< endreq;
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
  setFilterPassed( true );
 
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if ( !evt ) {   
    req << MSG::ERROR << "    Unable to Retrieve Event" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve L0-trigger info
  SmartDataPtr<L0DUReport> l0du( eventSvc(), L0DUReportLocation::Default );
  m_trig0 = 0; if( 0 != l0du ) if( l0du->decision() ) m_trig0 = 1;

  // Retrieve L1-trigger info
  SmartDataPtr<L1Report> L1Report( eventSvc(), L1ReportLocation::Default );
  m_trig1 = 0; if( 0 != L1Report ) if( L1Report->decision() ) m_trig1 = 1;

  // Counter of events processed
  req << MSG::DEBUG << ">>>>>  Tagging Event Nr " << evt->evtNum()
      << " Run " << evt->runNum() << "  <<<<<" << endreq;

  m_Run   = evt->runNum();
  m_Event = evt->evtNum();

  //----------------------------------------------------------------------
  // Fill the PhysDeskTop
  const ParticleVector&   parts = desktop()->particles();
  const VertexVector& verts_tmp = desktop()->vertices();    
  StatusCode scDesktop = desktop()->saveDesktop();
  if (scDesktop) {
    req << MSG::DEBUG 
	<< "  Nr Vertices: "  << verts_tmp.size() 
	<< "  Nr Particles: " << parts.size() <<endreq;
  } else {
    req << MSG::ERROR << "not able to save desktop in TES" << endreq;
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------
  Vertices::const_iterator iv, jv;
  VertexVector verts(0);
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

  //look for B in selection:
  req << MSG::DEBUG << "-------- Signal:" << endreq;

  Particle* AXB0 = 0;
  ParticleVector axdaugh(0);
  bool isBd = false;
  bool isBs = false;
  HepLorentzVector ptot;
  ParticleVector::const_iterator ipart, jpart;

  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if((*ipart)->particleID().hasBottom()) {
      AXB0 = (*ipart);
      axdaugh = FindDaughters(AXB0);
      axdaugh.push_back(AXB0);
      if( AXB0->particleID().hasDown() )    isBd = true;
      if( AXB0->particleID().hasStrange() ) isBs = true;
      req << MSG::DEBUG << "Found Selected B0/Bs!  daugh=" 
	  << axdaugh.size()-1 <<endreq;
      ptot = AXB0->momentum();
      m_B0mass = float(ptot.m()/GeV);
      m_B0the  = float(ptot.theta());
      m_B0phi  = float(ptot.phi());

      req << MSG::DEBUG << "Signal B-Mass=" << m_B0mass << endreq;
      break;
    }  
  }
  if( axdaugh.size() == 0 ) {
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
    req << MSG::ERROR <<"No Reconstructed Vertex!! Skip." <<endreq;
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
  ParticleVector vtags(0);

  for ( ipart = parts.end()-1; ipart != parts.begin()-1; ipart-- ){
    if( vtags.size() > MAXSIZETAGS-1 ) {
      req << MSG::WARNING << "Reached array limit on N" <<endreq;  
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
  req<<MSG::DEBUG<< "-------- Tagging Candidates: "<< vtags.size() <<endreq;
  m_N = 0;
  Particle* axp;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    axp = (*ipart);
    m_AXID[m_N]  = axp->particleID().pid(); 
    m_AXP[m_N]   = axp->p()/GeV;
    m_AXPt[m_N]  = axp->pt()/GeV;
    m_ch[m_N]    = (int) axp->charge();
    m_AXphi[m_N] = axp->momentum().phi();
    m_InvMss[m_N]= (ptot + axp->momentum()).m()/GeV;
    Hep3Vector vp = (axp->momentum()).vect() ;
    Hep3Vector vBp= (ptot+axp->momentum()).vect() ;
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
      if(proto->track()->isLong() ) m_trtyp[m_N] = 1;
      else if(proto->track()->isDownstream()) m_trtyp[m_N] = 3;
      else if(proto->track()->isUpstream()  ) m_trtyp[m_N] = 4;
      else if(proto->track()->isVelotrack() ) m_trtyp[m_N] = 5;
      else if(proto->track()->isTtrack()    ) m_trtyp[m_N] = 6;
      else if(proto->track()->isBackward()  ) m_trtyp[m_N] = 7;
    }
    printInfo(axp);

    m_N++;
  }

  ///----------------------------------------------------------VertexCharge
  double ach = 0;
  int nroftracks=0;
  if(m_UseVertexCharge) ach = VertexCharge( vtags, nroftracks );

  ///----------------------------------------------------------------------
  //select tag candidates

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

  for( int i=0; i < m_N; i++ ) {
    
    if( m_trtyp[i] > 2 ) continue; //keep only long tracks

    double IPsig= fabs(m_AXip[i]/m_AXiperr[i]);
    double dQ   = m_InvMss[i]-m_B0mass;

    switch ( abs(m_AXID[i]) ) {

    case 13:                           // selects OS muon tag
      if(m_AXPt[i] > m_AXPt_cut_muon)
	if(m_AXP[i]  > m_AXP_cut_muon)
	  if(IPsig > m_IP_cut_muon) 
	    if( m_AXPt[i] > ptmaxm ) { //Pt ordering
	      imuon = i;
	      ptmaxm = m_AXPt[i];
	    }   
      break;

    case 11:                           // selects OS electron tag      
      if(m_AXPt[i] > m_AXPt_cut_ele)
	if(m_AXP[i]  > m_AXP_cut_ele  &&
	   IPsig > m_IP_cut_ele ) {
	  if( m_AXPt[i] > ptmaxe ) { //Pt ordering
	    iele = i;
	    ptmaxe = m_AXPt[i];
	  }
	}
      break;

    case 321:                      // selects OS kaon tag
      if(m_AXPt[i] > m_AXPt_cut_kaon &&
	 m_AXP[i]  > m_AXP_cut_kaon  &&
	 m_IPPU[i] > m_IPPU_cut_kaon &&
	 IPsig > m_IP_cut_kaon ) {
	qk += m_ch[i];
	if( m_AXPt[i] > ptmaxk ) { //Pt ordering
	  ikaon = i;
	  ptmaxk = m_AXPt[i];
	}
      }
      if( isBs ) {                // selects SS kaon tag
	double deta= fabs(log(tan(m_B0the/2)/tan(asin(m_AXPt[i]/m_AXP[i])/2)));
	double dphi= fabs(m_AXphi[i]-m_B0phi); if(dphi>3.1416) dphi=6.2832-dphi;
	if(m_AXPt[i] > m_AXPt_cut_kaonS &&
	   m_AXP[i]  > m_AXP_cut_kaonS  &&
	   IPsig < m_IP_cut_kaonS &&
	   dphi < m_phicut_kaonS &&
	   deta < m_etacut_kaonS &&
	   dQ   < m_dQcut_kaonS ) {
	  if( m_AXPt[i] > ptmaxkS ) { //Pt ordering
	    ikaonS = i;
	    ptmaxkS = m_AXPt[i];
	  }
	}
      }
      break;

    case 211:                       // selects SS PION tag
      if( isBd ) {
	if(m_AXPt[i] > m_AXPt_cut_pionS &&
	   m_AXP[i]  > m_AXP_cut_pionS  &&
	   IPsig < m_IP_cut_pionS &&
	   dQ    < m_dQcut_pionS ) {
	  if( m_AXPt[i] > ptmaxpS ) { //Pt ordering
	    ipionS = i;
	    ptmaxpS = m_AXPt[i];
	  }
	}
      }
      break;
    }
  }
  //--- Combine tags: -------------------------------------------------------

  //in case ele and mu both exist kill lower Pt
  if( imuon != -1 && iele!= -1 ) {
    if(m_AXPt[imuon] > m_AXPt[iele]) iele= -1; else imuon= -1;
  } 
  //extra cut for pions SS candidates
  if(ipionS!=-1) if(m_InvMss[ipionS]-m_B0mass > m_dQ2cut_pionS) ipionS=-1;
  //-----------------

  if( imuon != -1)  ic+=10000;
  if( iele  != -1)  ic+= 1000;
  if( ikaon != -1)  ic+=  100;
  //same side (if pionSS fill only category nr7)
  if( isBs ) iSame=ikaonS;
  if( isBd && ic==0 ) iSame=ipionS;
  if( iSame != -1)  ic+=   10;

  if(     ic==10000) {    // mu 
    ix=1;
    tagdecision = -m_ch[imuon];
  }
  else if(ic== 1000) {    // el 
    ix=2;
    tagdecision = -m_ch[iele];
  }
  else if(ic==  100) {    // K 
    ix=3 ;
    if(qk != 0 ) tagdecision = qk>0 ? -1 : 1;
  }
  else if(ic==10100) {   // mu-k
    ix=4;
    chsum = m_ch[imuon]+m_ch[ikaon];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic== 1100) {   // e-k 
    ix=5;
    chsum = m_ch[iele]+m_ch[ikaon];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic==    0 && nroftracks >1 ) { // vertex charge
    ix=6;
    if(ach) tagdecision = ach>0 ? -1 : 1;
  }
  else if(ic==   10) {    // Same Side pion or kaon 
    ix=7;
    tagdecision = m_ch[iSame];
  }
  else if(ic==10010) {   // mu-kS 
    ix=8;
    chsum = m_ch[imuon]-m_ch[iSame];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic== 1010) {   // e-kS 
    ix=9;
    chsum = m_ch[iele]-m_ch[iSame];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic==  110) {   // k-kS 
    ix=10;
    chsum = m_ch[ikaon]-m_ch[iSame];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic==10110) {   // mu-k-kS 
    ix=11;
    chsum = m_ch[imuon]+m_ch[ikaon]-m_ch[iSame];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  else if(ic== 1110) {   // e-k-kS 
    ix=12;
    chsum = m_ch[iele]+m_ch[ikaon]-m_ch[iSame];
    if(chsum) tagdecision = chsum>0 ? -1 : 1;
  }
  if(tagdecision==0) ix=0; 

  ///OUTPUT to TES ---------------------------------------------------------
  if( m_WriteToTES ) {
    FlavourTag* theTag = new FlavourTag;

    theTag->setTaggedB( AXB0 );
    if(tagdecision== 1) theTag->setDecision( FlavourTag::bbar );
    else if(tagdecision==-1) theTag->setDecision( FlavourTag::b );
    else if(tagdecision==0 ) theTag->setDecision( FlavourTag::none );
    theTag->setCategory(ix);

    SmartDataPtr<FlavourTags> tags( eventSvc(), "/Event/Phys/BTagging/Tags" );
    if( tags ) {
      req << MSG::DEBUG << "Inserting in already existing container" <<endreq;
      tags->insert(theTag);
    } else {
      FlavourTags *tags = new FlavourTags;
      tags->insert(theTag);
      sc = eventSvc()->registerObject( "/Event/Phys/BTagging/Tags", tags );
      if( sc.isFailure() )
	req << MSG::ERROR << "Unable to register the tags"<< endreq;
      else req << MSG::DEBUG << "Registered tag into new container."<< endreq;
    }

    SmartDataPtr<Particles> tagCands(eventSvc(),"/Event/Phys/BTagging/Taggers");
    if( tagCands ) {
      req << MSG::DEBUG << "Inserting in already existing container: "
	  << "/Event/Phys/BTagging/Taggers" << endreq;
      if(imuon !=-1) tagCands->insert( *(vtags.begin()+imuon) );
      if(iele  !=-1) tagCands->insert( *(vtags.begin()+iele) );
      if(ikaon !=-1) tagCands->insert( *(vtags.begin()+ikaon) );
      if(ikaonS!=-1) tagCands->insert( *(vtags.begin()+ikaonS) );
      if(ipionS!=-1) tagCands->insert( *(vtags.begin()+ipionS) );

    } else {
      Particles* tagCands = new Particles;
      if(imuon !=-1) tagCands->insert( *(vtags.begin()+imuon) );
      if(iele  !=-1) tagCands->insert( *(vtags.begin()+iele) );
      if(ikaon !=-1) tagCands->insert( *(vtags.begin()+ikaon) );
      if(ikaonS!=-1) tagCands->insert( *(vtags.begin()+ikaonS) );
      if(ipionS!=-1) tagCands->insert( *(vtags.begin()+ipionS) );

      sc= eventSvc()->registerObject("/Event/Phys/BTagging/Taggers", tagCands);
      if( sc.isFailure() )
	req << MSG::ERROR << "Unable to register candidates."<< endreq;
      else
	req << MSG::DEBUG << "Registered a new container: "
	    << "/Event/Phys/BTagging/Taggers" << endreq;
    }
  }

  //check if the decision was right or wrong
  int truetag = 0;
  SmartDataPtr<GenMCLinks> sigL(evtSvc(), GenMCLinkLocation::Default);
  if( sigL ) {
    MCParticle* Bsig = (*(sigL->begin()))->signal();
    if(Bsig) truetag = Bsig->particleID().pid()/abs(Bsig->particleID().pid());
  }
  ///OUTPUT to logfile ----------------------------------------------------
  req << MSG::INFO << "TAGGINGINFO  " << m_Run 
      << std::setw(4) << m_Event 
      << std::setw(4) << tagdecision 
      << std::setw(4) << truetag
      << std::setw(3) << ix 
      << std::setw(4) << m_trig0 
      << " " << m_trig1 
      << " " << m_knrec
      << endreq;

  ///----------------------------------------------------------------------
  //internal counters:
  if(m_trig0) if(m_trig1) {
    nsele++;
    if(tagdecision == truetag) nrt[ix]++; 
    else if(tagdecision ==-truetag) nwt[ix]++;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTagging::finalize() { 

  MsgStream req( msgSvc(), name() );

  // calculate effective efficiency in 12 categories with errors
  double rtt=0;
  double wtt=0;
  double ef_tot=0;
  double effe_tot=0;
  double epsilerr, epsilerrtot=0;

  req << MSG::INFO 
      << "======================================================="<<endreq;
  req << MSG::INFO << "Summary : " <<endreq;
  req << MSG::INFO << "After L0 and L1 triggers, " <<endreq;
  req << MSG::INFO << " Category            EFF.          Etag         Wrong TF"
      << "      r       w       "<<endreq;

  for( int it=1; it < 12+1; it++ ) {

    std::string cats;
    if(it== 1) cats =  "   mu only";
    if(it== 2) cats =  "    e only";
    if(it== 3) cats =  "    k only";
    if(it== 4) cats =  "    mu + k";
    if(it== 5) cats =  "     e + k";
    if(it== 6) cats =  "  vtx only";
    if(it== 7) cats =  "        ks";
    if(it== 8) cats =  "   mu + ks";
    if(it== 9) cats =  "    e + ks";
    if(it==10) cats =  "    k + ks";
    if(it==11) cats =  "   mu+k+ks";
    if(it==12) cats =  "    e+k+ks";

    double rtag = nrt[it];               // right
    double wtag = nwt[it];               // wrong
    double utag = nsele-rtag-wtag;       // untagged

    if((rtag+wtag) == 0) continue; //empty category

    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    rtt      += rtag;
    wtt      += wtag;
    ef_tot   += eftag;
    effe_tot += epsil;

    //errors on efficiency and omega
    double eftag_err=
      sqrt((rtag*utag + 2*rtag*wtag + utag*wtag)/nsele)/nsele;
    double omtag_err=
      sqrt((rtag*wtag*(rtag*rtag + rtag*utag + wtag*(utag + wtag)))/
           ( pow(rtag + wtag,4) *(rtag + utag + wtag)));

    epsilerr = sqrt((pow(rtag - wtag,2)*
                     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
                      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
                    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    epsilerrtot=sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    req.setf(std::ios::fixed);
    req << MSG::INFO<<std::setw(2)<< it << cats
        <<" "<<std::setprecision(2)<<std::setw(8)
	<< epsil*100 << "+-" << epsilerr*100
        <<" "<<std::setw(8)<< eftag*100 << "+-" <<eftag_err*100
        <<" "<<std::setprecision(1)<<std::setw(8)
	<< omtag*100 << "+-" <<omtag_err*100
        <<" "<<std::setw(7)<< (int) rtag
        <<" "<<std::setw(7)<< (int) wtag
        << endreq;
  }

  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err=
    sqrt((rtt*utt + 2*rtt*wtt + utt*wtt)/nsele)/nsele;
  double avw_invert_err=
    sqrt((rtt*wtt*(rtt*rtt + rtt*utt + wtt*(utt + wtt)))/
         ( pow(rtt + wtt,4) *(rtt + utt + wtt)));

  req << MSG::INFO 
      << "---------------------------------------------------------"<<endreq;
  req << MSG::INFO << "Total nr of events =  "<<std::setw(5) << nsele << endreq;
  req << MSG::INFO << "Tagging efficiency =  "
      <<std::setprecision(2)<<std::setw(5)
      << ef_tot*100 << " +/- "<<eftot_err*100<< " %"<< endreq;
  req << MSG::INFO << "Wrong Tag fraction =  "
      <<std::setprecision(2)<<std::setw(5)
      << avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endreq;
  req << MSG::INFO << "EFFECTIVE COMB. TE =  "
      <<std::setprecision(2)<<std::setw(5)
      << effe_tot*100 << " +/- "<<epsilerrtot*100<< " %"<< endreq;
  req << MSG::INFO 
      << "========================================================="<<endreq;

  return StatusCode::SUCCESS; 
}

//============================================================================
bool BTagging::isinTree( Particle* axp, ParticleVector& sons ) {
  MsgStream req( msgSvc(), name() );

  for( ParticleVector::iterator ip = sons.begin(); ip != sons.end(); ip++ ){
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
    double ip, iperr;
    StatusCode sc=geomDispCalculator()->calcImpactPar(*axchild, *RecVert, ip,
						      iperr, ipVec, errMatrix);
    if( sc ) {
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
      ParticleProperty* ap= ppSvc->findByStdHepID(axchild->particleID().pid());
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
  ParticleVector tvector(0);
  for( SmartRefVector<Particle>::iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
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
double BTagging::VertexCharge( ParticleVector vtags, int& nroftracks ) {

  MsgStream req( msgSvc(), name() );
   
  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex Vfit, vtx;
  Particle* p1=0, *p2=0;
  ParticleVector::const_iterator jp, kp;

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
  req <<MSG::DEBUG<< "Seed Particles: -------------------------" <<endreq;
  if( p1 ) if( p2 ) { printInfo(p1); printInfo(p2); }
  req <<MSG::DEBUG<< "MAX probf= " << probf <<endreq;
  req <<MSG::DEBUG<< "---------------------------------------- " <<endreq;

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

  nroftracks = Pfit.size();
  return ach;
}
//=============================================================================
