// $Id: TrackMasterExtrapolator.cpp,v 1.27 2007-09-05 13:15:06 wouter Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"
#include "TrackInterfaces/ITrackExtraSelector.h"
#include "TrackInterfaces/IMaterialLocator.h"

// Local 
#include "TrackMasterExtrapolator.h"

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackMasterExtrapolator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMasterExtrapolator::TrackMasterExtrapolator( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : TrackExtrapolator(type, name, parent),
    m_materialLocator("DetailedMaterialLocator")
{
  //job options
  declareProperty( "ExtraSelector",
                   m_extraSelectorName = "TrackDistanceExtraSelector" );
  declareProperty( "ApplyMultScattCorr"  , m_applyMultScattCorr  = true );
  declareProperty( "ThickWall"           , m_thickWall           = 0.*mm );
  declareProperty( "ApplyEnergyLossCorr" , m_applyEnergyLossCorr = true );
  declareProperty( "MaxStepSize"         , m_maxStepSize         = 1000.*mm );
  declareProperty( "MaxSlope"            , m_maxSlope            = 5. );
  declareProperty( "MaxTransverse"       , m_maxTransverse       = 10.*m );
  // State correction tools
  declareProperty( "ThinMSCorrectionTool",
                   m_thinmstoolname  = "StateThinMSCorrectionTool" );
  declareProperty( "ThickMSCorrectionTool",
                   m_thickmstoolname  = "StateThickMSCorrectionTool" );
  declareProperty( "GeneralDedxCorrectionTool",
                   m_dedxtoolname     = "StateSimpleBetheBlochEnergyCorrectionTool" );
  declareProperty( "ElectronDedxCorrectionTool",
                   m_elecdedxtoolname = "StateElectronEnergyCorrectionTool" );
  //for electrons
  declareProperty( "ApplyElectronEnergyLossCorr",
                   m_applyElectronEnergyLossCorr = true );
  declareProperty( "StartElectronCorr", m_startElectronCorr = 2500.*mm );
  declareProperty( "StopElectronCorr",  m_stopElectronCorr  = 9000.*mm );
  declareProperty("MaterialLocator",m_materialLocator ) ;
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

  // tools for multiple scattering corrections
  m_thinmstool  = tool<IStateCorrectionTool>( m_thinmstoolname,"ThinMSTool",this  );
  m_thickmstool = tool<IStateCorrectionTool>( m_thickmstoolname,"ThickMSTool",this  );

  // tools for dE/dx corrections
  m_dedxtool     = tool<IStateCorrectionTool>( m_dedxtoolname     );
  m_elecdedxtool = tool<IStateCorrectionTool>( m_elecdedxtoolname );

  // selector
  m_extraSelector = tool<ITrackExtraSelector>( m_extraSelectorName,
                                               "ExtraSelector",this );

  // initialize transport service
  sc = m_materialLocator.retrieve() ;
  if( !sc.isSuccess() ) return Error( "Failed to retrieve material intersector", sc ) ;
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
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
                                               LHCb::ParticleID pid )
{ 
  ITrackExtrapolator* thisExtrapolator = m_extraSelector->select( zOld, zNew );
  return thisExtrapolator->propagate( stateVec, zOld, zNew, transMat, pid );
}

//=========================================================================
//  Main method: Extrapolate a State
//=========================================================================
StatusCode TrackMasterExtrapolator::propagate( LHCb::State& state, 
                                               double zNew,
                                               Gaudi::TrackMatrix* transMat,
                                               LHCb::ParticleID partId )
{
  StatusCode sc(StatusCode::SUCCESS,true) ;
  // Create transport update matrix
  TrackMatrix updateMatrix = TrackMatrix( ROOT::Math::SMatrixIdentity() );
  TrackMatrix* upMat = NULL;
  // Check transport matrix
  if( transMat != NULL )
    {
      *transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );
      upMat = &updateMatrix;
    }

  //check if not already at required z position
  const double zStart = state.z();
  if (fabs(zNew-zStart) < TrackParameters::propagationTolerance) {
    if( m_debugLevel ) debug() << "already at required z position" << endreq;
    return StatusCode::SUCCESS;
  }

  bool isUpstream = zStart > zNew ; 
  int    nbStep = (int)( fabs( zNew-zStart ) / m_maxStepSize ) + 1;
  double zStep  = ( zNew - zStart ) / nbStep;
  size_t nWallsTot(0) ;
  
  if( msgLevel( MSG::VERBOSE ) ) verbose() << "state_in = " << state << std::endl
					   << "z_out = " << zNew
					   << "num steps = " << nbStep << endreq ;

  for ( int step=0 ; nbStep > step ; ++step ) {
    TrackVector& tX = state.stateVector();
    XYZPoint start( tX[0], tX[1], state.z() );  // Initial position
    XYZVector vect( tX[2]*zStep, tX[3]*zStep, zStep );
    
    // protect against vertical or looping tracks
    if ( fabs(start.x()) > m_maxTransverse ) {
      if( m_debugLevel )
	debug() << "Protect against absurd tracks: x=" << start.x() 
		<< " (max " << m_maxTransverse << " allowed)." << endreq;
      return Warning( "Protect against absurd tracks. See debug for details" );
    }
    if ( fabs(start.y()) > m_maxTransverse ) {
               //          StatusCode::FAILURE, 1 );
      if( m_debugLevel )
	debug() << "Protect against absurd tracks: y=" << start.y() 
		<< " (max " << m_maxTransverse << " allowed)." << endreq;
      return Warning( "Protect against absurd tracks. See debug for details" );
    }
    if (fabs(state.tx()) > m_maxSlope) {
      if( m_debugLevel )
	debug() << "Protect against looping tracks: tx=" << state.tx() 
		<< " (max " << m_maxSlope << " allowed)." << endreq;
      return Warning( "Protect against looping tracks. See debug for details" );
    }    
    if (fabs(state.ty()) > m_maxSlope) {
      if( m_debugLevel )
	debug() << "Protect against looping tracks: ty=" << state.ty() 
		<< " (max " << m_maxSlope << " allowed). " << endreq;
      return Warning( "Protect against looping tracks. See debug for details" );
    }
    
    // process walls

    if( m_applyMultScattCorr || m_applyEnergyLossCorr || m_applyElectronEnergyLossCorr ) {

      IMaterialLocator::Intersections intersections;
      int nWall = m_materialLocator->intersect( start, vect,intersections  );
      nWallsTot += nWall ;

      for( IMaterialLocator::Intersections::const_iterator it = intersections.begin() ;
	   it != intersections.end(); ++it ) {
	double zWall = zScatter( it->z1, it->z2, isUpstream );
	double tWall = fabs( it->z2 - it->z1 ) ;
	//for thick scatterers it is always z2. double zWall = it->z2 ;
	ITrackExtrapolator* thisExtrapolator = m_extraSelector->select(state.z(),zWall);
	sc = thisExtrapolator->propagate( state, zWall, upMat );
	
	// check for success
	if ( sc.isFailure() ) {
	  if( m_debugLevel )
	    debug() << "Transport to " << zWall
		    << "using "+thisExtrapolator->name() << " FAILED" << endreq;
	  return Warning( "Transport to wall using "+thisExtrapolator->name()+ "FAILED", sc );
	}
	
	//update f
	if( transMat != 0 ) {
          TrackMatrix tempMatrix = *transMat;
          *transMat = updateMatrix * tempMatrix; 
	}
	
	// multiple scattering
	if ( m_applyMultScattCorr ) {
	  if ( tWall >= m_thickWall ) {
	    m_thickmstool -> correctState( state, it->material, tWall, isUpstream );
	  } else {
	    m_thinmstool -> correctState( state, it->material, tWall, isUpstream );
	  }
	}
	
	// dE/dx energy loss
	if ( m_applyEnergyLossCorr ) 
	  m_dedxtool -> correctState( state, it->material, tWall, isUpstream );
		
	// electron energy loss
	if ( ( 11 == partId.abspid() ) && 
	     m_applyElectronEnergyLossCorr &&
	     (state.z() > m_startElectronCorr) &&
	     (state.z() < m_stopElectronCorr) ) {
	  m_elecdedxtool -> correctState( state, it->material, tWall, isUpstream );
	}
      } // loop over walls
    }

    // propagate from last wall to target
    double ztarget = start.z() + vect.z() ;
    if( fabs( state.z() - ztarget )> TrackParameters::propagationTolerance ) {
      ITrackExtrapolator* thisExtrapolator = m_extraSelector->select(state.z(),ztarget);
      sc = thisExtrapolator->propagate( state, ztarget, upMat );
      
      // check for success
      if ( sc.isFailure() ) {
        if ( m_debugLevel ) debug() << "Transport to " << ztarget
				    << "using "+thisExtrapolator->name() << " FAILED" << endreq;
        return Warning( "Transport to wall using "+thisExtrapolator->name()+ "FAILED", sc );
      }
      
      //update f
      if( transMat != 0 ) {
	TrackMatrix tempMatrix = *transMat;
        *transMat = updateMatrix * tempMatrix; 
      }
    }

  } // loop over steps

  if( msgLevel( MSG::VERBOSE ) ) verbose() << "state_out = " << state << std::endl
					   << "number of walls = " << nWallsTot << endreq ;

  return sc ;
}

//=============================================================================
//
//=============================================================================
double TrackMasterExtrapolator::zScatter( const double z1, 
                                          const double z2,
					  bool isUpstream ) const
{
  double zS;
  if ( fabs(z1-z2) >= m_thickWall ) {
    // thick scatter
    zS = (isUpstream ) ? GSL_MIN(z1,z2) : GSL_MAX(z1,z2) ; 
  } else {
    // thin scatter - at centre in z
    zS = 0.5*(z1+z2);
  }
  return zS;
}

//=============================================================================
