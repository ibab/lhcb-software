// $Id: TrackPtKick.cpp,v 1.4 2006-05-02 13:17:05 erodrigu Exp $
// Include files
// -------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h"

// from LHCbDefinitions
#include "Kernel/PhysicalConstants.h"
#include "Kernel/TrackTypes.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/State.h"

// local
#include "TrackPtKick.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackPtKick
//
// 2000-08-16 : M. Needham
// 2005-05-13 : J. Nardulli (adaptations to new track event model)
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackPtKick>          s_factory ;
const        IToolFactory& TrackPtKickFactory = s_factory ; 


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
  declareInterface<ITrackPtKick>(this);

  declareProperty( "MomentumError",       m_MomentumError = 0.01 );
  declareProperty( "ParabolicCorrection", m_ParabolicCorrection  );
  declareProperty( "ConstantCorrection",  m_Constant = 0.*MeV    );

  m_ParabolicCorrection.push_back( -0.0092 );
  m_ParabolicCorrection.push_back( 0.0 );
  m_ParabolicCorrection.push_back( -0.112 );

};

//=============================================================================
// Destructor
//=============================================================================
TrackPtKick::~TrackPtKick() {}; 

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
         << m_ParabolicCorrection[1] <<" tx + "
         << m_ParabolicCorrection[2] <<" tx^2 " <<endreq;

  determineFieldPolarity();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Estimate the momentum P of a State
//=============================================================================
StatusCode TrackPtKick::calculate( LHCb::State* state ) const
{
  // calculate intial estimate of track momentum assuming it came from
  // the primary vertex

  StatusCode sc = StatusCode::SUCCESS;

  // scan in cm steps  
  Gaudi::XYZPoint  begin( 0., 0., 0. );
  Gaudi::XYZPoint  end( state->x(), state->y(), state->z() );
  Gaudi::XYZVector bdl;
  double zCenter;

  m_bIntegrator -> calculateBdlAndCenter( begin, end, state->tx(), 
                                          state->ty(), zCenter, bdl );
  double q = 0.;
  double p = 1e6 * MeV;

  if ( fabs( bdl.x() ) > TrackParameters::hiTolerance ) {
    //can estimate momentum and charge

    //Rotate to the  0-0-z axis and do the ptkick 
    double tX = state -> tx();
    double xCenter = state -> x() + tX * ( zCenter - state->z() );

    double zeta_trk = -tX / sqrt( 1.0 + tX*tX );
    double tx_vtx   = xCenter / zCenter;
    double zeta_vtx = -tx_vtx/ sqrt( 1.0 + tx_vtx*tx_vtx );
  
    // curvature
    double curv = ( zeta_trk - zeta_vtx );

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
    p = eplus * c_light *fabs(bdl.x()) 
        * sqrt((1.0 +tX*tX+gsl_pow_2(state->ty()))
                                           /(1.0 +gsl_pow_2(tX)))/fabs(curv);

    //   Addition Correction factor for the angle of the track!
    if ( m_ParabolicCorrection.size() == 3 ) {
      double tx = (float) state -> tx();
      //p*=(1 - (a + b*tx + c*tx*tx ) );
      p+= m_Constant;
      p*= ( 1. - ( m_ParabolicCorrection[0]
                   + m_ParabolicCorrection[1] * fabs(tx)
                   + m_ParabolicCorrection[2] * tx * tx ) );
    }

  }  
  else {
    // can't estimate momentum or charge
    error() << "B integral is 0!" << endreq;
    sc = StatusCode::FAILURE;
  }

  // set the state parameters
  state -> setQOverP( q / p );
  state -> setErrQOverP2( gsl_pow_2(m_MomentumError/p) );
  Gaudi::TrackSymMatrix& cov = state -> covariance();
  double errQOverP = m_MomentumError / p;
   cov(4,4) = errQOverP * errQOverP;

  return sc;
}

//=============================================================================
// Determination of the field polarity
//=============================================================================
void TrackPtKick::determineFieldPolarity()
{
 // determine the field polarity by sending out a test particle
 Gaudi::XYZPoint  begin( 0., 0., 0. );
 Gaudi::XYZPoint  end( 0., 0., 100. );
 Gaudi::XYZVector bdl;
 double z;

 m_bIntegrator -> calculateBdlAndCenter( begin, end, 0., 0., z, bdl );
 
 if ( bdl.x() > 0.0 ) {
   m_FieldPolarity =  1;
 } 
 else {
   m_FieldPolarity = -1; 
 }

};

//=============================================================================
