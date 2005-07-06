// Inclde files

// Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/ToolFactory.h"

// CLHEP
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// TrackEvent
#include "Event/TrackParameters.h"

// GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_poly.h"

// Local 
#include "TrackParabolicExtrapolator.h"

// Needed for the creation of TrackParabolicExtrapolator objects.
static const ToolFactory<TrackParabolicExtrapolator> s_factory;
const IToolFactory& TrackParabolicExtrapolatorFactory = s_factory;

// TrackParabolicExtrapolator constructor.
TrackParabolicExtrapolator::TrackParabolicExtrapolator
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : TrackExtrapolator(type, name, parent) {}

// TrackParabolicExtrapolator destructor.
TrackParabolicExtrapolator::~TrackParabolicExtrapolator() {}

StatusCode TrackParabolicExtrapolator::initialize()
{

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    {
      return Error("Failed to initialize", sc);
    }
  
  m_pIMF = svc<IMagneticFieldSvc>("MagneticFieldSvc", true);
 
  return StatusCode::SUCCESS;
}

// Propagate a state to a given z-position
StatusCode TrackParabolicExtrapolator::propagate( State& state, 
						  double zNew, 
						  ParticleID )
{
  // Reset the transport matrix
  m_F = HepMatrix(5, 5, 1);
  
  // check current z-position
  double dz = zNew - state.z();
  if (fabs(dz) < TrackParameters::hiTolerance)
    { 
    // already at required z position
    debug() <<"already at required z position"<<endreq;
    return StatusCode::SUCCESS; 
  }

  //get the B field  
  HepPoint3D P = state.position();
  m_pIMF->fieldVector( P, m_B );

  // to save some typing...
  double Tx = state.tx();
  double Ty = state.ty();
  double nTx = sqrt(1.0+gsl_pow_2(Tx));
  double nTy = sqrt(1.0+gsl_pow_2(Ty));
  double norm = sqrt(1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty));
 
  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
   
  // set non-zero diagonal elements
  updateTransportMatrix(dz, state); 

  // Extrapolate state EXACT  
  HepVector& tState = state.stateVector();
  tState[0] += dz*(Tx + 0.5*m_ax*tState[4]*eplus*c_light*dz);
  tState[1] += dz*(Ty + 0.5*m_ay*tState[4]*eplus*c_light*dz);
  tState[2] += m_ax*tState[4]*eplus*c_light*dz;
  tState[3] += m_ay*tState[4]*eplus*c_light*dz;

  // Update covariance
  HepSymMatrix& tStateCov = state.covariance();
  tStateCov = tStateCov.similarity(m_F); // F*C*F.T()
  state.setZ(zNew);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a state to the intersection point with a given plane
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state, 
                                                  const HepPlane3D& plane, 
                                                  ParticleID pid )
{
  StatusCode sc;
  
  // Determine disitance in Z to the intersection point 
  double dZ = 0.;
  sc = predict(state, plane, dZ);
  
  if( dZ != 0. ) { propagate( state, state.z()+dZ ); }
  
  // Check for success
  HepPoint3D P= state.position();
  if( TrackParameters::hiTolerance < fabs( plane.distance(P) ) )
  {
    sc = StatusCode::FAILURE;
  }
  
  return sc;  
}

//=============================================================================
// Predicts the distance in Z from the State to the plane
//=============================================================================
StatusCode TrackParabolicExtrapolator::predict( State& state,
                                                const HepPlane3D& plane,
                                                double& dZ ) 
{
  HepPoint3D P = state.position();

  // get the B field  
  m_pIMF->fieldVector( P, m_B );

  // to save some typing...
  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx  = sqrt(1.0+gsl_pow_2(Tx));
  double nTy  = sqrt(1.0+gsl_pow_2(Ty));
  double norm = sqrt(1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty));
 
  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
   
  // get reference to the State vector
  HepVector& tState = state.stateVector();

  double varA = 0.5*m_ax*tState[4]*eplus*c_light;
  double varB = 0.5*m_ay*tState[4]*eplus*c_light;

  double planeA = plane.a();
  double planeB = plane.b();
  double planeC = plane.c();
  double planeD = plane.d();
  
  double parA = planeA*varA + planeB*varB;
  double parB = planeA*Tx + planeB*Ty + planeC;
  double parC = tState[0]*planeA + tState[1]*planeB + state.z()*planeC + planeD;
  
  // Solve intersection equation
  // solutions are the dz values, so zNew = zOld + dz, only real solutions
  double sol1 = 0.;
  double sol2 = 0.;
  int nrOfRoots =  gsl_poly_solve_quadratic( parA, parB, parC, &sol1, &sol2 );
  // if two solutions, then stored in ascending order
  
  StatusCode sc;
  if( 2 != nrOfRoots ) { sc = StatusCode::FAILURE; }
  else
  {
    sc = StatusCode::SUCCESS;    
    // Choose best solution for dz
    if( fabs( plane.distance(P) - sol1 ) < fabs( plane.distance(P) - sol2 ) )
    {
      dZ = sol1;
    }
    else { dZ = sol2; }
  }

  return sc;
}

