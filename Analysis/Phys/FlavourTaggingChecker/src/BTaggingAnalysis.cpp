// Include files 
#include "BTaggingAnalysis.h"

#include "Event/ODIN.h" // event & run number
//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingAnalysis
//
// Authors: Marco Musy, Marc Grabalosa
// 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BTaggingAnalysis );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTaggingAnalysis::BTaggingAnalysis(const std::string& name, 
                                   ISvcLocator* pSvcLocator):
  DVAlgorithm(name, pSvcLocator), 
  m_debug(0), 
  m_electron(0), 
  m_forcedBtool(0),
  m_bkgCategory(0) ,
  m_pLifetimeFitter(0),
  m_util(0),
  m_descend(0),
  m_pvReFitter(0),
  m_TriggerTisTosTool(0),
  m_L0TriggerTisTosTool(0),
  m_assoc(0),
  m_svtool(0),
  m_fitter(0),
  m_BS(0)
{
  declareProperty( "EnableMC",  m_EnableMC = true ); //deasactivate to run with real data

  declareProperty( "BHypoCriterium",          m_BHypoCriterium   = "MinChi2");
  declareProperty( "ChoosePVCriterium",       m_ChoosePV         = "bestPV" );
  declareProperty( "PVReFit",                 m_ReFitPVs         = true );

  //trigger
  declareProperty( "RequireTrigger",          m_requireTrigger   = true );
  declareProperty( "RequireTisTos",           m_requireTisTos    = true );
  declareProperty( "SaveHlt1Lines",           m_saveHlt1Lines    = false );
  declareProperty( "SaveHlt2Lines",           m_saveHlt2Lines    = false );

  declareProperty( "TagOutputLocation",       m_TagLocation = FlavourTagLocation::Default );
  declareProperty( "SecondaryVertexToolName", m_SecondaryVertexToolName = "SVertexOneSeedTool");

  //preselection cuts
  declareProperty( "IPPU_cut",                m_IPPU_cut    = 3.0 );
  declareProperty( "distphi_cut",             m_distphi_cut = 0.005 );
  declareProperty( "thetaMin_cut",            m_thetaMin    = 0.012 );
}

BTaggingAnalysis::~BTaggingAnalysis() {}; 

