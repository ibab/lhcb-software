// Include files
// -------------
// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// from TrackKernel
#include "TrackKernel/CubicStateInterpolationTraj.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"
#include "TrackInterfaces/ITrackExtraSelector.h"
#include "TrackInterfaces/IMaterialLocator.h"

// Local 
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // ITrackExtrapolator::propagate" only partially overridden
  #pragma warning(push)
#endif
#include "TrackMasterExtrapolator.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackMasterExtrapolator )

// Disable warning on windows about using 'this' in constructors
// necessary for tool handles
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMasterExtrapolator::TrackMasterExtrapolator( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : TrackExtrapolator(type, name, parent),
    m_extraSelector("TrackDistanceExtraSelector",this)
{
  //job options
  declareProperty( "ExtraSelector", m_extraSelector ) ;
  declareProperty( "ApplyMultScattCorr"  , m_applyMultScattCorr  = true );
  declareProperty( "ApplyEnergyLossCorr" , m_applyEnergyLossCorr = true );
  declareProperty( "MaxStepSize"         , m_maxStepSize         = 1000.*mm );
  declareProperty( "MaxSlope"            , m_maxSlope            = 5. );
  declareProperty( "MaxTransverse"       , m_maxTransverse       = 10.*m );

  //for electrons
  declareProperty( "ApplyElectronEnergyLossCorr",m_applyElectronEnergyLossCorr = true );
  //declareProperty( "StartElectronCorr", m_startElectronCorr = 2500.*mm );
  //declareProperty( "StopElectronCorr",  m_stopElectronCorr  = 9000.*mm );
  declareProperty("MaterialLocator", m_materialLocatorname = "DetailedMaterialLocator" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackMasterExtrapolator::~TrackMasterExtrapolator() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackMasterExtrapolator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );

  // selector
  sc = m_extraSelector.retrieve() ;
  if( !sc.isSuccess() ) {
    error() << "Cannot retrieve ExtraSelector" << endmsg ;
  }
  
  // initialize transport service
  m_materialLocator = tool <IMaterialLocator> (m_materialLocatorname, "MaterialLocator", this);
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  return sc;
}

StatusCode TrackMasterExtrapolator::finalize()
{
  m_extraSelector.release().ignore() ;
  return TrackExtrapolator::finalize() ;
}

//=========================================================================
// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
// Note: energy loss correction is _NOT_ applied.
//=========================================================================
StatusCode TrackMasterExtrapolator::propagate( Gaudi::TrackVector& stateVec,
                                               double zOld,
                                               double zNew,
                                               Gaudi::TrackMatrix* transMat,
                                               LHCb::ParticleID pid ) const
{ 
  const ITrackExtrapolator* thisExtrapolator = m_extraSelector->select( zOld, zNew );
  return thisExtrapolator->propagate( stateVec, zOld, zNew, transMat, pid );
}


//=========================================================================
//  Main method: Extrapolate a State
//=========================================================================
StatusCode TrackMasterExtrapolator::propagate( LHCb::State& state, 
					       double zNew,
					       Gaudi::TrackMatrix* transMat,
					       LHCb::ParticleID pid ) const
{
  StatusCode sc(StatusCode::SUCCESS,true) ;

  // Create transport update matrix. The reason to make a pointer to a
  // local object (rather than just create it with new) is all the
  // intermediate returns.
  TrackMatrix updateMatrix = ROOT::Math::SMatrixIdentity();
  TrackMatrix* upMat = nullptr;
  if( transMat ) {
    *transMat = ROOT::Math::SMatrixIdentity();
    upMat = &updateMatrix ;
  }
  
  //check if not already at required z position
  const double zStart = state.z();
  if (std::abs(zNew-zStart) < TrackParameters::propagationTolerance) {
    if( m_debugLevel ) debug() << "already at required z position" << endmsg;
    return StatusCode::SUCCESS;
  }

  int    nbStep = (int)( std::abs( zNew-zStart ) / m_maxStepSize ) + 1;
  double zStep  = ( zNew - zStart ) / nbStep;
  size_t nWallsTot(0) ;
  
  if( msgLevel( MSG::VERBOSE ) ) verbose() << "state_in = " << state << std::endl
					   << "z_out = " << zNew
					   << "num steps = " << nbStep << endmsg ;

  for ( int step=0 ; nbStep > step ; ++step ) {
    TrackVector& tX = state.stateVector();
    XYZPoint start( tX[0], tX[1], state.z() );  // Initial position
    XYZVector vect( tX[2]*zStep, tX[3]*zStep, zStep );
    
    // protect against vertical or looping tracks
    if ( std::abs(start.x()) > m_maxTransverse ) {
      if( m_debugLevel )
	debug() << "Protect against absurd tracks: x=" << start.x() 
		<< " (max " << m_maxTransverse << " allowed)." << endmsg;
      return Warning( "Protect against absurd tracks. See debug for details", StatusCode::FAILURE,1 );
    }
    if ( std::abs(start.y()) > m_maxTransverse ) {
               //          StatusCode::FAILURE, 1 );
      if( m_debugLevel )
	debug() << "Protect against absurd tracks: y=" << start.y() 
		<< " (max " << m_maxTransverse << " allowed)." << endmsg;
      return Warning( "Protect against absurd tracks. See debug for details", StatusCode::FAILURE,1 );
    }
    if (std::abs(state.tx()) > m_maxSlope) {
      if( m_debugLevel )
	debug() << "Protect against looping tracks: tx=" << state.tx() 
		<< " (max " << m_maxSlope << " allowed)." << endmsg;
      return Warning( "Protect against looping tracks. See debug for details", StatusCode::FAILURE,1 );
    }    
    if (std::abs(state.ty()) > m_maxSlope) {
      if( m_debugLevel )
	debug() << "Protect against looping tracks: ty=" << state.ty() 
		<< " (max " << m_maxSlope << " allowed). " << endmsg;
      return Warning( "Protect against looping tracks. See debug for details", StatusCode::FAILURE,1 );
    }
 
    // propagate the state, without any material corrections:
    double zorigin = state.z() ;
    double ztarget = zorigin + zStep ;

    LHCb::State stateAtOrigin( state ) ;
    const ITrackExtrapolator* thisExtrapolator = m_extraSelector->select(zorigin,ztarget);
    sc = thisExtrapolator->propagate( state, ztarget, upMat );

    // check for success
    if ( sc.isFailure() ) {
      if ( m_debugLevel ) debug() << "Transport to " << ztarget
				  << "using "+thisExtrapolator->name() << " FAILED" << endmsg;
      return Warning( "Transport to wall using "+thisExtrapolator->name()+ "FAILED", sc,1 );
    }
    
    //update f
    if( transMat != 0 ) {
      TrackMatrix tempMatrix = *transMat;
      *transMat = updateMatrix * tempMatrix; 
    }
  
    // now apply material corrections
    if( m_applyMultScattCorr || m_applyEnergyLossCorr || m_applyElectronEnergyLossCorr ) {
      LHCb::CubicStateInterpolationTraj traj( stateAtOrigin, state ) ;
      IMaterialLocator::Intersections intersections ;
      if( m_materialLocator->intersect( traj, intersections) >0 ) {
	nWallsTot += intersections.size() ;
	m_materialLocator->applyMaterialCorrections(state,intersections,zorigin,pid,
						    m_applyMultScattCorr,
						    m_applyEnergyLossCorr || m_applyElectronEnergyLossCorr ) ;
      }
    }
  } // loop over steps
  
  if( msgLevel( MSG::VERBOSE ) ) verbose() << "state_out = " << state << std::endl
					   << "number of walls = " << nWallsTot << endmsg ;
  
  return sc ;
}

//=============================================================================