//=============================================================================
// Propagate a state to the closest position to the specified point
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state,
                                                  const HepPoint3D& point,
                                                  ParticleID pid )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Check whether not already at reference point
  HepPoint3D P    = state.position();
  HepPoint3D diff = point - P;
  if(diff.mag() < TrackParameters::hiTolerance) {return StatusCode::SUCCESS;}
  
  // The distance between the reference point and a point on the parabola
  // can be minimized by taking the derivative wrt Z and equal that to zero.
  // This implies solving a cubic equation, resulting in 1 or 3 solutions.

  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx  = sqrt(1.0+gsl_pow_2(Tx));
  double nTy  = sqrt(1.0+gsl_pow_2(Ty));
  double norm = sqrt(1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
  double varA     = 0.5*m_ax*state.qOverP()*eplus*c_light;
  double varB     = 0.5*m_ay*state.qOverP()*eplus*c_light;
  double alpha    = 2*( varA*varA + varB*varB );
  double beta     = 3*( Tx*varA + Ty*varB );
  
  double gamma    = 2*P.x()*varA + Tx*Tx + 1 -2*point.x()*varA + 
    2*P.y()*varB + Ty*Ty - 2*point.y()*varB;
  
  double delta    = 
    P.x()*Tx - point.x()*Tx + P.y()*Ty - point.y()*Ty + P.z() - point.z();
  
  // The coefficients of the cubic equation
  double coefA = beta  / alpha;
  double coefB = gamma / alpha;
  double coefC = delta / alpha;

  // Create pointers in which to store the solutions (zNew = zOld+sol)
  double so1 = 999.;
  double so2 = 999.;
  double so3 = 999.;

  double* sol1 = &so1;
  double* sol2 = &so2;
  double* sol3 = &so3;

  gsl_poly_solve_cubic( coefA, coefB, coefC, sol1, sol2, sol3 );

  // Choose the solution closest to the present position
  if( ((*sol1) >= 0.) && ((*sol1) != 999.) )
  {
    sc = propagate(state, P.z()+(*sol1), pid);
  }
  else { sc = propagate(state, P.z()+*sol3, pid); }
  
  return sc;
}

// Update the transport matrix
void TrackParabolicExtrapolator::updateTransportMatrix( const double dz, 
							State& state )
{
  //create the transport matrix dX/dX_0 for ptState's
  //has to be separate to allow fast transport
  
  // to save some typing...
  double Tx = state.tx();
  double Ty = state.ty();
  double norm = sqrt(1.+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  
  //calculate derivatives of Ax, Ay
  double dAx_dTx = (Tx*m_ax/gsl_pow_2(norm)) + norm*(Ty*m_B.x()-(2.*Tx*m_B.y())); 
  double dAx_dTy = (Ty*m_ax/gsl_pow_2(norm)) + norm*(Tx*m_B.x()+m_B.z());
  double dAy_dTx = (Tx*m_ay/gsl_pow_2(norm)) + norm*(-Ty*m_B.y()-m_B.z());
  double dAy_dTy = (Ty*m_ay/gsl_pow_2(norm)) + norm*(-Tx*m_B.y()+(2.*Ty*m_B.x()));
  
  // fill transport matrix 
  m_F(1,3) = dz + 0.5*dAx_dTx*state.qOverP() * eplus*c_light*gsl_pow_2(dz);
  m_F(1,4) = 0.5*dAx_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(1,5) = 0.5*m_ax*eplus*c_light*gsl_pow_2(dz);
  
  m_F(2,3) = 0.5*dAy_dTx*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(2,4) = dz + 0.5*dAy_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(2,5) = 0.5*m_ay*eplus*c_light*gsl_pow_2(dz);
  
  m_F(3,3) = 1.0+ dAx_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(3,4) = dAx_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(3,5) = m_ax*eplus*c_light*dz;
  
  m_F(4,3) = dAy_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(4,4) = 1.0 + dAy_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(4,5) = m_ay*eplus*c_light*dz;
  
}

