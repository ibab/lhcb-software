// $Id: TracksCreator.cpp,v 1.1.1.1 2005-03-03 14:36:10 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// LHCbEvent
#include "Event/EventHeader.h"
#include "Event/OTClusterOnTrack.h"
#include "Event/ITClusterOnTrack.h"
#include "Event/VeloRClusterOnTrack.h"
#include "Event/VeloPhiClusterOnTrack.h"
#include "Event/MCParticle.h"

// histogramming
#include "AIDA/IHistogram1D.h"

// local
#include "TracksCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TracksCreator
//
// 2005-02-22 : Eduardo Rodrigues
//-----------------------------------------------------------------------------
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 27-7-1999
//   Modified: 28-6-2000: R.M. van der Eijk
//   Modified: 28-5-2002: R.M. van der Eijk (adapt to xml-event model)
//   Modified: 05-7-2002: R.H. Hierck
//
// A TracksCreator is a Gaudi top level Algorithm that creates tracks and
// adds the true TrMeasurements to them. The true measurements are obtained
// by cheating into the Monte Carlo information. The tracks are then stored 
// in the transient event store.
//
// Currently the following types of measurement can be added:
//    - Outer Tracker clusters (OTHitOnTrack)
//    - Inner Tracker clusters (ITHitOnTrack)
//    - Velo R clusters (VeloClusterOnTrack)
//    - Velo Phi clusters (VeloClusterOnTrack)
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TracksCreator>          s_factory ;
const        IAlgFactory& TracksCreatorFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TracksCreator::TracksCreator( const std::string& name,
                              ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_otTracker(0)
  , m_itTracker(0)
  , m_velo(0)
  , fNumTracksHisto(0)
  , fNumHitsOnTrackHisto(0)
{
  // declare job options properties
  declareProperty( "AddOTTimes",      m_addOTTimes = true );
  declareProperty( "AddITClusters",   m_addITClusters = true );
  declareProperty( "AddVeloClusters", m_addVeloClusters = true );
  declareProperty( "MinNHits",        m_minNHits = 6 );
  declareProperty( "HistogramFlag" ,  m_produceHistogram = false );
  declareProperty( "TracksTESPath",
                   m_tracksTESPath = "Rec/FitTrack/Ideal" );
  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = "/dd/Structure/LHCb/Tracker/OT" );
  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = "/dd/Structure/LHCb/Tracker/IT" );
  declareProperty( "VeloGeometryPath",
                   m_otTrackerPath = "/dd/Structure/LHCb/Velo" );
}
//=============================================================================
// Destructor
//=============================================================================
TracksCreator::~TracksCreator() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TracksCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endreq;

  //MC associators
  sc = toolSvc() -> retrieveTool( "OTTime2MCDepositAsct", m_otTimeToMCDep );
  if ( !sc ) { return sc; }
   
  sc = toolSvc() -> retrieveTool( "ITCluster2MCParticleAsct", m_itClusToMCP );
  if ( !sc ) { return sc; }

  sc = toolSvc() -> retrieveTool( "VeloCluster2MCParticleAsct",m_veloClusToMCP );
  if ( !sc ) { return sc; }

   ///Geometry information
   // Loading OT Geometry from XML
   debug() << " start reading geometry ..." << endreq;
   m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );

   m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

   m_velo      = getDet<DeVelo>( m_veloPath );

   // MC judge
   m_trAcceptance = tool<ITrReconstructible>( "TrAcceptance" );
  //sc= toolSvc() -> retrieveTool( "TrAcceptance", m_trAcceptance );

   // TrTrueStateCreator
   sc= toolSvc() -> retrieveTool( "TrueStateCreator", m_stateCreator );

  // intialize histograms
   if( m_produceHistogram )
     sc = this -> initHistograms();

  info() << "initialized succesfully" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TracksCreator::execute() {

  debug() << "==> Execute" << endreq;

  StatusCode sc;

  // Event header info
  EventHeader* evtHdr = get<EventHeader>( EventHeaderLocation::Default );
  debug() << "Run " << evtHdr -> runNum()
          << "\tEvent " << evtHdr -> evtNum() << endreq;

  // Retrieve the MCParticle container
  MCParticles* partCtnr = get<MCParticles>( MCParticleLocation::Default );

  /// Retrieve the Containers for Clusters and Times
  OTTimes* otTimes           = get<OTTimes>( OTTimeLocation::Default );
  ITClusters* itClusters     = get<ITClusters>( ITClusterLocation::Default );
  VeloClusters* veloClusters = get<VeloClusters>( VeloClusterLocation::Default );
  
  // Make container for tracks
  TrFitTracks* tracksCont = new TrFitTracks();

  MCParticles::const_iterator pItr;
  for ( pItr = partCtnr -> begin(); partCtnr -> end() != pItr; pItr++ ) {
    MCParticle* thisMCPart = *pItr;
    if(    m_trAcceptance -> hasVeloAndSeed( thisMCPart )
        && m_trAcceptance -> hasTT1( thisMCPart ) ) {
      TrFitTrack* thisTrack = new TrFitTrack();
      
      //	HepVector stateVec = HepVector(5, 0);
      //HepSymMatrix stateCov = HepSymMatrix(5, 1);
      //TrStateP* pState = new TrStateP(0.0, stateVec, stateCov);
      //thisTrack -> addState(pState);
      
      // ============= Add OTTimes =============
      if ( m_addOTTimes == true && otTimes != 0 ) {
        // add OT times to tracks
        sc = addOTTimes( otTimes,thisMCPart,thisTrack );
        if ( sc.isFailure() )
          return Error( "Unable to add outer tracker times" );
        verbose() << "Added OT times..." << endreq;
      } // outer tracker times
      
      // ============= Add IT clusters =============
      if ( m_addITClusters == true && itClusters != 0 ) {
        // add IT clusters to tracks
        sc = addITClusters(itClusters,thisMCPart,thisTrack);
        if ( sc.isFailure() )
          return Error( "Unable to add inner tracker clusters" );
        verbose() << "Added IT clusters..." << endreq;
      } // inner tracker clusters
      
      
      //============= Add Velo R clusters=========================
      if ( m_addVeloClusters == true && veloClusters !=0 ) {
        
        debug() << "Retrieved " << veloClusters -> size()
                << " R clusters" << endreq;
        
        // add Velo R clusters to tracks
        sc = addVeloClusters(veloClusters,thisMCPart,thisTrack);
        if ( sc.isFailure() )
          return Error( "Unable to add velo R clusters" );
        verbose() << "Added Velo R clusters..." << endreq;
      } // velo R clusters
        
        
      //============= Initialize state=========================
      if( (int) thisTrack -> nMeasurements() > m_minNHits ) {
        thisTrack -> sortMeasurements();
        sc= this -> initializeState( (*(thisTrack->rbeginM()))->z(),
                                     thisTrack, thisMCPart );
        if( sc.isSuccess() ){
          debug() << " track added with nr of hits "
              << thisTrack -> nMeasurements()<< endreq;
          tracksCont -> add( thisTrack );
        }
        else {      
          delete thisTrack;
        }
      }
      else {
        delete thisTrack;
      }
    }
    
  } // looping of MCParticles


  // fill some monitoring histograms
  if( m_produceHistogram ) {
    TrFitTracks::iterator iTrackIterator= tracksCont -> begin();
    fNumTracksHisto -> fill((float)tracksCont -> size(),1.);
    for (iTrackIterator=tracksCont -> begin();iTrackIterator!=tracksCont -> end();
         iTrackIterator++){
      fNumHitsOnTrackHisto -> fill((*iTrackIterator) -> nMeasurements(),1.);
    }
  }

  // store tracks in EvDS
  sc = this -> registerTracks( tracksCont );
  if( sc.isFailure() ) return sc;

  debug() << " -> Finished Event. Created "
          << tracksCont -> size() << " tracks." << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TracksCreator::finalize() {

  debug() << "==> Finalize" << endreq;

  // Release all tools and services
  // ------------------------------
  if ( 0 != m_otTimeToMCDep ) {
    toolSvc() -> releaseTool( m_otTimeToMCDep );
    m_otTimeToMCDep = 0;
  }
  if ( 0 != m_itClusToMCP ) {
    toolSvc() -> releaseTool( m_itClusToMCP );
    m_itClusToMCP = 0;
  }
  if ( 0 != m_veloClusToMCP ) {
    toolSvc() -> releaseTool( m_veloClusToMCP );
    m_veloClusToMCP = 0;
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Register the tracks container in the TES. TES becomes owner of the cont.
//=============================================================================
StatusCode TracksCreator::registerTracks( TrFitTracks* tracksCont )
{
  StatusCode sc = put( tracksCont, m_tracksTESPath );
  if ( sc.isFailure() )
    error() << "Unable to register the output container at "
            << m_tracksTESPath << ". Status is " << sc << endreq;
  verbose() << "Track container stored in EvDS." << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TracksCreator::initializeState( double z, TrFitTrack* track,
                                           MCParticle* mcPart )
{
  TrState* tempState;
  m_stateCreator -> createState(mcPart,z,tempState);
  TrStateP* state = dynamic_cast<TrStateP*>( tempState );

//     // retrieve a MCTrackingHits for this spill
//     SmartDataPtr<MCHits>
//       mcOTTrackerHits(eventSvc(),MCHitLocation::OTHits);

//     if ( !mcOTTrackerHits ) {
//       // failed to find hits
//       log << MSG::DEBUG << "Unable to retrieve MC OT hits "<< endreq;
//     }


//     double dZ=10000.;
//     MCHit* closestHit=0;
//     MCHits::const_iterator iMCHit=mcOTTrackerHits->begin();
//     while( iMCHit!=mcOTTrackerHits->end())     {
//       if(mcPart == (*iMCHit)->mcParticle()){
//         if(fabs( z -(*iMCHit)->exit().z())<dZ){
//  	 dZ =fabs( z -(*iMCHit)->exit().z());
//  	 closestHit =*iMCHit;
//         }
//       }
//       iMCHit++;
//     }

//     // retrieve a MCTrackingHits for this spill
//     SmartDataPtr<MCHits>
//       mcITTrackerHits(eventSvc(),MCHitLocation::ITHits);

//     if ( !mcITTrackerHits ) {
//       // failed to find hits
//       log << MSG::DEBUG << "Unable to retrieve MC IT hits "<< endreq;
//     }

//     iMCHit=mcITTrackerHits->begin();
//     while( iMCHit!=mcITTrackerHits->end())     {
//       if(mcPart == (*iMCHit)->mcParticle()){
//         if(fabs( z -(*iMCHit)->exit().z())<dZ){
//  	 dZ =fabs( z -(*iMCHit)->exit().z());
//  	 closestHit =*iMCHit;
//         }
//       }
//       iMCHit++;
//     }


//     if(!closestHit){
//       return StatusCode::FAILURE;
//     }


//     if( fabs(closestHit->exit().z() - z ) >100.) {
//       return StatusCode::FAILURE;
//     }
//     else{

//       double HitTx  = (closestHit->exit().x()-closestHit->entry().x())/
//         (closestHit->exit().z()-closestHit->entry().z());
//       double HitTy  = (closestHit->exit().y()-closestHit->entry().y())/
//         (closestHit->exit().z()-closestHit->entry().z());

//       state->setX(closestHit->exit().x());
//       state->setY(closestHit->exit().y());
//       state->setZ(closestHit->exit().z());
//       state->setTx(HitTx);
//       state->setTy(HitTy);

//       double momentum = mcPart->momentum().vect().mag();
       double charge = mcPart -> particleID().threeCharge()/3.;
//       state->setQdivP(charge/momentum);
//       ///Set Covariance Matrix element

     track -> setCharge(charge);
     track -> addState(state);

     //}
   // end
   return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TracksCreator::addOTTimes( OTTimes* times,
                                      MCParticle* mcPart, TrFitTrack* track )
{
  /// Loop over outer tracker times and clusters to tracks

  // Get the association OTTime => MCParticle
  OTTime2MCDepositAsct::DirectType* m_pTable = m_otTimeToMCDep -> direct();
  if ( 0 == m_pTable )
    return Error( "Unable to retrieve the Direct OT Table" );

  // for now loop over everything
  OTTimes::const_iterator itOTTime = times -> begin();

  // Loop over outer tracker times
  while (itOTTime != times -> end()) {

    OTTime* aTime = *itOTTime;

     // retrieve MCParticle associated with this time
     OTTime2MCDepositAsct::DirectType::Range range =
       m_pTable -> relations(aTime);
     if ( !range.empty() ) {
       MCOTDeposit* aMCDeposit = range.begin() -> to();
       if((range.begin() -> to() -> mcHit())) {
	       MCParticle* aParticle =aMCDeposit -> mcHit() -> mcParticle();
	       if(aParticle == mcPart){
	         //	if(aParticle -> key() == mcPart -> key()){
	         int ambig = aMCDeposit -> ambiguity();
	         OTClusterOnTrack* otTime =
             new OTClusterOnTrack(aTime,ambig,m_otTracker);
	         track -> addMeasurement(otTime);
         }
       }
     }
    // next time
    itOTTime++;
  } // loop over outer tracker times

  debug() << " Found all OT Hits " << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TracksCreator::addITClusters( ITClusters* clusters,
                                         MCParticle* mcPart, TrFitTrack* track )
{
  /// loop over inner tracker clusters and add hits to tracks

  // retrieve IT part table
  ITCluster2MCParticleAsct::DirectType* m_pTable = m_itClusToMCP -> direct();
  if ( 0 == m_pTable ) {
    warning() << "Failed to find part table" << endreq;
    return StatusCode::FAILURE;
  }

  // for now loop over everything
  ITClusters::const_iterator itITCluster = clusters -> begin();

  // Loop over inner tracker clusters
  while ( itITCluster != clusters -> end() ) {

    ITCluster* aCluster = *itITCluster;

    // retrieve MCParticle associated with this cluster
    ITCluster2MCParticleAsct::DirectType::Range range =
      m_pTable -> relations(aCluster);
    if ( !range.empty() ) {
      MCParticle* aParticle = range.begin() -> to();
      if( aParticle == mcPart ) {
	      ITClusterOnTrack* itClus = new ITClusterOnTrack(aCluster,m_itTracker);
	      track -> addMeasurement(itClus);
      }
    }

    // next cluster
    itITCluster++;
  } // loop over inner tracker clusters

  // end
  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TracksCreator::addVeloClusters( VeloClusters* clusters,
                                           MCParticle* mcPart,
                                           TrFitTrack* track )
{
  // retrieve IT part table
  VeloCluster2MCParticleAsct::DirectType* m_pTable = m_veloClusToMCP -> direct();
  if (0 == m_pTable){
    warning() << "Failed to find part table" << endreq;
    return StatusCode::FAILURE;
  }

  // for now loop over everything
  VeloClusters::const_iterator itVeloCluster = clusters -> begin();

  // Loop over inner tracker clusters
  while (itVeloCluster != clusters -> end()) {
    VeloCluster* aCluster = *itVeloCluster;

    // retrieve MCParticle associated with this cluster
    VeloCluster2MCParticleAsct::DirectType::Range range =
      m_pTable -> relations(aCluster);
    if (!range.empty()){
      MCParticle* aParticle = range.begin() -> to();
      if(aParticle == mcPart){
	if(m_velo -> isRSensor(aCluster -> sensor())){
	  VeloRClusterOnTrack* VeloRClus = new VeloRClusterOnTrack(aCluster,m_velo);
	  track -> addMeasurement(VeloRClus);
	}
	else{
	  VeloPhiClusterOnTrack* VeloPhiClus =
	    new VeloPhiClusterOnTrack(aCluster,m_velo);
	  track -> addMeasurement(VeloPhiClus);
	}
      }
    }

    // next cluster
    itVeloCluster++;
  } // loop over inner tracker clusters



    // end
    return StatusCode::SUCCESS;
}

//=============================================================================
//  Histograms initialization
//=============================================================================
StatusCode TracksCreator::initHistograms()
{
  // retrieve histo dir path
  //std::string tDirPath = this -> histoDirPath();
  std::string tDirPath = "Tr/TrackIdealPR";

  // book histograms
  fNumTracksHisto = histoSvc() -> book( tDirPath + "1000",
                                        "Number of Tracks to be fitted",
                                        201, 0., 200.5 );
  if ( !fNumTracksHisto )
    return Error( "Unable to book the histogram \"NumTracksHisto\"!" );

  fNumHitsOnTrackHisto = histoSvc() -> book( tDirPath + "2000",
                                             "Number of hits on track",
                                             201, -0.5, 200.5);
  if ( !fNumHitsOnTrackHisto )
    return Error( "Unable to book the histogram \"NumHitsOnTrackHisto\!" );

  return StatusCode::SUCCESS;
}

//=============================================================================