//=============================================================================
StatusCode BTaggingAnalysis::initialize() {

  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator", this);
  if( !m_assoc ) {
    fatal() << "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }
  if(m_requireTrigger){
    m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos", this); 
    if ( !m_TriggerTisTosTool ) {   
      fatal() << "TriggerTisTosTool could not be found" << endreq;
      return StatusCode::FAILURE;
    }
    m_L0TriggerTisTosTool = tool<ITriggerTisTos>( "L0TriggerTisTos",this);
    if ( !m_L0TriggerTisTosTool ) {   
      fatal() << "L0TriggerTisTosTool could not be found" << endreq;
      return StatusCode::FAILURE;
    }
  }
  m_bkgCategory = tool<IBackgroundCategory>("BackgroundCategory", this);
  if (m_EnableMC) if(!m_bkgCategory){
    fatal() << "Unable to retrieve BackgroundCategory tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_pLifetimeFitter = tool<ILifetimeFitter>("PropertimeFitter", this);
  if(!m_pLifetimeFitter){
    fatal() << "Unable to retrieve LifetimeFitter tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_debug = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if (m_EnableMC) if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if (m_EnableMC) if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_electron = tool<ICaloElectron>( "CaloElectron");
  if(! m_electron) {
    fatal() << "Unable to retrieve ICaloElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_util = tool<ITaggingUtilsChecker> ( "TaggingUtilsChecker", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtilsChecker tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
  if(! m_pvReFitter) {
    fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
    return StatusCode::FAILURE;
  }
  m_svtool = tool<ISecondaryVertexTool> (m_SecondaryVertexToolName, this);
  if(! m_svtool) {
    warning()<< "*** No Vertex Charge tag will be used! " 
             << m_SecondaryVertexToolName << endreq;
  }
  m_fitter = tool<IVertexFit>("OfflineVertexFitter");
  if ( !m_fitter ) {   
    err() << "Unable to Retrieve Default VertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingAnalysis::execute() {
  
  setFilterPassed( false );
   
  //new ntuple
  Tuple tuple = nTuple("mytagging","mytagging",CLID_ColumnWiseTuple);
    
  //----------------------------------------------------------------------
 // Load the ODIN
  LHCb::ODIN* odin(NULL);
  if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default );
  }else if( exist<ODIN>( LHCb::ODINLocation::Default, false ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default, false );
  }else{
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }
  debug()<<">>>>>  Processing Run "<<odin->runNumber() 
         <<"   Event "<<odin->eventNumber()<<"  <<<<<" <<endreq;
  tuple -> column ("run",             odin->runNumber());
  tuple -> column ("event",   (double)odin->eventNumber());
  tuple -> column ("gpsTime", (double)odin->gpsTime());
  tuple -> column ("bunchID",         odin->bunchId());


  /*
  RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);

  debug()<<">>>>>  Processing Run "<<evt->runNumber() 
         <<"   Event "<<evt->evtNumber()<<"  <<<<<" <<endreq;
  tuple -> column ("run", evt->runNumber());
  //  tuple -> column ("event", (long)evt->evtNumber());
  tuple -> column ("event", (double)evt->evtNumber());
  */
  //----------------------------------------------------------------------
  //MCinfo
  MCParticles* mcpart = 0;
  if (m_EnableMC) {
    GenHeader* gene = get<GenHeader> (GenHeaderLocation::Default);

    // Retrieve MCParticles and MCVertex
    mcpart = get<MCParticles> (MCParticleLocation::Default);
    debug() << "Nr of MCParticles retrieved="<< mcpart->size()<< endreq;
    MCVertices* mcvert = get<MCVertices> (MCVertexLocation::Default);
    debug() << "Nr of MCVertices retrieved="<< mcvert->size()<< endreq;
    
    // Retrieve information about primary pp collisions
    const SmartRefVector<GenCollision>& mcColls = gene->collisions();
    if(mcColls.empty()) err()<< "No pp mcCollision retrieved" << endreq;
    debug() << "Nr of Collisions = " << mcColls.size() << endreq;
    long kType = 0;
    for ( unsigned int icoll=0; icoll!=mcColls.size(); icoll++ ) {
      if( mcColls.at(icoll)->isSignal() ) {
        kType = mcColls.at(icoll)->processType();
        debug() << "Collision process type: " << kType << endreq;
      }
    }
    tuple -> column ("evType",gene->evType());
    tuple -> column ("kType", kType);
  }
  
  //----------------------------------------------------------
  //PhysDeskTop
  const Particle::Range& parts = this->particles();
  const RecVertex::Range verts=get<RecVertex::Range>(RecVertexLocation::Primary);
  debug()<<"  Nr of rec. Vertices: " << verts.size() 
         <<"  Nr of rec. Particles: "<< parts.size() <<endreq;

  //----------------------------------------------
  //Choose Signal B
  const Particle* AXBS = chooseBHypothesis(parts);
  if(!AXBS) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }
  tuple -> column ("minBChi2", AXBS->endVertex()->chi2PerDoF());
  tuple -> column ("Bmass", AXBS->measuredMass()/GeV);
  tuple -> column ("Bid", AXBS->particleID().pid());

  //--------------------------------------------------------------------
  //build primary and pileup vertices
  RecVertex PV(0);
  const RecVertex::ConstVector PileUpVtx = choosePrimary(AXBS, verts, PV);
  const RecVertex* RecVert=   (const RecVertex*)  &PV;

  if( !RecVert ) {
    err() <<"No Reconstructed Vertex!! Skip." <<endreq;
    return StatusCode::SUCCESS;
  }
  double ipPVBS, iperrPVBS;
  ipPVBS=iperrPVBS=0;
  m_util->calcIP(AXBS, RecVert, ipPVBS, iperrPVBS);
  tuple -> column ("Bip", ipPVBS);
  tuple -> column ("Biperr", iperrPVBS);
  tuple -> column ("krec", verts.size() );
  tuple -> column ("RVx",  RecVert->position().x()/mm);
  tuple -> column ("RVy",  RecVert->position().y()/mm);
  tuple -> column ("RVz",  RecVert->position().z()/mm);
  debug()<<" **** RecVertex z: "<<RecVert->position().z()/mm <<", IPPVBS: " <<ipPVBS<< ", PU size: "<<verts.size()<<endreq;
  //lifetime fitter
  double ct=0., ctErr=0., ctChi2=0.;
  StatusCode sc = m_pLifetimeFitter->fit(*RecVert, *AXBS, ct, ctErr, ctChi2);
  if(sc){
    tuple -> column ("tau", ct/picosecond);
    tuple -> column ("tauErr", ctErr/picosecond);
    tuple -> column ("ctChi2", ctChi2);
  } else {
    tuple -> column ("tau", 0. );
    tuple -> column ("tauErr", 1.);
    tuple -> column ("ctChi2", 0.);
  }

  //--------------------------------------------------------------------
  //Official tag of the event ------------------------------------------
  ProtoParticle::ConstVector partsInSV = tagevent( tuple, AXBS );
  //  ProtoParticle::ConstVector partsInSV = tagevent( tuple, AXBS , RecVert); //to select ourselves PV

  //---------------------------------------
  //trigger
  sc = FillTrigger(tuple, AXBS);
  if(!sc) err()<<"FillTrigger returns failure."<<endreq;

  //fill info signal B
  Particle::ConstVector axdaugh = FillSelectedB(tuple, AXBS);
  debug() <<"End FillSelectedB selected" <<endreq;

  //fill MCinfo signal/opposite B
  if (m_EnableMC) {
    m_BS = m_forcedBtool->forcedB();
    sc = FillMCInfoOfB(tuple, mcpart);
    if(!sc)err()<<"FillMCInfoOfB returns failure."<<endreq;
    //Background category
    int bcat = -1;
    if(AXBS) if(m_bkgCategory) if( ! AXBS->isBasicParticle() ){
      bcat = (int) m_bkgCategory->category(AXBS);
      debug() << "Result of BackgroundCategory is: " << bcat << endreq;
    }
    tuple -> column ("bkgCat", bcat);
  }

  //------------------------------------------------------------------------------
  //fill particle tagging candidates
  const Particle::ConstVector vtags = chooseCandidates(parts, axdaugh, PileUpVtx);
  debug() << "-------- my Tagging Candidates: " << vtags.size() <<endreq;

  //------------------------------------------------------------------------------
  //fill seed info for vtxch
  std::vector<Vertex> svertices(0);
  const Particle *SVpart1, *SVpart2; 
  FillSeedInfo(tuple, RecVert, vtags, svertices, SVpart1, SVpart2);
  debug() << "  Loop on seeds done " <<endreq;


  //------------------------------------------------------------------------------
  //Fill Tagger info -------------------------------------------------------------
  //------------------------------------------------------------------------------
  debug()<<"Fill tagger info, vtags: "<<vtags.size()<<endreq;

  std::vector<float> pID(0), pP(0), pPt(0), pphi(0), pch(0), pip(0), pipsign(0), piperr(0), pipPU(0), pPl(0), ptheta(0);
  std::vector<float> ptrtyp(0), plcs(0), pcloneDist(0), ptsal(0), pgprob(0), pdistPhi(0), pveloch(0), pEOverP(0);
  std::vector<float> pPIDe(0), pPIDm(0), pPIDk(0), pPIDp(0),pPIDfl(0);
  std::vector<float> pPIDNNe(0), pPIDNNm(0), pPIDNNk(0), pPIDNNp(0),pPIDNNpi(0);
  std::vector<float> pMCID(0), pMCP(0), pMCPt(0), pMCphi(0), pMCx(0), pMCy(0), pMCz(0), pMCPl(0),
    pmothID(0), pancID(0), pbFlag(0), pxFlag(0), pvFlag(0), pMC_OS_muon_type(0);
  std::vector<float> pIPSV(0), pIPSVerr(0), pDOCA(0), pDOCAerr(0);
  std::vector<float> pdeta(0), pdphi(0), pdQ(0);
  std::vector<float> ppionCombinedMass(0);
  // data for multPV 
  std::vector<float> pnippu(0), pnippuerr(0), pipmean(0), pxpos(0), pypos(0), pzpos(0), 
    pxerrpos(0), pyerrpos(0), pzerrpos(0), pippubs(0), pippuchi2bs(0); 
  std::vector<float> pnippu2(0), pnippuerr2(0), pxpos2(0), pypos2(0), pzpos2(0), 
    pxerrpos2(0), pyerrpos2(0), pzerrpos2(0), pippubs2(0), pippuchi2bs2(0); 
  std::vector<float> pnippu3(0), pnippuerr3(0), pxpos3(0), pypos3(0), pzpos3(0), 
    pxerrpos3(0), pyerrpos3(0), pzerrpos3(0), pippubs3(0), pippuchi2bs3(0); 
  std::vector<float> pntracks(0), pntracks2(0), pntracks3(0);
  std::vector<float> ptrackxfirst(0), ptrackyfirst(0), ptrackzfirst(0), ptrackp(0);

  Particle::ConstVector::const_iterator ip;
  for( ip = vtags.begin(); ip != vtags.end(); ip++ ) {

    const Particle* axp = (*ip);
    const ProtoParticle* proto = axp->proto();
    const Track* track = proto->track();

    //calculate some useful vars
    long   ID  = axp->particleID().pid(); 
    double P   = axp->p()/GeV;
    double Pt  = axp->pt()/GeV;
    double Pl  = axp->momentum().Dot(AXBS->momentum())/GeV;
    double deta= fabs(log(tan(AXBS->momentum().theta()/2.)/tan(asin(Pt/P)/2.)));
    double dphi= fabs(axp->momentum().phi() - AXBS->momentum().phi()); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    double dQ  = ((AXBS->momentum() + axp->momentum()).M() - AXBS->momentum().M()) /GeV;
    double lcs = track->chi2PerDoF();
    double cloneDist = track->info(LHCb::Track::CloneDist, -1.) ;
    double trackxfirst = track->position().x();
    double trackyfirst = track->position().y();
    double trackzfirst = track->position().z();
    double trackp = track->p();
    long trtyp = track->type();
    //track studies (forward and matched tracks) TrackMatching->PatMatch 
    //if (track->checkHistory(Track::TrackMatching) == true) trtyp=7; //old code had that
    double distphi;
    m_util->isinTree( axp, axdaugh, distphi );

    double pionCombinedMass = GetInvariantMass(AXBS->measuredMass()/GeV, AXBS->momentum()/GeV, 0.1395702, axp->momentum()/GeV);
    
    //calculate signed IP wrt RecVert
    double IP, IPerr, IPsign;
    IP=-1;
    if(!(axp->particleID().hasBottom())) {
      m_util->calcIP(axp, RecVert, IPsign, IPerr);
      IP=fabs(IPsign); 
    }

    //calculate min IP wrt all pileup vtxs 
    double IPPU = 10000;
    double ipval, iperr;
    m_util->calcIP( axp, PileUpVtx, ipval, iperr );
    if(iperr) IPPU=ipval/iperr;
    //cal IP wrt different PU
    //debug()<<"--> min ippu "<<ipval<<endreq;
    double nippu, nippuerr, ipmean, xpos, ypos, zpos, xerrpos, yerrpos, zerrpos;
    int ntracks;
    double ippubs, ippuchi2bs;
    m_util->calcIPPU( axp, AXBS, PileUpVtx, 0, nippu, nippuerr, ipmean, xpos, ypos, zpos, 
                      xerrpos, yerrpos, zerrpos, ntracks, ippubs, ippuchi2bs );
    double nippu2, nippuerr2, xpos2, ypos2, zpos2, xerrpos2, yerrpos2, zerrpos2;
    int ntracks2;
    double ippubs2, ippuchi2bs2;
    m_util->calcIPPU( axp, AXBS, PileUpVtx, nippu, nippu2, nippuerr2, ipmean, xpos2, ypos2, zpos2, 
                      xerrpos2, yerrpos2, zerrpos2, ntracks2, ippubs2, ippuchi2bs2 );
    double nippu3, nippuerr3, xpos3, ypos3, zpos3, xerrpos3, yerrpos3, zerrpos3;
    int ntracks3;
    double ippubs3, ippuchi2bs3;
    m_util->calcIPPU( axp, AXBS, PileUpVtx, nippu2, nippu3, nippuerr3, ipmean, xpos3, ypos3, zpos3, 
                      xerrpos3, yerrpos3, zerrpos3, ntracks3, ippubs3, ippuchi2bs3  );

    //calculate min IP wrt SV
    double ipSV=0, iperrSV=0, docaSV=0, docaErrSV=0;
    if(!svertices.empty()) {
      debug()<<" secondary vertices information ============= "<<endreq;    
      Vertex mySV = svertices.at(0);
      verbose()<< " vertex size: "<<svertices.size()<<endreq;    
      verbose()<< " svpart1 "<<SVpart1->pt()/GeV<<endreq;
      if( proto != SVpart1->proto() && proto != SVpart2->proto()){ 
        m_util->calcIP( axp, &mySV, ipSV, iperrSV );
        ipSV=fabs(ipSV);
        verbose()<<" ipSV: "<<ipSV<<endreq;
        verbose()<<" svpart for doca! svpart1 pt: "<<SVpart1->pt()/GeV<<endreq;
        m_util->calcDOCAmin( axp, SVpart1, SVpart2, docaSV, docaErrSV);
        debug()<<" doca: "<<docaSV<<endreq;
      }
    }

    //Fill NTP info -------------------------------------------------------   
    ptsal       .push_back(track->likelihood());
    pgprob      .push_back(track->ghostProbability());
    ptrtyp      .push_back(trtyp);
    pID         .push_back(ID);
    pP          .push_back(P);
    pPt         .push_back(Pt);
    pPl         .push_back(Pl);
    pphi        .push_back(axp->momentum().phi());
    ptheta      .push_back(axp->momentum().theta());
    pch         .push_back((int) axp->charge());
    pip         .push_back(IP);
    pipsign     .push_back(IPsign);
    piperr      .push_back(IPerr);
    pipPU       .push_back(IPPU);
    plcs        .push_back(lcs);
    pcloneDist  .push_back(cloneDist);
    ptrackxfirst.push_back(trackxfirst);
    ptrackyfirst.push_back(trackyfirst);
    ptrackzfirst.push_back(trackzfirst);
    ptrackp     .push_back(trackp);
    pdistPhi    .push_back(distphi);
    pIPSV       .push_back(ipSV);
    pIPSVerr    .push_back(iperrSV);
    pDOCA       .push_back(docaSV);
    pDOCAerr    .push_back(docaErrSV);
    pdeta       .push_back(deta);
    pdphi       .push_back(dphi);
    pdQ         .push_back(dQ);
    ppionCombinedMass.push_back(pionCombinedMass);
    //different PU studies
    pipmean     .push_back(ipmean);
    pnippu      .push_back(nippu);
    pnippuerr   .push_back(nippuerr);
    pntracks    .push_back((double)ntracks);
    pippubs     .push_back(ippubs);
    pippuchi2bs .push_back(ippuchi2bs);
    pxpos       .push_back(xpos);
    pypos       .push_back(ypos);
    pzpos       .push_back(zpos);
    pxerrpos    .push_back(xerrpos);
    pyerrpos    .push_back(yerrpos);
    pzerrpos    .push_back(zerrpos);
    pnippu2     .push_back(nippu2);
    pnippuerr2  .push_back(nippuerr2);
    pntracks2   .push_back((double)ntracks2);
    pippubs2    .push_back(ippubs2);
    pippuchi2bs2.push_back(ippuchi2bs2);
    pxpos2      .push_back(xpos2);
    pypos2      .push_back(ypos2);
    pzpos2      .push_back(zpos2);
    pxerrpos2   .push_back(xerrpos2);
    pyerrpos2   .push_back(yerrpos2);
    pzerrpos2   .push_back(zerrpos2);
    pnippu3     .push_back(nippu3);
    pnippuerr3  .push_back(nippuerr3);
    pntracks3   .push_back((double)ntracks3);
    pippubs3    .push_back(ippubs3);
    pippuchi2bs3.push_back(ippuchi2bs3);
    pxpos3      .push_back(xpos3);
    pypos3      .push_back(ypos3);
    pzpos3      .push_back(zpos3);
    pxerrpos3   .push_back(xerrpos3);
    pyerrpos3   .push_back(yerrpos3);
    pzerrpos3   .push_back(zerrpos3);

    //PID information
    // electrons
    pPIDe.push_back( proto->info( ProtoParticle::CombDLLe, -1000.0 ) );
    pPIDNNe.push_back( proto->info( ProtoParticle::ProbNNe, -1000.0 ) );
    double eOverP  = -999.9;
    if(m_electron->set(axp)){ /// CaloElectron tool
      eOverP  = m_electron->eOverP();
    }
    pEOverP.push_back(eOverP);
    pveloch.push_back(proto->info( ProtoParticle::VeloCharge, 0.0 ));        
    // muons
    pPIDm.push_back(proto->info( ProtoParticle::CombDLLmu, -1000.0 ));
    pPIDNNm.push_back(proto->info( ProtoParticle::ProbNNmu, -1000.0 ));
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    // kaons
    pPIDk.push_back(proto->info( ProtoParticle::CombDLLk, -1000.0 ));
    pPIDNNk.push_back(proto->info( ProtoParticle::ProbNNk, -1000.0 ));
    // protons
    pPIDp.push_back(proto->info( ProtoParticle::CombDLLp, -1000.0 ));
    pPIDNNp.push_back(proto->info( ProtoParticle::ProbNNp, -1000.0 ));
    // pions
    pPIDNNpi.push_back(proto->info( ProtoParticle::ProbNNpi, -1000.0 ));

    // global flags 
    const bool inEcalACC = proto->info(ProtoParticle::InAccEcal, false);
    const bool inHcalACC = proto->info(ProtoParticle::InAccHcal, false);
    long PIDfl= 0;
    if( proto->muonPID() ) 
      if(proto->muonPID()->IsMuon()) PIDfl += 100000;
    if( muonNSH>0 )                  PIDfl +=  10000;
    if( inHcalACC )                  PIDfl +=   1000;
    if( inEcalACC )                  PIDfl +=    100;
    if( proto->info(ProtoParticle::MuonPIDStatus,0)) PIDfl += 10;
    if( proto->info(ProtoParticle::RichPIDStatus,0)) PIDfl +=  1;
    pPIDfl.push_back(PIDfl);
    
    // secondary vertex flag
    int vFlag = 0;
    ProtoParticle::ConstVector::iterator prkp;
    for(prkp = partsInSV.begin(); prkp != partsInSV.end(); prkp++) {
      if( proto == (*prkp) ) {
        vFlag = 1; //Flag as 1 the track in SV from FT package
        break;
      }
    }
    if(!svertices.empty()) {
      const SmartRefVector<Particle> partsInSVnew = 
        svertices.at(0).outgoingParticles();
      for(SmartRefVector<Particle>::const_iterator i = 
            partsInSVnew.begin(); i != partsInSVnew.end(); i++) {
        if( proto == (*i)->proto() ) {
          vFlag += 10;//add 10 if track is in new SV 
          break;
        }
      }
    }
    pvFlag.push_back(vFlag);
    
    //-------------------------------------------------------
    //debug
    if (msgLevel(MSG::DEBUG)) {
      debug() << " --- trtyp="<<trtyp<<" ID="<<ID<<" P="<<P<<" Pt="<<Pt  
              << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ 
              << " IPsig="<<(IPerr!=0 ? fabs(IP/IPerr) : -999)
              << " IPPU="<<IPPU;
      if(vFlag) debug() << "Found to be in SVTX ------------";
      debug() <<endreq;
    }

    //store MC info of tagging candidate
    if (m_EnableMC) {
      float MCP= 0.0, MCPt= 0.0, MCPl=0.0, MCphi=-999.0, MCx= -999.0, MCy= -999.0, MCz= -999.0;
      long  MCID = 0, mothID= 0, ancID = 0, bFlag = 0, xFlag = 0, MC_OS_muon_type = -1;
      
      const MCParticle* mcp = m_assoc->relatedMCP( axp );
      if( mcp ) {
        MCP = mcp->momentum().P()/GeV;
        MCPt = mcp->pt()/GeV;
        MCPl = mcp->momentum().Dot(m_BS->momentum())/GeV;
        MCphi = mcp->momentum().phi();
        MCID = mcp->particleID().pid();
	
        const MCParticle* mother = mcp->mother();
        if(mother) {
          mothID = mother->particleID().pid();
          const SmartRefVector<MCVertex>& motherVtx = mother->endVertices();
          if(motherVtx.size()) {
            MCx = motherVtx.at(0)->position().x()/mm;
            MCy = motherVtx.at(0)->position().y()/mm;
            MCz = motherVtx.at(0)->position().z()/mm;
          }
        }

        const MCParticle* ancestor = m_util->originof(mcp) ;
        ancID = ancestor->particleID().pid();
        if( ancestor->particleID().hasBottom() ) {
          bFlag = 1;  

          if(m_BS) if( ancestor == m_BS ) {
            bFlag = -1;
            debug() <<" Warning: tag from signal! ID=" << mcp->particleID().pid() 
                    <<" P="<< mcp->momentum().P() << endreq;
          }
        }
        if(m_BS) xFlag = m_util->comes_from_excitedB(m_BS, mcp);
        if(xFlag) debug()<<" comes_from_excitedB="<< xFlag << endreq;

        //Check direct mother of particle to see if it is the OSB. Excludes secondary OS muons.
        bool isTruePrimaryOS = (mcp->mother() && mcp->mother()->particleID().hasBottom());
        bool isTrueSecondaryOS = (mcp->mother() && mcp->mother()->mother() && mcp->mother()->mother()->particleID().hasBottom());
        if(isTruePrimaryOS) {
          MC_OS_muon_type = 1;
        }
        if(isTrueSecondaryOS) {
          MC_OS_muon_type = 2;
        }
	
      }//if( mcp )

      pMCID  .push_back(MCID);
      pMCP   .push_back(MCP);
      pMCPt  .push_back(MCPt);
      pMCPl  .push_back(MCPl);
      pMCphi .push_back(MCphi);
      pMCx   .push_back(MCx);
      pMCy   .push_back(MCy);
      pMCz   .push_back(MCz);
      pmothID.push_back(mothID);
      pancID .push_back(ancID);
      pxFlag .push_back(xFlag);
      pbFlag .push_back(bFlag);
      pMC_OS_muon_type.push_back(MC_OS_muon_type);
      
    }
  }

  if(pID.size() > 199) {
    warning()<<"Bursting array limit of 200.  "<<pID.size()<<endreq;
    return StatusCode::SUCCESS;
  }

  debug()<<"writing ntuple"<<endreq;
  tuple -> farray ("ID",      pID, "N", 200);
  tuple -> farray ("P",       pP, "N", 200);
  tuple -> farray ("Pt",      pPt, "N", 200);
  tuple -> farray ("Pl",      pPl, "N", 200);
  tuple -> farray ("phi",     pphi, "N", 200);
  tuple -> farray ("theta",     ptheta, "N", 200);
  tuple -> farray ("ch",      pch, "N", 200);
  tuple -> farray ("ip",      pip, "N", 200);
  tuple -> farray ("ipsign",      pipsign, "N", 200);
  tuple -> farray ("iperr",   piperr, "N", 200);
  tuple -> farray ("ipPU",    pipPU, "N", 200);
  tuple -> farray ("ipmean",  pipmean, "N", 200);
  tuple -> farray ("nippu",   pnippu, "N", 200);
  tuple -> farray ("nippuerr",pnippuerr, "N", 200);
  tuple -> farray ("tracks",   pntracks, "N", 200);
  tuple -> farray ("ippubs",   pippubs, "N", 200);
  tuple -> farray ("ippuchi2bs",pippuchi2bs, "N", 200);
  tuple -> farray ("xpos",    pxpos, "N", 200);
  tuple -> farray ("ypos",    pypos, "N", 200);
  tuple -> farray ("zpos",    pzpos, "N", 200);
  tuple -> farray ("xerrpos", pxerrpos, "N", 200);
  tuple -> farray ("yerrpos", pyerrpos, "N", 200);
  tuple -> farray ("zerrpos", pzerrpos, "N", 200);
  tuple -> farray ("nippu2",   pnippu2, "N", 200);
  tuple -> farray ("nippuerr2",pnippuerr2, "N", 200);
  tuple -> farray ("tracks2",   pntracks2, "N", 200);
  tuple -> farray ("ippubs2",   pippubs2, "N", 200);
  tuple -> farray ("ippuchi2bs2",pippuchi2bs2, "N", 200);
  tuple -> farray ("xpos2",    pxpos2, "N", 200);
  tuple -> farray ("ypos2",    pypos2, "N", 200);
  tuple -> farray ("zpos2",    pzpos2, "N", 200);
  tuple -> farray ("xerrpos2", pxerrpos2, "N", 200);
  tuple -> farray ("yerrpos2", pyerrpos2, "N", 200);
  tuple -> farray ("zerrpos2", pzerrpos2, "N", 200);
  tuple -> farray ("nippu3",   pnippu3, "N", 200);
  tuple -> farray ("nippuerr3",pnippuerr3, "N", 200);
  tuple -> farray ("tracks3",   pntracks3, "N", 200);
  tuple -> farray ("ippubs3",   pippubs3, "N", 200);
  tuple -> farray ("ippuchi2bs3",pippuchi2bs3, "N", 200);
  tuple -> farray ("xpos3",    pxpos3, "N", 200);
  tuple -> farray ("ypos3",    pypos3, "N", 200);
  tuple -> farray ("zpos3",    pzpos3, "N", 200);
  tuple -> farray ("xerrpos3", pxerrpos3, "N", 200);
  tuple -> farray ("yerrpos3", pyerrpos3, "N", 200);
  tuple -> farray ("zerrpos3", pzerrpos3, "N", 200);
  tuple -> farray ("trtyp",   ptrtyp, "N", 200);
  tuple -> farray ("lcs",     plcs, "N", 200);
  tuple -> farray ("cloneDist", pcloneDist, "N", 200);
  tuple -> farray ("trackxfirst", ptrackxfirst, "N", 200);
  tuple -> farray ("trackyfirst", ptrackyfirst, "N", 200);
  tuple -> farray ("trackzfirst", ptrackzfirst, "N", 200);
  tuple -> farray ("trackp", ptrackp, "N", 200);
  tuple -> farray ("tsal",    ptsal, "N", 200);
  tuple -> farray ("gprob",   pgprob, "N", 200);
  tuple -> farray ("distPhi", pdistPhi, "N", 200);
  tuple -> farray ("veloch",  pveloch, "N", 200);
  tuple -> farray ("EOverP",  pEOverP, "N", 200);
  tuple -> farray ("PIDe",    pPIDe, "N", 200);
  tuple -> farray ("PIDm",    pPIDm, "N", 200);
  tuple -> farray ("PIDk",    pPIDk, "N", 200);
  tuple -> farray ("PIDp",    pPIDp, "N", 200);
  tuple -> farray ("PIDfl",   pPIDfl, "N", 200);
  tuple -> farray ("PIDNNe",  pPIDNNe, "N", 200);
  tuple -> farray ("PIDNNm",  pPIDNNm, "N", 200);
  tuple -> farray ("PIDNNk",  pPIDNNk, "N", 200);
  tuple -> farray ("PIDNNp",  pPIDNNp, "N", 200);
  tuple -> farray ("PIDNNpi",  pPIDNNpi, "N", 200);
  if (m_EnableMC) {
    tuple -> farray ("MCID",    pMCID, "N", 200);
    tuple -> farray ("MCP",     pMCP, "N", 200);
    tuple -> farray ("MCPt",    pMCPt, "N", 200);
    tuple -> farray ("MCPl",    pMCPl, "N", 200);
    tuple -> farray ("MCphi",   pMCphi, "N", 200);
    tuple -> farray ("MCx",     pMCx, "N", 200);
    tuple -> farray ("MCy",     pMCy, "N", 200);
    tuple -> farray ("MCz",     pMCz, "N", 200);
    tuple -> farray ("mothID",  pmothID, "N", 200);
    tuple -> farray ("ancID",   pancID, "N", 200);
    tuple -> farray ("bFlag",   pbFlag, "N", 200);
    tuple -> farray ("xFlag",   pxFlag, "N", 200);
    tuple -> farray ("MC_OS_muon_type",   pMC_OS_muon_type, "N", 200);
  }
  tuple -> farray ("vFlag",   pvFlag, "N", 200);
  tuple -> farray ("IPSV",    pIPSV, "N", 200);
  tuple -> farray ("IPSVerr", pIPSVerr, "N", 200);
  tuple -> farray ("DOCA",    pDOCA, "N", 200);
  tuple -> farray ("DOCAerr", pDOCAerr, "N", 200);
  tuple -> farray ("dphi",     pdphi, "N", 200);
  tuple -> farray ("deta",     pdeta, "N", 200);
  tuple -> farray ("dQ",     pdQ, "N", 200);
  tuple -> farray ("pionCombinedMass",     ppionCombinedMass, "N", 200);

  if( !( tuple->write()) ) err() << "Cannot fill mytagging Ntuple" << endreq;
  ///----------------------------------------------------------------------

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}


//=============================================================================
//=============================================================================
//=============================================================================

//=============================================================================
const Particle* BTaggingAnalysis::chooseBHypothesis(const Particle::Range& parts) {

  const Particle* AXBS=0;

  if( m_BHypoCriterium == "MaximumPt" ){//need by Cheated Selection
    double maxptB=0;
    Particle::Range::const_iterator ip;
    for ( ip = parts.begin(); ip != parts.end(); ip++){
      if(!(*ip)->particleID().hasBottom()) continue;
      if(maxptB > (*ip)->pt()) continue; else maxptB=(*ip)->pt();
      AXBS = (*ip);
    }
  } else if( m_BHypoCriterium == "MinChi2" ){
    double minchi2B=99999;
    Particle::Range::const_iterator jp;
    for ( jp = parts.begin(); jp != parts.end(); jp++){
      if(!(*jp)->particleID().hasBottom()) continue;
      double chi2 = (*jp)->endVertex()->chi2PerDoF();
      if(minchi2B < chi2) continue; else minchi2B = chi2;
      AXBS = (*jp);
    }
  } else {
    err()<<"Unknown BHypoCriterium: "<<m_BHypoCriterium<<endreq;
    return NULL;
  }

  return AXBS;
}

//=============================================================================
const RecVertex::ConstVector 
BTaggingAnalysis::choosePrimary(const Particle* AXB,
                                const RecVertex::Range& verts,
                                RecVertex& RecVert) {
  const RecVertex* PV(0);
  
  RecVertex::ConstVector PileUpVtx(0); //will contain all the other primary vtx's
  debug()<<" choosePrimary Vertex with ";
  if (m_ChoosePV == "bestPV") { //choose bestPV according IRelatedPVFinder
    debug()<<"bestPV criteria";
    PV = (const RecVertex*) bestPV(AXB);
  } else {
    double kdmin = 1000000;
    RecVertex::Range::const_iterator iv;
    for(iv=verts.begin(); iv!=verts.end(); iv++){
      double var, ip, iperr;
      if(m_ChoosePV == "CheatPV") {//mc vertex
        debug()<<"CheatPV criteria";
        if (m_EnableMC) {
          var = fabs( m_BS->primaryVertex()->position().z() - (*iv)->position().z() );
          debug()<<" distance from true PV="<<var<<endreq;
        } else warning()<<"MC disable and try to use CheatPV -> Change ChoosePVCriterium!"<<endreq;
      } else if(m_ChoosePV=="PVbyIP") { //cheated sel needs this
        debug()<<"PVbyIP criteria";
        m_util->calcIP(AXB, *iv, ip, iperr);
        var=fabs(ip); 	
      } else if(m_ChoosePV=="PVbyIPs") { 
        debug()<<"PVbyIPs criteria";
        m_util->calcIP(AXB, *iv, ip, iperr);
        if(!iperr){
          err()<<"IPerror zero or nan, skip vertex: "<<iperr<<endreq;
          continue;
        }
        var=fabs(ip/iperr); 	
      } else {
        err()<<"Invalid option ChoosePVCriterium: "<<m_ChoosePV<<endreq;
        return PileUpVtx;
      }
      if( var < kdmin ) {
        kdmin = var;
        PV = (*iv);
      }    
    }    
  }//else bestPV
  debug()<<endreq;
  
  if(!PV) {      
    err() <<"PV vertex found! Skip. "<<endreq;
    return PileUpVtx;
  }
  
  //-------------------------------------------------------
  // Refit PV without the B tracks
  if(m_ReFitPVs){  
    debug()<<"Now explicitly do the PV refit !!!!! "<<endreq;
    
    RecVertex newPV(*PV);
    Particle newPart(*AXB);
    StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);     
    if(!sc) { 
      err()<<"ReFitter fails!"<<endreq; 
    } else {
      RecVert = newPV;
    }    
    debug()<<"RecVert before: z="<<PV->position().z()<<" after Refit : z="<<newPV.position().z()<<endreq;  
  } else {
    RecVert = *PV;    
  }  
  
  //-------------------------------------------------------
  //build a vector of pileup vertices 
  
  double min_chiPV=1000;
  double the_chiPV=1000;
  int nPV=0;
  
  RecVertex::Range::const_iterator jv;
  for(jv=verts.begin(); jv!=verts.end(); jv++){
    double chiPV = sqrt( 
                        pow((RecVert.position().x()-(*jv)->position().x()),2)/RecVert.covMatrix()(0,0) +
                        pow((RecVert.position().y()-(*jv)->position().y()),2)/RecVert.covMatrix()(1,1) +
                        pow((RecVert.position().z()-(*jv)->position().z()),2)/RecVert.covMatrix()(2,2)
                        );
    
    if(chiPV < min_chiPV) min_chiPV = chiPV;
    
    if(chiPV < 3) {
      the_chiPV = chiPV;      
      nPV++;      
      continue;
      
    } else    
      PileUpVtx.push_back(*jv);
  }
  
  if(min_chiPV!=the_chiPV || nPV!=1 ) {
    PileUpVtx.clear();    
    for(jv=verts.begin(); jv!=verts.end(); jv++){
      double chiPV = sqrt( 
                          pow((RecVert.position().x()-(*jv)->position().x()),2)/RecVert.covMatrix()(0,0) +
                          pow((RecVert.position().y()-(*jv)->position().y()),2)/RecVert.covMatrix()(1,1) +
                          pow((RecVert.position().z()-(*jv)->position().z()),2)/RecVert.covMatrix()(2,2)
                          );
      
      if(chiPV == min_chiPV) continue;
      else PileUpVtx.push_back(*jv);
     }   
  }

  return PileUpVtx;
}

//=============================================================================
const Particle::ConstVector 
BTaggingAnalysis::chooseCandidates(const Particle::Range& parts,
                                   Particle::ConstVector axdaugh,
                                   const RecVertex::ConstVector PileUpVtx) {

  //loop over Particles, preselect tags 
  double distphi;
  Particle::ConstVector vtags(0);
  Particle::Range::const_iterator ip, jp;
  for ( ip = parts.begin(); ip != parts.end(); ip++){
    
    const ProtoParticle* proto = (*ip)->proto();
    if( ! (*ip)->proto() )  continue;
    if( (*ip)->p() < 2000 ) continue;  
    if( (*ip)->momentum().theta()  < m_thetaMin ) continue;   
    if( (*ip)->charge() == 0 ) continue;                
    if( ! proto->track() )     continue;
    if( proto->track()->type() < 3 ) continue; 
    if( proto->track()->type() > 4 ) continue; 
    if( (*ip)->p()  > 200000 ) continue;
    if( (*ip)->pt() >  10000 ) continue;
    if( m_util->isinTree(*ip, axdaugh, distphi) ) continue ; 
    if( distphi < m_distphi_cut ) continue;

    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    m_util->calcIP( *ip, PileUpVtx, ippu, ippuerr );
    if(ippuerr) if( ippu/ippuerr < m_IPPU_cut ) continue; //preselection (eliminate from vtags all parts coming from a pileup vtx)

    bool dup=false;
    double partp= (*ip)->p();
    Particle::Range::const_iterator ik;
    for ( ik = ip+1; ik != parts.end(); ik++) {
      if((*ik)->proto() == proto ) { 
        dup=true; 
        break; 
      }
      if( (*ik)->p() == partp )  { 
       	warning()<<"Same particle momentum but different protoparticle"<<endreq;
       	dup=true; 
       	break; 
      }
    }
    if(dup) continue; 

    /////////////////////////////////////
    vtags.push_back(*ip);              // Fill container of candidates
    /////////////////////////////////////

    if (msgLevel(MSG::DEBUG)) 
      debug() <<"   part ID="<<(*ip)->particleID().pid()
              <<" p="<<(*ip)->p()/GeV
              <<" pt="<<(*ip)->pt()/GeV
              <<" PIDm="<<proto->info( ProtoParticle::CombDLLmu, 0)
              <<" PIDe="<<proto->info( ProtoParticle::CombDLLe, 0)
              <<" PIDk="<<proto->info( ProtoParticle::CombDLLk, 0)
              <<" proto="<<proto<<endreq;
  }

  return vtags;
}

//=============================================================================
const ProtoParticle::ConstVector BTaggingAnalysis::tagevent (Tuple& tuple, 
                                                             const Particle* AXBS ) {
                                                             //const Particle* AXBS, const RecVertex* PV ) { //to select ourselves the PV
  ProtoParticle::ConstVector partsInSV(0);

  bool foundb = false;
  FlavourTags*  tags = 0;
  FlavourTag* theTag = 0;

  if (exist<FlavourTags>(m_TagLocation)){//there is already something in TES
    tags = get<FlavourTags>(m_TagLocation);
    if (0!=tags) {
      
      if(tags->size()>1) {
        info()<<"FlavourTag objects in TES:"<<tags->size()
              <<"  Search for the highest pT B.."<<endreq;
      }
    
      FlavourTags::const_iterator ti;
      for( ti=tags->begin(); ti!=tags->end(); ti++ ) {
        if((*ti)->taggedB() == AXBS) {
          theTag = (*ti);
          foundb = true;
          //          debug()<<"Will use candidate with pT="<<AXBS->pt()<<" and PV = ("
          //                 <<PV->position().x()<<","<<PV->position().y()<<","<<PV->position().z()<<") "<<endreq;
        }
      }
      if(!foundb) warning()<<"B Signal mismatch! Redo tagging..."<<endreq;
    }
    
  } 

  if(!foundb ){
    info()<<"Will tag the B hypo now."<<endreq;
    theTag = new FlavourTag;
    StatusCode sc = flavourTagging()->tag( *theTag, AXBS );
    //StatusCode sc = flavourTagging()->tag( *theTag, AXBS, PV ); //to select ourselves the PV
    if (!sc) {
      err() <<"Tagging Tool returned error."<< endreq;
      delete theTag;
      return partsInSV;
    }
    if(!exist<FlavourTags>(m_TagLocation)){
      tags = new FlavourTags;
      tags->insert(theTag);
      put(tags, m_TagLocation);
      debug()<<"Inserted tags into "<<m_TagLocation<<endreq;
    }
  }

  if(!theTag) return partsInSV;

  tuple -> column ("Tag",     theTag->decision());
  tuple -> column ("Omega",   theTag->omega());
  tuple -> column ("TagCat",  theTag->category());
  tuple -> column ("TagOS",   theTag->decisionOS());
  tuple -> column ("OmegaOS", theTag->omegaOS());
  tuple -> column ("TagCatOS",theTag->categoryOS());

  debug()<<"looking at taggers --------------- "<<endreq;
  std::vector<float> tagtype, tagdecision, tagomega;
  std::vector<Tagger> taggers = theTag->taggers();
  debug()<<"tagger size: "<<taggers.size()<<endreq;
  for(size_t i=0; i<taggers.size(); ++i) {
    Tagger itagger = taggers.at(i);
    if( itagger.decision() ) {
      debug()<<"Tagger "<<itagger.type()
             <<" decision= "<<itagger.decision()<<endreq;
      tagtype.push_back(itagger.type());
      tagdecision.push_back(itagger.decision());
      tagomega.push_back(itagger.omega());
      Particle::ConstVector::iterator kp;
      if(itagger.type()==Tagger::VtxCharge) {
        for( unsigned int i=0; i!=itagger.taggerParts().size(); i++){
          partsInSV.push_back( itagger.taggerParts().at(i)->proto() );
        }
      }
    }
  }
  tuple -> farray ("TaggerType",    tagtype, "T", 10);
  tuple -> farray ("TaggerDecision",tagdecision, "T", 10);
  tuple -> farray ("TaggerOmega",   tagomega, "T", 10);

  return partsInSV;
}

//=============================================================================
StatusCode BTaggingAnalysis::FillTrigger (Tuple& tuple, const Particle* AXBS) {

  long L0Decision = -1;
  long L0DecisionByName = 0;
  long HLT1Decision = 0;
  long HLT2Decision = 0;
  if(m_requireTrigger){
    if( !exist<L0DUReport>(L0DUReportLocation::Default) ) {
      err() << "L0DUReport not found in " 
            << L0DUReportLocation::Default << endreq;
      return StatusCode::FAILURE;
    }
    L0DUReport* l0 = get<L0DUReport>(L0DUReportLocation::Default);
    L0Decision  = l0->decision();
    //channelDecisionByName
    if (l0->channelDecisionByName("Electron")) L0DecisionByName      +=         1;
    if (l0->channelDecisionByName("Photon"))   L0DecisionByName      +=        10;    
    if (l0->channelDecisionByName("Hadron"))   L0DecisionByName      +=       100;    
    if (l0->channelDecisionByName("LocalPi0")) L0DecisionByName      +=      1000;    
    if (l0->channelDecisionByName("GlobalPi0"))L0DecisionByName      +=     10000;    
    if (l0->channelDecisionByName("Muon"))     L0DecisionByName      +=    100000;    
    if (l0->channelDecisionByName("DiMuon"))   L0DecisionByName      +=   1000000;    
    if (l0->channelDecisionByName("Muon,lowMult"))  L0DecisionByName +=  10000000;    
    if (l0->channelDecisionByName("DiMuon,lowMult"))L0DecisionByName += 100000000;
    if( exist<HltDecReports>( HltDecReportsLocation::Default ) ){ 
      const HltDecReports* decReports = 
        get<HltDecReports>( HltDecReportsLocation::Default );
      HLT1Decision = decReports->decReport("Hlt1Global") ? 
        decReports->decReport("Hlt1Global")->decision() : 0 ;
      HLT2Decision = decReports->decReport("Hlt2Global") ? 
        decReports->decReport("Hlt2Global")->decision() : 0 ;
    }
  } 
  long trigger = HLT2Decision*100 +HLT1Decision*10 + L0Decision;
  debug()<<"Require trigger"<<endreq;
  debug()<<" l0: "<<L0Decision<<endreq;
  debug()<<" l0 by name: "<<L0DecisionByName<<endreq;
  debug()<<" trig: "<<trigger
         <<" (Hlt1Global and Hlt2Global can include triggers in no physics lines)"<<endreq;
  tuple -> column ("L0byName", L0DecisionByName);
  tuple -> column ("trig", trigger);

  //TIS TOS ----------------------------------------
  long L0TisTos  = 0;
  long HltTisTos = 0;
  if(m_requireTrigger) {
    if(m_requireTisTos) {
      m_L0TriggerTisTosTool->setOfflineInput( *AXBS );
      //***************************************************************
      //Fill the decision, tis and tos parametres for the L0 as a whole             
      bool decision=false;
      bool tis=0;
      bool tos=0;
      m_L0TriggerTisTosTool->setTriggerInput("L0.*Decision");
      ITriggerTisTos::TisTosTob classifiedL0Dec( m_L0TriggerTisTosTool->tisTosTrigger() );     
      decision = classifiedL0Dec.decision();
      debug()<<"L0Global: "<<decision
             <<", tis: "<<classifiedL0Dec.tis()
             <<", tos: "<<classifiedL0Dec.tos()<<endreq;
      if(classifiedL0Dec.tis()) L0TisTos += 10;
      if(classifiedL0Dec.tos()) L0TisTos +=  1;

      std::vector<std::string> l0lines = m_L0TriggerTisTosTool->triggerSelectionNames();
      for ( unsigned int i=0; i!=l0lines.size(); i++){
        m_L0TriggerTisTosTool->triggerTisTos(l0lines.at(i),decision,tis,tos);
        debug()<<" "<<l0lines.at(i)<<" dec: "<<decision
               <<", tis: "<<tis
               <<", tos:"<<tos<<endreq;
      }
      m_TriggerTisTosTool->setOfflineInput( *AXBS );
     
      //***************************************************************
      //Do the Hlt1                                                                 
      m_TriggerTisTosTool->setTriggerInput("Hlt1.*Decision");
      m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
      debug()<<"HLT1Global: "<<decision<<", tis: "<<tis<<", tos: "<<tos<<endreq;
      if(tis) HltTisTos +=  1000;
      if(tos) HltTisTos +=   100;
      if(m_saveHlt1Lines){
        //Look and save HLT1 lines
        std::vector<std::string> hltlines = m_TriggerTisTosTool->triggerSelectionNames();
        bool linehltdecision = false;
        long hltlinetrigger=0;
        for ( unsigned int i=0; i!=hltlines.size(); i++){
          linehltdecision = m_TriggerTisTosTool->hltObjectSummaries(hltlines.at(i)).size() > 0;
          ITriggerTisTos::TisTosDecision lineHLTDec = m_TriggerTisTosTool
            ->selectionTisTos( m_TriggerTisTosTool->triggerSelectionNames( hltlines.at(i)) );
          hltlinetrigger = linehltdecision*100 + lineHLTDec.tis()*10 + lineHLTDec.tos()*1;
          debug()<<" "<<hltlines.at(i)<<" dec: "<<linehltdecision
                 <<", tis: "<<lineHLTDec.tis()
                 <<", tos: "<<lineHLTDec.tos()<<endreq;
          tuple->column( hltlines.at(i), hltlinetrigger ); 
        }
      }
      
      //***************************************************************
      //Do the Hlt2                                                                 
      m_TriggerTisTosTool->setTriggerInput("Hlt2.*Decision");
      m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
      debug()<<"HLT2Global: "<<decision<<", tis: "<<tis<<", tos: "<<tos<<endreq;
      if(tis)   HltTisTos +=    10;
      if(tos)   HltTisTos +=     1;
      if(m_saveHlt2Lines) {
        //Look and save HLT2 lines
        std::vector<std::string> hlt2lines = m_TriggerTisTosTool->triggerSelectionNames();
        bool linehlt2decision = false;
        long hlt2linetrigger=0;
        for ( unsigned int i=0; i!=hlt2lines.size(); i++){
          linehlt2decision = m_TriggerTisTosTool->hltObjectSummaries(hlt2lines.at(i)).size() > 0;
          ITriggerTisTos::TisTosDecision lineHLT2Dec = 
            m_TriggerTisTosTool->selectionTisTos( m_TriggerTisTosTool
                                                  ->triggerSelectionNames( hlt2lines.at(i)) );
          hlt2linetrigger = linehlt2decision*100 + lineHLT2Dec.tis()*10 + lineHLT2Dec.tos()*1;
          debug()<<" "<<hlt2lines.at(i)<<" dec: "<<linehlt2decision
                 <<", tis: "<<lineHLT2Dec.tis()
                 <<", tos:"<<lineHLT2Dec.tos()<<endreq;
          tuple->column( hlt2lines.at(i), hlt2linetrigger ); //Hlt2lines
        }
      }
    }
  }
  tuple -> column ("L0TisTos",  L0TisTos);
  tuple -> column ("HltTisTos", HltTisTos);

  return StatusCode::SUCCESS;
}

//=============================================================================
Particle::ConstVector BTaggingAnalysis::FillSelectedB (Tuple& tuple, const Particle* AXBS) {

  Particle::ConstVector axdaugh = m_descend->descendants(AXBS);
  axdaugh.push_back(AXBS);
  debug() << "Found Selected B with "<< axdaugh.size()-1 
          << "  daughters, Mass=" << AXBS->momentum().M()/GeV
          << "  pT="<<AXBS->pt()/GeV<< endreq;

  std::vector<float> sigID, sigMothID, sigP, sigPt, sigPhi, sigMass;
  std::vector<float> sigVx, sigVy, sigVz;
  std::vector<float> sigMCID, sigMCMothID, sigMCP, sigMCPt, sigMCPhi;
  
  Particle::ConstVector::const_iterator ip;
  for ( ip = axdaugh.begin(); ip != axdaugh.end(); ip++){
    sigID.push_back((*ip)->particleID().pid());
    const Particle* mater = m_util->motherof(*ip, axdaugh);
    if(mater) sigMothID.push_back(mater->particleID().pid()); else sigMothID.push_back(0);
    sigP.push_back((*ip)->p() /GeV);
    sigPt.push_back((*ip)->pt()/GeV);
    sigPhi.push_back((*ip)->momentum().Phi());
    sigMass.push_back((*ip)->momentum().M() /GeV);

    const Vertex* endv = (*ip)->endVertex();
    if(endv){
      sigVx.push_back((*ip)->endVertex()->position().x() /mm);
      sigVy.push_back((*ip)->endVertex()->position().y() /mm);
      sigVz.push_back((*ip)->endVertex()->position().z() /mm);
    } else { 
      sigVx.push_back(0); 
      sigVy.push_back(0); 
      sigVz.push_back(20000); 
    }
    if (m_EnableMC) {
      //mc truth to use as link 
      const MCParticle* mcp = m_assoc->relatedMCP( *ip );
      if( mcp ) {
        sigMCP.push_back(mcp->p()/GeV);
        sigMCPt.push_back(mcp->pt()/GeV);
        sigMCPhi.push_back(mcp->momentum().Phi());
        sigMCID.push_back(mcp->particleID().pid());
        const MCParticle* sigmcmother = mcp->mother();
        if(sigmcmother) sigMCMothID.push_back(sigmcmother->particleID().pid());
        else sigMCMothID.push_back(0);
      }
      else {
        sigMCP.push_back(0);
        sigMCPt.push_back(0);
        sigMCPhi.push_back(0);
        sigMCID.push_back(0);
        sigMCMothID.push_back(0);
      }
    }
  
  }

  tuple -> farray ("sID",     sigID, "M", 10);
  tuple -> farray ("sMothID", sigMothID, "M", 10);
  tuple -> farray ("sP",      sigP, "M", 10);
  tuple -> farray ("sPt",     sigPt, "M", 10);
  tuple -> farray ("sPhi",    sigPhi, "M", 10);
  tuple -> farray ("sMass",   sigMass, "M", 10);
  tuple -> farray ("sVx",     sigVx, "M", 10);
  tuple -> farray ("sVy",     sigVy, "M", 10);
  tuple -> farray ("sVz",     sigVz, "M", 10);
  if (m_EnableMC) tuple -> farray ("sMCID",     sigMCID, "M", 10);
  if (m_EnableMC) tuple -> farray ("sMCMothID", sigMCMothID, "M", 10);
  if (m_EnableMC) tuple -> farray ("sMCP",      sigMCP, "M", 10);
  if (m_EnableMC) tuple -> farray ("sMCPt",     sigMCPt, "M", 10);
  if (m_EnableMC) tuple -> farray ("sMCPhi",    sigMCPhi, "M", 10);
  
  return axdaugh;
}

//=============================================================================
StatusCode BTaggingAnalysis::FillMCInfoOfB(Tuple& tuple, 
                                           const LHCb::MCParticles* mcpart) {

  //check what is the B forced to decay
  debug() << "Looking for MC"<< endreq;
  if ( !m_BS ) warning() << "Missing Signal B meson in MC!"<< endreq;

  long TrueTag = 0;
  if(m_BS) TrueTag = m_BS->particleID().pid()>0 ? 1 : -1; 
  tuple -> column ("TrueTag", TrueTag);

  int   BSosc = 0;
  float BSx = 0, BSy = 0, BSz = 0;
  if(m_BS) {
    if( m_BS->hasOscillated() ) BSosc = 1;
    const SmartRefVector<MCVertex>& BSvert = m_BS->endVertices() ;
    if( BSvert.empty() ) return StatusCode::SUCCESS;
    BSx = (BSvert.at(0))->position().x()/mm;
    BSy = (BSvert.at(0))->position().y()/mm;
    BSz = (BSvert.at(0))->position().z()/mm;
  } 
  tuple -> column ("BSosc", BSosc);
  tuple -> column ("BSx",   BSx);
  tuple -> column ("BSy",   BSy);
  tuple -> column ("BSz",   BSz);
  if(m_BS) {
    tuple -> column ("kx", m_BS->originVertex()->position().x()/mm);
    tuple -> column ("ky", m_BS->originVertex()->position().y()/mm);
    tuple -> column ("kz", m_BS->originVertex()->position().z()/mm);
  } else {
    tuple -> column ("kx", 0.);
    tuple -> column ("ky", 0.);
    tuple -> column ("kz", 0.);
  }

  //look for opposite B
  const MCParticle* BO = 0;
  MCParticles::const_iterator imc;
  double maxBP = -1;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( !(*imc)->particleID().hasBottom() ) continue;
    if( (*imc) == m_BS) continue;
    int aid  = (*imc)->particleID().abspid();
    if( aid==511 || aid==521 || aid==531 || aid==541
        || (aid>5000 && aid<5599) ) {
      bool close2BS = false;
      if(m_BS) if(fabs((*imc)->originVertex()->position().z() 
                       - m_BS->originVertex()->position().z()) 
                  /mm < 1.0) close2BS = true;
      if( close2BS || (!m_BS) ) {
        if( maxBP < (*imc)->momentum().P() ) {
          maxBP = (*imc)->momentum().P();
          BO = (*imc);
        }
      }
    }
  }
  if ( !BO ) warning() << "Missing Opposite B meson in MC!"<< endreq;
 
  //debug()<<"SIGNAL B:"<<endreq; m_debug -> printTree(BS);
  //if(BO)debug()<<"OPPOSITE B (TAGGING B):"<<endreq; m_debug->printTree(BO);

  int BOosc = 0;
  long BOID =0;
  float BOx=-10000., BOy=-10000., BOz=-10000.;
  float BOP=-1., BOthe=10000., BOphi=10000.;
  if(BO) {
    if( BO->hasOscillated() ) BOosc = 1; 
    debug()<< "BOppost: " <<BO->particleID().pid()<<" osc? "<<BOosc<<endreq;
    BOID = BO->particleID().pid();
    BOP  = BO->momentum().P()/GeV;
    BOthe= BO->momentum().Theta();
    BOphi= BO->momentum().Phi();
    const SmartRefVector<MCVertex>& BOvert = BO->endVertices() ;
    if( BOvert.empty() ) return StatusCode::SUCCESS;
    BOx = (BOvert.at(0))->position().x()/mm;
    BOy = (BOvert.at(0))->position().y()/mm;
    BOz = (BOvert.at(0))->position().z()/mm;
  } 
  tuple -> column ("BOID",  BOID);
  tuple -> column ("BOP",   BOP);
  tuple -> column ("BOthe", BOthe);
  tuple -> column ("BOphi", BOphi);
  tuple -> column ("BOx",   BOx);
  tuple -> column ("BOy",   BOy);
  tuple -> column ("BOz",   BOz);
  tuple -> column ("BOosc", BOosc);

  return StatusCode::SUCCESS;

}

