// Include files 
#include "BTaggingTool.h"
// from DaVinci
//#include "Kernel/StringUtils.h"

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingTool
//
// Author: Marco Musy
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BTaggingTool );

//=========================================================================
BTaggingTool::BTaggingTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ), m_eventSvc(0), m_Geom(0) {

  declareInterface<IBTaggingTool>(this);

  declareProperty( "SecondaryVertexName",
		   m_SecondaryVertexToolName = "SVertexTool" );
  declareProperty( "CombineTaggersName",
		   m_CombineTaggersName = "CombineTaggersNNet" );

  declareProperty( "IPPU_cut",     m_IPPU_cut = 3.0 );
  declareProperty( "thetaMin_cut", m_thetaMin = 0.012 );

  declareProperty( "RequireTrigger",  m_RequireTrigger = false );
  declareProperty( "RequireL0",       m_RequireL0      = false );
  declareProperty( "RequireL1",       m_RequireL1      = false );
  declareProperty( "RequireHLT",      m_RequireHLT     = false );
  declareProperty( "EnableMuonTagger",    m_EnableMuon    = true );
  declareProperty( "EnableElectronTagger",m_EnableElectron= true );
  declareProperty( "EnableKaonOSTagger",  m_EnableKaonOS  = true );
  declareProperty( "EnableKaonSSTagger",  m_EnableKaonSS  = true );
  declareProperty( "EnablePionTagger",    m_EnablePionSS  = true );
  declareProperty( "EnableVertexChargeTagger",m_EnableVertexCharge= true);
  declareProperty( "EnableJetSameTagger", m_EnableJetSame = false );
  declareProperty( "OutputLocation",
		   m_outputLocation = "/Event/Phys/BTaggingTool" );
  m_svtool = 0;
  m_taggerMu=m_taggerEle=m_taggerKaon=0;
  m_taggerKaonS=m_taggerPionS=m_taggerVtx=0 ;
}
BTaggingTool::~BTaggingTool() {}; 

