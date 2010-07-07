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
    //Choose default (no momentum estimate) value from PatVeloFitLHCbIDs
    declareProperty( "stepError", m_stepError = 0.002 );
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
  
  
  m_clusters = get<LHCb::VeloPixLiteCluster::VeloPixLiteClusters>(LHCb::VeloPixLiteClusterLocation::Default );
  std::vector<Gaudi::XYZPoint> thePoints;
  
  std::vector<LHCb::LHCbID>::const_iterator iID;

 double MSError = 0;


  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    const LHCb::VeloPixLiteCluster* liteclus = m_clusters->object( iID->velopixID () );
    if(liteclus == NULL) continue;
    IVeloPixClusterPosition::toolInfo clusInfo = m_positiontool->position(liteclus) ;
    const DeVeloPixSquareType* sqDet = 
      static_cast<const DeVeloPixSquareType*>(m_veloPix->squareSensor(clusInfo.pixel.sensor()));
    Gaudi::XYZPoint thePixPoint = sqDet->globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;
    thePoints.push_back(thePixPoint);

    double x = thePixPoint.x();
    double y = thePixPoint.y();
    double z = thePixPoint.z();

   //Weight calculation using Variance
   std::pair<double,double> pixSize = sqDet->PixelSize(clusInfo.pixel.pixel());
   double dx = pixSize.first*clusInfo.fractionalError.first ;
   double dy = pixSize.second*clusInfo.fractionalError.second;

   double xVariance = dx * dx;
   double yVariance = dy * dy;

   // weights
   double xWeight = 1.0 / (xVariance + MSError);
   double yWeight = 1.0 / (yVariance + MSError);
   MSError += m_stepError;
   
      
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


// //     m_sx2 = m_sx2 +  w * x * x;
// //     m_sy2 = m_sy2 +  w * y * y;
// //     m_sz2 = m_sz2 + w * w * y * y;

   //calculate slopes, intercept and errors
  
   //m_slopeX =  (m_sxz - m_sx*m_sz/thePoints.size())/(m_sx2-m_sx*m_sx/thePoints.size());
   //m_sxz/m_sx2;//m_sz/m_s0 - m_interceptX * m_sx/m_s0 ; 
   // m_slopeY =  (m_syz - m_sy*m_sz/thePoints.size())/(m_sy2-m_sy*m_sy/thePoints.size());
   // m_syz/m_sy2;//m_sz/m_s0 - m_interceptY * m_sy/m_s0 ;

   // m_interceptX = m_sz/thePoints.size() - m_slopeX*m_sx/thePoints.size();
   // m_interceptY = m_sz/thePoints.size() - m_slopeY*m_sy/thePoints.size();

   //double epsX(0.);
   //double epsY(0.);
  

//     epsX += pow(sqrt(w)*(pointP.z()-m_slopeX*pointP.x()-m_interceptX),2);
//     epsY += pow(sqrt(w)*(pointP.z()-m_slopeY*pointP.y()-m_interceptY),2);
  
  // epsX = 1/thePoints.size()/(thePoints.size()-2)*(thePoints.size()*m_sz2-m_sz*m_sz-m_slopeX*m_slopeX*(m_sx2-m_sx*m_sx));
  // epsY = 1/thePoints.size()/(thePoints.size()-2)*(thePoints.size()*m_sz2-m_sz*m_sz-m_slopeY*m_slopeY*(m_sy2-m_sy*m_sy));
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
