// $Id: TrackLinearExtrapolator.cpp,v 1.1 2005-03-10 14:41:03 hernando Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrEvent
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/TrGeneral.h"

// local
#include "TrackLinearExtrapolator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackLinearExtrapolator
//
// 2004-12-17 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackLinearExtrapolator>          s_factory ;
const        IToolFactory& TrackLinearExtrapolatorFactory = s_factory ;

//=============================================================================
// Extrapolate the State to z=zNew using the transport matrix  m_F
// (i.e. it performs the mathematical calculation)
//=============================================================================
/*
void TrackLinearExtrapolator::extrapolate( State* state ) const
{
  // get reference to the State vector and covariance
  HepVector& tX = state -> state();
  HepSymMatrix& tC = state -> covariance();

  // calculate new state
  tX = m_F * tX;           // X * F
  tC = tC.similarity(m_F); // F * C *F.T()
}
*/

//=============================================================================
// Propagate a State to a given z-position
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                            double zNew,
                                            ParticleID pid )
{
  // create transport matrix
  unsigned int ndim = state.nParameters();
  
  m_F = HepMatrix(ndim, ndim, 1);

  // check current z-position
  double dz = zNew - state.z();
  if ( fabs(dz) > TrGeneral::hiTolerance ) {
    m_F[0][2] = dz; // tx*dz
    m_F[1][3] = dz; // ty*dz
    // extrapolate
    //extrapolate(state);
    //state -> setZ( zNew );
    updateState( state, zNew );
  }

  debug() << " z propagation " << zNew 
          << " of particle pid " << pid.pid() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a State to the intersection point with a given plane
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                            HepPlane3D& plane,
                                            ParticleID pid )
{
  // calculation of the z-position by linear extrapolation to the plane
  // ------------------------------------------------------------------
  // given the plane defined by (a,b,c,d) and the input state of
  // position (x0,y0,z0) and slopes (Tx,Ty),
  // the (x,y,z) of the intersection point verify the set of equations
  //    a*x + b*y + c*z + d = 0
  //    x = x0 + (z-z0)*Tx
  //    x = y0 + (z-z0)*Ty
  HepNormal3D nVec   = plane.normal();
  HepPoint3D  posVec = state.position();
  HepVector3D slpVec = state.slopes();

  double den = nVec.dot( slpVec );

  if ( den < TrGeneral::looseTolerance ) return StatusCode::FAILURE;

  slpVec *= ( state.z() );
  posVec -= slpVec;
  
  double nom = - ( nVec.dot( posVec ) + plane.d() );
  
  double zNew = nom / den;

  debug() << " z propagation " << zNew 
          << " of particle pid " << pid.pid() << endreq;

  return propagate( state, zNew, pid );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackLinearExtrapolator::TrackLinearExtrapolator(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : TrackExtrapolator ( type, name, parent )
  , m_F()
{
  //declareInterface<ITrackExtrapolator>( this );

  // create transport matrix
  m_F = HepMatrix(5, 5, 1);
}

//=============================================================================
// Destructor
//=============================================================================
TrackLinearExtrapolator::~TrackLinearExtrapolator() {};
