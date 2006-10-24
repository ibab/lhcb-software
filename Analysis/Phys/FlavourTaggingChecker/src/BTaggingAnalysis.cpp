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
		   ISvcLocator* pSvcLocator) : DVAlgorithm(name, pSvcLocator)
  , m_setDecay(false)
  , m_evtTypeSvc(NULL)
  , m_mcFinder(NULL)
  , m_linkLinks(0)
{
  declareProperty( "SecondaryVertexType", m_SVtype = "SVertexTool" );
  declareProperty( "TrVeloChargeName",m_veloChargeName = "TrVeloCharge" );
  declareProperty( "ProbMin", m_ProbMin = 0.52);
  declareProperty( "VchOmega", m_VchOmega = 0.40);

  declareProperty( "TagOutputLocation", m_TagLocation = "" );

  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );

  declareProperty( "AssociatorInputData", m_setInputData );

  m_vtxtool = 0;
}

BTaggingAnalysis::~BTaggingAnalysis() {}; 
//=============================================================================
//return a vector containing all daughters of signal 
Particle::ConstVector BTaggingAnalysis::FindDaughters( const Particle* axp ) {

   Particle::ConstVector apv(0), apv2, aplist(0);
   apv.push_back(axp);

   do {
     apv2.clear();
     for( Particle::ConstVector::const_iterator ip=apv.begin(); ip!=apv.end(); ip++ ) {
       if( (*ip)->endVertex() ) {
	 Particle::ConstVector tmp = (*ip)->endVertex()->outgoingParticlesVector();
	 for( Particle::ConstVector::const_iterator itmp=tmp.begin(); 
	      itmp!=tmp.end(); itmp++) {
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
StatusCode BTaggingAnalysis::initialize() {

  m_linkLinks = new Particle2MCLinker( this, Particle2MCMethod::Links , m_setInputData);
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
//   m_veloCharge= tool<ITrVeloCharge> (m_veloChargeName, this);
//   if ( ! m_veloCharge ) {   
//     fatal() << " Unable to retrieve " << m_veloChargeName << endreq;
//     return StatusCode::FAILURE;
//   }
  m_vtxtool = tool<ISecondaryVertexTool> (m_SVtype);
  if(! m_vtxtool) {
    fatal() << "Unable to retrieve SecondaryVertexTool"<< endreq;
    return StatusCode::FAILURE;
  }
//   m_visTool = tool<IVisPrimVertTool> ("VisPrimVertTool", this );
//   if( !m_visTool ) {
//      fatal() << "Unable to retrieve VisPrimVertTool" << endreq;
//      return StatusCode::FAILURE;
//   }

//   m_muonIDdll = tool<IMuonIDDLLTool>("MuonIDDLLTool");
//   if(! m_muonIDdll) {
//     fatal() << "Unable to retrieve MuonIDDLLTool"<< endreq;
//     return StatusCode::FAILURE;
//   }
//   m_muonIDnsh = tool<IMuonIDNSharedHitsTool>("MuonIDNSharedHitsTool");
//   if(! m_muonIDnsh ) {
//     fatal() << "Unable to retrieve MuonIDNSharedHitsTool"<< endreq;
//     return StatusCode::FAILURE;
//   }

  // Now book ntuples
  NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/1");
  if ( !nt ) {
    nt = ntupleSvc()->book ("/NTUPLES/FILE1", 1,
			     CLID_ColumnWiseTuple, "BTaggingAnalysis");
    if ( !nt ) {
      fatal() << "Could not book Ntuple" << endreq;
      return StatusCode::FAILURE;
    }
    // Add elements to ntuple definition:
    nt->addItem ("Run",    m_Run);
    nt->addItem ("Event",  m_Event);
    nt->addItem ("TrueTag",m_TrueTag);
    nt->addItem ("Tag",    m_Tag);
    nt->addItem ("TagCat", m_TagCat);
    nt->addItem ("evType", m_type);
    nt->addItem ("trig",   m_trigger);
    nt->addItem ("L0tamp", m_L0tamp);
    nt->addItem ("L1tamp", m_L1tamp);

    //reconstructed signal
    nt->addItem ("BSx",   m_BSx);
    nt->addItem ("BSy",   m_BSy);
    nt->addItem ("BSz",   m_BSz);
    nt->addItem ("flighterr", m_flighterr);
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
    nt->addItem ("K",      m_K);
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
    nt->addItem ("N",     m_N, 0, 100 ); //limite
    nt->addItem ("ID",     m_N, m_ID);
    nt->addItem ("P",      m_N, m_P);
    nt->addItem ("Pt",     m_N, m_Pt);
    nt->addItem ("phi",    m_N, m_phi);
    nt->addItem ("ch",     m_N, m_ch);
    nt->addItem ("ip",     m_N, m_IP);
    nt->addItem ("iperr",  m_N, m_IPerr);
    nt->addItem ("ipPU",   m_N, m_IPPU);
    nt->addItem ("trtyp",  m_N, m_trtyp);
    nt->addItem ("lcs",    m_N, m_lcs);
    nt->addItem ("dQ",     m_N, m_dQ);
    nt->addItem ("veloch", m_N, m_veloch);
    nt->addItem ("Emeas",  m_N, m_Emeas);
    nt->addItem ("PIDe",   m_N, m_PIDe);
    nt->addItem ("PIDm",   m_N, m_PIDm);
    nt->addItem ("PIDk",   m_N, m_PIDk);
    nt->addItem ("PIDp",   m_N, m_PIDp);
    nt->addItem ("PIDfl",  m_N, m_PIDfl);
    nt->addItem ("vFlag",  m_N, m_vFlag);

    nt->addItem ("MCID",   m_N, m_MCID);
    nt->addItem ("MCP",    m_N, m_MCP);
    nt->addItem ("MCPt",   m_N, m_MCPt);
    nt->addItem ("MCphi"  ,m_N, m_MCphi);
    nt->addItem ("MCz"    ,m_N, m_MCz);
    nt->addItem ("mothID" ,m_N, m_mothID);
    nt->addItem ("ancID"  ,m_N, m_ancID);
    nt->addItem ("bFlag"  ,m_N, m_bFlag);
    nt->addItem ("dFlag"  ,m_N, m_dFlag);
    nt->addItem ("IPT"    ,m_N, m_IPT);
 
    //Particles FITTED
    nt->addItem ("M",        m_M); //limite
    nt->addItem ("TVx",      m_TVx);
    nt->addItem ("TVy",      m_TVy);
    nt->addItem ("TVz",      m_TVz);
    nt->addItem ("Tchi",     m_Tchi);
    nt->addItem ("Vch",      m_Vch);

  } else {
    fatal() << "    The ntuple was already booked" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingAnalysis::execute() {

  setFilterPassed( false );
   
  NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/1");
  if ( !nt ) {
    err() << "    unable to book ntpl" << endreq;
    return StatusCode::FAILURE;
  }

//   // Retrieve informations about event
//   ProcessHeader* evt = get<ProcessHeader> (ProcessHeaderLocation::Default);
//   if ( !evt ) {   
//     err() << "Unable to Retrieve Event" << endreq;
//     return StatusCode::SUCCESS;
//   }

  // Retrieve trigger/tampering info
//   double L0tamp = 100;
//   double L1tamp = 100;
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

//   m_L0tamp = (int) L0tamp;
//   m_L1tamp = (int) L1tamp;

  // Retrieve informations about
  GenHeader* gene = get<GenHeader> (GenHeaderLocation::Default);
  if ( !gene ) {
    err() << "    unable to retrieve GenHeader" << endreq;
    return StatusCode::FAILURE;
  }

  //----------------------------------------------------------------------
  // Retrieve MCParticles
  MCParticles* mcpart = get<MCParticles> (MCParticleLocation::Default);
  if ( ! mcpart ) { 
    err() << "No MCParticles retrieved" << endreq;
    return StatusCode::FAILURE;
  }
  debug() << "Nr of MCParticles retrieved="<< mcpart->size()<< endreq;

  //----------------------------------------------------------------------
  // Retrieve MCVertex
  MCVertices* mcvert = get<MCVertices> (MCVertexLocation::Default);
  if(!mcvert){
    err()<< " No mcvertex retrieved" << endreq;
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------
  // Retrieve information about primary pp collisions
//   const SmartRefVector<GenCollision>& mcCollisions = gene->collisions();
//   if(!mcCollisions.empty()){
//     fatal()<< "No pp mcCollision retrieved" << endreq;
//     return StatusCode::FAILURE;
//   }
//   debug() << "-------- VERTEX:" << endreq;
//   debug() << "Nr di Collisioni= " << mcCollisions.size() << endreq;
//   if( mcCollisions.size() < 1 ) return StatusCode::SUCCESS;

//   m_K  = 0;
//   m_kType = 0;
//   for ( __gnu_cxx::__normal_iterator<const SmartRef<LHCb::GenCollision>*, 
// 	  std::vector<SmartRef<LHCb::GenCollision>, 
// 	  std::allocator<SmartRef<LHCb::GenCollision> > > > colls = mcCollisions.begin(); 
//        colls != mcCollisions.end(); colls++) {
//     //if( m_visTool->isVisible(*colls) ) {
//       m_K++;
//       if( (*colls)->isSignal() ) {
// //   	m_kx = (*colls)->primVtxPosition().x()/Gaudi::Units::mm;//xxx
// //   	m_ky = (*colls)->primVtxPosition().y()/Gaudi::Units::mm;
// //   	m_kz = (*colls)->primVtxPosition().z()/Gaudi::Units::mm;
//   	m_kType = (*colls)->processType();
//   	debug() << "Collision process type: " << m_kType 
// 	  //<< " at z=" << (*colls)->primVtxPosition().z()/Gaudi::Units::mm 
// 		<< endreq;
//       }
//       // }
//   }

  // Counter of events processed
  //  debug() << ">>>>>  Tagging Event Nr " << evt->evtNum()
  //    << " Run " << evt->runNum() << "  <<<<<" << endreq;
//   m_Run  = evt->runNum();
//   m_Event= evt->evtNum();
//   m_type = gene->evType();

  //----------------------------------------------------------------------
  //PhysDeskTop
  const Particle::ConstVector&  parts = desktop()->particles();
  const RecVertex::ConstVector& verts = desktop()->primaryVertices();
  debug() << "  Nr Vertices: "  << verts.size() 
	  << "  Nr Particles: " << parts.size() <<endreq;
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
  double BSmass=0, BSthe=0, BSphi=0, BSP=0;

  Particle::ConstVector::const_iterator ipart,jp;
  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if((*ipart)->particleID().hasBottom()) {
      AXBS = (*ipart);
      axdaugh = FindDaughters(AXBS);
      axdaugh.push_back(AXBS);
      if( AXBS->particleID().hasDown() )    isBd = true;
      if( AXBS->particleID().hasStrange() ) isBs = true;
      if( AXBS->particleID().hasUp() )      isBu = true;
      debug() << "Found Selected B!  daugh=" 
	      << axdaugh.size()-1 <<endreq;
      ptotBS = AXBS->momentum();
      BSmass = ptotBS.M()/Gaudi::Units::GeV;
      BSthe  = ptotBS.theta();
      BSphi  = ptotBS.phi();
      BSP    = ptotBS.P()/Gaudi::Units::GeV;

      m_BSmass = BSmass;
      m_BSthe  = BSthe;
      m_BSphi  = BSphi;
      m_BSP    = BSP;
      
      debug() << "Signal B-Mass=" << BSmass << endreq;
      break;
    }  
  }
  if( axdaugh.size() == 0 ) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  //choose as primary vtx the one with smallest IP wrt B signal
  const RecVertex* RecVert = 0; 
  int krec = 0;
  double kdmin = 1000000;
  RecVertex::ConstVector::const_iterator iv;
//   for(iv=verts.begin(); iv!=verts.end(); iv++){
//     krec++;
//     double ip, iperr;
//     calcIP(AXBS, *iv, ip, iperr);//does not work for cheated AXBS
//     debug() <<"Vertex IP="<<ip<<" IPerr="<<iperr<<endreq;
//     if(iperr) if( fabs(ip/iperr) < kdmin ) {
//       kdmin = fabs(ip/iperr);
//       RecVert = (*iv);
//     }
//   }
  if(verts.size()==1) RecVert = verts.at(0); else return StatusCode::SUCCESS;
  if( !RecVert ) {
    err() <<"No Reconstructed Vertex!! Skip." <<endreq;
    return StatusCode::SUCCESS;
  }    
  m_krec = krec;

  //proper time calculation
  m_BSx = AXBS->endVertex()->position().x()/Gaudi::Units::mm;
  m_BSy = AXBS->endVertex()->position().y()/Gaudi::Units::mm;
  m_BSz = AXBS->endVertex()->position().z()/Gaudi::Units::mm;

  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  const MCParticle *BS = 0, *BO = 0;
  SmartDataPtr<HepMCEvents> hepVect(eventSvc(), HepMCEventLocation::Default);
  if ( ! hepVect ) {
    fatal() << "This event has no HepMCEvent" << endreq;
    return StatusCode::FAILURE;
  }
  for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	    p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      debug()<<"truth search for: "<<(*p)->pdg_id()
	     <<"  p="<<(*p)->momentum().vect().mag()
	     <<"  th="<<(*p)->momentum().vect().theta()
	     <<"  status "<<(*p)->status()<<endreq;
      BS = associatedofHEP(*p);
      if(BS) break; 
    }
  }
  if(BS){
    debug()<<"Found true B="<<BS->particleID().pid()<<endreq;
  } 
  //look for opposite
  MCParticles::const_iterator imc;
  double maxBP = -1;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( (*imc) != BS ) {
      if((*imc)->particleID().hasBottom()) {
	if( maxBP < (*imc)->momentum().P()/Gaudi::Units::GeV ) {
	  maxBP = (*imc)->momentum().P()/Gaudi::Units::GeV;
	  BO = (*imc);
	}
      }
    }
  }

  ////////////////////////////////////////////////////
  if ( !BS || !BO ) {
    err() << "Missing B mesons in MC!!"<< endreq;
    return StatusCode::SUCCESS;                      
  }
  ////////////////////////////////////////////////////

  m_BSID    = BS->particleID().pid();
  m_MCBSP   = float(BS->momentum().P()/Gaudi::Units::GeV);
  m_MCBSthe = float(BS->momentum().Theta());
  m_MCBSphi = float(BS->momentum().Phi());
  if( BS->hasOscillated() ) m_BSosc = 1; else m_BSosc = 0;
  if( BO->hasOscillated() ) m_BOosc = 1; else m_BOosc = 0;
  //b opposto vero
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
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/Gaudi::Units::mm <<endreq;
  }
  m_RVx = RecVert->position().x()/Gaudi::Units::mm ;
  m_RVy = RecVert->position().y()/Gaudi::Units::mm ;
  m_RVz = RecVert->position().z()/Gaudi::Units::mm ;

  //loop over Particles, preselect tags ////////////////////////
  Particle::ConstVector vtags(0);
  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){

    if( (*ipart)->p()/Gaudi::Units::GeV < 2.0 ) continue;      //preselection
    if( (*ipart)->momentum().Theta() < 0.012 ) continue;                  //preselection
    if( (*ipart)->charge() == 0 ) continue;                    //preselection
    if( trackType(*ipart) > 3 ) continue;                      //preselection
    if( isinTree( *ipart, axdaugh ) )  continue ;             //exclude signal

//     for ( jp = ipart+1; jp != parts.end(); jp++) {
//       if( (*jp)->proto() == (*ipart)->proto() ) {
// 	err()<<"duplicate part found! p="<<(*jp)->p()/Gaudi::Units::GeV<<endreq;
// 	continue;
//       }
//     }
    ///////////////////////////////////////
    vtags.push_back(*ipart);            //
    /////////////////////////////////////
  }

  ///Vertex charge ---------------------------
  //look for a secondary Vtx due to opposite B
  Vertex Vfit;
  std::vector<Vertex> vv;
  if(m_vtxtool) {
    vv = m_vtxtool->buildVertex(*RecVert, vtags);
    if(!vv.empty()) Vfit = vv.at(0); //take first
  } else {
    debug() << "No secondary vtx available."<<endreq;
  }
  double Vch = 0;  
  Particle::ConstVector Pfit = Vfit.outgoingParticlesVector();
  if( Pfit.size() ) {
    Particle::ConstVector::iterator kp;
    for(kp = Pfit.begin(); kp != Pfit.end(); kp++) {
      Vch += (*kp)->charge();
      debug() << m_SVtype <<" particle p=" << (*kp)->p()/Gaudi::Units::GeV << endreq;
    }
    debug() << "  Vertex charge: " << Vch << endreq;
  }
  m_Vch = (int) Vch;
  m_M   = Pfit.size();
  m_TVx = Vfit.position().x()/Gaudi::Units::mm;
  m_TVy = Vfit.position().y()/Gaudi::Units::mm;
  m_TVz = Vfit.position().z()/Gaudi::Units::mm;
  m_Tchi= Vfit.chi2()/Vfit.nDoF();

  ///------------------------------------------------------- Choose Taggers
  //look for muon (elec, kaon) taggers, if more
  //than one satisfies cuts, take the highest pt one

//   SmartDataPtr<MuonIDs> muonpids (eventSvc(), MuonIDLocation::Default);
//   if( !muonpids ) info()<<"Failed to retrieve any MuonIDs "<<endreq;

  m_N = 0;
  debug() << "-------- Tagging Candidates: " << vtags.size() <<endreq;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    const Particle* axp = (*ipart);
    if( axp->p()/Gaudi::Units::GeV  > 500 ) continue;
    if( axp->pt()/Gaudi::Units::GeV > 100 ) continue;
    //calculate some useful vars
    double Emeas= -1;
    double lcs  = 1000.;
    const ProtoParticle* proto = (*ipart)->proto();
    if (!proto) continue ;
    
    //const SmartRefVector<CaloHypo>& vcalo = proto->calo();
    //if(vcalo.size()==1) Emeas = (vcalo.at(0))->e()/Gaudi::Units::GeV;//xxx

    const Track* track = proto->track();
    if((track->measurements()).size() > 5)
      lcs = track->chi2PerDoF();   // /((track->measurements()).size()-5);

    double veloch = proto->info( ProtoParticle::VeloCharge, 0.0 );
    long trtyp = trackType(axp);
    long   ID  = axp->particleID().pid(); 
    double P   = axp->p()/Gaudi::Units::GeV;
    double Pt  = axp->pt()/Gaudi::Units::GeV;
    double deta  = fabs(log(tan(BSthe/2.)/tan(asin(Pt/P)/2.)));
    double dphi  = fabs(axp->momentum().phi() - BSphi); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    double dQ    = (ptotBS + axp->momentum()).M()/Gaudi::Units::GeV - BSmass;
	
    //calculate signed IP wrt RecVert
    double IP, IPerr;
    if(!(axp->particleID().hasBottom())) calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr) continue;
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

    //-------------------------------------------------------
    debug() << " trtyp="<<trtyp << " ID="<<ID 
	    << " P="<<P <<" Pt="<<Pt <<endreq;
    debug() << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
    debug() << " IPsig="<<IPsig << " IPPU="<<IPPU << " IPT="<<IPT<<endreq;

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
    m_dQ[m_N]    = dQ;
    m_veloch[m_N]= veloch;
    m_Emeas[m_N] = -1;
    m_lcs[m_N]   = 1000.;

    proto = axp->proto();
    //const SmartRefVector<CaloHypo>& calohypo = proto->calo();
    //if(calohypo.size()==1) m_Emeas[m_N] = (calohypo.at(0))->e()/Gaudi::Units::GeV;

    track = proto->track();
//     if((track->measurements()).size() > 5)
//       m_lcs[m_N] = track->chi2(); // /((track->measurements()).size()-5);//xxx
    m_lcs[m_N] = track- > chi2PerDoF ( ) ;

    //info PID
//     m_PIDe[m_N] = proto->richPID()->particleDeltaLL(Rich::Electron );
//     m_PIDm[m_N] = proto->richPID()->particleDeltaLL(Rich::Muon );
//     m_PIDk[m_N] = proto->richPID()->particleDeltaLL(Rich::Kaon );
//     m_PIDp[m_N] = proto->richPID()->particleDeltaLL(Rich::Proton );
    m_PIDe[m_N] = proto->info( ProtoParticle::CombDLLe, 0.0 );
    m_PIDm[m_N] = proto->info( ProtoParticle::CombDLLmu, 0.0 );
    m_PIDk[m_N] = proto->info( ProtoParticle::CombDLLk, 0.0 );
    m_PIDp[m_N] = proto->info( ProtoParticle::CombDLLp, 0.0 );
    m_PIDfl[m_N]= 0;
    //if( proto->caloeBit()) m_PIDfl[m_N] += 100;//xxx
    if( proto->info( ProtoParticle::MuonPIDStatus, 0 ) ) m_PIDfl[m_N] +=  10;
    if( proto->info( ProtoParticle::RichPIDStatus, 0 ) )  m_PIDfl[m_N] += 1;

    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    if( muonNSH > 0 ) m_PIDm[m_N] -= 1000.; //shift to -1000 if bad
    debug()<<" muonNSH= "<<muonNSH<<endreq;

    // Load muonPid results
//     int muonNSH   =  -1;
//     double muonDLL=-999.;
//     if( muonpids ) {
//       /// Look at the number of shared hits
//       MuonIDs::const_iterator iMuon;
//       for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon){
// 	if( (*iMuon)->IsMuon() ) {
// 	  TrStoredTrack* MuonIDtrack = (*iMuon)->idTrack();
// 	  if( MuonIDtrack == track ) {
// 	    //m_muonIDdll->calcMuonDLL( *iMuon, muonDLL ); //unused
// 	    m_muonIDnsh->calcSharedHits( *iMuon, muonNSH );
// 	    debug()<<"muonDLL= "<<muonDLL<<" muonNSH= "<<muonNSH<<endreq;
// 	  }
// 	}
//       }
//     }
//     if( muonNSH > 0 ) m_PIDm[m_N] -= 1000.; //shift to -1000 if bad

    m_vFlag[m_N] = 0;
    Particle::ConstVector::iterator kp;
    for(kp = Pfit.begin(); kp != Pfit.end(); kp++) {
      if( axp == *kp ) {m_vFlag[m_N] = 1; break;}
    }

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
      if( ancestor->particleID().hasBottom() )  m_bFlag[m_N] = 1;  
      else m_bFlag[m_N] = 0;
      if( originD(mcp) ) m_dFlag[m_N] = 1;
    }//if( mcp )

    //---------------
    if(m_N<99) m_N++;
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

  std::vector<Tagger> taggers = theTag->taggers();
  for(size_t i=0; i<taggers.size(); ++i) {
    if(taggers[i].decision()) debug()<< "Tagger "<<taggers[i].type()
	   <<" decision = " << taggers[i].decision() << endreq;
  }

  ///----------------------------------------------------------------------
  if( !(nt->write()) ) err() << "Cannot fill tagging Ntuple" << endreq;
  ///----------------------------------------------------------------------

  delete theTag;

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTaggingAnalysis::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTaggingAnalysis::isinTree( const Particle* axp, Particle::ConstVector& sons ) {

  for( Particle::ConstVector::iterator ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->proto() == axp->proto() ) {
      debug() << "excluding signal part: " 
	      << axp->particleID().pid() <<" with p="<<axp->p()/Gaudi::Units::GeV<<endreq;
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
                                 const RecVertex::ConstVector PileUpVtx,
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


//==========================================================================
long BTaggingAnalysis::trackType( const Particle* axp ) {
  long trtyp=0;
  const ProtoParticle* proto = axp->proto();
  if ( proto ) {
    const Track* track = proto->track();
    if     (track->type() == Track::Long ) trtyp = 1;
    //else if(track->match() ) trtyp = 2;//xxx
    else if(track->type() == Track::Upstream   ) trtyp = 3;
    else if(track->type() == Track::Downstream ) trtyp = 4;
    else if(track->type() == Track::Velo       ) trtyp = 5;
  }
  return trtyp;
}
//=============================================================================
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
//============================================================================
MCParticle* BTaggingAnalysis::associatedofHEP(HepMC::GenParticle* hepmcp) {

  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );

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
//============================================================================
StatusCode BTaggingAnalysis::setDecayToFind( const int evtCode ) {

  // Check if code exist
  if( !(m_evtTypeSvc->typeExists( evtCode )) ) {
    fatal() << "EvtCode " << evtCode << "is not known by the EvtTypeSvc"
            << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve tool and set decay descriptor
  m_mcFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
  std::string sdecay = m_evtTypeSvc->decayDescriptor( evtCode );
  if( (m_mcFinder->setDecay( sdecay )).isFailure() ) {
    fatal() << "Unable to set decay for EvtCode " << evtCode << endmsg;
    return StatusCode::FAILURE;
  }

  m_setDecay = true;
  m_evtCode  = evtCode;   // in case called when reading data

  return StatusCode::SUCCESS;
}
//============================================================================
