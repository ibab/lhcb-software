// $Id: TestDST.cpp,v 1.5 2007-04-02 08:45:04 ukerzel Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "Event/MCParticle.h"

// ODIN
#include "Event/ODIN.h"

// local
#include "TestDST.h"

#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TestDST
//
// 2007-02-06 : Ulrich Kerzel
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestDST );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestDST::TestDST( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : DVAlgorithm        ( name , pSvcLocator ),
    m_printPVtracks      ( false              ),
    m_printCaloElectrons ( false              ),
    m_printCaloPRS       ( false              ),
    m_printMCContainer   ( false              ),
    m_part2MCLinkerComposite(),
    m_part2MCLinkerLinks()
{
 
  declareProperty( "PrintPVTracks"      , m_printPVtracks      = false       );
  declareProperty( "PrintCaloElectrons" , m_printCaloElectrons = false       );
  declareProperty( "PrintCaloPRS"       , m_printCaloPRS       = false       );
  declareProperty( "PrintMCContainer"   , m_printMCContainer   = false       );

}
//=============================================================================
// Destructor
//=============================================================================
TestDST::~TestDST() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestDST::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  // initialise associator to MC
  m_part2MCLinkerComposite = new Particle2MCLinker(this,Particle2MCMethod::Composite, "");
  m_part2MCLinkerLinks     = new Particle2MCLinker(this,Particle2MCMethod::Links    , rootOnTES() + LHCb::TrackLocation::Default);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestDST::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  debug()<< " rootOnTES is " << rootOnTES () << endmsg;

  LHCb::ODIN * odin = get<LHCb::ODIN>( rootOnTES()+ LHCb::ODINLocation::Default );
  
  verbose() << " ODIN event time " << odin->eventTime() 
            << " runNr "           << odin->runNumber ()
            << " orbit Nr "        << odin->orbitNumber()
            << " event type "      << odin->eventType()
            << " event number "    << odin->eventNumber()
            << " detector status " << odin->detectorStatus () 
            << endmsg;

  //
  // print information about default primary vertices
  //
  StatusCode sc;

  sc = TestDST::PrintDefaultPV();
  sc = TestDST::PrintParticles();

  if (m_printMCContainer) {
    sc = TestDST::PrintMCParticles();
    sc = TestDST::PrintMCVertices();
  }//if

//  LHCb::CaloHypo::Container* caloE = get<LHCb::CaloHypo::Container>("/Event/Rec/Calo/Electrons");  
//  if (m_printCaloElectrons) {
//    LHCb::CaloHypo::Container::const_iterator iCalo;
//    LHCb::CaloHypo::Container::const_iterator iCaloBegin = caloE->begin();
//    LHCb::CaloHypo::Container::const_iterator iCaloEnd   = caloE->end();
//    for (iCalo = iCaloBegin; iCalo != iCaloEnd; iCalo++) {
//      verbose() << "iCalo " << *iCalo << " --- " << *(*iCalo) << endmsg;
//      verbose() << "  energy " << (*iCalo)->e() << endmsg;
//    }//for
//  }// if print caloE
//
//  if (m_printCaloPRS) {
//    LHCb::CaloDigit::Container* caloDigitPrs = get<LHCb::CaloDigit::Container>("/Event/Raw/Prs/Digits");
//    LHCb::CaloDigit::Container::const_iterator iCaloPRS;
//    LHCb::CaloDigit::Container::const_iterator iCaloPRSBegin = caloDigitPrs->begin();
//    LHCb::CaloDigit::Container::const_iterator iCaloPRSEnd   = caloDigitPrs->end();
//    for (iCaloPRS = iCaloPRSBegin; iCaloPRS != iCaloPRSEnd; iCaloPRS ++){
//      verbose() <<  "iCaloPrs " << *iCaloPRS << "---" << *(*iCaloPRS) << endmsg;
//    }//for
//  } // if print caloPRS






  verbose() << "--> end of EXECUTE <--" << endmsg;
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestDST::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode  TestDST::PrintDefaultPV(){

  const LHCb::RecVertex::ConstVector &primaryVertices = desktop()->primaryVertices();
  int nPrimVertices = primaryVertices.size();
  debug() << "*** got # primary vertices " << nPrimVertices << endmsg;
  if (nPrimVertices == 0) {
    err() << "no primary Vertices found in this event, skip" << endmsg;
    return StatusCode::SUCCESS;
  } // if #primVert

  LHCb::RecVertex::ConstVector::const_iterator iPV;
  LHCb::RecVertex::ConstVector::const_iterator iPVBegin = primaryVertices.begin();
  LHCb::RecVertex::ConstVector::const_iterator iPVEnd   = primaryVertices.end();  
  LHCb::RecVertices* verts                              = get<LHCb::RecVertices>(rootOnTES()+ LHCb::RecVertexLocation::Primary);

  for (iPV = iPVBegin;  iPV !=iPVEnd ; iPV++){      
   const LHCb::RecVertex *thisPV = (*iPV);
   verbose() << "number of tracks at this PV " << thisPV->tracks().size() << endmsg;
   verbose() << "  more info " << *thisPV << endmsg;
   verbose() << "  position  " << thisPV->position() << endmsg;

   if (m_printPVtracks) {
     const SmartRefVector< LHCb::Track > &tracks = thisPV->tracks();
     SmartRefVector< LHCb::Track >::const_iterator iTrack;
     SmartRefVector< LHCb::Track >::const_iterator iTrackBegin = tracks.begin();
     SmartRefVector< LHCb::Track >::const_iterator iTrackEnd   = tracks.end();
     for (iTrack = iTrackBegin; iTrack != iTrackEnd; iTrack++) {
       const SmartRef< LHCb::Track > trackRef = *iTrack;
       const LHCb::Track *track = *iTrack;
       verbose() << " smartRef " << trackRef 
                 << " hintID " << iTrack->hintID()  << " linkID " << iTrack->linkID()  << endmsg;
       if (trackRef) {
         verbose() << "pt " << trackRef->pt() << endmsg;
       }// if trackRef
       
       
       if ( iTrack->hintID() >= 0 )  {
         LinkManager::Link* lnk = verts->linkMgr()->link(iTrack->hintID());
         
         verbose() << " Got link [" << iTrack->hintID() << "] " << (void*)lnk << endmsg;
         if ( lnk )  {
           verbose() << " -> " << lnk->path() << endmsg;
         }
       }//if

     }//for iTrack
   }// if printTracks

 } // for PVIter

  verbose() << "--> end of PrintDefaultPV <--" << endmsg;

  return StatusCode::SUCCESS;
} // sc PrintPV
//=============================================================================
StatusCode TestDST::PrintParticles() {

  
  LHCb::Particle::ConstVector particleVector = desktop()->particles();
  const int nParticles = particleVector.size();
  verbose() << "got # particles " << nParticles << endmsg;
  LHCb::Particle::ConstVector::const_iterator iParticle;
  LHCb::Particle::ConstVector::const_iterator iParticleBegin = particleVector.begin();
  LHCb::Particle::ConstVector::const_iterator iParticleEnd   = particleVector.end();
  for (iParticle = iParticleBegin; iParticle != iParticleEnd; iParticle++) {

    verbose() << " ----> consider new particle <-----" << endmsg;

    //
    // print some generic particle information
    //
    verbose() << "Mass " << (*iParticle)->measuredMass()/Gaudi::Units::GeV
              << " +- " << (*iParticle)->measuredMassErr()/Gaudi::Units::GeV
              << endmsg;

    std::string locTES = TestDST::objectLocation((*iParticle)->parent() );
    verbose() << "location on TES " << locTES << endmsg;

    
    //
    // print PV information
    //
    TestDST::PrintRelatedPV(*iParticle);

    // print some extra-info
    if ( (*iParticle)->hasInfo(100) )
      verbose() << "extra info with key 100 " << (*iParticle)->info(100, -999) << endmsg;
    if ( (*iParticle)->hasInfo(101) )
      verbose() << "extra info with key 101 " << (*iParticle)->info(101, -999) << endmsg;

    TestDST::PrintParticleInfo(*iParticle);
    TestDST::PrintMCParticle(*iParticle);

    //
    // print information about daughers  
    //
    if ((*iParticle)->endVertex()){
      verbose() << "  position of decay vertex " << (*iParticle)->endVertex()->position() << endmsg;
    }//ifEndVertex
    
    verbose() << "  number of daughters " << (*iParticle)->daughters().size() << endmsg;    
    if ((*iParticle)->daughters().size() > 0) {

      const SmartRefVector< LHCb::Particle > &daughters = (*iParticle)->daughters();    
      SmartRefVector< LHCb::Particle >::const_iterator iDaughter;
      SmartRefVector< LHCb::Particle >::const_iterator iDaughterBegin = daughters.begin();
      SmartRefVector< LHCb::Particle >::const_iterator iDaughterEnd   = daughters.end();
      for (iDaughter = iDaughterBegin; iDaughter != iDaughterEnd; iDaughter++) {    
        verbose() << "--> print information about next daughter" << endmsg;
        TestDST::PrintParticleInfo(*iDaughter);
        TestDST::PrintMCParticle(*iDaughter);
      } //for iDaughter
    }// if #daughter

  }//for iParticle
  verbose() << "--> end of PrintParticles <-- " << endmsg;

  return StatusCode::SUCCESS;
}// sc PrintParticles
//=============================================================================
StatusCode TestDST::PrintRelatedPV(const LHCb::Particle* particle) {

  //
  // print information about associated primary vertex
  //  from PhysDesktiop
  //
  const LHCb::VertexBase *relatedPV = desktop()->relatedVertex(particle);
  if (relatedPV) {
    verbose() << "got related PV at " << relatedPV->position() 
              << " weight " << desktop()->weight(particle, relatedPV)
              << " #tracks " << (dynamic_cast<const LHCb::RecVertex*>(relatedPV))->tracks().size()
              << endmsg;
  } else {
    verbose() << "no related PV" << endmsg;
  } // if relatedPV
  
  //
  // get linked vertices
  //  from links stored into microDST
  std::string linksLocation =  rootOnTES() +  LHCb::RecVertexLocation::Primary;;
  verbose() << " get associator from " << linksLocation << endmsg;
  Part2Vertex associator(eventSvc(), linksLocation  );
  const Part2VertexTable* table = associator.direct();
  if (table) {
    verbose() << "got table from associator, try got get associated vertices" << endmsg;
    Part2VertexRange range = table->relations(particle);
    verbose() << "got #associations " << range.size() << endmsg;
    for (Part2VertexIterator iRel = range.begin(); iRel != range.end(); iRel++) {
      const LHCb::RecVertex* thisPV = iRel->to();
      double                 weight = iRel->weight();
      verbose() << "particle related to PV at " << thisPV->position() 
                << " #tracks " << thisPV->tracks().size()
                << " with weight "
                << weight << endmsg;
    }//for
  } // if table

  verbose() << "--> end of PrintRelatedPV <--" << endmsg;
  
  return StatusCode::SUCCESS;
} // sc PrintRelatedPV
//=============================================================================
StatusCode TestDST::PrintParticleInfo(const LHCb::Particle *particle) {

  //
  // print some generic info testing proto particle
  //
  verbose() << "---- new particle PID " << (particle)->particleID().pid() << endmsg;
  verbose() << " p " << (particle)->p() << endmsg;

    if ((particle)->proto()) {
      verbose() << " RICH DLL e           " << (particle)->proto()->info(LHCb::ProtoParticle::RichDLLe , -999) << endmsg;
      verbose() << " Muon LL  mu          " << (particle)->proto()->info(LHCb::ProtoParticle::MuonMuLL , -999) << endmsg;
      verbose() << " RICH PID             " << (particle)->proto()->richPID() << endmsg;
      if ((particle)->proto()->richPID())
        verbose() << " RICH best PID        " << (particle)->proto()->richPID()->bestParticleID () << endmsg;
      
      verbose() << " Muon                 "  << (particle)->proto()->muonPID() << endmsg;
      if ((particle)->proto()->muonPID())
        verbose() << " Muon isMuon "      << (particle)->proto()->muonPID()->IsMuon() << endmsg;
      
      verbose() << " get Calo" << endmsg;
      const SmartRefVector< LHCb::CaloHypo > &caloVec = (particle)->proto()->calo();
      verbose() << "    #elements " << caloVec.size() << endmsg;
      SmartRefVector< LHCb::CaloHypo >::const_iterator iCalo;
      SmartRefVector< LHCb::CaloHypo >::const_iterator iCaloBegin = caloVec.begin();
      SmartRefVector< LHCb::CaloHypo >::const_iterator iCaloEnd   = caloVec.end();
      for (iCalo = iCaloBegin; iCalo != iCaloEnd; iCalo ++) {
        verbose() << "new calo info obtained from proto-particle" << endmsg;
        const SmartRef<LHCb::CaloHypo > caloRef = *iCalo;
        verbose() << " smartRef " << caloRef 
                  << " hintID " << iCalo->hintID()  << " linkID " << iCalo->linkID()  << endmsg;
        if (caloRef) {
          verbose() << "  energy " << (*iCalo)->e() << endmsg;
        }//if

//        // look up element in container
//        if ( iCalo->hintID() >= 0 )  {
//          LinkManager::Link* lnk = caloE->linkMgr()->link(iCalo->hintID());
//          
//          verbose() << " Got link [" << iCalo->hintID() << "] " << (void*)lnk << endmsg;
//          if ( lnk )  {
//            verbose() << " -> " << lnk->path() << " address " << lnk->address()<< endmsg;
//          }
//        }//if

      }//for calo

    }//if Proto

    verbose() << "--> end of PrintParticleInfo <-- " << endmsg;
    
  return StatusCode::SUCCESS;
} // sc PrintParticleInfo
//=============================================================================
StatusCode TestDST::PrintMCParticles() {

  std::string locTES = rootOnTES()+  LHCb::MCParticleLocation::Default;
  if (exist<LHCb::MCParticles>(locTES)) {
    LHCb::MCParticles *MCParticles = get<LHCb::MCParticles>(locTES);
    verbose() << " got #MC Particles " << MCParticles->size() << endmsg;
    LHCb::MCParticles::const_iterator iMCPart;
    LHCb::MCParticles::const_iterator iMCPartBegin = MCParticles->begin();
    LHCb::MCParticles::const_iterator iMCPartEnd   = MCParticles->end();
    for (iMCPart = iMCPartBegin;  iMCPart != iMCPartEnd;  iMCPart++ ){
      verbose() << "--> next MC particle PID " << (*iMCPart)->particleID().pid() 
                << " p "                       << (*iMCPart)->momentum() 
                << endmsg;
      if ((*iMCPart)->originVertex()) {
        verbose() << "    origin Vtx " <<  (*iMCPart)->originVertex()->position()  << endmsg;
      } // if originVertex
      if ((*iMCPart)->mother()) {
        verbose() << "    on TES " << TestDST::objectLocation((*iMCPart)->mother() ->parent() ) << endmsg;
        verbose() << "    motherID " << (*iMCPart)->mother()->particleID().pid() 
                  << " p " << (*iMCPart)->mother()->momentum()
                  << endmsg;
      } // if mother
    }//for

  }//if exist


  verbose() << "--> end of PrintMCParticles <--" << endmsg;
  return StatusCode::SUCCESS;
} // sc PrintMCParticles

//=============================================================================
StatusCode TestDST::PrintMCVertices() {


  std::string locTES = rootOnTES()+  LHCb::MCVertexLocation::Default;
  verbose() << "try to load MC vertices from " << locTES << endmsg;

  if (exist<LHCb::MCVertices>(locTES)) {

    LHCb::MCVertices *MCVertices = get<LHCb::MCVertices>(locTES);
    verbose() << "got #MC vertices " << MCVertices->size() << endmsg;

    LHCb::MCVertices::const_iterator iMCVert;
    LHCb::MCVertices::const_iterator iMCVertBegin = MCVertices->begin();
    LHCb::MCVertices::const_iterator iMCVertEnd   = MCVertices->end();
    for (iMCVert = iMCVertBegin; iMCVert != iMCVertEnd; iMCVert++){
      verbose() << "--> next MC vertex at " << (*iMCVert)->position() << endmsg;
      if ((*iMCVert)->mother()) {
        verbose() << "   mother PID " << (*iMCVert)->mother()->particleID().pid() << endmsg;
      }//if
    }//for
  } else {
    verbose() << "No MC vertices found" << endmsg;
  } //if exist

  verbose() << "--> end of PrintMCVertices <--" << endmsg;
  return StatusCode::SUCCESS;

}// sc PrintMCVertices


//=============================================================================
std::vector<const LHCb::MCParticle*> TestDST::GetMCParticle (const LHCb::Particle* particle,
                                                             Particle2MCLinker* particleLinker) {


  verbose() << "get MC particles associated to particle with PID " 
            << particle -> particleID().pid()
            << " and mass " << particle->measuredMass() << " +- " 
            << particle->measuredMassErr()
            << endmsg;
  //
  // init
  //
  std::vector<const LHCb::MCParticle*> associatedParticles;


  //
  // check direct association for composite particles
  //
  std::string locTES = TestDST::objectLocation(particle->parent() );
  verbose() << "try to get direct link for particle on TES " << locTES << endmsg;
  Part2MCPart p2pAssociator(eventSvc(), locTES);
  const Part2MCPartTable *p2pTable = p2pAssociator.direct();
  if (p2pTable) {
    Part2MCPartRange p2pRange =  p2pTable->relations(particle);
    verbose() << "found #relations " << p2pRange.size() << endmsg;
    for (Part2MCPartIterator iRel = p2pRange.begin(); iRel != p2pRange.end(); iRel++) {
      const LHCb::MCParticle *mcPart = iRel->to();
      double                  weight = iRel->weight();
      verbose() << "particle related to MCPart with pid " << mcPart->particleID().pid() 
                << " weight " << weight 
                << " on TES " <<  TestDST::objectLocation(mcPart->parent())
                << endmsg;     
    }//for iRel
  } else {
    verbose() << "table for direct association not found " << endmsg;
  }// if table

  //
  // check direct association for track to MC link
  //

//  if (particle->proto() ) {
//    verbose() << "found proto-particle " << endmsg;
//    if (particle->proto()->track()) {
//      verbose() << "found track with key " << particle->proto()->track()->key() << endmsg;
//      std::string linksLocation = rootOnTES() + LHCb::TrackLocation::Default;
//      verbose() << " get associator from " << linksLocation << endmsg;
//      Track2MCPart associator(eventSvc(), linksLocation  );
//      const Track2MCPartTable *table = associator.direct();
//      if (table) {
//        Track2MCPartRange range =  table->relations(particle->proto()->track());
//        verbose() << "found #relations " << range.size() << endmsg;
//        for (Track2MCPartIterator iRel = range.begin(); iRel != range.end(); iRel++){
//          const LHCb::MCParticle *mcPart = iRel->to();
//          double                  weight = iRel->weight();
//          verbose() << "track related to MCPart with pid " << mcPart->particleID().pid() 
//                    << " weight " << weight 
//                    << " on TES " <<  TestDST::objectLocation(mcPart->parent())
//                    << endmsg;
//        } //for iRed
//      } else {
//        verbose() << "table not found" << endmsg;
//      }// if table
// //     verbose() << "read linker Table from " << LHCb::TrackLocation::Default << endmsg;
// //     Object2MCLinker< LHCb::Track > track2MCLink( this, "", "", 
// //                                                  LHCb::TrackLocation::Default);
// //     double weight = 0.;
// //     const LHCb::MCParticle* mcPart = track2MCLink.first(particle->proto()->track(), weight);
////      if (mcPart) {
////        verbose() << "track associated to MC part with PID " << mcPart->particleID().pid() << endmsg;
////      } else {
////        verbose() << "track not associated to MC particle " << endmsg;
////      } // if mcpart
//    }// if track
//  } else {
//    verbose() << "no proto-particle associated" << endmsg;
//  } // if proto
  

  //
  // check if associated to MC truth
  //
  bool isAssociatedLinks     = particleLinker-> isAssociated(particle);
  verbose() << "Associated to MC truth ? "   << isAssociatedLinks     << endmsg;
  if (!isAssociatedLinks) {
    debug() << " true particle " << particle->particleID().pid () 
            << " not found" << endmsg;
    associatedParticles.clear();
    return associatedParticles;
  } // if 
  
  


  // loop over all associations
  const LHCb::MCParticle* mcParticle = particleLinker->firstMCP( particle );    
  if (mcParticle) {      
    verbose() << "Found MC particle " << endmsg;
    // loop over all possible assignments
    while (mcParticle) {
      verbose() << "particle is associated  to MC particle with key "  
                << mcParticle->key() << endmsg;
      verbose() << " particle ID " << particle   -> particleID().pid () 
                << " true pid    " << mcParticle -> particleID().pid ()
                << endmsg;
      associatedParticles.push_back(mcParticle);
      mcParticle = particleLinker->next();
    } // while
  } else {
    verbose() << "MC particle not found" << endmsg;
  } // if mcParticle


  verbose() << "return #associated particles " << associatedParticles.size() << endmsg;  
  return associatedParticles;

}// void GetMCParticleLinks
//=============================================================================
StatusCode TestDST::PrintMCParticle(const LHCb::Particle* particle) {

    //
    // print MC truth info
    //
    std::vector<const LHCb::MCParticle*> associatedMCParticles;

    if ((particle)->daughters().size() > 0)
      associatedMCParticles = TestDST::GetMCParticle (particle,m_part2MCLinkerComposite);
    else
      associatedMCParticles = TestDST::GetMCParticle (particle,m_part2MCLinkerLinks);

    verbose() << "#associated MC particles " << associatedMCParticles.size() << endmsg;
    if (associatedMCParticles.size() >0 ) {
      std::vector<const LHCb::MCParticle*>::const_iterator iMCPart;
      std::vector<const LHCb::MCParticle*>::const_iterator iMCPartBegin = associatedMCParticles.begin();
      std::vector<const LHCb::MCParticle*>::const_iterator iMCPartEnd   = associatedMCParticles.end();
      for (iMCPart = iMCPartBegin; iMCPart != iMCPartEnd; iMCPart++) {
        verbose() << "  consider next associated MC particle : rec PID " 
                  << (particle)->particleID().pid()
                  << " true PID " << (*iMCPart)->particleID().pid()
                  << endmsg;
        if ( (*iMCPart)->mother() ) {
          verbose() << "  mother PID " << (*iMCPart)->mother()->particleID().pid() << endmsg;
        }// if mother
      } // for iMCPart
    }// if # >0

   return StatusCode::SUCCESS;
 }// sc PrintMCParticle
//=============================================================================
//=============================================================================
//=============================================================================