//==========================================================================
StatusCode BTaggingTool::initialize() {

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
  }
  m_physd = tool<IPhysDesktop> ("PhysDesktop", this);
  if(! m_physd) {
    fatal() << "Unable to retrieve PhysDesktop"<< endreq;
    return StatusCode::FAILURE;
  }
  m_physd->imposeOutputLocation(m_outputLocation);

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  m_svtool = tool<ISecondaryVertexTool> ("SVertexTool", 
					 m_SecondaryVertexToolName, this);
  if(! m_svtool) {
    warning()<< "*** No Vertex Charge tag will be used! " 
             << m_SecondaryVertexToolName <<endreq;
  }
  m_taggerMu = tool<ITagger> ("TaggerMuonTool", this);
  if(! m_taggerMu) {
    fatal() << "Unable to retrieve TaggerMuonTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerEle = tool<ITagger> ("TaggerElectronTool", this);
  if(! m_taggerEle) {
    fatal() << "Unable to retrieve TaggerElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaon = tool<ITagger> ("TaggerKaonOppositeTool", this);
  if(! m_taggerKaon) {
    fatal() << "Unable to retrieve TaggerKaonOppositeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaonS = tool<ITagger> ("TaggerKaonSameTool", this);
  if(! m_taggerKaonS) {
    fatal() << "Unable to retrieve TaggerKaonSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerPionS = tool<ITagger> ("TaggerPionSameTool", this);
  if(! m_taggerPionS) {
    fatal() << "Unable to retrieve TaggerPionSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerVtxCh= tool<ITagger> ("TaggerVertexChargeTool", this);
  if(! m_taggerVtxCh) {
    fatal() << "Unable to retrieve TaggerVertexChargeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerJetS = tool<ITagger> ("TaggerJetSameTool", this);
  if(! m_taggerJetS) {
    fatal() << "Unable to retrieve TaggerJetSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_combine = tool<ICombineTaggersTool> (m_CombineTaggersName, this);
  if(! m_combine) {
    fatal() << "Unable to retrieve "<< m_CombineTaggersName << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, 0, p ) ;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB, 
                              const RecVertex* RecVert ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, RecVert, p );
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB, 
                              const RecVertex* RecVert,
                              Particle::ConstVector& vtags ) {

  theTag.setDecision( FlavourTag::none );
  if ( ! AXB ) {
    err() << "No B hypothesis to tag!" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve informations about event
//   EventHeader* evt=0;
//   if( exist<EventHeader> (EventHeaderLocation::Default)) {
//     evt = get<EventHeader> (EventHeaderLocation::Default);
//   } else {
//     err() << "Unable to Retrieve Event" << endreq;
//     return StatusCode::FAILURE;
//   }
  // Retrieve trigger info
//   int trigger=-1;
//   TrgDecision* trg = 0;
//   HltScore*    hlt = 0 ;
//   if (m_RequireTrigger) {
//     debug()<<"Retrieve TrgDecision from "
// 	   <<TrgDecisionLocation::Default<<endreq;
//     if ( !exist<TrgDecision>(TrgDecisionLocation::Default) ){
//       warning() << "No TrgDecision" << endmsg ;
//     } else {
//       trg = get<TrgDecision> (TrgDecisionLocation::Default);
//       if ( !exist<HltScore>(HltScoreLocation::Default) ){
//         debug() << "No HLT score" << endreq ;
//       } else hlt = get<HltScore>(HltScoreLocation::Default) ;
//     }
//   } 
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

  //----------------------------
  // Counter of events processed
//   debug() << ">>>>>  Tagging Run Nr " << evt->runNum()
//           << " Event " << evt->evtNum() << "  <<<<<" << endreq;

  //build desktop
  //const Particle::ConstVector& parts = m_physd->particles();
  //m_physd->saveDesktop();
  
  Particle::ConstVector parts ;
  if ( !exist<Particles>( m_outputLocation+"/Particles" )) {
    debug() << "Making tagging particles to be saved in " 
	    << m_outputLocation << endreq ;
    if( !(m_physd->getEventInput()) ) return StatusCode::SUCCESS;
    parts = m_physd->particles();
    if( !(m_physd->saveDesktop()) ) return StatusCode::SUCCESS;
  } else {
    debug() << "Getting tagging particles saved in " 
	    << m_outputLocation << endmsg ;
    const Particles* ptmp = get<Particles>( m_outputLocation+"/Particles" );
    for( Particles::const_iterator icand = ptmp->begin(); 
	 icand != ptmp->end(); icand++ ) {
      parts.push_back(*icand);
    }
  }

  const RecVertex::ConstVector& verts = m_physd->primaryVertices();
  debug() << "  Nr Vertices: "  << verts.size() 
          << "  Nr Particles: " << parts.size() <<endreq;
  
  //----------------------------
  RecVertex::ConstVector::const_iterator iv;
  if( ! RecVert ) {
    //if the prim vtx is not provided by the user,
    //choose as primary vtx the one with smallest IP wrt B signal
    //this is a guess for the actual PV chosen by the selection.
    double kdmin = 1000000;
    for(iv=verts.begin(); iv!=verts.end(); iv++){
      double ip, iperr;
      calcIP(AXB, *iv, ip, iperr);
      debug() << "Vertex IP="<< ip <<" iperr="<<iperr<<endreq;
      if(iperr) if( fabs(ip/iperr) < kdmin ) {
	kdmin = fabs(ip/iperr);
	RecVert = (*iv);
      }     
    }
    if( ! RecVert ) {
      err() <<"No Reconstructed Vertex!! Skip." <<endreq;
      return StatusCode::SUCCESS;
    }    
  }

  //build a vector of pileup vertices --------------------------
  RecVertex::ConstVector PileUpVtx(0); //contains all the other primary vtx's
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }

  //loop over Particles, preselect taggers /////////////////////
  theTag.setTaggedB( AXB );
  Particle::ConstVector::const_iterator ip;
  Particle::ConstVector axdaugh = FindDaughters( AXB ); //B daughters
  axdaugh.push_back( AXB );
  if( vtags.empty() ) { //tagger candidate list is not provided, build one
    for ( ip = parts.begin(); ip != parts.end(); ip++ ){

      if( (*ip)->p()/GeV < 2.0 ) continue;               //preselection
      if( (*ip)->momentum().theta()<m_thetaMin) continue;//preselection
      if( (*ip)->charge() == 0 ) continue;               //preselection 
      if( (*ip)->charge() == 0 ) continue;               //preselection
      if( trackType(*ip) < 3 )   continue;               //preselection
      if( trackType(*ip) > 4 )   continue;               //preselection
      if( isinTree( *ip, axdaugh ) )  continue ;         //exclude signal

      //calculate the min IP wrt all pileup vtxs
      double ippu, ippuerr;
      calcIP( *ip, PileUpVtx, ippu, ippuerr );
      //eliminate from vtags all parts coming from a pileup vtx
      if(ippuerr) if( ippu/ippuerr<m_IPPU_cut ) continue; //preselection

      //////////////////////////////////
      vtags.push_back(*ip);          // store tagger candidate
      ////////////////////////////////
    }
  } else {
    //tagger candidate list is already provided, it is the user responsibility
    //to check that there is not a signal B daughter inside...
    debug()<<"User tagger candidate list of size = "<<vtags.size()<<endreq;
  }

  //AXB is the signal B from selection
  bool isBd = false; if( AXB->particleID().hasDown() )   isBd = true;
  bool isBs = false; if( AXB->particleID().hasStrange()) isBs = true;
  bool isBu = false; if( AXB->particleID().hasUp() )     isBu = true;

  ///--- Inclusive Secondary Vertex ---
  //look for a secondary Vtx due to opposite B
  std::vector<Vertex> vvec(0);
  Vertex Vfit(0); //secondary vertex found
  if(m_svtool) {
    vvec = m_svtool->buildVertex(*RecVert, vtags);
    if(!vvec.empty()) Vfit = vvec.at(0); //take first
  }
  std::vector<const Vertex*> allVtx;
  if(!vvec.empty()) allVtx.push_back(&Vfit);

  ///Choose Taggers ------------------------------------------------------ 
  debug() <<"determine taggers" <<endreq;
  Tagger muonTag, elecTag, kaonTag, kaonSTag, pionSTag, vtxChTag, jetSTag;
  if(m_EnableMuon)     muonTag = m_taggerMu   -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableElectron) elecTag = m_taggerEle  -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonOS)   kaonTag = m_taggerKaon -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonSS) if(isBs)  
                       kaonSTag= m_taggerKaonS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnablePionSS) if(isBd || isBu)
                       pionSTag= m_taggerPionS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableVertexCharge) 
                       vtxChTag= m_taggerVtxCh-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableJetSame)  jetSTag = m_taggerJetS -> tag(AXB, RecVert, allVtx, vtags);
  std::vector<Tagger*> taggers;
  taggers.push_back(&muonTag);
  taggers.push_back(&elecTag);
  taggers.push_back(&kaonTag);
  if( isBs ) taggers.push_back(&kaonSTag);
  if( isBu || isBd ) taggers.push_back(&pionSTag);
  taggers.push_back(&vtxChTag);

  debug()<<"tagger mu  "<< muonTag.decision()<<" w="<< muonTag.omega() <<endreq;
  debug()<<"tagger ele "<< elecTag.decision()<<" w="<< elecTag.omega() <<endreq;
  debug()<<"tagger kO  "<< kaonTag.decision()<<" w="<< kaonTag.omega() <<endreq;
  debug()<<"tagger kS  "<< kaonSTag.decision()<<" w="<< kaonSTag.omega() <<endreq;
  debug()<<"tagger vtx "<< vtxChTag.decision()<<" w="<< vtxChTag.omega() <<endreq;

  ///---------------------------------------------------------------------
    
  //--------------------------------------------------
  //Now combine the individual tagger decisions into 
  //one final B flavour tagging decision. Such decision 
  //can be made and categorised in two ways: the "TDR" 
  //approach is based on the particle type, while the 
  //"NNet" approach is based on the wrong tag fraction
  //-------------------------------------------------- 
  debug() <<"combine taggers "<< taggers.size() <<endreq;
  int category = m_combine -> combineTaggers( theTag, taggers );

  ///OUTPUT to Logfile ---------------------------------------------------
  int sameside = kaonSTag.decision();
  if(!sameside) sameside = pionSTag.decision();
  info() << "BTAGGING TAG   " 
    //<< std::setw(7)  << evt->runNum()//xxx
    //<< std::setw(14) << evt->evtNum()//xxx
         << std::setw(5) << theTag.decision()
         << std::setw(3) << category
	 << std::setw(5) << muonTag.decision()
	 << std::setw(3) << elecTag.decision()
	 << std::setw(3) << kaonTag.decision()
	 << std::setw(3) << sameside
	 << std::setw(3) << vtxChTag.decision()
         << endreq;

  return StatusCode::SUCCESS;
}
//=========================================================================
StatusCode BTaggingTool::finalize() { return StatusCode::SUCCESS; }

