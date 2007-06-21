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
                                   ISvcLocator* pSvcLocator) : 
  DVAlgorithm(name, pSvcLocator), 
  m_linkLinks(0) , 
  m_electron(0), 
  m_forcedBtool(0)
{
  declareProperty( "SecondaryVertexType", m_SVtype    = "SVertexTool" );
  declareProperty( "TagOutputLocation", m_TagLocation = "" );
  declareProperty( "AssociatorInputData", m_setInputData );

  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );

  declareProperty( "IPPU_cut",     m_IPPU_cut    = 3.0 );
  declareProperty( "distphi_cut",  m_distphi_cut = 0.005 );
  declareProperty( "thetaMin_cut", m_thetaMin    = 0.012 );

  m_util = 0;
  m_vtxtool = 0;
  m_descend = 0;
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
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
    return StatusCode::FAILURE;
  }

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

  m_util = tool<ITaggingUtilsChecker> ( "TaggingUtilsChecker", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtilsChecker tool "<< endreq;
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
    nt->addItem ("Omega",  m_omega);
    nt->addItem ("TagCat", m_TagCat);
    nt->addItem ("evType", m_type);
    nt->addItem ("trig",   m_trigger);
    nt->addItem ("tamper", m_tamper);

    //reconstructed signal
    nt->addItem (       "M",     m_M, 0, 10 ); //limit
    nt->addIndexedItem ("sID",   m_M, m_sID);
    nt->addIndexedItem ("sMothID",m_M, m_sMothID);
    nt->addIndexedItem ("sP",     m_M, m_sP);
    nt->addIndexedItem ("sPt",    m_M, m_sPt);
    nt->addIndexedItem ("sPhi",   m_M, m_sPhi);
    nt->addIndexedItem ("sMass",  m_M, m_sMass);
    nt->addIndexedItem ("sMCID",  m_M, m_sMCID);
    nt->addIndexedItem ("sMCMothID",   m_M, m_sMCMothID);
    nt->addIndexedItem ("sMCP",    m_M, m_sMCP);
    nt->addIndexedItem ("sMCPt",   m_M, m_sMCPt);
    nt->addIndexedItem ("sMCPhi",  m_M, m_sMCPhi);
    nt->addIndexedItem ("sVx",   m_M, m_sVx);
    nt->addIndexedItem ("sVy",   m_M, m_sVy);
    nt->addIndexedItem ("sVz",   m_M, m_sVz);

    //B opposite
    nt->addItem ("BOID",    m_BOID);
    nt->addItem ("BOP",     m_BOP);
    nt->addItem ("BOthe",   m_BOthe);
    nt->addItem ("BOphi",   m_BOphi);
    nt->addItem ("BOx",     m_BOx);
    nt->addItem ("BOy",     m_BOy);
    nt->addItem ("BOz",     m_BOz);    
    nt->addItem ("BOosc",   m_BOosc);
    nt->addItem ("BSosc",   m_BSosc);

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
    nt->addItem ("N",     m_N, 0, 200 ); //limit
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
    nt->addIndexedItem ("IPT"    ,m_N, m_IPT);
    nt->addIndexedItem ("MCID",   m_N, m_MCID);
    nt->addIndexedItem ("MCP",    m_N, m_MCP);
    nt->addIndexedItem ("MCPt",   m_N, m_MCPt);
    nt->addIndexedItem ("MCphi"  ,m_N, m_MCphi);
    nt->addIndexedItem ("MCz"    ,m_N, m_MCz);
    nt->addIndexedItem ("mothID" ,m_N, m_mothID);
    nt->addIndexedItem ("ancID"  ,m_N, m_ancID);
    nt->addIndexedItem ("bFlag"  ,m_N, m_bFlag);
    nt->addIndexedItem ("xFlag"  ,m_N, m_xFlag);
    nt->addIndexedItem ("vFlag",  m_N, m_vFlag);
 
    //Particles FITTED
    nt->addItem ("SVx",      m_SVx);
    nt->addItem ("SVy",      m_SVy);
    nt->addItem ("SVz",      m_SVz);
    nt->addItem ("Schi",     m_Schi);
    nt->addItem ("SVch",     m_SVch);

    ///Added by SPOSS to have the tagger info
    nt->addItem ("T",               m_T, 0 , 10); //limit
    nt->addItem ("TaggerType",      m_T, m_TaggerType);
    nt->addItem ("TaggerDecision",  m_T, m_TaggerDecision);
    nt->addItem ("TaggerOmega",     m_T, m_TaggerOmega);
    nt->addItem ("TaggerUsedInSV",  m_T, m_TaggerUsedInSV);
    nt->addItem ("TaggerRef",       m_T, m_TaggerRef);

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
  m_tamper = 0;
  
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
  const SmartRefVector<GenCollision>& mcColls = gene->collisions();
  if(mcColls.empty()) err()<< "No pp mcCollision retrieved" << endreq;
  debug() << "Nr of Collisions = " << mcColls.size() << endreq;
  m_kType = 0;
  for ( unsigned int icoll=0; icoll!=mcColls.size(); icoll++ ) {
    if( mcColls.at(icoll)->isSignal() ) {
      m_kType = mcColls.at(icoll)->processType();
      debug() << "Collision process type: " << m_kType << endreq;
    }
  }

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
  double BSmass= 0, BSthe= 0, BSphi= 0;
  int nrofB=0;

  Particle::ConstVector::const_iterator ip;
  for ( ip = parts.begin(); ip != parts.end(); ip++){
    if(!(*ip)->particleID().hasBottom()) continue;
    nrofB++;
    AXBS = (*ip);
    axdaugh = m_descend->descendants(AXBS);
    axdaugh.push_back(AXBS);
    if( AXBS->particleID().hasDown() )    isBd = true;
    if( AXBS->particleID().hasStrange() ) isBs = true;
    if( AXBS->particleID().hasUp() )      isBu = true;
    ptotBS = AXBS->momentum();
    BSmass = ptotBS.M()/Gaudi::Units::GeV;
    BSthe  = ptotBS.theta();
    BSphi  = ptotBS.phi();
    debug() << "Found Selected B!  daugh="<< axdaugh.size()-1 
            << "  B-Mass=" << BSmass << endreq;
    break;
  }
  
  if( axdaugh.size() == 0 ) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }
  if( nrofB>1 ) {
    err() <<"More than one B signal found. Skip event!" <<endreq;
    return StatusCode::SUCCESS;
  }

  //fill signal particle block:
  m_M=0;
  for ( ip = axdaugh.begin(); ip != axdaugh.end(); ip++){
    m_sID[m_M] = (*ip)->particleID().pid();
    const Particle* mater   = m_util->motherof(*ip, parts);
    if(mater) m_sMothID[m_M]= mater->particleID().pid(); else m_sMothID[m_M]=0;
    m_sP[m_M]   = (*ip)->p() /Gaudi::Units::GeV;
    m_sPt[m_M]  = (*ip)->pt()/Gaudi::Units::GeV;
    m_sPhi[m_M] = (*ip)->momentum().Phi();
    m_sMass[m_M]= (*ip)->momentum().M() /Gaudi::Units::GeV;

    const Vertex* endv = (*ip)->endVertex();
    if(endv){
      m_sVx[m_M]= (*ip)->endVertex()->position().x() /Gaudi::Units::mm;
      m_sVy[m_M]= (*ip)->endVertex()->position().y() /Gaudi::Units::mm;
      m_sVz[m_M]= (*ip)->endVertex()->position().z() /Gaudi::Units::mm;
    } else { m_sVx[m_M]=0; m_sVy[m_M]=0; m_sVz[m_M]=20000; }

    //mc truth
    m_sMCID[m_M]  = 0;
    m_sMCMothID[m_M]= 0;
    m_sMCP[m_M]   = 0;
    m_sMCPt[m_M]  = 0;
    m_sMCPhi[m_M] = 0;
    const MCParticle* mcp = m_linkLinks->first( *ip );
    if( mcp ) {
      m_sMCID[m_M] = mcp->particleID().pid();
      if( mcp->mother() ) m_sMCMothID[m_M] = mcp->mother()->particleID().pid();
      m_sMCP[m_M]  = mcp->p()/Gaudi::Units::GeV;
      m_sMCPt[m_M] = mcp->pt()/Gaudi::Units::GeV;
      m_sMCPhi[m_M]= mcp->momentum().Phi();
    }
    if(m_M<9) m_M++;
  }

  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  const MCParticle* BS = m_forcedBtool->forcedB();

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
    warning() << "Missing Signal B meson in MC! Skip event."<< endreq;
    return StatusCode::SUCCESS;                      
  }
  if ( !BO ) {
    warning() << "Missing Opposite B meson in MC! Skip event."<< endreq;
    return StatusCode::SUCCESS;                      
  }
  ////////////////////////////////////////////////////
  //debug()<<"SIGNAL B:"<<endreq; m_debug -> printTree(BS);
  //debug()<<"OPPOSITE B (TAGGING B):"<<endreq; m_debug -> printTree(BO);

  const RecVertex* RecVert = 0; 
  m_krec = 0;
  RecVertex::ConstVector::const_iterator iv;
  //NEED CHANGE:
  //if the prim vtx is not provided by the user,
  //choose as primary vtx the one with smallest IP wrt B signal
  //this is a guess for the actual PV chosen by the selection.
  double kdmin = 1000000;
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    m_krec++;
    double ip, iperr;
    m_util->calcIP(AXBS, *iv, ip, iperr);
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

    if( (*ip)->p() < 2000 ) continue;  
    if( (*ip)->momentum().theta()  < m_thetaMin ) continue;   
    if( (*ip)->charge() == 0 ) continue;                
    if( !(*ip)->proto() )      continue;
    if( !(*ip)->proto()->track() ) continue;
    if( (*ip)->proto()->track()->type() < 3 ) continue; 
    if( (*ip)->proto()->track()->type() > 4 ) continue; 
    if( (*ip)->p()  > 200000 ) continue;
    if( (*ip)->pt() >  10000 ) continue;
    if( m_util->isinTree(*ip, axdaugh, distphi) ) continue ; 
    if( distphi < m_distphi_cut ) continue;

    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    m_util->calcIP( *ip, PileUpVtx, ippu, ippuerr );
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
    m_SVch= (int) Vch;
    m_SVx = Vfit.position().x()/Gaudi::Units::mm;
    m_SVy = Vfit.position().y()/Gaudi::Units::mm;
    m_SVz = Vfit.position().z()/Gaudi::Units::mm;
    m_Schi= Vfit.chi2()/Vfit.nDoF();
  } else { m_SVch=0; m_SVx=0; m_SVy=0; m_SVz=0; m_Schi=0; }

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
    double deta= fabs(log(tan(BSthe/2.)/tan(asin(Pt/P)/2.)));
    double dphi= fabs(axp->momentum().phi() - BSphi); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    double dQ  = (ptotBS + axp->momentum()).M()/Gaudi::Units::GeV - BSmass;
    double lcs = track->chi2PerDoF();
    long trtyp = track->type();
    if (track->checkHistory(Track::TrackMatching) == true) trtyp=7;

    m_util->isinTree( axp, axdaugh, distphi );

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    if(!(axp->particleID().hasBottom())) m_util->calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr) continue;                                      //preselection cut
    double IPsig = fabs(IP/IPerr);
    //calculate signed IP wrt SecondaryVertex
    double IPT = -1000, ip, iperr;
    if(Pfit.size()){
      m_util->calcIP(axp, &Vfit, ip, iperr);
      if(iperr) IPT = ip/iperr;
    }
    //calculate min IP wrt all pileup vtxs 
    double IPPU = 10000;
    m_util->calcIP( axp, PileUpVtx, ip, iperr );
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

    const bool inEcalACC = proto->info(ProtoParticle::InAccEcal, false);
    const bool inHcalACC = proto->info(ProtoParticle::InAccHcal, false);
    //     m_Emeas[m_N] = -1.0;
    //     m_elChi[m_N] = proto->info(LHCb::ProtoParticle::CaloEcalChi2, 10000.);
    //     if( inEcalACC ){
    //       const SmartRefVector<CaloHypo> hypos = proto->calo();// get CaloHypos
    //       if( hypos.size() !=0) {      
    //         // When available the 'electron' hypo is the first one
    //         // This implies inEcal = true (the possible second one is BremStrahlung)
    //         SmartRefVector<CaloHypo>::const_iterator ihypo =  hypos.begin();
    //         const CaloHypo* hypo = *ihypo;
    //         if(hypo) if( hypo->hypothesis() == CaloHypo::EmCharged) 
    //           m_Emeas[m_N] = hypo->position()->e();
    //         debug()<<" Calo Emeas="<<m_Emeas[m_N]<<" elChi="<<m_elChi[m_N]<<endreq;
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
    if( inHcalACC )                  m_PIDfl[m_N] +=   1000;
    if( inEcalACC )                  m_PIDfl[m_N] +=    100;
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

      const MCParticle* ancestor = m_util->originof(mcp) ;
      m_ancID[m_N] = ancestor->particleID().pid();
      if( ancestor->particleID().hasBottom() ) {
        m_bFlag[m_N] = 1;  
        if(ancestor == BS ) {
          m_bFlag[m_N] = -1;
          debug() <<" Warning: tag from signal! ID=" << mcp->particleID().pid() 
                  <<" P="<< mcp->momentum().P() << endreq;
        }
      }

      m_xFlag[m_N] = m_util->comes_from_excitedB(BS, mcp);
      if(m_xFlag[m_N]) debug() <<" comes_from_excitedB=" 
                               << m_xFlag[m_N] << endreq;

    }//if( mcp )

    //---------------
    if(m_N<199) m_N++;
  }

  //-------------------- OFFICIAL TAG of the Event --------------------
  FlavourTag* theTag = new FlavourTag;
  StatusCode sc = flavourTagging()->tag( *theTag, AXBS );
  //if you want to enforce same particle & vertex in official tagging, use:
  //StatusCode sc = flavourTagging()->tag( *theTag, AXBS, RecVert, vtags );
  if (!sc) {
    err() <<"Tagging Tool returned error."<< endreq;
    delete theTag;
    return StatusCode::SUCCESS;
  }

  m_Tag     = theTag->decision();
  m_omega   = theTag->omega();
  m_TagCat  = theTag->category();
  m_TrueTag = BS->particleID().pid()>0 ? 1 : -1;
  std::vector<Tagger> taggers = theTag->taggers();
  for(size_t i=0; i<taggers.size(); ++i) {
    Tagger itagger = taggers.at(i);
    if( itagger.decision() ) {
      debug()<<"Tagger "<<itagger.type()
             <<" decision= "<<itagger.decision()<<endreq;
      m_TaggerType[m_T]     = itagger.type();
      m_TaggerDecision[m_T] = itagger.decision();
      m_TaggerOmega[m_T]    = itagger.omega();
      m_TaggerUsedInSV[m_T] = 0;
      Particle::ConstVector::iterator kp;
      const ProtoParticle* iproto = itagger.taggerParts().at(0).proto();
      for(kp = Pfit.begin(); kp != Pfit.end(); kp++) {
        if( (*kp)->proto() == iproto ) m_TaggerUsedInSV[m_T]=1;
      }
      int ref=0;
      m_TaggerRef[m_T] = -1;
      for( kp = vtags.begin(); kp != vtags.end(); kp++ ) {
        if( (*kp)->proto() == iproto ) m_TaggerRef[m_T] = ref;
        ref++;
      }
      if(m_T<9) m_T++;
    }      
  }
  
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

