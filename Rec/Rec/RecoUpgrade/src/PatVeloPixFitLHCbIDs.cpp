// $Id: PatVeloPixFitLHCbIDs.cpp,v 1.4 2010/03/10 16:25:59 lcarson Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// non-local classes used
#include "Event/Track.h"

// local
#include "PatVeloPixFitLHCbIDs.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloPixFitLHCbIDs
// Takes a list of VeloPix LHCbIDs from a track and reproduces the PatVelo 
// internal track fit
//
// 2010-03-10 : Laurence Carson and Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace Tf{
  DECLARE_TOOL_FACTORY( PatVeloPixFitLHCbIDs );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::PatVeloPixFitLHCbIDs::PatVeloPixFitLHCbIDs( const std::string& type,
					  const std::string& name,
					  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
    declareInterface<ITrackFitter>(this);
    m_s0 = 0;
    m_sx = 0;
    m_sy = 0;
    m_sz = 0;
    m_sx2 = 0;
    m_sy2 = 0;
    m_sz2 = 0;
    m_sxz = 0;
    m_syz = 0;
    m_slopeX = 0;
    m_slopeY = 0;
    m_interceptX = 0;
    m_interceptY = 0;
    m_meanZ = 0;

}
//=============================================================================
// Destructor
//=============================================================================
Tf::PatVeloPixFitLHCbIDs::~PatVeloPixFitLHCbIDs() {
}

StatusCode Tf::PatVeloPixFitLHCbIDs::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  
  m_veloPix = getDet<DeVeloPix>( DeVeloPixLocation::Default );

 m_positiontool =  tool<IVeloPixClusterPosition>("VeloPixClusterPosition");
  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVeloPixFitLHCbIDs::fit( LHCb::Track & track, LHCb::ParticleID){

  m_s0 = 0;
  m_sx = 0;
  m_sy = 0;
  m_sz = 0;
  m_sx2 = 0;
  m_sy2 = 0;
  m_sz2 = 0;
  m_sxz = 0;
  m_syz = 0;
  m_slopeX = 0;
  m_slopeY = 0;
  m_interceptX = 0;
  m_interceptY = 0;
  m_errorSlopX = 0;
  m_errorSlopY = 0;
  m_meanZ = 0; 

  m_clusters = get<LHCb::VeloPixLiteCluster::VeloPixLiteClusters>(LHCb::VeloPixLiteClusterLocation::Default );
  std::vector<Gaudi::XYZPoint> thePoints;
  
  std::vector<LHCb::LHCbID>::const_iterator iID;


  
  
  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    const LHCb::VeloPixLiteCluster* liteclus = m_clusters->object( iID->velopixID () );
    IVeloPixClusterPosition::toolInfo clusInfo = m_positiontool->position(liteclus) ;
    const DeVeloPixSquareType* sqDet = 
      static_cast<const DeVeloPixSquareType*>(m_veloPix->squareSensor(clusInfo.pixel.sensor()));
    Gaudi::XYZPoint thePixPoint = sqDet->globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;
    thePoints.push_back(thePixPoint);
  }
  std::vector<Gaudi::XYZPoint>::const_iterator iPt;
  
  // Set the first point in order to weight the hits
  
  bool backFlag = track.checkFlag(LHCb::Track::Backward);
  int indexFirstPoint(0);
  if (backFlag == true ) indexFirstPoint = thePoints.size()-1;
  const Gaudi::XYZPoint theFirstPixPoint = thePoints[indexFirstPoint];
  

  for(iPt = thePoints.begin() ; iPt != thePoints.end() ; ++iPt){
    const Gaudi::XYZPoint thePixPoint = (*iPt);
    double x = thePixPoint.x();
    double y = thePixPoint.y();
    double z = thePixPoint.z();
    double w = 1.;
    double DeltaZ = z - theFirstPixPoint.z();
    // Check that it is not the first point
    if ( fabs(DeltaZ) > 1e-6 ){
      if (backFlag) DeltaZ = -DeltaZ;
      w = 1. / DeltaZ;
    }

        m_s0  = m_s0  + w;
        m_sx  = m_sx  + w * x;
        m_sy  = m_sy  + w * y;
        m_sz  = m_sz  + w * z;
        m_sxz = m_sxz + w * x * z;
        m_syz = m_syz + w * y * z;
        m_sz2 = m_sz2 + w * z * z;

  }//end of loop over lhcbids

   //calculate slopes, intercept and errors
   double denom = ( m_sz2 * m_s0 - m_sz * m_sz );
   m_slopeX = ( m_sxz * m_s0  - m_sx  * m_sz ) / denom;
   m_slopeY = ( m_syz * m_s0  - m_sy  * m_sz ) / denom;
   m_interceptX  = ( m_sx  * m_sz2 - m_sxz * m_sz ) / denom;
   m_interceptY  = ( m_sy  * m_sz2 - m_syz * m_sz ) / denom;
   m_meanZ    = m_sz / m_s0;
   m_errorSlopX = m_s0 / denom;
   m_errorSlopY = m_s0 / denom;

