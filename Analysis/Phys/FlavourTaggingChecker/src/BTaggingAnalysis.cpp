// Include files 
#include "BTaggingAnalysis.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingAnalysis
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BTaggingAnalysis );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTaggingAnalysis::BTaggingAnalysis(const std::string& name,
		   ISvcLocator* pSvcLocator) : DVAlgorithm(name, pSvcLocator), 
					       m_linkLinks(0) , 
					     m_electron(0)//, m_forcedBtool(0)
{
  declareProperty( "SecondaryVertexType", m_SVtype = "SVertexTool" );

  declareProperty( "TagOutputLocation", m_TagLocation = "" );

  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );

  declareProperty( "AssociatorInputData", m_setInputData );

  declareProperty( "IPPU_cut",     m_IPPU_cut    = 3.0 );
  declareProperty( "distphi_cut",  m_distphi_cut = 0.005 );
  declareProperty( "thetaMin_cut", m_thetaMin    = 0.012 );

  m_vtxtool = 0;
}

BTaggingAnalysis::~BTaggingAnalysis() {}; 

//=============================================================================
StatusCode BTaggingAnalysis::initialize() {

  m_linkLinks = new Particle2MCLinker( this, 
				       //Particle2MCMethod::Chi2,
				       Particle2MCMethod::Links, 
				       m_setInputData);
  if( !m_linkLinks ) {
    fatal() << "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

//   m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
//   if( ! m_forcedBtool ) {
//     fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
//     return StatusCode::FAILURE;
//   }

  m_electron = tool<ICaloElectron>( "CaloElectron");
  if(! m_electron) {
    fatal() << "Unable to retrieve ICaloElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }

  m_vtxtool = tool<ISecondaryVertexTool> ( m_SVtype );
  if(! m_vtxtool) {
    fatal() << "Unable to retrieve SecondaryVertexTool"<< endreq;
    return StatusCode::FAILURE;
  }

  // Now book ntuples
  NTuplePtr nt(ntupleSvc(), "FILE1/tagging");
  if ( !nt ) {
    nt = ntupleSvc()->book ("FILE1/tagging",
			     CLID_ColumnWiseTuple, "BTaggingAnalysis");
    if ( !nt ) {
      fatal() << "Could not book Ntuple" << endreq;
      return StatusCode::FAILURE;
    }
    debug()<<"Add elements to ntuple definition "<<endreq;
    // Add elements to ntuple definition:
    nt->addItem ("Run",    m_Run);
    nt->addItem ("Event",  m_Event);
    nt->addItem ("TrueTag",m_TrueTag);
    nt->addItem ("Tag",    m_Tag);
    nt->addItem ("TagCat", m_TagCat);
    nt->addItem ("Taggers",m_Taggers);
    nt->addItem ("evType", m_type);
    nt->addItem ("trig",   m_trigger);

    //reconstructed signal
    nt->addItem ("BSx",   m_BSx);
    nt->addItem ("BSy",   m_BSy);
    nt->addItem ("BSz",   m_BSz);
    nt->addItem ("BSID",   m_BSID);
    nt->addItem ("BSP",    m_BSP);
    nt->addItem ("BSmass", m_BSmass);
    nt->addItem ("BSthe",  m_BSthe);
    nt->addItem ("BSphi",  m_BSphi);
    //mc truth
    nt->addItem ("MCBSP",  m_MCBSP);
    nt->addItem ("MCBSthe",m_MCBSthe);
    nt->addItem ("MCBSphi",m_MCBSphi);
    nt->addItem ("BSosc",  m_BSosc);

    //B opposite
    nt->addItem ("BOID",    m_BOID);
    nt->addItem ("BOP",     m_BOP);
    nt->addItem ("BOthe",   m_BOthe);
    nt->addItem ("BOphi",   m_BOphi);
    nt->addItem ("BOx",     m_BOx);
    nt->addItem ("BOy",     m_BOy);
    nt->addItem ("BOz",     m_BOz);    
    nt->addItem ("BOosc",   m_BOosc);

    //collision
    nt->addItem ("kx",     m_kx);
    nt->addItem ("ky",     m_ky);
    nt->addItem ("kz",     m_kz);
    nt->addItem ("krec",   m_krec);
    nt->addItem ("kType",  m_kType);

    //prim vtx coordinates
    nt->addItem ("RVx",    m_RVx);
    nt->addItem ("RVy",    m_RVy);
    nt->addItem ("RVz",    m_RVz);

    //particles
    nt->addItem ("N",     m_N, 0, 200 ); //limite
    nt->addIndexedItem ("ID",     m_N, m_ID);
    nt->addIndexedItem ("P",      m_N, m_P);
    nt->addIndexedItem ("Pt",     m_N, m_Pt);
    nt->addIndexedItem ("phi",    m_N, m_phi);
    nt->addIndexedItem ("ch",     m_N, m_ch);
    nt->addIndexedItem ("ip",     m_N, m_IP);
    nt->addIndexedItem ("iperr",  m_N, m_IPerr);
    nt->addIndexedItem ("ipPU",   m_N, m_IPPU);
    nt->addIndexedItem ("trtyp",  m_N, m_trtyp);
    nt->addIndexedItem ("lcs",    m_N, m_lcs);
    nt->addIndexedItem ("distPhi",m_N, m_distphi);
    nt->addIndexedItem ("veloch", m_N, m_veloch);
    nt->addIndexedItem ("EOverP", m_N, m_EOverP);
    nt->addIndexedItem ("PIDe",   m_N, m_PIDe);
    nt->addIndexedItem ("PIDm",   m_N, m_PIDm);
    nt->addIndexedItem ("PIDk",   m_N, m_PIDk);
    nt->addIndexedItem ("PIDp",   m_N, m_PIDp);
    nt->addIndexedItem ("PIDfl",  m_N, m_PIDfl);
    nt->addIndexedItem ("RichPID",m_N, m_RichPID);
    nt->addIndexedItem ("vFlag",  m_N, m_vFlag);

    nt->addIndexedItem ("MCID",   m_N, m_MCID);
    nt->addIndexedItem ("MCP",    m_N, m_MCP);
    nt->addIndexedItem ("MCPt",   m_N, m_MCPt);
    nt->addIndexedItem ("MCphi"  ,m_N, m_MCphi);
    nt->addIndexedItem ("MCz"    ,m_N, m_MCz);
    nt->addIndexedItem ("mothID" ,m_N, m_mothID);
    nt->addIndexedItem ("ancID"  ,m_N, m_ancID);
    nt->addIndexedItem ("bFlag"  ,m_N, m_bFlag);
    nt->addIndexedItem ("dFlag"  ,m_N, m_dFlag);
    nt->addIndexedItem ("xFlag"  ,m_N, m_xFlag);
    nt->addIndexedItem ("IPT"    ,m_N, m_IPT);
 
    //Particles FITTED
    nt->addItem ("TVx",      m_TVx);
    nt->addItem ("TVy",      m_TVy);
    nt->addItem ("TVz",      m_TVz);
    nt->addItem ("Tchi",     m_Tchi);
    nt->addItem ("Vch",      m_Vch);

  } else {
    fatal() << "The ntuple was already booked" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingAnalysis::execute() {

  setFilterPassed( false );
   
  NTuplePtr nt(ntupleSvc(), "FILE1/tagging");
  if ( !nt ) {
    err() << "Unable to book ntpl" << endreq;
    return StatusCode::FAILURE;
  }

  L0DUReport* L0decision = get<L0DUReport>( L0DUReportLocation::Default );
  debug() << "L0 decision : " << L0decision->decision() << " fired:";
  for ( int bit=0; 32 > bit; ++bit  ) {
    if ( L0decision->channelDecision( bit ) ) {
      debug() << bit << "=" << L0decision->channelName( bit ) << ",";
    }
  }
  debug() <<endreq;
  m_trigger = L0decision->decision();

// Retrieve trigger/tampering info
//   int trigger=-1;
//   debug()<<"Retrieve TrgDecision from "<<TrgDecisionLocation::Default<<endreq;
//   TrgDecision* trg = 0;
//   HltScore* hlt=0;
//   trg = get<TrgDecision> (TrgDecisionLocation::Default);
//   if( 0 == trg ) return StatusCode::FAILURE;
//   if ( !exist<HltScore>(HltScoreLocation::Default) ){
//     warning() << "No HLT score" << endreq ;
//   } 
//   else hlt = get<HltScore>(HltScoreLocation::Default) ;
//   int hltdec=0; if(hlt) hltdec=hlt->decision();
//   trigger =  hltdec*100 + trg->L1() *10 + trg->L0();
//   debug()<<"trigger is: "<<trigger<<endreq;
//   if(trg) {
//     // Select events on trigger
//     if( m_RequireL0 ) if( !trg->L0() ) return StatusCode::SUCCESS;
//     if( m_RequireL1 ) if( !trg->L1() ) return StatusCode::SUCCESS;
//     if( m_RequireHLT) {
//       if ( ! hlt ) return StatusCode::SUCCESS;
//       if ( !(hlt->decision()) ) return StatusCode::SUCCESS;
//     }
//     if ( 0!=hlt ) trigger = 100* hlt->decision() + 10* trg->L1() + trg->L0();
//     else trigger = 10* trg->L1() + trg->L0();
//   }
//   m_trigger = trigger; 

//   TamperingResults* tampres = get<TamperingResults> 
//     (TamperingResultsLocation::Default);
//   if(tampres) {
//     debug()<< " L0 Results (TIS,TOS,TOB | Status): " 
//            << tampres->L0TIS() << " " 
//            << tampres->L0TOS() << " " 
//            << tampres->L0TOB() << "  |  " 
//            << tampres->L0TamperingStatus() << " " << endreq; 
//     debug()<< " L1 Results (TIS,TOS,TOB | Status): " 
//            << tampres->L1TIS() << " " 
//            << tampres->L1TOS() << " " 
//            << tampres->L1TOB() << "  |  " 
//            << tampres->L1TamperingStatus() << " " << endreq; 
//     L0tamp = tampres->L0TIS()*100 + tampres->L0TOS() *10 + tampres->L0TOB();
//     L1tamp = tampres->L1TIS()*100 + tampres->L1TOS() *10 + tampres->L1TOB();
//   } else {
//     info() << " TamperingResults not found at " 
// 	   << TamperingResultsLocation::Default << endreq;     
//   }

  //----------------------------------------------------------------------
  RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);

  debug()<<">>>>>  Processing Run "<<evt->runNumber()
	 <<"   Event "<<evt->evtNumber()<<"  <<<<<" <<endreq;

  GenHeader* gene = get<GenHeader> (GenHeaderLocation::Default);

  // Retrieve MCParticles
  MCParticles* mcpart = get<MCParticles> (MCParticleLocation::Default);
  debug() << "Nr of MCParticles retrieved="<< mcpart->size()<< endreq;

  // Retrieve MCVertex
  MCVertices* mcvert = get<MCVertices> (MCVertexLocation::Default);
  debug() << "Nr of MCVertices retrieved="<< mcvert->size()<< endreq;

  // Retrieve information about primary pp collisions
//   const SmartRefVector<GenCollision>& mcColls = gene->collisions();
//   if(!mcColls.empty()){
//     err()<< "No pp mcCollision retrieved" << endreq;
//   }
//   debug() << "Nr of Collisions = " << mcColls.size() << endreq;

//   m_kType = 0;
//   for ( unsigned int icoll=0; icoll!=mcColls.size(); icoll++ ) {
//     if( mcColls.at(icoll)->isSignal() ) {
//       m_kType = mcColls.at(icoll)->processType();
//       debug() << "Collision process type: " << m_kType << endreq;
//     }
//   }

  MCVertices::const_iterator ivmc;
  m_kx=0; m_ky=0; m_kz=0;
  for( ivmc=mcvert->begin(); ivmc!=mcvert->end(); ++ivmc) {
    if( (*ivmc)->isPrimary() ) {
      m_kx = (*ivmc)->position().x()/Gaudi::Units::mm;
      m_ky = (*ivmc)->position().y()/Gaudi::Units::mm;
      m_kz = (*ivmc)->position().z()/Gaudi::Units::mm;
      break;
    }
  }
  m_Run  = evt->runNumber();
  m_Event= evt->evtNumber();
  m_type = gene->evType();

  //----------------------------------------------------------------------
  //PhysDeskTop
  const Particle::ConstVector&  parts = desktop()->particles();
  const RecVertex::ConstVector& verts = desktop()->primaryVertices();
  debug() << "  Nr of rec. Vertices: " << verts.size() 
	  << "  Nr of rec. Particles: " << parts.size() <<endreq;
  desktop()->saveDesktop();

  //----------------------------------------------------------------------
  //look for B in selection:
  debug() << "-------- Signal:" << endreq;

  const Particle* AXBS = 0;
  Particle::ConstVector axdaugh(0);
  bool isBd = false;
  bool isBs = false;
  bool isBu = false;
  Gaudi::LorentzVector ptotBS(0.0,0.0,0.0,0.0);

  Particle::ConstVector::const_iterator ip;
  for ( ip = parts.begin(); ip != parts.end(); ip++){
    if((*ip)->particleID().hasBottom()) {
      AXBS = (*ip);
      axdaugh = FindDaughters(AXBS);
      axdaugh.push_back(AXBS);
      if( AXBS->particleID().hasDown() )    isBd = true;
      if( AXBS->particleID().hasStrange() ) isBs = true;
      if( AXBS->particleID().hasUp() )      isBu = true;
      ptotBS = AXBS->momentum();
      m_BSmass = ptotBS.M()/Gaudi::Units::GeV;
      m_BSthe  = ptotBS.theta();
      m_BSphi  = ptotBS.phi();
      m_BSP    = ptotBS.P()/Gaudi::Units::GeV;
      debug() << "Found Selected B!  daugh="<< axdaugh.size()-1 
	      << "  B-Mass=" << m_BSmass << endreq;
      break;
    }  
  }
  if( axdaugh.size() == 0 ) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  //const MCParticle* BS = m_forcedBtool->forcedB();
  const MCParticle* BS = forcedB();

  //look for opposite
  const MCParticle* BO = 0;
  MCParticles::const_iterator imc;
  double maxBP = -1;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( (*imc) != BS ) 
      if((*imc)->particleID().hasBottom()) 
	if( ((*imc)->originVertex()->position().z() 
	     - BS->originVertex()->position().z()) /Gaudi::Units::mm < 1.0 ) 
	  if( maxBP < (*imc)->momentum().P() ) {
	    maxBP = (*imc)->momentum().P();
	    BO = (*imc);
	  }
  }

  ////////////////////////////////////////////////////
  if ( !BS ) {
    err() << "Missing Signal B meson in MC! Skip event."<< endreq;
    return StatusCode::SUCCESS;                      
  }
  if ( !BO ) {
    err() << "Missing Opposite B meson in MC! Skip event."<< endreq;
    return StatusCode::SUCCESS;                      
  }
  ////////////////////////////////////////////////////
  //debug()<<"SIGNAL B:"<<endreq; m_debug -> printTree(BS);
  //debug()<<"OPPOSITE B (TAGGING B):"<<endreq; m_debug -> printTree(BO);

  const RecVertex* RecVert = 0; 
  m_krec = 0;
  RecVertex::ConstVector::const_iterator iv;
  //if the prim vtx is not provided by the user,
  //choose as primary vtx the one with smallest IP wrt B signal
  //this is a guess for the actual PV chosen by the selection.
  double kdmin = 1000000;
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    m_krec++;
    double ip, iperr;
    calcIP(AXBS, *iv, ip, iperr);
    debug() << "Vertex IP="<< ip <<" iperr="<<iperr<<endreq;
    if(iperr) if( fabs(ip/iperr) < kdmin ) {
      kdmin = fabs(ip/iperr);
      RecVert = (*iv);
    }     
  }
  if( !RecVert ) {
    err() <<"No Reconstructed Vertex!! Skip." <<endreq;
    return StatusCode::SUCCESS;
  }    

  //for proper time calculation
  m_BSx = AXBS->endVertex()->position().x()/Gaudi::Units::mm;
  m_BSy = AXBS->endVertex()->position().y()/Gaudi::Units::mm;
  m_BSz = AXBS->endVertex()->position().z()/Gaudi::Units::mm;

  m_BSID    = BS->particleID().pid();
  m_MCBSP   = float(BS->momentum().P()/Gaudi::Units::GeV);
  m_MCBSthe = float(BS->momentum().Theta());
  m_MCBSphi = float(BS->momentum().Phi());
  if( BS->hasOscillated() ) m_BSosc = 1; else m_BSosc = 0;
  if( BO->hasOscillated() ) m_BOosc = 1; else m_BOosc = 0;
  debug()<< "BSignal: " <<BS->particleID().pid()<<" osc? "<<m_BSosc<<endreq;
  debug()<< "BOppost: " <<BO->particleID().pid()<<" osc? "<<m_BOosc<<endreq;

  //b opposite true
  m_BOID = BO->particleID().pid();
  m_BOP  = BO->momentum().P()/Gaudi::Units::GeV;
  m_BOthe= BO->momentum().Theta();
  m_BOphi= BO->momentum().Phi();

  const SmartRefVector<MCVertex>& BOvert = BO->endVertices() ;
  if( BOvert.size() == 0 ) return StatusCode::SUCCESS;
  m_BOx = (BOvert.at(0))->position().x()/Gaudi::Units::mm;
  m_BOy = (BOvert.at(0))->position().y()/Gaudi::Units::mm;
  m_BOz = (BOvert.at(0))->position().z()/Gaudi::Units::mm;

  //build VertexVector of pileup ----------------------------
  RecVertex::ConstVector PileUpVtx(0);
  for( iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug()<<"Pileup Vtx z="<< (*iv)->position().z()/Gaudi::Units::mm <<endreq;
  }
  m_RVx = RecVert->position().x()/Gaudi::Units::mm ;
  m_RVy = RecVert->position().y()/Gaudi::Units::mm ;
  m_RVz = RecVert->position().z()/Gaudi::Units::mm ;

  //loop over Particles, preselect tags ///////////////////////////////////////
  double distphi;
  Particle::ConstVector vtags(0);
  for ( ip = parts.begin(); ip != parts.end(); ip++){

    if( (*ip)->p()/Gaudi::Units::GeV < 2.0 ) continue;  
    if( (*ip)->momentum().theta()  < m_thetaMin ) continue;   
    if( (*ip)->charge() == 0 ) continue;                
    if( !(*ip)->proto() )      continue;
    if( !(*ip)->proto()->track() ) continue;
    if( (*ip)->proto()->track()->type() < 3 ) continue; 
    if( (*ip)->proto()->track()->type() > 4 ) continue; 
    if( (*ip)->p()  > 200000 ) continue;
    if( (*ip)->pt() >  10000 ) continue;
    if( isinTree(*ip, axdaugh, distphi) ) continue ; 
    if( distphi < m_distphi_cut ) continue;

    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    calcIP( *ip, PileUpVtx, ippu, ippuerr );
    //eliminate from vtags all parts coming from a pileup vtx
    if(ippuerr) if( ippu/ippuerr < m_IPPU_cut ) continue; //preselection

    ///////////////////////////////////////
    vtags.push_back(*ip);               // Fill container of candidates
    /////////////////////////////////////

  }

  ///------------------------------------------------------ Vertex charge info
  //look for a secondary Vtx due to opposite B
  Vertex Vfit;
  std::vector<Vertex> vv;
  if(m_vtxtool) {
    vv = m_vtxtool->buildVertex(*RecVert, vtags);
    if(!vv.empty()) Vfit = vv.at(0); //take first vertex built
  } else debug() << "No secondary vtx available."<<endreq;

  double Vch = 0;  
  Particle::ConstVector Pfit = Vfit.outgoingParticlesVector();
  if( Pfit.size() ) {
    Particle::ConstVector::iterator kp;
    for(kp = Pfit.begin(); kp != Pfit.end(); kp++) {
      Vch += (*kp)->charge();
      debug() << m_SVtype <<" particle p=" 
	      << (*kp)->p()/Gaudi::Units::GeV << endreq;
    }
    debug() << "  Vertex charge: " << Vch << endreq;
  }
  if(!vv.empty()) { 
    m_Vch = (int) Vch;
    m_TVx = Vfit.position().x()/Gaudi::Units::mm;
    m_TVy = Vfit.position().y()/Gaudi::Units::mm;
    m_TVz = Vfit.position().z()/Gaudi::Units::mm;
    m_Tchi= Vfit.chi2()/Vfit.nDoF();
  } else { m_Vch=0; m_TVx=0; m_TVy=0; m_TVz=0; m_Tchi=0; }

  ///------------------------------------------------------- Fill Tagger info
  m_N = 0;
  debug() << "-------- Tagging Candidates: " << vtags.size() <<endreq;
  for( ip = vtags.begin(); ip != vtags.end(); ip++ ) {

    const Particle* axp = (*ip);
    const ProtoParticle* proto = axp->proto();
    const Track* track = proto->track();

    //calculate some useful vars
    long   ID  = axp->particleID().pid(); 
    double P   = axp->p()/Gaudi::Units::GeV;
    double Pt  = axp->pt()/Gaudi::Units::GeV;
    double deta= fabs(log(tan(m_BSthe/2.)/tan(asin(Pt/P)/2.)));
    double dphi= fabs(axp->momentum().phi() - m_BSphi); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    double dQ  = (ptotBS + axp->momentum()).M()/Gaudi::Units::GeV - m_BSmass;
    double lcs = track->chi2PerDoF();
    long trtyp = track->type();
    if (track->checkHistory(Track::TrackMatching) == true) trtyp=7;

    isinTree( axp, axdaugh, distphi );

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    if(!(axp->particleID().hasBottom())) calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr) continue;                                      //preselection cut
    double IPsig = fabs(IP/IPerr);
    //calculate signed IP wrt SecondaryVertex
    double IPT = -1000, ip, iperr;
    if(Pfit.size()){
      calcIP(axp, &Vfit, ip, iperr);
      if(iperr) IPT = ip/iperr;
    }
    //calculate min IP wrt all pileup vtxs 
    double IPPU = 10000;
    calcIP( axp, PileUpVtx, ip, iperr );
    if(iperr) IPPU=ip/iperr;

    //Fill NTP info -------------------------------------------------------
    m_trtyp[m_N] = trtyp;
    m_ID[m_N]    = ID;
    m_P[m_N]     = P;
    m_Pt[m_N]    = Pt;
    m_phi[m_N]   = axp->momentum().phi();
    m_ch[m_N]    = (int) axp->charge();
    m_IP[m_N]    = IP;
    m_IPerr[m_N] = IPerr;
    m_IPT[m_N]   = IPT;
    m_IPPU[m_N]  = IPPU;
    m_lcs[m_N]   = lcs;
    m_distphi[m_N]= distphi;

// electrons
    m_PIDe[m_N] = proto->info( ProtoParticle::CombDLLe, -1000.0 );

    double eOverP  = -999.9;
    if(m_electron->set(axp)){ /// CaloElectron tool
      eOverP  = m_electron->eOverP();
    }
    m_EOverP[m_N] = eOverP;
    m_veloch[m_N]= proto->info( ProtoParticle::VeloCharge, 0.0 );

//     m_Emeas[m_N] = -1.0;
//     m_elChi[m_N] = proto->info(LHCb::ProtoParticle::CaloEcalChi2, 10000.);
//     const bool inEcalACC = proto->info(ProtoParticle::InAccEcal, false);
//     const bool inHcalACC = proto->info(ProtoParticle::InAccHcal, false);
//     if( inEcalACC ){
//       const SmartRefVector<CaloHypo> hypos = proto->calo();// get CaloHypos
//       if( hypos.size() !=0) {      
// 	// When available the 'electron' hypo is the first one
// 	// This implies inEcal = true (the possible second one is BremStrahlung)
// 	SmartRefVector<CaloHypo>::const_iterator ihypo =  hypos.begin();
// 	const CaloHypo* hypo = *ihypo;
// 	if(hypo) if( hypo->hypothesis() == CaloHypo::EmCharged) 
// 	  m_Emeas[m_N] = hypo->position()->e();
// 	debug()<<" Calo Emeas="<<m_Emeas[m_N]<<" elChi="<<m_elChi[m_N]<<endreq;
//       }
//     }
  
// muons
    m_PIDm[m_N] = proto->info( ProtoParticle::CombDLLmu, -1000.0 );
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );

// kaons
    m_PIDk[m_N] = proto->info( ProtoParticle::CombDLLk, -1000.0 );

// protons
    m_PIDp[m_N] = proto->info( ProtoParticle::CombDLLp, -1000.0 );

// global flags 
    m_PIDfl[m_N]= 0;
    if( proto->muonPID() ) 
      if(proto->muonPID()->IsMuon()) m_PIDfl[m_N] += 100000;
    if( muonNSH>0 )                  m_PIDfl[m_N] +=  10000;
//     if( inHcalACC )                  m_PIDfl[m_N] +=   1000;
//     if( inEcalACC )                  m_PIDfl[m_N] +=    100;
    if( proto->info(ProtoParticle::MuonPIDStatus, 0) ) m_PIDfl[m_N] += 10;
    if( proto->info(ProtoParticle::RichPIDStatus, 0) ) m_PIDfl[m_N] +=  1;

    m_RichPID[m_N] = 0;
    const RichPID * rpid = proto->richPID ();
    if ( rpid ) { 
      if ( rpid->kaonHypoAboveThres()) m_RichPID[m_N] += 1000;
      if ( rpid->usedRich2Gas() )      m_RichPID[m_N] +=  100;
      if ( rpid->usedRich1Gas() )      m_RichPID[m_N] +=   10;
      if ( rpid->usedAerogel() )       m_RichPID[m_N] +=    1;
    }

// secondary vertex flag
    m_vFlag[m_N] = 0;
    Particle::ConstVector::iterator kp;
    for(kp = Pfit.begin(); kp != Pfit.end(); kp++) {
      if( axp == *kp ) {m_vFlag[m_N] = 1; break;}
    }
  
    //-------------------------------------------------------
    debug() << " --- trtyp="<<trtyp<<" ID="<<ID<<" P="<<P<<" Pt="<<Pt <<endreq;
    debug() << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
    debug() << " IPsig="<<IPsig << " IPPU="<<IPPU << " IPT="<<IPT<<endreq;
    debug() << " sigPhi="<<m_distphi[m_N]<< " lcs " << lcs << endreq;
    debug() << " DLLe,m,k "<<m_PIDe[m_N]<<" "<<m_PIDm[m_N]<<" "<<m_PIDk[m_N]
	    << " mNSH="<<muonNSH<< " vFlag="<<m_vFlag[m_N]<<endreq;

    //store MC info 
    m_MCID[m_N]  = 0;
    m_MCP[m_N]   = 0.0;
    m_MCPt[m_N]  = 0.0;
    m_MCphi[m_N] = -999.0;
    m_MCz[m_N]   = -999.0;
    m_mothID[m_N]= 0;
    m_ancID[m_N] = 0;
    m_bFlag[m_N] = 0;
    m_dFlag[m_N] = 0;
    m_xFlag[m_N] = 0;

    const MCParticle* mcp = m_linkLinks->first( axp );
    if( mcp ) {
      m_MCID[m_N]   = mcp->particleID().pid();
      m_MCP[m_N]    = mcp->momentum().P()/Gaudi::Units::GeV;
      m_MCPt[m_N]   = mcp->pt()/Gaudi::Units::GeV;
      m_MCphi[m_N]  = mcp->momentum().phi();

      const MCParticle* mother = mcp->mother();
      if(mother) {
	m_mothID[m_N] = mother->particleID().pid();
	const SmartRefVector<MCVertex>& motherVtx = mother->endVertices();
	if(motherVtx.size()) 
	  m_MCz[m_N] = (motherVtx.at(0))->position().z()/Gaudi::Units::mm;
      }

      const MCParticle* ancestor = originof(mcp) ;
      m_ancID[m_N] = ancestor->particleID().pid();
      if( ancestor->particleID().hasBottom() ) {
	m_bFlag[m_N] = 1;  
	if(ancestor == BS ) {
	  m_bFlag[m_N] = -1;
	  debug() <<" Warning: tag from signal! ID=" << mcp->particleID().pid() 
		  <<" P="<< mcp->momentum().P() << endreq;
	}
	if( originD(mcp) ) m_dFlag[m_N] = 1;
      }

      m_xFlag[m_N] = comes_from_excitedB(BS, mcp);
      if(m_xFlag[m_N]) debug() <<" comes_from_excitedB=" 
			       << m_xFlag[m_N] << endreq;

    }//if( mcp )

    //---------------
    if(m_N<199) m_N++;
  }

  //-------------------- OFFICIAL TAG of the Event 
  FlavourTag* theTag = new FlavourTag;
  StatusCode sc = flavourTagging()->tag( *theTag, AXBS );
  if (!sc) {
    err() <<"Tagging Tool returned error."<< endreq;
    delete theTag;
    return StatusCode::SUCCESS;
  }
  m_Tag     = theTag->decision();
  m_TagCat  = theTag->category();
  m_TrueTag = BS->particleID().pid()>0 ? 1 : -1;
  int taggers_code = 0;
  std::vector<Tagger> taggers = theTag->taggers();
  for(size_t i=0; i<taggers.size(); ++i) {
    int tdec = taggers[i].decision();
    if(tdec) switch ( taggers[i].type() ) {
    case Tagger::OS_Muon     : taggers_code += 10000 *(tdec+2); break;
    case Tagger::OS_Electron : taggers_code +=  1000 *(tdec+2); break;
    case Tagger::OS_Kaon     : taggers_code +=   100 *(tdec+2); break;
    case Tagger::SS_Kaon     : taggers_code +=    10 *(tdec+2); break;
    case Tagger::SS_Pion     : taggers_code +=    10 *(tdec+2); break;
    case Tagger::VtxCharge   : taggers_code +=     1 *(tdec+2); break;
    }
    if(taggers[i].decision()) debug()<< "Tagger "<<taggers[i].type()
	   << " decision = " << tdec << endreq;
  }
  m_Taggers = taggers_code;

  ///----------------------------------------------------------------------
  if( !(nt->write()) ) err() << "Cannot fill tagging Ntuple" << endreq;
  else if(m_Tag) info() << "Wrote tagged event to Ntuple." << endreq;
  ///----------------------------------------------------------------------

  delete theTag;

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTaggingAnalysis::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTaggingAnalysis::isinTree( const Particle* axp, 
				 Particle::ConstVector& sons, 
				 double& dist_phi ) {
  double p_axp  = axp->p();
  double pt_axp = axp->pt();
  double phi_axp= axp->momentum().phi();
  dist_phi=1000.;

  for( Particle::ConstVector::iterator ip = sons.begin(); 
       ip != sons.end(); ip++ ){

    double dphi = fabs(phi_axp-(*ip)->momentum().phi()); 
    if(dphi>3.1416) dphi=6.283-dphi;
    dist_phi= std::min(dist_phi, dphi);

    if( (    fabs(p_axp -(*ip)->p()) < 0.1 
	  && fabs(pt_axp-(*ip)->pt())< 0.01 
	  && fabs(phi_axp-(*ip)->momentum().phi())< 0.1 )
	|| axp->proto()==(*ip)->proto() ) {
      debug() << "excluding signal part: " << axp->particleID().pid() 
	      << " with p="<<p_axp/Gaudi::Units::GeV 
	      << " pt="<<pt_axp/Gaudi::Units::GeV 
	      << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto()<<endreq;

      const MCParticle* mc = m_linkLinks->first(axp);
      if(mc) {
	m_debug->printAncestor(mc);
	debug() << "   assoc to "<<mc->particleID().pid()
		<<" "<< mc->p()/Gaudi::Units::GeV
		<<" "<< mc->pt()/Gaudi::Units::GeV
		<<" "<< mc->momentum().phi()<<endreq;
      } else debug()<<endreq; 

      return true;
    }
  }
  return false;
}

