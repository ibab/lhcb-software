// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Linker
#include "Linker/AllLinks.h"
#include "Event/MCTrackInfo.h"
#include "Event/Track.h"
// local
#include "CheatedVeloPixPat.h"

#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Linker/LinkedTo.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "Event/MCHit.h"

#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteMeasurement.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CheatedVeloPixPat
//
// 2010-02-25 Laurence Carson / Victor Coco
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CheatedVeloPixPat );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedVeloPixPat::CheatedVeloPixPat( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "outputTracksLocation",    m_outputTracksLocation = LHCb::TrackLocation::VeloPix);
  declareProperty( "MinimalMCHitForTrack",    m_minIDs = 2 );
  declareProperty( "UseLinearFit",    m_UseLinearFit = true );
  
}
//=============================================================================
// Destructor
//=============================================================================
CheatedVeloPixPat::~CheatedVeloPixPat() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheatedVeloPixPat::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;
  // retrieve the postion tool
  m_positiontool =  tool<IVeloPixClusterPosition>("VeloPixClusterPosition");
  
  m_veloPix = getDet<DeVeloPix>( DeVeloPixLocation::Default );
  
  m_veloPixFitter = tool<ITrackFitter>("Tf::PatVeloPixFitLHCbIDs") ;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedVeloPixPat::execute() {
  debug() << "==> Execute" << endmsg;

  verbose()<<"tag 0"<<endreq;
  m_clusters = get<LHCb::VeloPixLiteCluster::VeloPixLiteClusters>(LHCb::VeloPixLiteClusterLocation::Default );

  LHCb::MCParticles* mcParts =
    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  AllLinks<LHCb::MCParticle,ContainedObject> allIds( evtSvc(), msgSvc(), "Pat/LHCbID" );
  std::vector< std::vector<int> > linkedIds;
  LHCb::MCParticle* part = allIds.first();
  // loop over the MCPs in the linker
  while ( NULL != part ) {
    unsigned int minSize = part->key();
    //increase the size of the vector so that it have at least part->key() element
    //to access it via key... not very nice but practical.
    while ( linkedIds.size() <= minSize ) {
      std::vector<int> dum;
      linkedIds.push_back( dum );
    }
    linkedIds[part->key()].push_back( allIds.key() );//push back this lhcbid value
    part = allIds.next();
  }


  //== Prepare the container for tracks, and location 
  LHCb::Tracks * outputTracks;
  // create output track container on TES, if it does not exist
  if ( exist<LHCb::Tracks>( m_outputTracksLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputTracksLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputTracksLocation );
  }
  // make space for the new tracks 
  outputTracks->reserve(linkedIds.size());  
  
  //== Loop over MCParticle and create LHCb::Track out of the LHCbIDs
  LHCb::MCParticles::const_iterator itP;
  for ( itP = mcParts->begin(); mcParts->end() != itP; ++itP ) {
    LHCb::MCParticle* part = *itP;   
    LHCb::Track* newTrack = new LHCb::Track(outputTracks->size());
    newTrack->setHistory(LHCb::Track::PatVeloPixCheated);
    // this is for cheated cheated...
    double tx(part->momentum().px()/part->momentum().pz());
    double ty(part->momentum().py()/part->momentum().pz()); 
    if (part->momentum().pz()<0){
      newTrack->setFlag( LHCb::Track::Backward, true );
    }

    std::vector<IVeloPixClusterPosition::toolInfo> veloPixPositions;
    std::vector<Gaudi::XYZPoint> veloPixPoints;
    std::vector< LHCb::LHCbID >  lhcbidsList;
    unsigned int countHits = 0;
    unsigned int prevSensor = 200;
    
    // Loop over the LHCbID corresponding to this MCParticle
    if ( linkedIds.size() > (unsigned int) part->key() ) {
      for ( std::vector<int>::const_iterator itIm = linkedIds[part->key()].begin();
            linkedIds[part->key()].end() != itIm; ++itIm ) {
        // Get the Lite cluster corresponding to the LHCbID
        LHCb::LHCbID temp;
        
        temp.setDetectorType( (*itIm) >> 28 );  // create LHCbId from int. Clumsy !"
        temp.setID( (*itIm) );
        if ( ! temp.isVeloPix ()) continue;
      
        const LHCb::VeloPixLiteCluster* liteclus = m_clusters->object( temp.velopixID () );
 
        // Get the position (halfbox) and errors (most important is error, position is can be accessed via detelem)
        IVeloPixClusterPosition::toolInfo clusInfo = m_positiontool->position(liteclus) ;
        veloPixPositions.push_back(clusInfo) ;
        // Get the MCPostion of the Hit to check were not 10mm away...
        const DeVeloPixSquareType* sqDet =static_cast<const DeVeloPixSquareType*>(m_veloPix->squareSensor(clusInfo.pixel.sensor()));
        if (prevSensor != clusInfo.pixel.sensor()){
          prevSensor= clusInfo.pixel.sensor();
          countHits ++ ;
        } 
        Gaudi::XYZPoint thePixPoint = sqDet->globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;
        std::pair<double,double> pixSize = sqDet->PixelSize(clusInfo.pixel.pixel());

        double dx = pixSize.first*clusInfo.fractionalError.first ;
        if (sqDet->isLong(clusInfo.pixel.pixel())) dx = 0.1 ;//fixed to 0.1 mm whatever is the angle for long pixel
        double dy = pixSize.second*clusInfo.fractionalError.second;

        LHCb::State aState;
        aState.setState(thePixPoint.x(),thePixPoint.y(),thePixPoint.z() ,tx,ty,0.);
        Gaudi::TrackSymMatrix aCov;
        aCov(0,0)=dx*dx;
        aCov(1,1)=dy*dy;
        // Not so sure...
        aCov(2,2)= 6.e-5 ;
        aCov(3,3)= 6.e-5 ;
        aCov(4,4)= 1.e-6 ;
        aState.setCovariance(aCov);
        aState.setLocation( LHCb::State::EndVelo );
        lhcbidsList.push_back(temp);
        newTrack->addToStates(aState);
      }
      newTrack->setLhcbIDs(lhcbidsList  );
      newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );
      newTrack->setType( LHCb::Track::VeloPix );
      
    }
    // Be aware that the hit from same sensors are counted only once, but are all added to the list of ID
    if (countHits>m_minIDs){
      if (m_UseLinearFit == true){
        StatusCode scFit = m_veloPixFitter->fit(*newTrack, LHCb::ParticleID(211));
        if(!scFit.isSuccess()){
          warning()<<"The linear fit failed"<<endreq;
        }
      }
      outputTracks->insert(newTrack);
    }
    else{
      delete(newTrack);
    }
  }
  return StatusCode::SUCCESS;
};


//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheatedVeloPixPat::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiTupleAlg::finalize();  // must be called after all other actions
};


//=============================================================================