// //     m_s0   = m_s0   + w;
// //     m_s02  = m_s02  + w * w;
// //     m_sx  = m_sx  + w * x;
// //     m_sy  = m_sy  + w * y;
// //     m_sz  = m_sz  + w * z;
// //     m_sxz = m_sxz +  w * x * z;
// //     m_syz = m_syz +  w * y * z;
// //     m_sx2 = m_sx2 +  w * x * x;
// //     m_sy2 = m_sy2 +  w * y * y;
// //     m_sz2 = m_sz2 + w * w * y * y;
//     m_sx = m_sx  +  x;
//     m_sy = m_sy  +  y;
//     m_sz = m_sz  +  z;
//     m_sx2 = m_sx2 + x * x;
//     m_sy2 = m_sy2 + y * y;
//     m_sz2 = m_sz2 + z * z;
//     m_sxz = m_sxz +  w * x * z;
//     m_syz = m_syz +  w * y * z;
//   }//end of loop over lhcbids


   //calculate slopes, intercept and errors
  
   //m_slopeX =  (m_sxz - m_sx*m_sz/thePoints.size())/(m_sx2-m_sx*m_sx/thePoints.size());//m_sxz/m_sx2;//m_sz/m_s0 - m_interceptX * m_sx/m_s0 ; 
  // m_slopeY =  (m_syz - m_sy*m_sz/thePoints.size())/(m_sy2-m_sy*m_sy/thePoints.size()); // m_syz/m_sy2;//m_sz/m_s0 - m_interceptY * m_sy/m_s0 ;

   // m_interceptX = m_sz/thePoints.size() - m_slopeX*m_sx/thePoints.size();
   // m_interceptY = m_sz/thePoints.size() - m_slopeY*m_sy/thePoints.size();

  double epsX(0.);
  double epsY(0.);
  
//   std::vector<Gaudi::XYZPoint>::const_iterator iPoint;
//   for ( iPoint = thePoints.begin() ; iPoint != thePoints.end() ; ++iPoint){
//     const Gaudi::XYZPoint pointP = (*iPoint); 
//     double w = 1.;
//     double z = pointP.z();
//     double DeltaZ = z - theFirstPixPoint.z();
//     // Check that it is not the first point
//     if ( fabs(DeltaZ) > 1e-6 ){
//       if (backFlag) DeltaZ = -DeltaZ;
//       w = 1. / DeltaZ;
//     }
//     epsX += pow(sqrt(w)*(pointP.z()-m_slopeX*pointP.x()-m_interceptX),2);
//     epsY += pow(sqrt(w)*(pointP.z()-m_slopeY*pointP.y()-m_interceptY),2);
//   }
  
  epsX = 1/thePoints.size()/(thePoints.size()-2)*(thePoints.size()*m_sz2-m_sz*m_sz-m_slopeX*m_slopeX*(m_sx2-m_sx*m_sx));
  epsY = 1/thePoints.size()/(thePoints.size()-2)*(thePoints.size()*m_sz2-m_sz*m_sz-m_slopeY*m_slopeY*(m_sy2-m_sy*m_sy));
  //  m_errorSlopX=thePoints.size()*epsX*epsX/(thePoints.size()*m_sx2-m_sx*m_sx);
  //  m_errorSlopY=thePoints.size()*epsY*epsY/(thePoints.size()*m_sy2-m_sy*m_sy);
  
  
  // m_errorSlopX = m_s02 * epsX / (m_sx2*m_s0*m_s0 - m_s02*m_sx*m_sx); // 1/(n-2) what to do with weights???
//m_errorSlopY = m_s02 * epsY / (m_sy2*m_s0*m_s0 - m_s02*m_sy*m_sy) ;

   //update the states
  const std::vector< LHCb::State * > states = track.states();
  std::vector< LHCb::State * >::const_iterator iState;

  for( iState = states.begin() ; iState != states.end() ; ++iState ){
    (*iState)->setTx(m_slopeX);
    (*iState)->setTy(m_slopeY);
    
    Gaudi::TrackSymMatrix covMatrix = (*iState)->covariance();
    covMatrix(2,2) = m_errorSlopX;
    covMatrix(3,3) = m_errorSlopY;
    (*iState)->setCovariance(covMatrix);
  }//end of loop over states


  return StatusCode::SUCCESS;
}
