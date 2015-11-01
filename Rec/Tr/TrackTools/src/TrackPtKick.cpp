// Include files
// -------------

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h" 

// from GSL
#include "gsl/gsl_math.h"

// Boost
#include <boost/assign/list_of.hpp>

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/State.h"
#include "Event/TrackTypes.h"

// local
#include "TrackPtKick.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackPtKick
//
// 2000-08-16 : M. Needham
// 2005-05-13 : J. Nardulli (adaptations to new track event model)
// 2006-07-24 : M Needham - tune for DC 06
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackPtKick )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackPtKick::TrackPtKick( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_bIntegrator(0)
  , m_FieldPolarity(1)

{
  declareInterface<ITrackMomentumEstimate>(this);

  std::vector<double> tmp1 = boost::assign::list_of(0.015)(0.29);
  declareProperty( "resParams", m_resParams = tmp1);
  
  std::vector<double> tmp2 = boost::assign::list_of(1.04)(0.14);
  declareProperty( "ParabolicCorrection", m_ParabolicCorrection = tmp2);

  declareProperty( "ConstantCorrection",  m_Constant = 0.*Gaudi::Units::MeV );
}

//=============================================================================
// Destructor
//============================================================================
TrackPtKick::~TrackPtKick() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackPtKick::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  m_bIntegrator = tool<IBIntegrator>( "BIntegrator" );
  
  info() << " Pt kick parameters(" << m_ParabolicCorrection.size()
         << ") ==" <<m_ParabolicCorrection[0] << " + " 
         << m_ParabolicCorrection[1] <<" tx^2 " <<endmsg;

  sc = determineFieldPolarity();
  if (sc.isFailure()){
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Estimate the momentum P of a State
//=============================================================================
StatusCode TrackPtKick::calculate( const LHCb::State* /* veloState */,
                                   const LHCb::State* tState, 
				   double& qOverP, double& sigmaQOverP, bool tCubicFit ) const
{
  return calculate (tState, qOverP, sigmaQOverP, tCubicFit);
}

//=============================================================================
// Estimate the momentum P of a State
//=============================================================================
StatusCode TrackPtKick::calculate( const LHCb::State* tState,
                                   double& qOverP, double& sigmaQOverP,
                                   bool /* tCubicFit */ ) const
{
  // calculate intial estimate of track momentum assuming it came from
  // the primary vertex

  // scan in cm steps  
  Gaudi::XYZPoint  begin( 0., 0., 0. );
  Gaudi::XYZPoint  end( tState->x(), tState->y(), tState->z() );
  Gaudi::XYZVector bdl;
  double zCenter;

  StatusCode sc = m_bIntegrator -> calculateBdlAndCenter(begin, end, tState->tx(), 
                                                         tState->ty(), zCenter, bdl );
  if (sc.isFailure()){
    return Warning("Failed to integrate field", StatusCode::FAILURE, 1);
  }

  double q = 0.;
  double p = 1e6 * Gaudi::Units::MeV;

  if ( fabs( bdl.x() ) > TrackParameters::hiTolerance ) {
    //can estimate momentum and charge

    //Rotate to the  0-0-z axis and do the ptkick 
    const double tX = tState -> tx();
    const double xCenter = tState -> x() + tX * ( zCenter - tState->z() );

    const double zeta_trk = -tX / sqrt( 1.0 + tX*tX );
    const double tx_vtx   = xCenter / zCenter;
    const double zeta_vtx = -tx_vtx/ sqrt( 1.0 + tx_vtx*tx_vtx );
  
    // curvature
    const double curv = ( zeta_trk - zeta_vtx );

    // charge
    int sign = 1;
    if( curv < TrackParameters::hiTolerance ) {
      sign *= -1;
    }
    if ( bdl.x() < TrackParameters::hiTolerance ) {
      sign *= -1;      
    }
    q = -1. * m_FieldPolarity*sign;

    // momentum
    p = Gaudi::Units::eplus * Gaudi::Units::c_light *fabs(bdl.x()) 
        * sqrt((1.0 +tX*tX+gsl_pow_2(tState->ty()))
                                           /(1.0 +gsl_pow_2(tX)))/fabs(curv);

    //   Addition Correction factor for the angle of the track!
    if ( m_ParabolicCorrection.size() == 2u ) {
      //p*= (a + b*tx*tx ) 
      p+= m_Constant;
      p*= ( m_ParabolicCorrection[0] + (m_ParabolicCorrection[1] * tX * tX ));
    }

  }  
  else {
    // can't estimate momentum or charge
    error() << "B integral is 0!" << endmsg;
    sc = StatusCode::FAILURE;
  }

  
  qOverP = q / p;  
  sigmaQOverP = std::hypot(m_resParams[0],m_resParams[1]/p)/p;
 
  return sc;
}

//=============================================================================
// Determination of the field polarity
//=============================================================================
StatusCode TrackPtKick::determineFieldPolarity()
{
 // determine the field polarity by sending out a test particle
 Gaudi::XYZPoint  begin( 0., 0., 0. );
 Gaudi::XYZPoint  end( 0., 0., 7000. );
 Gaudi::XYZVector bdl;
 double z;

 StatusCode sc = m_bIntegrator -> calculateBdlAndCenter( begin, end, 0., 0., z, bdl );
 if (sc.isFailure()){
   return Error("Failed to find field centre !", StatusCode::FAILURE);
 } 

 if ( bdl.x() > 0.0 ) {
   m_FieldPolarity =  1;
 } 
 else {
   m_FieldPolarity = -1; 
 }

 return StatusCode::SUCCESS;
}

//=============================================================================