//=============================================================================
StatusCode BTaggingAnalysis::calcIP( const Particle* axp, const Vertex* RecVert, 
				     double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc = 
    m_Geom->calcImpactPar(*axp, *RecVert, ip,
					iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
  return sc;
}
//==========================================================================
StatusCode BTaggingAnalysis::calcIP( const Particle* axp, 
                                 const RecVertex* RecVert, 
                                 double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc = 
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
  return sc;
}
//=========================================================================
StatusCode BTaggingAnalysis::calcIP( const Particle* axp, 
                                 const RecVertex::ConstVector& PileUpVtx,
                                 double& ip, double& ipe) {
  double ipmin = 100000.0;
  double ipminerr = 0.0;
  StatusCode sc, lastsc=SUCCESS;

  RecVertex::ConstVector::const_iterator iv;
  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
    double ipx, ipex;
    sc = m_Geom->calcImpactPar(*axp, **iv, ipx, ipex);
    if( sc ) if( ipx < ipmin ) {
      ipmin = ipx;
      ipminerr = ipex;
    } else lastsc = sc;
  }
  ip  = ipmin;
  ipe = ipminerr;
  return lastsc;
}

//=============================================================================
//return a vector containing all daughters of signal 
Particle::ConstVector BTaggingAnalysis::FindDaughters( const Particle* axp ) {

   Particle::ConstVector apv(0), apv2, aplist(0);
   apv.push_back(axp);

   do {
     apv2.clear();
     for( Particle::ConstVector::const_iterator ip=apv.begin(); 
	  ip!=apv.end(); ip++ ) {
       if( (*ip)->endVertex() ) {
	 Particle::ConstVector tmp= (*ip)->endVertex()->outgoingParticlesVector();
	 for( Particle::ConstVector::const_iterator itmp=tmp.begin(); 
	      itmp!=tmp.end(); itmp++) {
	   apv2.push_back(*itmp);
	   aplist.push_back(*itmp);
     debug() << " ID daughter= "<< (*itmp)->particleID().pid() 
             << " P=" << (*itmp)->p()/Gaudi::Units::GeV  
             << " Pt=" << (*itmp)->pt()/Gaudi::Units::GeV  << endreq;
	 }
       }
     }
     apv = apv2;
   } while ( apv2.size() );
   	    
   return aplist;
}
//==========================================================================
const MCParticle* BTaggingAnalysis::originof( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product; 
  else return originof( mother );
}
//============================================================================
const MCParticle* BTaggingAnalysis::originD( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( product->particleID().hasCharm() ) return product;
  if ( !mother ) return 0;
  else return originD( mother );
}
//=============================================================================
MCParticle::ConstVector 
BTaggingAnalysis::prodsBstarstar( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstar2=0;
  MCParticle::ConstVector ExcitedProdsBstar2(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator  
	    iances = HEPB0->production_vertex()->
	    particles_begin(HepMC::ancestors);
	  iances != HEPB0->production_vertex()->
	    particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 535 || genid == 525 || genid == 515 || 
	  genid == 10511 || genid == 10521 || genid == 10531 || 
	  genid == 10513 || genid == 10523 || genid == 10533 || 
	  genid == 20513 || genid == 20523 || genid == 20533 
	  ){
	//B** found
	Bstar2 = (*iances);
	break;
      }
    }
  }
  // return all descendants of B**
  if ( Bstar2 ) {
    for ( HepMC::GenVertex::particle_iterator idescend 
	    = Bstar2->end_vertex()->
	    particles_begin(HepMC::descendants);
	  idescend != Bstar2->end_vertex()->
	    particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) {
	ExcitedProdsBstar2.push_back(mcdescend);
// 	debug() <<"Bstarstar descendant "<<(*idescend)->pdg_id()
// 		<<" correps MCPart P="
// 		<<mcdescend->momentum().P()/Gaudi::Units::GeV 
// 		<<endreq;
      }
    }
  }
  
  return ExcitedProdsBstar2;
}
//=============================================================================
MCParticle::ConstVector 
BTaggingAnalysis::prodsBstar( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstar1=0;
  MCParticle::ConstVector ExcitedProdsBstar1(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator iances 
	    = HEPB0->production_vertex()->
	    particles_begin(HepMC::ancestors);
	  iances != HEPB0->production_vertex()->
	    particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 533 || genid == 523 || genid == 513){
	//B* found
	Bstar1 = (*iances);
	break;
      }
    }
  }
  if ( Bstar1 ) {
    for ( HepMC::GenVertex::particle_iterator idescend 
	    = Bstar1->end_vertex()->
	    particles_begin(HepMC::descendants);
	  idescend != Bstar1->end_vertex()->
	    particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) ExcitedProdsBstar1.push_back(mcdescend);
//       if(mcdescend) debug() <<"Bstar descendant "<<(*idescend)->pdg_id()
// 			    <<" correps MCPart P="
// 			    <<mcdescend->momentum().P()/Gaudi::Units::GeV
// 			    <<endreq;
    }
  }

  return ExcitedProdsBstar1;
}
//=============================================================================
MCParticle::ConstVector 
BTaggingAnalysis::prodsBstring( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstring=0;
  MCParticle::ConstVector BstringProds(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator iances 
	    = HEPB0->production_vertex()->
	    particles_begin(HepMC::ancestors);
	  iances != HEPB0->production_vertex()->
	    particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 91 || genid == 92 ){
        Bstring = (*iances);
	break;
      }
    }
  }
  // return all descendants of B string
  if ( Bstring ) {
    for ( HepMC::GenVertex::particle_iterator idescend
            = Bstring->end_vertex()->
            particles_begin(HepMC::descendants);
          idescend != Bstring->end_vertex()->
            particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) BstringProds.push_back(mcdescend);
    }
  }
  
  return BstringProds;
}
//=============================================================================
HepMC::GenParticle* BTaggingAnalysis::HEPassociated(const MCParticle* mcp) {

  LHCb::HepMCEvents* hepVect =
    get<LHCb::HepMCEvents>( HepMCEventLocation::Default );

  int mid = mcp->particleID().pid();
  double mothmom = mcp->momentum().P();
  double moththeta = mcp->momentum().Theta();

  for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator
            p  = (*q)->pGenEvt()->particles_begin();
          p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( mid == (*p)->pdg_id() ) {
	if( fabs(mothmom - (*p)->momentum().vect().mag()< 1.0) ){
	  if( fabs(moththeta -(*p)->momentum().vect().theta())< 0.0001 ){
	    return (*p);
	  }
	}
      }
    }
  }
  return 0;
}
//============================================================================
MCParticle* BTaggingAnalysis::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> (MCParticleLocation::Default);

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
        if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
          return (*imc);
        }
      }
    }
  }
  return 0;
}
//=============================================================================
int BTaggingAnalysis::comes_from_excitedB(const MCParticle* BS,
					  const MCParticle* mcp ) {
  MCParticle::ConstVector::iterator iexc;
  int origin=0;

  MCParticle::ConstVector BstringProds= prodsBstring(BS);
  for( iexc=BstringProds.begin();
       iexc!=BstringProds.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=3; break; }
  }
  MCParticle::ConstVector ExcitedProdsBstar1= prodsBstar(BS);
  for( iexc=ExcitedProdsBstar1.begin(); 
       iexc!=ExcitedProdsBstar1.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=1; break; }
  }
  MCParticle::ConstVector ExcitedProdsBstar2= prodsBstarstar(BS);
  for( iexc=ExcitedProdsBstar2.begin(); 
       iexc!=ExcitedProdsBstar2.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=2; break; }
  }

  return origin;
}
//=============================================================================
const MCParticle* BTaggingAnalysis::forcedB() {

  //check what is the B forced to decay
  const MCParticle *BS = 0;
  HepMCEvents* hepVect = get<HepMCEvents>( HepMCEventLocation::Default );

  for( std::vector<HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	  p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      BS = associatedofHEP(*p);
      if(BS) break; 
    }
  }
  return BS;

}
