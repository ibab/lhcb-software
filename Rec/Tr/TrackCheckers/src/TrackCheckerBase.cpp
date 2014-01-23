// $Id: TrackCheckerBase.cpp,v 1.12 2009-09-09 14:32:55 cattanem Exp $
// Include files 
#include "TrackCheckerBase.h"
#include "Event/Track.h"

#include "Map.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCheckerBase::TrackCheckerBase( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ),
  m_associator(0,""),
  m_associator_refContainer(0,""),
  m_all("ALL"){

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = LHCb::TrackLocation::Default  );

  declareProperty( "TracksRefContainer",
		   m_tracksRefContainer = ""  );
 
  declareProperty( "Selector",
                   m_selectorName = "MCReconstructible" );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackMasterExtrapolator" );  
  declareProperty( "LinkerInTable",   m_linkerInTable = ""       );

  declareProperty("SplitByAlgorithm", m_splitByAlgorithm = false);
  declareProperty("SplitByType",      m_splitByType = false);

  declareProperty("GhostClassification",
                  m_ghostToolName = "LongGhostClassification" );

  declareProperty("SelectionCriteria", m_selectionCriteria = "ChargedLong"); 

}

//=============================================================================
// Destructor
//=============================================================================
TrackCheckerBase::~TrackCheckerBase() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackCheckerBase::initialize()
{

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerInTable == "" ) m_linkerInTable = m_tracksInContainer;


  m_selector = tool<IMCReconstructible>(m_selectorName,
                                        "Selector", this );
  
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);
  
  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>("TrajPoca");
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_stateCreator = tool<IIdealStateCreator>("IdealStateCreator");
  m_visPrimVertTool = tool<IVisPrimVertTool>("VisPrimVertTool");
  m_ghostClassification = tool<ITrackGhostClassification>(m_ghostToolName,"GhostTool",this); 
  
  const TrackMaps::RecMap& theMap = TrackMaps::recDescription();
  m_recCat = theMap.find(m_selectionCriteria)->second;

  return StatusCode::SUCCESS;
}

