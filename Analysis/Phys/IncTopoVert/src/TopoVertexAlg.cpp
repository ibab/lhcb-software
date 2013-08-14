// $Id: $
// Include files 
#ifdef _USE_CALLGRIND_
#include "callgrind.h"
#endif

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
//#include "GaudiAlg/TupleObj.h"
//#include "GaudiAlg/Tuple.h"
// local
#include "TopoVertexAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TopoVertexAlg
//
// 2012-10-11 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------
using namespace LHCb ;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TopoVertexAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TopoVertexAlg::TopoVertexAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("TopoVertexToolType",m_topoVertexToolType = "TopoVertexTool" );
  declareProperty("TopoVertexToolName",m_topoVertexToolName = "TopoVertexTool" );

  declareProperty("MC",m_mc = true );

  declareProperty("Opt",m_opt = true );
  declareProperty("Rdn_IPS",m_rdn_ips = false );

  declareProperty("FilterTracksToolType",m_filterTracksToolType = "FilterTracksTool" );
  declareProperty("FilterTracksToolName",m_filterTracksToolName = "FilterTracksTool" );
  declareProperty("MCFilterTracksToolType",m_mcFilterTracksToolType = "MCFilterTracksTool" );
  declareProperty("MCFilterTracksToolName",m_mcFilterTracksToolName = "MCFilterTracksTool" );
  declareProperty("FilterTracksTypeToolType",m_filterTracksTypeToolType = "TrackTypeFilterVtxTopoTracksTool" );
  declareProperty("FilterTracksTypeToolName",m_filterTracksTypeToolName = "TrackTypeFilter" );



  declareProperty("FilterVfName",m_filterVfToolName = "VfFilter" );
  declareProperty("MCAssoType", m_p2mcAssocType = "DaVinciSmartAssociator");
  


  //   declareProperty("InputTrack4VFLocation",m_inputTrack4VFLocation = "/Event/Rec/Track/Best" );
  //   declareProperty("InputTrackLocation",m_inputTrackLocation = "/Event/Rec/Track/Best" );

  declareProperty("OutputLocation"    ,m_outputLocation     = "Event/Rec/TopoVertex" );
  declareProperty("MCOnlyZVTOP"    ,m_mc_only_ZVTOP     = false );
  declareProperty("MCOnlyVf"    ,m_mc_only_Vf     = false );

  declareProperty("DistanceCalculatorToolType",m_distanceCalculatorToolType="LoKi::TrgDistanceCalculator"); //LoKi::DistanceCalculator

  declareProperty("TrackVertexerToolType",m_trackVertexerToolType="TrackVertexer");
  declareProperty("TrackVertexerToolName",m_trackVertexerToolName="TrackVertexerOptTool");
  
  declareProperty("HLT1Loc"    ,m_HLT1Loc     = HltDecReportsLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
TopoVertexAlg::~TopoVertexAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TopoVertexAlg::initialize() {



  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  m_topoVertexTool = tool<ITopoVertexTool>(m_topoVertexToolType,m_topoVertexToolName, this);
  if( 0 == m_topoVertexTool ){
    fatal() << " m_topoVertexTool is "<<m_topoVertexTool<< endmsg;
    return Error("TopoVertexTool not found, type: "+m_topoVertexToolType+", name: "+m_topoVertexToolName
                 ,StatusCode::FAILURE);
  }


  m_mcFilterTracksTool = tool<IFilterVtxTopoTracksTool>(m_mcFilterTracksToolType,m_mcFilterTracksToolName, this);
  if( 0 == m_mcFilterTracksTool ){
    fatal() << " m_mcFilterTracksTool is "<<m_mcFilterTracksTool<< endmsg;
    return Error("FilterVtxTopoTracksTool not found, type: "+m_mcFilterTracksToolType+", name: "+m_mcFilterTracksToolName
                 ,StatusCode::FAILURE);
  }


  m_filterTracksTypeTool = tool<IFilterVtxTopoTracksTool>(m_filterTracksTypeToolType,m_filterTracksTypeToolName, this);
  if( 0 == m_filterTracksTypeTool ){
    fatal() << " m_filterTracksTypeTool is "<<m_filterTracksTypeTool<< endmsg;
    return Error("FilterVtxTopoTracksTypeTool not found, type: "+m_filterTracksTypeToolType+", name: "+m_filterTracksTypeToolName
                 ,StatusCode::FAILURE);
  }


  m_filterTracksTool = tool<IFilterVtxTopoTracksTool>(m_filterTracksToolType,m_filterTracksToolName, this);
  if( 0 == m_filterTracksTool ){
    fatal() << " m_filterTracksTool is "<<m_filterTracksTool<< endmsg;
    return Error("FilterVtxTopoTracksTool not found, type: "+m_filterTracksToolType+", name: "+m_filterTracksToolName
                 ,StatusCode::FAILURE);
  }

  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  if( 0 == m_timerTool){
    fatal() << " m_timertool is "<<m_timerTool<< endmsg;
    return Error("SequencerTimerTool not found"	 ,StatusCode::FAILURE);
  }
  m_timer = m_timerTool->addTimer( "ZVTOP_Timer" );


  m_Geom = tool<IDistanceCalculator> (m_distanceCalculatorToolType, this);
  if ( ! m_Geom ) {
    return Error("DistanceCalculator could not be found",StatusCode::FAILURE);
  }

  m_trackVertexer  = tool<ITrackVertexer>(m_trackVertexerToolType,m_trackVertexerToolName);
  if (0 == m_trackVertexer) 
    return Error("Unable to load trackVertexerTool "+m_trackVertexerToolType+" "+m_trackVertexerToolName,
                 StatusCode::FAILURE);



  m_vtx_analyser = tool<ITopoVertexAnalyserTupleTool>("TopoVertexAnalyserTupleTool","Vtx_Analyser");



  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, 
                                             this);
  

  m_bkgCategory = tool<IBackgroundCategory>("BackgroundCategory", this);
  if(!m_bkgCategory){
    fatal() << "Unable to retrieve BackgroundCategory tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TopoVertexAlg::execute() {
  //if ( !exist<Particle::Range>( "/Event/AllStreams/Phys/B2XTau_TauTau_TOSLine/Particles" ) ) return  StatusCode::SUCCESS;
  bool run_ZVTOP = true;
 
  
  //pepare ntuple
  Tuples::Tuple tuple = nTuple("TopoVtxAlg",CLID_ColumnWiseTuple);

  std::vector<const Track*> tracks;
  int areRecAsLong = -1;
  std::vector<const Track*> tracks_temp = m_mcFilterTracksTool->filteredTracks(&tuple); 
  std::vector<const Track*> tracks_temp_Type = m_filterTracksTypeTool->filteredTracks(tracks_temp); 
  if (tracks_temp_Type.size()<6){
    areRecAsLong = 0;
  }
  else areRecAsLong = 1;
  tuple->column("areMatchedPionRecoAsLong", areRecAsLong);
  

  //check trigger
  int isTriggered = -1;
  int isTriggeredL0 = -1;
  int isTriggeredHLT1 = -1;

  bool L0Decision = false;
  //L0
  if( exist<L0DUReport>(L0DUReportLocation::Default) ) {
      L0DUReport* L0Report = get<L0DUReport>(L0DUReportLocation::Default);
      L0Decision  = L0Report->decision();
      if (L0Decision) isTriggeredL0 = 1;
      else isTriggeredL0 = 0;
  }
  
  //HLT
  bool HLT1Decison = false;
  bool HLT2Decison = false;

  const HltDecReports* HLTReports = getIfExists<LHCb::HltDecReports>(evtSvc(),LHCb::HltDecReportsLocation::Default); 
  if ( !HLTReports ) {
    HLTReports = getIfExists<LHCb::HltDecReports>(evtSvc(),LHCb::HltDecReportsLocation::Default,false);
  } 
  
  if ( HLTReports ){
  
    //HLT1  
    if( HLTReports->hasDecisionName("Hlt1TrackAllL0Decision")){
      HLT1Decison = HLTReports->decReport("Hlt1TrackAllL0Decision")->decision();
    }
    
    if (L0Decision && HLT1Decison) isTriggeredHLT1 =1;
    else isTriggeredHLT1 =0;
    //HLT2
    TString HLT2Line[6] = {"Hlt2Topo2BodyBBDTDecision","Hlt2Topo2BodySimpleDecision", 
			    "Hlt2Topo3BodyBBDTDecision", "Hlt2Topo3BodySimpleDecision",
			    "Hlt2Topo4BodyBBDTDecision", "Hlt2Topo4BodySimpleDecision"};
    for (int i =0; i<6; i++){
      if( HLTReports->hasDecisionName(HLT2Line[i].Data())){
	HLT2Decison = HLT2Decison || HLTReports->decReport(HLT2Line[i].Data())->decision();
      }
    }

    if (L0Decision && HLT1Decison && HLT2Decison) isTriggered = 1;
    else isTriggered =0;

  }
  //else debug()<<"Not Found HLT1 reports at "+m_HLT1Loc<<endreq;

  tuple->column("isTriggered", isTriggered);
  tuple->column("isTriggeredL0", isTriggeredL0);
  tuple->column("isTriggeredHLT1", isTriggeredHLT1);


  //check the stripping
  int isStrip = -1;  
  int isStrip2 = -1;  
  if ( exist<Particle::Range>( "/Event/AllStreams/Phys/B2XTau_TauTau_TOSLine/Particles" ) ){

    //loop over candidate and check they are matched
    Particle::Range   parts = get<Particle::Range>( "/Event/AllStreams/Phys/B2XTau_TauTau_TOSLine/Particles" ); 
    if (parts.size()==0) { isStrip = 2;isStrip2 = 2;}
    if (parts.size()>0) { isStrip = 0;isStrip2 = 0;}

    //my own matching... doesn't work apparently
    debug()<<"Nb cand: "<< parts.size()<<endreq;
    Particle::ConstVector::const_iterator ip;
    for ( ip = parts.begin(); ip != parts.end() ; ++ip) {
      if (isMatched(*ip) == true) {
        isStrip = 1;
        break;
      }
    }

    //background category
    for ( ip = parts.begin(); ip != parts.end() ; ++ip) {
      IBackgroundCategory::categories cat = m_bkgCategory->category(*ip);
      if (cat == 0) {
        isStrip2 = 1;
        break;
      }
    }
  }
  tuple->column("isStripped", isStrip);
  tuple->column("isStripped2", isStrip2);


  //ZVTOP
  double t_zvtop = -1;
  if (areRecAsLong == 1) {
     //prepare the track list to be run on
    std::vector<const Track*> tracksZVTOPIPS = m_filterTracksTool->filteredTracks(); //filter IPS
    std::vector<const Track*> tracksZVTOP = m_filterTracksTypeTool->filteredTracks(tracksZVTOPIPS); //filter Type
    
    m_topoVertexTool->setVertexFunctionTracks(tracksZVTOP);
    
    //run ZVTOP
    m_timerTool->start(m_timer) ;
    int n_sig_track_step[6] = {0,0,0,0,0,0};
    StatusCode sc = m_topoVertexTool->findVertices(tracksZVTOP, n_sig_track_step);
    t_zvtop = m_timerTool->stop(m_timer) ; 
    if (sc.isFailure ()) Error("Topo vertex tool failed to find vertice",StatusCode::SUCCESS,20).ignore();
    sc = m_topoVertexTool->writeOnTES(m_outputLocation);
    if (sc.isFailure ()) Error("Topo vertex tool failed to write on TES",StatusCode::SUCCESS,20).ignore();
  }
  tuple->column( "ZVTOP_time"           , t_zvtop ); 
  StatusCode  sc = m_vtx_analyser->analyseVertices(&tuple, m_outputLocation);


tuple->write();
return StatusCode::SUCCESS;
}
//=============================================================================
//  isMatched
//=============================================================================
  bool TopoVertexAlg::isMatched(const  Particle* P)  {
    //loop over particle daughters i.e tau
    Particle::ConstVector v_tau = P->daughtersVector(); //m_descend->descendants(P);
    Particle::ConstVector::const_iterator ip_tau;
    bool foundTaup = false;
    bool foundTaum = false;
    bool decision = false;
    
    //TAU
    debug()<<"Nb desc direct: "<< v_tau.size()<<endreq;
    for ( ip_tau = v_tau.begin(); ip_tau != v_tau.end(); ip_tau++) {
      bool hasTaup = false;
      bool hasTaum = false;
      bool hasB = false;
      const MCParticle* mclink0 = 0;
      mclink0 = m_p2mcAssoc->relatedMCP(*ip_tau);
      if(mclink0)  debug()<<"\tParticle0 Id :"<<mclink0->particleID().pid()<<endreq;
      else debug()<<"\t\tNo MCLink"<<endreq;

      //PIONS OR RHO
      Particle::ConstVector v_pion = (*ip_tau)->daughtersVector();
      debug()<<"\tNb desc of desc of desc: "<<v_pion.size()<<endreq;
      Particle::ConstVector::const_iterator ip_pion;
      for ( ip_pion = v_pion.begin(); ip_pion != v_pion.end(); ip_pion++) {
	if ( (*ip_pion)->isBasicParticle()){
	  const MCParticle* mclink = 0;
	  mclink = m_p2mcAssoc->relatedMCP(*ip_pion);
	  if(mclink) { 
	    debug()<<"\t\tParticle1 Id :"<<mclink->particleID().pid()<<endreq;
	    if (hasPartancestor( mclink, 15) >0 ) hasTaum = true;
	    if (hasPartancestor( mclink, -15) >0 ) hasTaup = true;
	    if (hasPartancestor( mclink, 531) >0 || hasPartancestor( mclink, -531) ) hasB = true;
	  }
	  else debug()<<"\t\tNo MCLink"<<endreq;
	  debug()<<"\t\tGenealogie (PION): Taup? "<<hasTaup <<", Taum? "<<hasTaum<< ",  B?: "<< hasB<<endreq;
	}
	else{
	  Particle::ConstVector v_pion2 = (*ip_pion)->daughtersVector();
	  debug()<<"\tNb desc of desc: "<<v_pion2.size()<<endreq;
	  Particle::ConstVector::const_iterator ip_pion2;
	  for ( ip_pion2 = v_pion2.begin(); ip_pion2 != v_pion2.end(); ip_pion2++) {
	    if ( (*ip_pion2)->isBasicParticle()){
	      const MCParticle* mclink2 = 0;
	      mclink2 = m_p2mcAssoc->relatedMCP(*ip_pion2);
	      if(mclink2) { 
		debug()<<"\t\tParticle1 Id :"<<mclink2->particleID().pid()<<endreq;
		if (hasPartancestor( mclink2, 15) >0 ) hasTaum = true;
		if (hasPartancestor( mclink2, -15) >0 ) hasTaup = true;
		if (hasPartancestor( mclink2, 531) >0 || hasPartancestor( mclink2, -531) ) hasB = true;
	      }
	      else debug()<<"\t\tNo MCLink"<<endreq;
	      debug()<<"\t\tGenealogie (RHO): Taup? "<<hasTaup <<", Taum? "<<hasTaum<< ",  B?: "<< hasB<<endreq;
	    }
	    else error()<<"Particle Should be basic WTF"<<endreq;
	  }
	}
      }
      
      if( hasTaum == true  && hasTaup ==false && hasB == true) foundTaum =true;
      if( hasTaum == false && hasTaup ==true  && hasB == true) foundTaup =true;        
    }
    if (foundTaum == true && foundTaup == true) decision = true;
    
    return decision;
    
  }


const double TopoVertexAlg::hasPartancestor(const MCParticle*imc, int part)
{
  //this function returns +p^2 if the particle has a ancestor=part (based on MC truth) else itreturns  -p^2
  if(imc->mother()) {
    
    const MCParticle*mc_mom = imc->mother();
    if( (mc_mom->particleID()).pid() == part ){
      return pow(mc_mom->p()*mc_mom->p(),0.5);
    }
    else return hasPartancestor(mc_mom, part);
    
  }
  
  else {
        
    return pow(imc->p()*imc->p(),0.5);
    
  }
  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode TopoVertexAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions

}

//=============================================================================
