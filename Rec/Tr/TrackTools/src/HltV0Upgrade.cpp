#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "Kernel/ITrajPoca.h"
#include "Event/TwoProngVertex.h"
#include "Event/Track.h"
#include "TrackKernel/TrackTraj.h"
#include <boost/assign/list_of.hpp>
#include "HltV0Upgrade.h"

/** @file HltV0Upgrade.cpp
 *  
 *  @author Jaap Panman
 *  @date   2008-03-04
 *  based on example made by Wouter Hulsbergen
 */

DECLARE_TOOL_FACTORY( HltV0Upgrade )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltV0Upgrade::HltV0Upgrade( const std::string& type, 
			    const std::string& name,
			    const IInterface* parent )
  :
  GaudiTool(type,name,parent)
{
  // need to declare interface 
  declareInterface<IHltV0Upgrade>(this);
}


//=============================================================================
// Destructor
//============================================================================
HltV0Upgrade::~HltV0Upgrade() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltV0Upgrade::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_magfieldsvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_pocatool = tool<ITrajPoca>( "TrajPoca" );
  m_trackfitter = tool<ITrackFitter>( "TrackMasterFitter", "TrackFitter", this ) ;
  
  return sc;
}

StatusCode HltV0Upgrade::fittrack( LHCb::Track& track ) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  if( track.fitStatus()==LHCb::Track::FitFailed ) {
    sc = StatusCode::FAILURE ;
  } else if( track.fitStatus()==LHCb::Track::FitStatusUnknown ) {
    sc = m_trackfitter->fit( track ) ;
    // unfortunately, the fitter does not set the status flag!
    if( sc.isSuccess() ) track.setFitStatus( LHCb::Track::Fitted );
    else                 track.setFitStatus( LHCb::Track::FitFailed );
  }
  return sc ;
}


StatusCode HltV0Upgrade::process( LHCb::TwoProngVertex& vertex ) const
{
  // returned statuscode
  StatusCode sc = StatusCode::SUCCESS ;

  // extract the two tracks (it doesn't really matter which one is the positive one)
  LHCb::Track* trackA = const_cast<LHCb::Track*>(vertex.trackA()) ;
  LHCb::Track* trackB = const_cast<LHCb::Track*>(vertex.trackB()) ;
  
  // fit the tracks if necessary
  sc = fittrack(*trackA) && fittrack(*trackB) ;

  // only do the rest if both tracks fitted successfully
  if( sc.isSuccess() ) {

    // turn the tracks into trajectories. it will be slightly faster here to
    // leave the fieldsvc pointer away.
    LHCb::TrackTraj trajA( *trackA, m_magfieldsvc) ;
    LHCb::TrackTraj trajB( *trackB, m_magfieldsvc) ;
    
    // mu is the expansion parameter along the tracktraj. seed the
    // expansion with the z position of the vertex:
    double muA,muB ;
    muA = muB = vertex.position().z() ;
    
    // calls pocatool. last argument is required precision.
    Gaudi::XYZVector deltaX; 
    StatusCode sc1 = m_pocatool->minimize( trajA,muA,trajB,muB,deltaX,0.001*Gaudi::Units::mm );
    
    // if successful, update the vertex
    if( sc1.isSuccess() ) {
      LHCb::StateVector stateA = trajA.stateVector(muA) ;
      LHCb::StateVector stateB = trajB.stateVector(muB) ;
     
      // update the momentum of the first track
      vertex.momA()(0) = stateA.tx() ;
      vertex.momA()(1) = stateA.ty() ;
      vertex.momA()(2) = stateA.qOverP() ;

      // update the momentum of the second track
      vertex.momB()(0) = stateB.tx() ;
      vertex.momB()(1) = stateB.ty() ;
      vertex.momB()(2) = stateB.qOverP() ;
      
      // update the position of the vertex
      Gaudi::XYZPoint position( 0.5*(stateA.x() + stateB.x()),
				0.5*(stateA.y() + stateB.y()),
				0.5*(stateA.z() + stateB.z()) ) ;
      vertex.setPosition( position ) ;

      // need to fill the distance somewhere. let's put it in the chsiquare
      vertex.setChi2( deltaX.R() ) ;
    }
  }
  return sc ;
}

//=============================================================================
