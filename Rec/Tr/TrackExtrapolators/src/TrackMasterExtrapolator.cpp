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
  : TrackExtrapolator(type, name, parent)
{
  //job options
  declareProperty( "ExtraSelector",
                   m_extraSelectorName = "TrackDistanceExtraSelector" );
  declareProperty( "ApplyMultScattCorr"  , m_applyMultScattCorr  = true );
  declareProperty( "ThickWall"           , m_thickWall           = 0.*mm );
  declareProperty( "ApplyEnergyLossCorr" , m_applyEnergyLossCorr = true );
  declareProperty( "MaxStepSize"         , m_maxStepSize         = 1000.*mm );
  declareProperty( "MinRadThreshold"     , m_minRadThreshold     = 1.0e-4 );
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
  m_thinmstool  = tool<IStateCorrectionTool>( m_thinmstoolname  );
  m_thickmstool = tool<IStateCorrectionTool>( m_thickmstoolname );

  // tools for dE/dx corrections
  m_dedxtool     = tool<IStateCorrectionTool>( m_dedxtoolname     );
  m_elecdedxtool = tool<IStateCorrectionTool>( m_elecdedxtoolname );

  // selector
  m_extraSelector = tool<ITrackExtraSelector>( m_extraSelectorName,
                                               "ExtraSelector",this );

  // initialize transport service
  m_transportSvc = svc<ITransportSvc>( "TransportSvc", true );

  m_debugLevel = msgLevel( MSG::DEBUG );

  m_25m = 25. * m;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main method: Extrapolate a State
//=========================================================================
StatusCode TrackMasterExtrapolator::propagate( State& state, 
                                               double zNew,
                                               ParticleID partId )
{
  StatusCode sc = StatusCode::SUCCESS;

  // reset transport matrix
  m_F = TrackMatrix( ROOT::Math::SMatrixIdentity() );

  //check if not already at required z position
  const double zStart = state.z();
  if (fabs(zNew-zStart) < TrackParameters::hiTolerance) {
    debug() << "already at required z position" << endreq;
    return StatusCode::SUCCESS;
  }

  // check whether upstream or downstream
  zStart > zNew ? m_upStream = true : m_upStream = false; 

  int    nbStep = (int)( fabs( zNew-zStart ) / m_maxStepSize ) + 1;
  double zStep  = ( zNew - zStart ) / nbStep;
  int    nWall;
  
  for ( int step=0 ; nbStep > step ; ++step ) {
    ILVolume::Intersections intersept;
    TrackVector& tX = state.stateVector();
    XYZPoint start( tX[0], tX[1], state.z() );  // Initial position
    XYZVector vect( tX[2]*zStep, tX[3]*zStep, zStep );

    // protect against vertical or looping tracks
    if ( fabs(start.x()) > m_maxTransverse ) {
      Warning( "Protect against absurd tracks. See debug for details",
               StatusCode::FAILURE, 1 );
      debug() << "Protect against absurd tracks: x=" << start.x() 
              << " (max " << m_maxTransverse << " allowed)." << endreq;
      return StatusCode::FAILURE;
    }
    if ( fabs(start.y()) > m_maxTransverse ) {
      Warning( "Protect against absurd tracks. See debug for details",
               StatusCode::FAILURE, 1 );
      debug() << "Protect against absurd tracks: y=" << start.y() 
              << " (max " << m_maxTransverse << " allowed)." << endreq;
      return StatusCode::FAILURE;
    }
    if (fabs(state.tx()) > m_maxSlope) {
      Warning( "Protect against looping tracks. See debug for details",
               StatusCode::FAILURE, 1 );
      debug() << "Protect against looping tracks: tx=" << state.tx() 
              << " (max " << m_maxSlope << " allowed)." << endreq;
      return StatusCode::FAILURE;
    }    
    if (fabs(state.ty()) > m_maxSlope) {
      Warning( "Protect against looping tracks. See debug for details",
               StatusCode::FAILURE, 1 );
      debug() << "Protect against looping tracks: ty=" << state.ty() 
              << " (max " << m_maxSlope << " allowed). " << endreq;
      return StatusCode::FAILURE;
    }

    // check if transport is within LHCb
    if (fabs(start.x()) > m_25m || fabs(start.y()) > m_25m ||
        fabs(start.z()) > m_25m ||
        fabs(start.x()+vect.x()) > m_25m ||
        fabs(start.y()+vect.y()) > m_25m ||
        fabs(start.z()+vect.z()) > m_25m ) {
      nWall = 0;
      if ( m_debugLevel )
        debug() << "No transport between z= " << start.z() << " and " 
                << start.z() + vect.z() 
                << ", since it reaches outside LHCb" << endreq;
    } else {
      chronoSvc()->chronoStart("TransportSvcT");
      nWall = m_transportSvc->intersections( start, vect, 0., 1., 
                                             intersept, m_minRadThreshold );
      chronoSvc()->chronoStop("TransportSvcT");  
    }
  
    // local to global transformation of intersections
    transformToGlobal(zStep,start.z(),intersept);
 
    // add virtual wall at target
    ILVolume::Interval inter(start.z() + vect.z(), start.z() + vect.z());
    const Material* dummyMat = 0;
    intersept.push_back(std::make_pair(inter,dummyMat));
    nWall = intersept.size();      
 
    // loop over the walls - last wall is `virtual one at target z'
    for ( int iStep = 0; iStep < nWall; ++iStep ) {
      double zWall = zScatter( intersept[iStep].first.first,
                               intersept[iStep].first.second );
      ITrackExtrapolator* thisExtrapolator = m_extraSelector->select(state.z(),
                                                                     zWall);
      sc = thisExtrapolator->propagate( state, zWall );
     
      // check for success
      if ( sc.isFailure() ) {
        Warning( "Transport to wall using "+thisExtrapolator->name()+ "FAILED",
                 StatusCode::FAILURE, 1 );
      
        debug() << "Transport to " << zWall
                << "using "+thisExtrapolator->name() << " FAILED" << endreq;
      }
      
      //update f
      updateTransportMatrix( thisExtrapolator->transportMatrix() );  
      
      // protect against vertical or looping tracks
      if (fabs(state.tx()) > m_maxSlope) {
        Warning( "Protect against looping tracks. See debug for details",
                 StatusCode::FAILURE, 1 );
        debug() << "Protect against looping tracks: tx=" << state.tx() 
                << " (max " << m_maxSlope << " allowed)." << endreq;
        return StatusCode::FAILURE;
      }    
      if (fabs(state.ty()) > m_maxSlope) {
        Warning( "Protect against looping tracks. See debug for details",
                 StatusCode::FAILURE, 1 );
        debug() << "Protect against looping tracks: ty=" << state.ty() 
                << " (max " << m_maxSlope << " allowed). " << endreq;
        return StatusCode::FAILURE;
      }
      
      // The thickness of the wall
      double tWall = fabs( intersept[iStep].first.first - 
                           intersept[iStep].first.second );
      const Material* theMaterial = intersept[iStep].second;

      // multiple scattering
      if ( ( theMaterial != 0 ) && m_applyMultScattCorr ) {
        if ( tWall >= m_thickWall ) {
          m_thickmstool -> correctState( state, theMaterial, tWall, m_upStream );
        } else {
          m_thinmstool -> correctState( state, theMaterial, tWall, m_upStream );
        }
      }
      
      // dE/dx energy loss
      if ( ( theMaterial != 0 ) && m_applyEnergyLossCorr ) {
        m_dedxtool -> correctState( state, theMaterial, tWall, m_upStream );
      }
      
      // electron energy loss
      if ( ( 11 == partId.abspid() ) && 
           m_applyElectronEnergyLossCorr &&
           (theMaterial != 0) &&
           (state.z() > m_startElectronCorr) &&
           (state.z() < m_stopElectronCorr) ) {
        m_elecdedxtool -> correctState( state, theMaterial, 0., m_upStream );
      }
    } // loop over walls
  } // loop over steps

  if ( m_debugLevel ) debug() << "State extrapolated succesfully" << endreq;
  
  return sc;
}

//=============================================================================
//
//=============================================================================
double TrackMasterExtrapolator::zScatter( const double z1, 
                                          const double z2 ) const
{
  double zS;
  if ( fabs(z1-z2) >= m_thickWall ) {
    // thick scatter
    zS = (m_upStream ) ? GSL_MIN(z1,z2) : GSL_MAX(z1,z2) ; 
  } else {
    // thin scatter - at centre in z
    zS = 0.5*(z1+z2);
  }
  return zS;
}

//=============================================================================
