// $Id: TestDST.cpp,v 1.1.1.1 2007-02-15 14:07:05 ukerzel Exp $
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
  m_printCaloPRS       ( false              )
{
 
  declareProperty( "PrintPVTracks"      , m_printPVtracks      = false       );
  declareProperty( "PrintCaloElectrons" , m_printCaloElectrons = false       );
  declareProperty( "PrintCaloPRS"       , m_printCaloPRS       = false       );

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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestDST::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  debug()<< " rootOnTES is " << rootOnTES () << endmsg;

  LHCb::ODIN * odin = get<LHCb::ODIN>( "/Event/" +
                                       rootOnTES()+ "/" +
                                       LHCb::ODINLocation::Default );
  
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
  StatusCode sc = TestDST::PrintDefaultPV();

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

//  LHCb::MCParticles *MCParticles = get<LHCb::MCParticles>("/Event/uDST/MC/Particles");
//  verbose() << " got #MC Particles " << MCParticles->size() << endmsg;
//  LHCb::MCParticles::const_iterator iMCPart;
//  LHCb::MCParticles::const_iterator iMCPartBegin = MCParticles->begin();
//  LHCb::MCParticles::const_iterator iMCPartEnd   = MCParticles->end();
//  for (iMCPart = iMCPartBegin;  iMCPart != iMCPartEnd;  iMCPart++ ){
//    verbose() << "--> next MC particle PID " << (*iMCPart)->particleID().pid() 
//              << " pt " << (*iMCPart)->pt() << endmsg;
//  }//for
//
//  LHCb::MCVertices *MCVertices = get<LHCb::MCVertices>("/Event/uDST/MC/Vertices");
//  verbose() << "got #MC vertices " << MCVertices->size() << endmsg;
//  LHCb::MCVertices::const_iterator iMCVert;
//  LHCb::MCVertices::const_iterator iMCVertBegin = MCVertices->begin();
//  LHCb::MCVertices::const_iterator iMCVertEnd   = MCVertices->end();
//  for (iMCVert = iMCVertBegin; iMCVert != iMCVertEnd; iMCVert++){
//    verbose() << "--> next MC vertex at " << (*iMCVert)->position() << endmsg;
//    if ((*iMCVert)->mother()) {
//      verbose() << "   mother PID " << (*iMCVert)->mother()->particleID().pid() << endmsg;
//    }//if
//  }//for


  
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
    // print information about associated primary vertex
    //
    const LHCb::VertexBase *relatedPV = desktop()->relatedVertex(*iParticle);
    if (relatedPV) {
      verbose() << "got related PV at " << relatedPV->position() 
                << " weight " << desktop()->weight(*iParticle, relatedPV)
                << " #tracks " << (dynamic_cast<const LHCb::RecVertex*>(relatedPV))->tracks().size()
                << endmsg;
    } else {
      verbose() << "no related PV" << endmsg;
    } // if relatedPV

    // print some generic info testing proto particle
    verbose() << "---- new particle PID " << (*iParticle)->particleID().pid() << endmsg;
    if ((*iParticle)->proto()) {
      verbose() << " RICH DLL e           " << (*iParticle)->proto()->info(LHCb::ProtoParticle::RichDLLe , -999) << endmsg;
      verbose() << " Muon LL  mu          " << (*iParticle)->proto()->info(LHCb::ProtoParticle::MuonMuLL , -999) << endmsg;
      verbose() << " RICH PID             " << (*iParticle)->proto()->richPID() << endmsg;
      if ((*iParticle)->proto()->richPID())
        verbose() << " RICH best PID        " << (*iParticle)->proto()->richPID()->bestParticleID () << endmsg;
      
      verbose() << " Muon                 "  << (*iParticle)->proto()->muonPID() << endmsg;
      if ((*iParticle)->proto()->muonPID())
        verbose() << " Muon isMuon "      << (*iParticle)->proto()->muonPID()->IsMuon() << endmsg;

      
      verbose() << " get Calo" << endmsg;
      const SmartRefVector< LHCb::CaloHypo > &caloVec = (*iParticle)->proto()->calo();
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
    verbose() << " p " << (*iParticle)->p() << endmsg;

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
        verbose() << "  new daughter with PID       " << (*iDaughter)->particleID().pid() << endmsg;
        verbose() << "      number of sub-daughters " << (*iDaughter)->daughters().size() << endmsg;   
          if ((*iDaughter)->proto()) {
            verbose() << "         RICH PID " << (*iDaughter)->proto()->richPID() << endmsg;
            if ((*iDaughter)->proto()->richPID()) {
              verbose() << "         RICH best PID       " << (*iDaughter)->proto()->richPID()->bestParticleID () << endmsg;
            }// if RICH PID

            verbose() << "         get Calo" << endmsg;
            const SmartRefVector< LHCb::CaloHypo > &caloVec = (*iDaughter)->proto()->calo();
            verbose() << "       #elements " << caloVec.size() << endmsg;
            SmartRefVector< LHCb::CaloHypo >::const_iterator iCalo;
            SmartRefVector< LHCb::CaloHypo >::const_iterator iCaloBegin = caloVec.begin();
            SmartRefVector< LHCb::CaloHypo >::const_iterator iCaloEnd   = caloVec.end();
            for (iCalo = iCaloBegin; iCalo != iCaloEnd; iCalo ++) {
              verbose() << "new calo info obtained from proto-particle" << endmsg;
              const SmartRef<LHCb::CaloHypo > caloRef = *iCalo;
              verbose() << " smartRef " << caloRef 
                        << " hintID " << iCalo->hintID()  << " linkID " << iCalo->linkID()  << endmsg;
              
//              if ( iCalo->hintID() >= 0 )  {
//                LinkManager::Link* lnk = caloE->linkMgr()->link(iCalo->hintID());
//                
//                verbose() << " Got link [" << iCalo->hintID() << "] " << (void*)lnk << endmsg;
//                if ( lnk )  {
//                  verbose() << " -> " << lnk->path() << " address " << lnk->address()<< endmsg;
//                }
//              }//if
              
              
              if (caloRef) {
                verbose() << "  energy " << (*iCalo)->e() << endmsg;
              }//if
            }//for calo
          } // if proto
      } //for iDaughter
    }// if #daughter

  } //for



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
  LHCb::RecVertices* verts                              = get<LHCb::RecVertices>("/Event/" +
                                                                                 rootOnTES()+ "/" +
                                                                                 LHCb::RecVertexLocation::Primary);

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
  return StatusCode::SUCCESS;
} // sc PrintPV
//=============================================================================
