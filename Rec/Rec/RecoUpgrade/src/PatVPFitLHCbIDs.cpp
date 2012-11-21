// $Id: PatVPFitLHCbIDs.cpp,v 1.4 2010/03/10 16:25:59 lcarson Exp $
// Include files

// from Gu
#include "GaudiKernel/ToolFactory.h"

// non-local classes used
#include "Event/Track.h"

// local
#include "PatVPFitLHCbIDs.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatVPFitLHCbIDs
// Takes a list of VP LHCbIDs from a track and reproduces the PatVelo 
// internal track fit
//
// 2010-03-10 : Laurence Carson and Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace Tf{
  DECLARE_TOOL_FACTORY( PatVPFitLHCbIDs )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::PatVPFitLHCbIDs::PatVPFitLHCbIDs( const std::string& type,
					  const std::string& name,
					  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
    declareInterface<ITrackFitter>(this); 
    declareProperty( "stepError", m_stepError = 0.002 );
    declareProperty( "variableMS"      , m_variableMS       = false     );
}
//=============================================================================
// Destructor
//=============================================================================
Tf::PatVPFitLHCbIDs::~PatVPFitLHCbIDs() {
}

StatusCode Tf::PatVPFitLHCbIDs::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  m_vP = getDet<DeVP>( DeVPLocation::Default );
  m_positiontool =  tool<IVPClusterPosition>("VPClusterPosition");
  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVPFitLHCbIDs::fit( LHCb::Track & track, LHCb::ParticleID){
  
  m_sa = 0;
  m_sb = 0;
  m_s02 = 0;
  m_sax = 0;
  m_sby = 0;
  m_saz = 0;
  m_sbz = 0;
  m_saxz = 0;
  m_sbyz = 0;
  m_sax2 = 0;
  m_sby2 = 0;
  m_saz2 = 0;
  m_sbz2 = 0;
  m_slopeX = 0;
  m_slopeY = 0;
  m_interceptX = 0;
  m_interceptY = 0;
  m_errorSlopX = 0;
  m_errorSlopY = 0;
  m_meanZ_a = 0;
  m_meanZ_b = 0;  
  
  m_clusters = get<LHCb::VPLiteCluster::VPLiteClusters>(LHCb::VPLiteClusterLocation::Default );
  std::vector<Gaudi::XYZPoint> thePoints;

  // place to store the LHCbIDs from other detectors
  std::vector<LHCb::LHCbID> nonVELOIDs;
  std::vector<LHCb::LHCbID>::const_iterator iID1;

  for( iID1 = track.lhcbIDs().begin() ; iID1 != track.lhcbIDs().end() ; ++iID1){
    if( ! iID1->isVP() ) {
      nonVELOIDs.push_back(*iID1);
    }
  }

  // collect all states past the end of the VELO plus q/p if present
  // to copy back into track
  double veloQoverP = 0.;
  double veloQoverPerr2 = 0.;

  // Place to store the states from other detectors
  LHCb::Track::StateContainer savedStates;

  const std::vector< LHCb::State * > states = track.states();  

  debug()<<"!!-------------- Track of type:  "<<track.type()<<endreq;
  debug()<<"State size before : "<<states.size()<<endreq;

  std::vector< LHCb::State * >::const_iterator iState;
  for( iState = states.begin() ; iState != states.end() ; ++iState ){
    LHCb::State::Location location = (*iState)->location();
    if( ( location == LHCb::State::ClosestToBeam || 
	  location == LHCb::State::FirstMeasurement ||
	  location == LHCb::State::EndVelo ) && veloQoverP == 0.){
      veloQoverP = (*iState)->qOverP();
      veloQoverPerr2 = (*iState)->errQOverP2();
    }
    else{
      if((*iState)->z()<800)always()<<"(*iState)->z()<800"<<endreq;
      savedStates.push_back((*iState)->clone());
    }
  }

  debug()<<"Non Velo State: "<<savedStates.size()<<" velo state: "<<states.size()-savedStates.size()<<endreq;


  // Get the VP LHCbIDs and sort them (maybe not really needed)
  std::vector< std::pair< double, LHCb::LHCbID > > IDsToSort;
  std::vector<LHCb::LHCbID>::const_iterator iIDtmp;
  for( iIDtmp = track.lhcbIDs().begin() ; iIDtmp != track.lhcbIDs().end() ; ++iIDtmp){
    if (iIDtmp->isVP()){
       const LHCb::VPLiteCluster* liteclusL = m_clusters->object(iIDtmp ->vpID () );
       IVPClusterPosition::toolInfo clusInfoL = m_positiontool->position(liteclusL) ;
       const DeVPSquareType* sqDetL = 
         static_cast<const DeVPSquareType*>(m_vP->squareSensor(clusInfoL.pixel.sensor()));
       double zL = sqDetL->globalXYZ(clusInfoL.pixel.pixel(),clusInfoL.fractionalPosition).z() ;
       IDsToSort.push_back(std::make_pair(zL,*iIDtmp));
    }
  }
  if (track.checkFlag( LHCb::Track::Backward )){
    std::sort( IDsToSort.begin(), IDsToSort.end(),DecreasingByZ );
  }
  else{
    std::sort( IDsToSort.begin(), IDsToSort.end(),IncreasingByZ );
  }


  // Compute the multiple scattering correction term
  double msCorr = m_stepError;
  // use a rough functional form but only if the momentum has been estimated
  if(m_variableMS && veloQoverP!=0.){
    if( fabs(veloQoverP) > 2.e-4 ) {
      msCorr = 0.03; // fixed below 5 GeV/c
    }else{
      msCorr = 0.00025 + 0.04*exp(-7.e-5/fabs(veloQoverP));
    }
  }

  double MSError = 0;

  /////// Save the track properties
  LHCb::Track::History history = track.history();
  const SmartRefVector< LHCb::Track > ances = track.ancestors ();
  bool isback = track.checkFlag( LHCb::Track::Backward );
  
  /////// Now reset the track....
  track.reset(); 
  track.setHistory(history);
  if( ances.size() > 0 ) {
    for (SmartRefVector< LHCb::Track >::const_iterator iances = ances.begin();ances.end()!=iances;iances++){
      track.addToAncestors( (*iances).data () ); 
    }
  }
  if (isback)track.setFlag( LHCb::Track::Backward,isback);
  track.setType( LHCb::Track::Velo );
  

  //00309     newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );

  
  // Loop over the LHCb IDs and set a state for each + compute the linear regression
  std::vector< std::pair< double, LHCb::LHCbID > >::const_iterator iID;
  int index_ids = 0 ;
  for( iID = IDsToSort.begin() ; iID != IDsToSort.end() ; ++iID){
    track.addToLhcbIDs(((*iID).second)  );
    const LHCb::VPLiteCluster* liteclus = m_clusters->object( ((*iID).second).vpID () );
    if(liteclus == NULL) continue;
    IVPClusterPosition::toolInfo clusInfo = m_positiontool->position(liteclus) ;
    const DeVPSquareType* sqDet = 
      static_cast<const DeVPSquareType*>(m_vP->squareSensor(clusInfo.pixel.sensor()));
    Gaudi::XYZPoint thePixPoint = sqDet->globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;
    thePoints.push_back(thePixPoint);
    
    double x = thePixPoint.x();
    double y = thePixPoint.y();
    double z = thePixPoint.z();
    
    //Weight calculation using Variance
    std::pair<double,double> pixSize = sqDet->PixelSize(clusInfo.pixel.pixel());
    
    double dx = pixSize.first*clusInfo.fractionalError.first ;
    if (sqDet->isLong(clusInfo.pixel.pixel())) dx = 0.1 ;//fixed to 0.1 mm whatever is the angle for long pixel THIS IS NEEDED!
    double dy = pixSize.second*clusInfo.fractionalError.second;
    

    if(0 == index_ids){
      LHCb::State aState;
      aState.setState(thePixPoint.x(),thePixPoint.y(),thePixPoint.z() ,0.,0.,0.);
      Gaudi::TrackSymMatrix aCov;
      // Not so sure...
      aCov(0,0)=dx*dx;
      aCov(1,1)=dy*dy;
      aCov(2,2)= 6.e-5 ;
      aCov(3,3)= 6.e-5 ;
      aCov(4,4)= 1.e-6 ;
      aState.setCovariance(aCov);
      aState.setLocation( LHCb::State::FirstMeasurement );
      track.addToStates(aState);
    }
    else if ((int) IDsToSort.size()-1 == index_ids ){
      LHCb::State aState;
      aState.setState(thePixPoint.x(),thePixPoint.y(),thePixPoint.z() ,0.,0.,0.);
      Gaudi::TrackSymMatrix aCov;
      // Not so sure...
      aCov(0,0)=dx*dx;
      aCov(1,1)=dy*dy;
      aCov(2,2)= 6.e-5 ;
      aCov(3,3)= 6.e-5 ;
      aCov(4,4)= 1.e-6 ;
      aState.setCovariance(aCov);
      aState.setLocation( LHCb::State::EndVelo );
      track.addToStates(aState);
    }
    
    double xVariance = dx * dx;
    double yVariance = dy * dy;
    
    // weights
    double xWeight = 1.0 / (xVariance + MSError);
    double yWeight = 1.0 / (yVariance + MSError);
    MSError += msCorr;
    
    m_sa  = m_sa  + xWeight;
    m_sb  = m_sb  + yWeight;
    
    m_sax  = m_sax  + xWeight * x;
    m_sby  = m_sby  + yWeight * y;
    
    m_saz  = m_saz  + xWeight * z;
    m_sbz  = m_sbz  + yWeight * z;
    
    m_saxz = m_saxz + xWeight * x * z;
    m_sbyz = m_sbyz + yWeight * y * z;
    
    m_saz2 = m_saz2 + xWeight * z * z;
    m_sbz2 = m_sbz2 + yWeight * z * z;

    index_ids ++;
    
  }//end of loop over LHCbIDs 
  
   //calculate slopes, intercept and errors
  double xDenom = ( m_saz2 * m_sa - m_saz * m_saz );
  double yDenom = ( m_sbz2 * m_sb - m_sbz * m_sbz );
  
  m_slopeX = ( m_saxz * m_sa  - m_sax  * m_saz ) / xDenom;
  m_slopeY = ( m_sbyz * m_sb  - m_sby  * m_sbz ) / yDenom;
  
  m_interceptX  = ( m_sax  * m_saz2 - m_saxz * m_saz ) / xDenom;
  m_interceptY  = ( m_sby  * m_sbz2 - m_sbyz * m_sbz ) / yDenom;
  
  m_meanZ_a    = m_saz / m_sa;
  m_meanZ_b    = m_sbz / m_sb;
  
  //Need to work out correct formulae...
  m_errorSlopX = m_sa / xDenom;
  m_errorSlopY = m_sb / yDenom;


  
  //update the velo states
  const std::vector< LHCb::State * > statesUp = track.states();
  debug()<<"Velo State size after   : "<<statesUp.size()<<endreq;
  for( iState = statesUp.begin() ; iState != statesUp.end() ; ++iState ){
    (*iState)->setTx(m_slopeX);
    (*iState)->setTy(m_slopeY);
    
    Gaudi::TrackSymMatrix covMatrix = (*iState)->covariance();
    covMatrix(2,2) = m_errorSlopX;
    covMatrix(3,3) = m_errorSlopY;
    (*iState)->setCovariance(covMatrix);
    
    LHCb::State::Location location = (*iState)->location();
    if( location == LHCb::State::ClosestToBeam || 
        location == LHCb::State::FirstMeasurement ||
        location == LHCb::State::EndVelo ) {
      (*iState)->setQOverP(veloQoverP);
      (*iState)->setErrQOverP2(veloQoverPerr2);
    }
  }

  // add back the non-velo LHCbIDs
  for( iID1 = nonVELOIDs.begin() ; iID1 != nonVELOIDs.end() ; ++iID1){
    track.addToLhcbIDs(*iID1);
   }
 
  // copy the other track states back into the new track
  if(!savedStates.empty()){
    track.addToStates(savedStates);
  }


  return StatusCode::SUCCESS;
}