//==========================================================================
bool BTaggingTool::isinTree( const Particle* axp, Particle::ConstVector& sons ) {

  Particle::ConstVector::const_iterator ip;
  for( ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->proto() == axp->proto() ) {
      debug() << "excluding signal part: " 
              << axp->particleID().pid() <<" with p="<<axp->p()/GeV<<endreq;
      return true;
    }
  }
  return false;
}
//==========================================================================
StatusCode BTaggingTool::calcIP( const Particle* axp, 
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
StatusCode BTaggingTool::calcIP( const Particle* axp, 
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
//==========================================================================
long BTaggingTool::trackType( const Particle* axp ) {

  const ProtoParticle* proto = axp->proto();
  if ( proto ) {
    const Track* track = proto->track();
    return track->type();
  }
  return 0;
}
//==========================================================================
//return a vector containing all daughters of signal 
Particle::ConstVector BTaggingTool::FindDaughters( const Particle* axp ) {

  Particle::ConstVector apv, apv2, aplist;
  apv.push_back(axp);  
  do {
    apv2.clear();
    for( Particle::ConstVector::const_iterator 
           ip=apv.begin(); ip!=apv.end(); ip++ ) {
      if( (*ip)->endVertex() ) {
        Particle::ConstVector tmp = (*ip)->endVertex()->outgoingParticlesVector();
        for( Particle::ConstVector::const_iterator 
               itmp=tmp.begin(); itmp!=tmp.end(); itmp++){
          apv2.push_back(*itmp);
          aplist.push_back(*itmp);
        }
      }
    }
    apv = apv2;
  } while ( !apv2.empty() );
   	    
  return aplist;
}
//==========================================================================
