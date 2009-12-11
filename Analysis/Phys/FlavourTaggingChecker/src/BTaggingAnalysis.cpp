// Include files 
#include "BTaggingAnalysis.h"

//ntuple
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingAnalysis
//
// Authors: Marco Musy, Marc Grabalosa
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BTaggingAnalysis );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTaggingAnalysis::BTaggingAnalysis(const std::string& name,
                                   ISvcLocator* pSvcLocator):
  DVAlgorithm(name, pSvcLocator), 
  m_electron(0), 
  m_forcedBtool(0),
  m_bkgCategory(0) ,
  m_pLifetimeFitter(0),
  m_util(0),
  m_vtxtool(0),
  m_descend(0),
  m_TriggerTisTosTool(0),
  m_assoc(0) 
{
  declareProperty( "SecondaryVertexType",m_SVtype      = "SVertexTool" );
  declareProperty( "TagOutputLocation",  m_TagLocation = FlavourTagLocation::Default );
  declareProperty( "AssociatorInputData",m_setInputData );
  declareProperty( "UseMCTrueFlavour",   m_UseMCTrueFlavour = true );
  declareProperty( "RequireTrigger",     m_requireTrigger   = true );

  declareProperty( "IPPU_cut",     m_IPPU_cut    = 3.0 );
  declareProperty( "distphi_cut",  m_distphi_cut = 0.005 );
  declareProperty( "thetaMin_cut", m_thetaMin    = 0.012 );

  m_setInputData.clear();
  m_setInputData.push_back( "Phys/BTaggingAnalysis" );
  
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
  }
  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }//becomes obsolete but still used at present

  m_bkgCategory = tool<IBackgroundCategory>("BackgroundCategory", this);
  if(!m_bkgCategory){
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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingAnalysis::execute() {

  setFilterPassed( false );
   
  //new ntuple
  debug()<<"Booking ntuple"<<endreq;
  Tuple tuple = nTuple("mytagging","mytagging",CLID_ColumnWiseTuple);
    
  //----------------------------------------------------------------------
  long L0Decision = -1;
  long HLTDecision = 0;
  if(m_requireTrigger){
    if( !exist<L0DUReport>(L0DUReportLocation::Default) ) {
      err() << "L0DUReport not found in " 
            << L0DUReportLocation::Default << endreq;
      return StatusCode::SUCCESS;
    }
    L0DUReport* l0 = get<L0DUReport>(L0DUReportLocation::Default);
    L0Decision  = l0->decision();
    if( exist<HltDecReports>( HltDecReportsLocation::Default ) ){ 
      const HltDecReports* decReports = 
        get<HltDecReports>( HltDecReportsLocation::Default );
      HLTDecision = decReports->decReport("HltGlobal") ? 
        decReports->decReport("HltGlobal")->decision() : 0 ;
    }
  } 
  long trigger = HLTDecision*10 + L0Decision;
  debug()<<"trig: "<<trigger<<endreq;
  tuple -> column ("trig", trigger);
    
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
  long kType = 0;
  for ( unsigned int icoll=0; icoll!=mcColls.size(); icoll++ ) {
    if( mcColls.at(icoll)->isSignal() ) {
      kType = mcColls.at(icoll)->processType();
      debug() << "Collision process type: " << kType << endreq;
    }
  }
  tuple -> column ("kType", kType);
  tuple -> column ("Run", evt->runNumber());
  tuple -> column ("Event", (long) evt->evtNumber());
  tuple -> column ("evType", gene->evType());
  

  //----------------------------------------------------------------------
  //PhysDeskTop
  const Particle::ConstVector& parts = desktop()->particles();
  const RecVertex::Container*  verts = desktop()->primaryVertices();
  debug() << "  Nr of rec. Vertices: " << verts->size() 
          << "  Nr of rec. Particles: " << parts.size() <<endreq;
  desktop()->saveDesktop();

  //----------------------------------------------------------------------
  //look for B in selection:
  debug() << "-------- Signal:" << endreq;

  const Particle* AXBS = 0;
  double maxptB=0;
  Particle::ConstVector::const_iterator ip;
  for ( ip = parts.begin(); ip != parts.end(); ip++){
    if(!(*ip)->particleID().hasBottom()) continue;
    //Take ONLY the highest pt B hypothesis if more than 1 exists
    if(maxptB > (*ip)->pt()) continue; else maxptB=(*ip)->pt();
    AXBS = (*ip);
  }
  if(!AXBS) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  bool isBd = false;
  bool isBs = false;
  bool isBu = false;
  double BSmass= 0, BSthe= 0, BSphi= 0;
  Gaudi::LorentzVector ptotBS(0.0,0.0,0.0,0.0);
  Particle::ConstVector axdaugh = m_descend->descendants(AXBS);
  axdaugh.push_back(AXBS);
  if( AXBS->particleID().hasDown() )    isBd = true;
  if( AXBS->particleID().hasStrange() ) isBs = true;
  if( AXBS->particleID().hasUp() )      isBu = true;
  ptotBS = AXBS->momentum();
  BSmass = ptotBS.M()/Gaudi::Units::GeV;
  BSthe  = ptotBS.theta();
  BSphi  = ptotBS.phi();
  debug() << "Found Selected B with "<< axdaugh.size()-1 
          << "  daughters, B-Mass=" << BSmass 
          << "  pT="<<AXBS->pt()/Gaudi::Units::GeV<< endreq;

  //TIS TOS ----------------------------------------
  long L0TisTos  = 0;
  long HltTisTos = 0;
  if(m_requireTrigger) {
    m_TriggerTisTosTool->setOfflineInput( *AXBS );
    // L0 
    bool decisionL0, L0Tis, L0Tos;
    m_TriggerTisTosTool->setTriggerInput("Hlt1L0*Decision");
    std::vector<std::string> vs = m_TriggerTisTosTool->triggerSelectionNames();
    //Fill the decision, tis and tos parametres for the L0 as a whole
    m_TriggerTisTosTool->selectionTisTos(vs,decisionL0,L0Tis,L0Tos);
    // Hlt1
    bool decisionHltAlleys, alleysTis, alleysTos;
    m_TriggerTisTosTool->setTriggerInput("Hlt1*Decision");
    //Fill the decision, tis and tos parametres for the Hlt1 as a whole
    m_TriggerTisTosTool->triggerTisTos(decisionHltAlleys,alleysTis,alleysTos);
    // Hlt2
    bool decisionHltSelB, selBTis, selBTos;
    m_TriggerTisTosTool->setTriggerInput("Hlt2*Decision");
    //Fill the decision, tis and tos parametres for the Hlt2 as a whole
    m_TriggerTisTosTool->triggerTisTos(decisionHltSelB, selBTis, selBTos);

    if(L0Tis) L0TisTos += 10;
    if(L0Tos) L0TisTos +=  1;

    if(alleysTis) HltTisTos +=  1000;
    if(alleysTos) HltTisTos +=   100;
    if(selBTis)   HltTisTos +=    10;
    if(selBTos)   HltTisTos +=     1;
  }
  tuple -> column ("L0TisTos", L0TisTos);
  tuple -> column ("HltTisTos", HltTisTos);
  
  //fill signal particle block:
  debug()<<"fill sig block"<<endreq;
  std::vector<float> sigID, sigMothID, sigP, sigPt, sigPhi, sigMass;
  std::vector<float> sigMCID, sigMCMothID, sigMCP, sigMCPt, sigMCPhi, sigVx, sigVy, sigVz;
  
  for ( ip = axdaugh.begin(); ip != axdaugh.end(); ip++){
    sigID.push_back((*ip)->particleID().pid());
    const Particle* mater   = m_util->motherof(*ip, parts);
    if(mater) sigMothID.push_back(mater->particleID().pid()); else sigMothID.push_back(0);
    sigP.push_back((*ip)->p() /Gaudi::Units::GeV);
    sigPt.push_back((*ip)->pt()/Gaudi::Units::GeV);
    sigPhi.push_back((*ip)->momentum().Phi());
    sigMass.push_back((*ip)->momentum().M() /Gaudi::Units::GeV);

    const Vertex* endv = (*ip)->endVertex();
    if(endv){
      sigVx.push_back((*ip)->endVertex()->position().x() /Gaudi::Units::mm);
      sigVy.push_back((*ip)->endVertex()->position().y() /Gaudi::Units::mm);
      sigVz.push_back((*ip)->endVertex()->position().z() /Gaudi::Units::mm);
    } else { sigVx.push_back(0); sigVy.push_back(0); sigVz.push_back(20000); }

    //mc truth
    const MCParticle* mcp = m_assoc->relatedMCP( *ip, LHCb::MCParticleLocation::Default );
    if( mcp ) {
      sigMCID.push_back(mcp->particleID().pid());
      if( mcp->mother() ) sigMCMothID.push_back(mcp->mother()->particleID().pid()); 
      else sigMCMothID.push_back(0);
      sigMCP.push_back(mcp->p()/Gaudi::Units::GeV);
      sigMCPt.push_back(mcp->pt()/Gaudi::Units::GeV);
      sigMCPhi.push_back(mcp->momentum().Phi());
    } else { 
      sigMCID.push_back(0); sigMCMothID.push_back(0); 
      sigMCP.push_back(0); sigMCPt.push_back(0); sigMCPhi.push_back(0); 
    }
  }
  
  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  const MCParticle* BS = m_forcedBtool->forcedB();
  if ( !BS ) {
    warning() << "Missing Signal B meson in MC!"<< endreq;
    //return StatusCode::SUCCESS;                      
  }

  //look for opposite
  const MCParticle* BO = 0;
  MCParticles::const_iterator imc;
  double maxBP = -1;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( !(*imc)->particleID().hasBottom() ) continue;
    if( (*imc) == BS) continue;
    int aid  = (*imc)->particleID().abspid();
    if( aid==511 || aid==521 || aid==531 || aid==541
        || (aid>5000 && aid<5599) ) {
      bool close2BS = false;
      if(BS) if(fabs((*imc)->originVertex()->position().z() 
                     - BS->originVertex()->position().z()) 
                /Gaudi::Units::mm < 1.0)
        close2BS = true;
	
      if( close2BS || (!BS) ) {
        if( maxBP < (*imc)->momentum().P() ) {
          maxBP = (*imc)->momentum().P();
          BO = (*imc);
        }
      }
    }
  }

  if(BS) {
    tuple -> column ("kx", BS->originVertex()->position().x()/Gaudi::Units::mm);
    tuple -> column ("ky", BS->originVertex()->position().y()/Gaudi::Units::mm);
    tuple -> column ("kz", BS->originVertex()->position().z()/Gaudi::Units::mm);
  } else {
    tuple -> column ("kx", 0.);
    tuple -> column ("ky", 0.);
    tuple -> column ("kz", 0.);
  }
  
  ////////////////////////////////////////////////////
  if ( !BO ) warning() << "Missing Opposite B meson in MC!"<< endreq;
 
  ////////////////////////////////////////////////////
  //debug()<<"SIGNAL B:"<<endreq; m_debug -> printTree(BS);
  //debug()<<"OPPOSITE B (TAGGING B):"<<endreq; m_debug -> printTree(BO);

  //-------------------- OFFICIAL TAG of the Event --------------------
  bool foundb = false;
  ProtoParticle::ConstVector partsInSV;
  FlavourTags*  tags = new FlavourTags;
  FlavourTag* theTag = new FlavourTag;
  if (exist<FlavourTags>(m_TagLocation)){//there is already something in TES
    tags = get<FlavourTags>(m_TagLocation);
    if(tags->size()>1) info()<<"FlavourTag objects in TES:"<<tags->size()
			     <<"  Search for the highest pT B.."<<endreq;
    FlavourTags::const_iterator ti;
    for( ti=tags->begin(); ti!=tags->end(); ti++ ) {
      if((*ti)->taggedB() == AXBS) {
        theTag = (*ti);
        foundb = true;
        debug()<<"Will use candidate with pT="<<AXBS->pt()<<endreq;
      }
    }
    if(!foundb) warning()<<"B Signal mismatch! Redo tagging..."<<endreq;
  } 

  if(!foundb ){
    info()<<"Will tag the B hypo now."<<endreq;
    StatusCode sc = flavourTagging()->tag( *theTag, AXBS );
    if (!sc) {
      err() <<"Tagging Tool returned error."<< endreq;
      delete theTag;
      return StatusCode::SUCCESS;
    } 
    if(!exist<FlavourTags>(m_TagLocation)){
      tags->insert(theTag);
      put(tags, m_TagLocation);
      debug()<<"Inserted tags into "<<m_TagLocation<<endreq;
    }
  }
  tuple -> column ("Tag",     theTag->decision());
  tuple -> column ("Omega",   theTag->omega());
  tuple -> column ("TagCat",  theTag->category());
  tuple -> column ("TagOS",   theTag->decisionOS());
  tuple -> column ("OmegaOS", theTag->omegaOS());
  tuple -> column ("TagCatOS",theTag->categoryOS());

  long TrueTag = 0;
  if(BS) TrueTag = BS->particleID().pid()>0 ? 1 : -1; 
  if(!m_UseMCTrueFlavour) TrueTag = AXBS->particleID().pid()>0 ? 1 : -1; 
  tuple -> column ("TrueTag", TrueTag);

  debug()<<"looking taggers"<<endreq;
  std::vector<float> tagtype, tagdecision, tagomega;
  std::vector<Tagger> taggers = theTag->taggers();
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

  //Background category
  int bcat = -1;
  if(AXBS) if(m_bkgCategory) if( ! AXBS->isBasicParticle() ){
    bcat = (int) m_bkgCategory->category(AXBS);
    debug() << "Result of BackgroundCategory is: " << bcat << endreq;
  }
  tuple -> column ("bkgCat", bcat);

  info() << "BTAGGING MON "<< std::setw(3) << trigger 
         << std::setw(4) << TrueTag << std::setw(4) << bcat << endreq;
  //----------------------------------------------------------------------------

  //true Signal B
  int BSosc = 0;
  float BSx=0., BSy=0., BSz=0.;
  if(BS) {
    if( BS->hasOscillated() ) BSosc = 1;
    debug()<< "BSignal: " <<BS->particleID().pid()<<" osc? "<<BSosc<<endreq;
    const SmartRefVector<MCVertex>& BSvert = BS->endVertices() ;
    if( BSvert.empty() ) return StatusCode::SUCCESS;
    BSx = (BSvert.at(0))->position().x()/Gaudi::Units::mm;
    BSy = (BSvert.at(0))->position().y()/Gaudi::Units::mm;
    BSz = (BSvert.at(0))->position().z()/Gaudi::Units::mm;
  } 
  tuple -> column ("BSosc", BSosc);
  tuple -> column ("BSx", BSx);
  tuple -> column ("BSy", BSy);
  tuple -> column ("BSz", BSz);

  //true opposite B
  int BOosc = 0;
  long BOID =0;
  float BOx=-10000., BOy=-10000., BOz=-10000.;
  float BOP=-1., BOthe=10000., BOphi=10000.;
  if(BO) {
    if( BO->hasOscillated() ) BOosc = 1; 
    debug()<< "BOppost: " <<BO->particleID().pid()<<" osc? "<<BOosc<<endreq;
    BOID = BO->particleID().pid();
    BOP  = BO->momentum().P()/Gaudi::Units::GeV;
    BOthe= BO->momentum().Theta();
    BOphi= BO->momentum().Phi();
    const SmartRefVector<MCVertex>& BOvert = BO->endVertices() ;
    if( BOvert.empty() ) return StatusCode::SUCCESS;
    BOx = (BOvert.at(0))->position().x()/Gaudi::Units::mm;
    BOy = (BOvert.at(0))->position().y()/Gaudi::Units::mm;
    BOz = (BOvert.at(0))->position().z()/Gaudi::Units::mm;
  } 
  tuple -> column ("BOID", BOID);
  tuple -> column ("BOP", BOP);
  tuple -> column ("BOthe", BOthe);
  tuple -> column ("BOphi", BOphi);
  tuple -> column ("BOx", BOx);
  tuple -> column ("BOy", BOy);
  tuple -> column ("BOz", BOz);
  tuple -> column ("BOosc", BOosc);

  //if the prim vtx is not provided by the user,
  //choose as primary vtx the one with smallest IP wrt B signal
  //this is a guess for the actual PV chosen by the selection.
  const RecVertex* RecVert = 0; 
  long krec = 0;
  RecVertex::Container::const_iterator iv;
  double kdmin = 1000000;
  for(iv=verts->begin(); iv!=verts->end(); iv++){
    krec++;
    double ip, iperr;
    m_util->calcIP(AXBS, *iv, ip, iperr);
    debug() << "Vertex IP="<< ip <<" iperr="<<iperr<<endreq;
    if(iperr) if( fabs(ip/iperr) < kdmin ) {
      kdmin = fabs(ip/iperr);
      RecVert = (*iv);
    }     
  }
  tuple -> column ("krec", krec);

  if( !RecVert ) {
    err() <<"No Reconstructed Vertex!! Skip." <<endreq;
    return StatusCode::SUCCESS;
  }    

  //build VertexVector of pileup ----------------------------
  RecVertex::ConstVector PileUpVtx(0);
  for( iv=verts->begin(); iv!=verts->end(); iv++){
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug()<<"Pileup Vtx z="<< (*iv)->position().z()/Gaudi::Units::mm <<endreq;
  }
  tuple -> column ("RVx", RecVert->position().x()/Gaudi::Units::mm);
  tuple -> column ("RVy", RecVert->position().y()/Gaudi::Units::mm);
  tuple -> column ("RVz", RecVert->position().z()/Gaudi::Units::mm);

  //lifetime fitter
  double ct=0., ctErr=0., ctChi2=0.;
  for ( ip = parts.begin(); ip != parts.end(); ip++){
    if(!(*ip)->particleID().hasBottom()) continue;
    m_pLifetimeFitter->fit(*RecVert, *AXBS, ct, ctErr, ctChi2);
    tuple -> column ("tau", ct/Gaudi::Units::picosecond);
    tuple -> column ("tauErr", ctErr/Gaudi::Units::picosecond);
    tuple -> column ("ctChi2", ctChi2);
    break;
  }

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

    Particle::ConstVector::const_iterator ik;
    for ( ik = vtags.begin(); ik != vtags.end(); ik++)
      if((*ik)->proto() == (*ip)->proto() ) continue;//kill duplicates

    ///////////////////////////////////////
    vtags.push_back(*ip);               // Fill container of candidates
    /////////////////////////////////////

    if (msgLevel(MSG::DEBUG)) 
      debug() <<"part ID="<<(*ip)->particleID().pid()
 	      <<" p="<<(*ip)->p()/Gaudi::Units::GeV
 	      <<" PIDm="<<(*ip)->proto()->info( ProtoParticle::CombDLLmu, 0)
 	      <<" PIDe="<<(*ip)->proto()->info( ProtoParticle::CombDLLe, 0)
 	      <<" PIDk="<<(*ip)->proto()->info( ProtoParticle::CombDLLk, 0)
 	      <<endreq;

  }

  ///------------------------------------------------------- Fill Tagger info
  std::vector<float> pID, pP, pPt, pphi, pch, pip, piperr, 
    pipPU, pipC, pipChi2, pipPUC, pipPUChi2;
  std::vector<float> ptrtyp, plcs, ptsal, pdistPhi, pveloch, pEOverP, 
    pPIDe, pPIDm, pPIDk, pPIDp,pPIDfl;
  std::vector<float> pMCID, pMCP, pMCPt, pMCphi, pMCz, 
    pmothID, pancID, pbFlag, pxFlag, pvFlag;

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

    ptsal.push_back(track->likelihood());
    
    //calculate signed IP wrt RecVert
    double IP, IPerr;
    if(!(axp->particleID().hasBottom())) m_util->calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr) continue;                                      //preselection cut
    //calculate min IP wrt all pileup vtxs 
    double IPPU = 10000;
    double ipval, iperr, IPc=-1, IPchi2=-1;
    m_util->calcIP( axp, PileUpVtx, ipval, iperr );
    if(iperr) IPPU=ipval/iperr;
  
    //calculate the IP chi2
    bool testip = distanceCalculator()->distance( axp, RecVert, IPc, IPchi2 );
    if( !testip ){
      IPc=-1;
      IPchi2=-1;
    }
    double ippumin = 100000.0, ippuchi=0.;
    RecVertex::ConstVector::const_iterator iv;
    for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
      double ipx, chi;
      StatusCode sc = 0;
      sc = distanceCalculator()->distance(axp, *iv, ipx, chi);
      if( sc ) if( ipx < ippumin ) {
        ippumin = ipx;
        ippuchi = chi;
      } 
    }

    //Fill NTP info -------------------------------------------------------   
    ptrtyp.push_back(trtyp);
    pID   .push_back(ID);
    pP    .push_back(P);
    pPt   .push_back(Pt);
    pphi  .push_back(axp->momentum().phi());
    pch   .push_back((int) axp->charge());
    pip   .push_back(IP);
    piperr.push_back(IPerr);
    pipPU .push_back(IPPU);
    pipC  .push_back(IPc);
    pipChi2  .push_back( IPchi2);
    pipPUC   .push_back(ippumin);
    pipPUChi2.push_back(ippuchi);
    plcs     .push_back(lcs);
    pdistPhi .push_back(distphi);
    
    // electrons
    pPIDe.push_back( proto->info( ProtoParticle::CombDLLe, -1000.0 ) );
    double eOverP  = -999.9;
    if(m_electron->set(axp)){ /// CaloElectron tool
      eOverP  = m_electron->eOverP();
    }
    pEOverP.push_back(eOverP);
    pveloch.push_back(proto->info( ProtoParticle::VeloCharge, 0.0 ));        
    // muons
    pPIDm.push_back(proto->info( ProtoParticle::CombDLLmu, -1000.0 ));
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    // kaons
    pPIDk.push_back(proto->info( ProtoParticle::CombDLLk, -1000.0 ));
    // protons
    pPIDp.push_back(proto->info( ProtoParticle::CombDLLp, -1000.0 ));

    // global flags 
    const bool inEcalACC = proto->info(ProtoParticle::InAccEcal, false);
    const bool inHcalACC = proto->info(ProtoParticle::InAccHcal, false);
    long PIDfl= 0;
    if( proto->muonPID() ) 
      if(proto->muonPID()->IsMuon()) PIDfl += 100000;
    if( muonNSH>0 )                  PIDfl +=  10000;
    if( inHcalACC )                  PIDfl +=   1000;
    if( inEcalACC )                  PIDfl +=    100;
    if( proto->info(ProtoParticle::MuonPIDStatus, 0) ) PIDfl += 10;
    if( proto->info(ProtoParticle::RichPIDStatus, 0) ) PIDfl +=  1;
    pPIDfl.push_back(PIDfl);
    
    // secondary vertex flag
    int vFlag = 0;
    ProtoParticle::ConstVector::iterator prkp;
    for(prkp = partsInSV.begin(); prkp != partsInSV.end(); prkp++) {
      if( axp->proto() == (*prkp) ) {
        vFlag = 1;
        break;
      }
    }
    pvFlag.push_back(vFlag);
    
    //-------------------------------------------------------
    if (msgLevel(MSG::DEBUG)) {
      debug() << " --- trtyp="<<trtyp<<" ID="<<ID<<" P="<<P<<" Pt="<<Pt <<endreq;
      debug() << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
      debug() << " IPsig="<<fabs(IP/IPerr) << " IPPU="<<IPPU <<endreq;
      //debug() << " sigPhi="<<distphi<< " lcs " << lcs << endreq;
      //debug()<< " mNSH="<<muonNSH<< " vFlag="<<vFlag<<endreq;
      if(vFlag) debug() << "Found to be in VTX "<<endreq;
    }


    //store MC info 
    long MCID  = 0;
    float MCP  = 0.0;
    float MCPt = 0.0;
    float MCphi= -999.0;
    float MCz  = -999.0;
    long mothID= 0;
    long ancID = 0;
    long bFlag = 0;
    long xFlag = 0;

    const MCParticle* mcp = m_assoc->relatedMCP( axp, LHCb::MCParticleLocation::Default );
    if( mcp ) {
      MCP = mcp->momentum().P()/Gaudi::Units::GeV;
      MCPt = mcp->pt()/Gaudi::Units::GeV;
      MCphi = mcp->momentum().phi();
      MCID = mcp->particleID().pid();

      const MCParticle* mother = mcp->mother();
      if(mother) {
        mothID = mother->particleID().pid();
        const SmartRefVector<MCVertex>& motherVtx = mother->endVertices();
        if(motherVtx.size()) 
          MCz = (motherVtx.at(0))->position().z()/Gaudi::Units::mm;
      }

      const MCParticle* ancestor = m_util->originof(mcp) ;
      ancID = ancestor->particleID().pid();
      if( ancestor->particleID().hasBottom() ) {
        bFlag = 1;  
        if(BS) if( ancestor == BS ) {
          bFlag = -1;
          debug() <<" Warning: tag from signal! ID=" << mcp->particleID().pid() 
                  <<" P="<< mcp->momentum().P() << endreq;
        }
      }

      if(BS) xFlag = m_util->comes_from_excitedB(BS, mcp);
      if(xFlag) debug()<<" comes_from_excitedB="<< xFlag << endreq;

    }//if( mcp )

    pMCID .push_back(MCID);
    pMCP  .push_back(MCP);
    pMCPt .push_back(MCPt);
    pMCphi.push_back(MCphi);
    pMCz.push_back(MCz);
    pmothID.push_back(mothID);
    pancID.push_back(ancID);
    pxFlag.push_back(xFlag);
    pbFlag.push_back(bFlag);
    
    //---------------
  }

  debug()<<"writing ntuple"<<endreq;

  tuple -> farray ("sID", sigID, "M", 10);
  tuple -> farray ("sMothID", sigMothID, "M", 10);
  tuple -> farray ("sP", sigP, "M", 10);
  tuple -> farray ("sPt", sigPt, "M", 10);
  tuple -> farray ("sPhi", sigPhi, "M", 10);
  tuple -> farray ("sMass", sigMass, "M", 10);
  tuple -> farray ("sMCID", sigMCID, "M", 10);
  tuple -> farray ("sMCMothID", sigMCMothID, "M", 10);
  tuple -> farray ("sMCP", sigMCP, "M", 10);
  tuple -> farray ("sMCPt", sigMCPt,"M", 10);
  tuple -> farray ("sMCPhi", sigMCPhi,"M", 10);
  tuple -> farray ("sVx", sigVx, "M", 10);
  tuple -> farray ("sVy", sigVy, "M", 10);
  tuple -> farray ("sVz", sigVz, "M", 10);
  tuple -> farray ("TaggerType", tagtype, "T", 10);
  tuple -> farray ("TaggerDecision", tagdecision, "T", 10);
  tuple -> farray ("TaggerOmega", tagomega, "T", 10);
  tuple -> farray ("ID", pID, "N", 200);
  tuple -> farray ("P", pP, "N", 200);
  tuple -> farray ("Pt", pPt, "N", 200);
  tuple -> farray ("phi", pphi, "N", 200);
  tuple -> farray ("ch", pch, "N", 200);
  tuple -> farray ("ip", pip, "N", 200);
  tuple -> farray ("iperr", piperr, "N", 200);
  tuple -> farray ("ipPU", pipPU, "N", 200);
  tuple -> farray ("ipC", pipC, "N", 200);
  tuple -> farray ("ipChi2", pipChi2, "N", 200);
  tuple -> farray ("ipPUC", pipPUC, "N", 200);
  tuple -> farray ("ipPUChi2", pipPUChi2, "N", 200);
  tuple -> farray ("trtyp", ptrtyp, "N", 200);
  tuple -> farray ("lcs", plcs, "N", 200);
  tuple -> farray ("tsal", ptsal, "N", 200);
  tuple -> farray ("distPhi", pdistPhi, "N", 200);
  tuple -> farray ("veloch", pveloch, "N", 200);
  tuple -> farray ("EOverP", pEOverP, "N", 200);
  tuple -> farray ("PIDe", pPIDe, "N", 200);
  tuple -> farray ("PIDm", pPIDm, "N", 200);
  tuple -> farray ("PIDk", pPIDk, "N", 200);
  tuple -> farray ("PIDp", pPIDp, "N", 200);
  tuple -> farray ("PIDfl", pPIDfl, "N", 200);
  tuple -> farray ("MCID", pMCID, "N", 200);
  tuple -> farray ("MCP", pMCP, "N", 200);
  tuple -> farray ("MCPt", pMCPt, "N", 200);
  tuple -> farray ("MCphi", pMCphi, "N", 200);
  tuple -> farray ("MCz", pMCz, "N", 200);
  tuple -> farray ("mothID", pmothID, "N", 200);
  tuple -> farray ("ancID", pancID, "N", 200);
  tuple -> farray ("bFlag", pbFlag, "N", 200);
  tuple -> farray ("xFlag", pxFlag, "N", 200);
  tuple -> farray ("vFlag", pvFlag, "N", 200);

  if( !( tuple->write()) ) err() << "Cannot fill mytagging Ntuple" << endreq;
  ///----------------------------------------------------------------------

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTaggingAnalysis::finalize() {  return DVAlgorithm::finalize(); }