//=============================================================================
void BTaggingAnalysis::FillSeedInfo(Tuple& tuple, const RecVertex* RecVert,
                                    const Particle::ConstVector& vtags,
                                    std::vector<Vertex>& svertices,
                                    const Particle *&SVpart1, 
                                    const Particle *&SVpart2 ) { 
			   
  //look for a secondary Vtx due to opposite B (inclusive sec vertex)
  std::vector<float> 
    pBOVtx_x(0),  pBOVtx_y(0), pBOVtx_z(0),   pBOVtx_chi2(0),  pBOVtx_zerr(0),
    pSecVtx_x(0), pSecVtx_y(0), pSecVtx_z(0), pSecVtx_chi2(0), pSecVtx_zerr(0),
    pSecVtx_pt1(0), pSecVtx_pt2(0);
  

  svertices.clear();
  SVpart1=0;
  SVpart2=0; 
  
  std::vector<Vertex>::const_iterator isv;
  if(m_svtool) svertices = m_svtool -> buildVertex( *RecVert, vtags ); // SVertexOneSeedTool from FT

  debug() << "  Look seed vertex position " <<endreq;
  debug() << "  vertices: " <<svertices.size()<<endreq;

  Vertex tmpseed, tmp_BOvtx;
  int seeds=0;
  //save NEW seed vertex positions
  for (isv=svertices.begin(); isv!=svertices.end(); ++isv) {
    
    //We keep only the first two tracks, which are the ones which 
    //makes the seed. The vertexvector inherits from an old way to
    //to obtain all the possibles seed 

    //the svertices.clear is to put the vector at zero and breaks because is the only one
    //the svertices.at(0) put the seed, fitted again with the 2 tracks, breaks because is the only one

    if (seeds>0) break;//select only first seed
    seeds++;

    Particle::ConstVector Pfit = (*isv).outgoingParticlesVector();
    debug() << "seed 2 tracks, vtx "<< Pfit.size()<<endreq;
    if(Pfit.size()==0) {
      svertices.clear();
      break;
    }

    SVpart1=Pfit.at(0);
    SVpart2=Pfit.at(1);
    m_fitter->fit(tmpseed, *SVpart1,*SVpart2);
    tmpseed.addToOutgoingParticles(SVpart1);
    tmpseed.addToOutgoingParticles(SVpart2);

    pSecVtx_x.push_back(tmpseed.position().x()/mm);
    pSecVtx_y.push_back(tmpseed.position().y()/mm);
    pSecVtx_z.push_back(tmpseed.position().z()/mm);
    pSecVtx_zerr.push_back(tmpseed.covMatrix().At(2,2));
    debug()<<"seed xposition: "<<tmpseed.position().x()/mm<<endreq;
    debug()<<"     yposition: "<<tmpseed.position().y()/mm<<endreq;
    debug()<<"     zposition: "<<tmpseed.position().z()/mm<<endreq;
    pSecVtx_chi2.push_back(tmpseed.chi2PerDoF());
    debug()<<"Pfit.at(0) pt: "<<Pfit.at(0)->pt()/GeV<<endreq;
    pSecVtx_pt1.push_back(Pfit.at(0)->pt()/GeV);
    pSecVtx_pt2.push_back(Pfit.at(1)->pt()/GeV);
    svertices.at(0)=tmpseed;

    m_fitter->fit(tmp_BOvtx, Pfit);
    pBOVtx_x.push_back(tmp_BOvtx.position().x()/mm);
    pBOVtx_y.push_back(tmp_BOvtx.position().y()/mm);
    pBOVtx_z.push_back(tmp_BOvtx.position().z()/mm);
    pBOVtx_zerr.push_back(tmp_BOvtx.covMatrix().At(2,2));
    pBOVtx_chi2.push_back(tmp_BOvtx.chi2PerDoF());
    break;
  }

  tuple -> farray ("SecVtx_pt1", pSecVtx_pt1, "V", 100);
  tuple -> farray ("SecVtx_pt2", pSecVtx_pt2, "V", 100);
  tuple -> farray ("SecVtx_x",   pSecVtx_x, "V", 100);
  tuple -> farray ("SecVtx_y",   pSecVtx_y, "V", 100);
  tuple -> farray ("SecVtx_z",   pSecVtx_z, "V", 100);
  tuple -> farray ("SecVtx_zerr",pSecVtx_zerr, "V", 100);
  tuple -> farray ("SecVtx_chi2",pSecVtx_chi2, "V", 100);

  // fill the info of the BO opposite vertex (seed + additional tracks)
  tuple -> farray ("BOVtx_x",    pBOVtx_x, "V", 100);
  tuple -> farray ("BOVtx_y",    pBOVtx_y, "V", 100);
  tuple -> farray ("BOVtx_z",    pBOVtx_z, "V", 100);
  tuple -> farray ("BOVtx_zerr", pBOVtx_zerr, "V", 100);
  tuple -> farray ("BOVtx_chi2", pBOVtx_chi2, "V", 100);
}

//=============================================================================
double BTaggingAnalysis::GetInvariantMass(double MA, Gaudi::LorentzVector PA, double MB, Gaudi::LorentzVector PB)
{
  double aPdot  = PA.px()*PB.px() + PA.py()*PB.py() + PA.pz()*PB.pz();
  double AA = 2*( (PA.E()*PB.E()) - aPdot);
  double msq = ((MA*MA) + (MB*MB) + AA );
  double massInv= 0.0;
  if(msq > 0.0 ) massInv = sqrt(msq);
  return massInv;
}



//=============================================================================
StatusCode BTaggingAnalysis::finalize() {  return DVAlgorithm::finalize(); }