StatusCode TrackCheckerBase::execute(){
  
  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackCheckerBase::initializeEvent(){

  // get the association table 
  m_associator = AsctTool(evtSvc(), m_tracksInContainer);
  m_directTable = m_associator.direct();
  if (!m_directTable)  
    return Error("Failed to find direct table", StatusCode::FAILURE);
  
  m_inverseTable = m_associator.inverse();
  if (!m_inverseTable)  
    return Error("Failed to find inverse table", StatusCode::FAILURE);

  if (m_tracksRefContainer != ""){
    m_associator_refContainer = AsctTool(evtSvc(), m_tracksRefContainer);
    m_directTable_refContainer = m_associator_refContainer.direct();
    if (!m_directTable_refContainer)
      return Error("Failed to find direct table for reference container",
                   StatusCode::FAILURE);
    
    m_inverseTable_refContainer = m_associator_refContainer.inverse();
    if (!m_inverseTable_refContainer)
      return Error("Failed to find inverse table for refContainer", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
} 

TrackCheckerBase::LinkInfo TrackCheckerBase::reconstructed(const LHCb::MCParticle* particle) const{
  
  TrackCheckerBase::LinkInfo info; 
  info.track = 0; // in case it is a ghost
  InverseRange range = m_inverseTable->relations(particle);
  if (!range.empty()) {
    info.track = range.begin()->to();
    info.clone = range.size() - 1u;
    info.purity = range.begin()->weight();
  }
  else{
    info.track = NULL;
    info.clone = 0;
    info.purity = -1.;
  }
  return info; 
}

TrackCheckerBase::LinkInfo TrackCheckerBase::reconstructedInRefContainer(const LHCb::MCParticle* particle) const {
  
  TrackCheckerBase::LinkInfo info; 
  info.track = 0; // in case it is a ghost
  InverseRange range = m_inverseTable_refContainer->relations(particle);
  if (!range.empty()) {
    info.track = range.begin()->to();
    info.clone = range.size() - 1u;
    info.purity = range.begin()->weight();
  } 
  else{
    info.track = NULL;
    info.clone = 0;
    info.purity = -1.;
  }
  return info; 
}

TrackCheckerBase::InverseRange TrackCheckerBase::reconstructedTracks(const LHCb::MCParticle* particle) const{
  return m_inverseTable->relations(particle);
}

const LHCb::MCParticle* TrackCheckerBase::mcTruth(const LHCb::Track* aTrack) const
{
  const LHCb::MCParticle * particle = NULL;
  if (!m_directTable) 
  {
    Error( "Track MCParticle table missing. Did you call initializeEvent() ?",StatusCode::FAILURE,0).ignore();
  }
  else
  {
    TrackCheckerBase::DirectRange range = m_directTable->relations(aTrack);
    if (range.empty() == false) {
      particle = range.begin()->to();
      if( particle && particle->particleID().threeCharge() ==0 ) {
        Warning("Linker table for track contains pointer to particle with zero charge",StatusCode::SUCCESS,0).ignore() ;
        particle = 0 ;
      }
    }
  }
  return particle;
}

bool TrackCheckerBase::bAncestorWithReconstructibleDaughters(const LHCb::MCParticle* mcPart) const
{
  // loop back and see if there is a B in the history
  bool fromB = false;
  const LHCb::MCParticle* mother = mcPart->mother();
  while ( mother !=0 && fromB == false) {
    fromB = mother->particleID().hasBottom() && ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() );
    if (fromB && !allDaughtersReconstructible(mother))
	return false;
    mother = mother->mother();
  } // loop
  return fromB;
}

bool TrackCheckerBase::bAncestor(const LHCb::MCParticle* mcPart) const
{
  // loop back and see if there is a B in the history
  bool fromB = false;
  const LHCb::MCParticle* mother = mcPart->mother();
  while ( mother !=0 && fromB == false) {
    fromB = mother->particleID().hasBottom() && ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) ;
    mother = mother->mother();
  } // loop
  return fromB;
}

bool TrackCheckerBase::ksLambdaAncestor(const LHCb::MCParticle* mcPart) const
{
  // loop back and see if there is a B in the history
  bool fromKsL = false;
  const LHCb::MCParticle* mother = mcPart->mother();
  while ( mother !=0 && fromKsL == false) {
    if(abs(mother->particleID().pid()) == 310 || 
       abs(mother->particleID().pid()) == 3122)
      fromKsL = true;
    mother = mother->mother();
  } // loop
  return fromKsL;
}

 bool TrackCheckerBase::allDaughtersReconstructible(const LHCb::MCParticle* mcPart) const
{
  const SmartRefVector<LHCb::MCVertex> &vtx = mcPart->endVertices();

  for(SmartRefVector<LHCb::MCVertex>::const_iterator i=vtx.begin();i!=vtx.end();++i){
    const SmartRefVector<LHCb::MCParticle> &ch = (*i)->products();
    for(SmartRefVector<LHCb::MCParticle>::const_iterator j=ch.begin();
	j!=ch.end();++j){
      
      if ((abs((*j)->particleID().pid())==321 ||
	       abs((*j)->particleID().pid())==211 ||
	       abs((*j)->particleID().pid())==13  ||
           abs((*j)->particleID().pid())==11  ||
	       abs((*j)->particleID().pid())==2212)){
	if ( !selected(*j) && 
	     (*j)->mother()->particleID().pid()!=22 &&
	     (*j)->mother()->particleID().pid()!=-99000000 &&
	     (*j)->mother()->particleID().pid()!=130 &&
	     (*j)->mother()->particleID().pid()!=310 &&
	     (*j)->mother()->particleID().pid()!=3122){
	  return false;
	}
      }
      else
	if(!allDaughtersReconstructible(*j))                              
	  return false;
    }
  }
    
  return true;
}



StatusCode TrackCheckerBase::finalize()
{
  return GaudiHistoAlg::finalize();
}


