// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// Local
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // ITrackExtrapolator::propagate" only partially overridden
  #pragma warning(push)
#endif
#include "TrackHerabExtrapolator.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackHerabExtrapolator )

/** @file TrackHerabExtrapolator.cpp
 *
 *  A TrackHerabExtrapolator is a ITrackExtrapolator which does a
 *  'HerabRK5' extrapolation of a State.
 *  It doesn't take into account Multiple Scattering.
 *
 *  @author Matt Needham
 *  @author Updated 29-03-2002 Olivier Callot: Include Hera-B Runge-Kutta in.
 *  @date   22-04-2000
 */

/// TrackHerabExtrapolator constructor.
TrackHerabExtrapolator::TrackHerabExtrapolator(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent):
  TrackFieldExtrapolatorBase(type, name, parent)
{
  declareInterface<ITrackExtrapolator>( this );

  m_stepMin    = 200.* Gaudi::Units::mm;
  m_stepMinRK5 = 20. * Gaudi::Units::mm;
  m_qpCurls    = 0.02 ;

  declareProperty( "extrapolatorID"    , m_extrapolatorID = 5);
  declareProperty( "requiredPrecision" , m_error =  0.005*Gaudi::Units::mm );
}

StatusCode TrackHerabExtrapolator::initialize() 
{
  info() << "ExtrapolatorID=" << m_extrapolatorID << endmsg ;
  return TrackFieldExtrapolatorBase::initialize() ;
}

/// TrackHerabExtrapolator destructor.
TrackHerabExtrapolator::~TrackHerabExtrapolator()
{
}

// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
StatusCode TrackHerabExtrapolator::propagate( Gaudi::TrackVector& stateVec,
                                              double zOld,
                                              double zNew,
                                              Gaudi::TrackMatrix* transMat,
                                              LHCb::ParticleID /*pid*/ ) const
{
  // Bail out if already at destination
  const double dz = zNew - zOld;
  if( fabs(dz) < TrackParameters::propagationTolerance ) { 
    if( msgLevel( MSG::DEBUG ) ) debug() << "already at required z position" << endmsg;
    if( transMat ) *transMat = ROOT::Math::SMatrixIdentity();
    return StatusCode::SUCCESS ;
  }
  
  // prepare Q/p transport - note RK expects zStart as 1st argument
  double pIn[5];
  int i;
  for ( i = 0; i < 5; ++i ) { pIn[i] = stateVec[i]; }

  // return parameters
  double fQp[25];

  for ( i = 0; i < 25; ++i ) { fQp[i] = 0.; }
  double zIn     = zOld;
  double pOut[5] = {0.,0.,0.,0.,0.};
  int istat      = 0;

  // transport
  this->extrapolate( zIn,pIn,zNew,pOut,fQp,istat );

  // check for sucess
  if( istat != 0 ) {
    std::string warnMsg = "Runga kutta: transport impossible";
    if( 1 == istat ) warnMsg += ": curling track";
    if (transMat) *transMat = ROOT::Math::SMatrixIdentity();  
    return Warning( warnMsg, StatusCode::FAILURE, 1 );
  }

  // update the transport matrix
  if( transMat != NULL ) {
    (*transMat) = ROOT::Math::SMatrixIdentity();    
    for( i = 0; i < 5; ++i ) {
      for( int j = 0; j < 5; ++j ) {
        (*transMat)(i,j) = fQp[(5*j)+i];
      }
    }
  }
  
  // Update the state vector
  for( i = 0; i < 5; ++i ) { stateVec[i] = pOut[i]; }
  
  return StatusCode::SUCCESS;
}

//===========================================================================
// Interface to Hera-b code
//===========================================================================
void TrackHerabExtrapolator::extrapolate(double& zIn,double pIn[5], 
                                         double& zNew,
                                         double pOut[5], double fQp[25],
                                         int& istat) const
{
  switch ( m_extrapolatorID ) {

  case 1:
    rk4fast(zIn,pIn,zNew,pOut,fQp,istat);
    break;

  case 2:
    rk4order(zIn,pIn,zNew,pOut,fQp,istat);
    break;

  case 3:
    rk5fast(zIn,pIn,m_error,zNew,pOut,fQp,istat);
    break;

  case 4:
    rk5order(zIn,pIn,m_error,zNew,pOut,fQp,istat);
    break;

  case 5:
    rk5numde(zIn,pIn,m_error,zNew,pOut,fQp,istat);
    break;

  default:
    Warning( "Incorrect Extrapolator name - taking rk5order !!!!",
             StatusCode::SUCCESS, 1 ).ignore();
    rk5order(zIn,pIn,m_error,zNew,pOut,fQp,istat);
    break;
  }

}

//===========================================================================
// Runge-Kutta computation 5th order
//===========================================================================
//-- Author : A.Spiridonov  26.03.98

void TrackHerabExtrapolator::rk5order(
                    double& z_in , // z value for input parameters
                    double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                    double& error,  // acceptable errors in mm
                    double& z_out, // z value for output parameters
                    double* p_out, // output track parameters
                    double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
                    int&    ierror) const // = 0 ok, = 1 track curls
  //
  // Fifth-order Runge-Kutta method with adaptive stepsize control
  // for solution of the equation of motion of a particle with
  //       parameter qp = Q /P in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{
  static const double a[6]  = {0.0, 0.2, 0.3, 0.6, 1.0, 7.0/8.0};
  static const double c[6]  = {37.0/378.0, 0.0, 250.0/621.0, 125.0/594.0, 0.,
                         512.0/1771.0};
  static const double c1[6] = {2825.0/27648.0, 0., 18575.0/48384.0, 13525.0/55296.0,
                         277.0/14336.0,1.0/4.0};
  static const double b[16] = {0.0, 1.0/5.0, 3.0/40.0, 9.0/40.0, 3.0/10.0,
                         -9.0/10.0, 6.0/5.0,
                         -11.0/54.0, 5.0/2.0, -70.0/27.0, 35.0f/27.0,
                         1631.0/55296.0, 175.0/512.0, 575.0/13824.0,
                         44275.0/110592.0, 253.0/4096.0};

  double qp,hC,h;
  double k[24],x0[4],x[4],k1[24];
  double tx,ty,tx2,ty2,txty,tx2ty2,tx2ty2qp,tx2ty21,I_tx2ty21;
  double Ax[6],Ay[6],Ax_tx[6],Ay_tx[6],Ax_ty[6],Ay_ty[6];

  int step_j,step4;
  double p1_out[5],z2_out,p2_out[5],rkd1[25],rkd2[25];

  //----------------------------------------------------------------

  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  ierror = (fabs(qp) > m_qpCurls) ? 1 : 0;
  h = z_out - z_in;
  hC   = h * Gaudi::Units::c_light;

  x0[0] = p_in[0];
  x0[1] = p_in[1];
  x0[2] = p_in[2];
  x0[3] = p_in[3];

  //
  //   Runge-Kutta step
  //

  int i, j, step;

  for (step = 0; step < 6; ++step) {
    for(i=0; i < 4; ++i) {
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j) {
        x[i] += b[step_j + j] * k[j*4+i];
      }
    }

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    tx = x[2]; ty = x[3]; tx2 = tx * tx; ty2 = ty * ty; txty = tx * ty;
    tx2ty21= 1.0 + tx2 + ty2; I_tx2ty21 = 1.0 / tx2ty21 * qp;
    tx2ty2 = sqrt(tx2ty21 ) ; 
    // double I_tx2ty2 = qp * hC / tx2ty2 ; // unused?
    tx2ty2 *= hC; tx2ty2qp = tx2ty2 * qp;

    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + (ty*m_B.x()-2.*tx*m_B.y())*tx2ty2qp;
    Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + (tx*m_B.x()+m_B.z())*tx2ty2qp;
    Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*m_B.y()-m_B.z())*tx2ty2qp;
    Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*m_B.y()+2.*ty*m_B.x())*tx2ty2qp;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i)    {
    p_out[i]=x0[i]+c[0]*k[i]+c[2]*k[8+i]+c[3]*k[12+i]+c[5]*k[20+i];
  }
  p_out[4]=p_in[4];

  //      printf("p_out %8f3 %8f3 %8f3 %8f3  \n"
  //      ,p_out[0],p_out[1],p_out[2],p_out[3]);

  //
  //     The embedded fourth-order formula for x and y
  //
  for (i = 0; i < 2; ++i)    {
    p1_out[i]=x0[i]+c1[0]*k[i]+c1[2]*k[8+i]+c1[3]*k[12+i]+c1[4]*k[16+i]
      +c1[5]*k[20+i];
  }

  //
  //     Derivatives    dx/dqp
  //

  x0[0] = 0.; x0[1] = 0.; x0[2] = 0.; x0[3] = 0.;

  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 6; ++step)    {
    for(i=0; i < 4; ++i) {
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j) {
        x[i] += b[step_j + j] * k1[j*4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  for (i = 0; i < 4; ++i ) {
    rkd[20+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
  }
  rkd[24] = 1.;

  //      end of derivatives dx/dqp
  //
  //     Derivatives    dx/tx
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 1.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dtx
  //

  for (step = 0; step < 6; ++step) {
    for(i=0; i < 4; ++i) {
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j) {
        if(i != 2) {x[i] += b[step_j + j] * k1[j*4+i];} // tx fixed
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    //    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dtx

  for (i = 0; i < 4; ++i ) {
    if(i != 2) {
      rkd[10+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
    }
  }
  //      end of derivatives dx/dtx
  rkd[12] = 1.;
  rkd[14] = 0.;

  //     Derivatives    dx/ty
  //

  x0[0] = 0; x0[1] = 0.; x0[2] = 0.; x0[3] = 1.;

  //
  //   Runge-Kutta step for derivatives dx/dty
  //

  for (step = 0; step < 6; ++step) {
    for(i=0; i < 4; ++i) {
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j) {
        if( i != 3){ x[i] += b[step_j + j] * k1[j*4+i];} //    ty fixed
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    //    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dty

  for (i = 0; i < 3; ++i ) {
    rkd[15+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
  }
  //      end of derivatives dx/dty
  rkd[18] = 1.;
  rkd[19] = 0.;

  //
  //    derivatives dx/dx and dx/dy

  for(i = 0; i < 10; ++i){ rkd[i] = 0.;}
  rkd[0] = 1.; rkd[6] = 1.;

  //
  //      stepsize control
  //

  if( (fabs(p1_out[0]-p_out[0])  > error ) ||
      (fabs(p1_out[1]-p_out[1])  > error )     ) {
    if(fabs(h) > m_stepMinRK5 ) {
      z2_out = z_in + 0.5 * h;
      rk5order(z_in  , p_in,   error, z2_out, p2_out, rkd1, ierror);
      rk5order(z2_out, p2_out, error, z_out,  p_out , rkd2, ierror);

      rkd[0]  = 1.; rkd[1] = 0.; rkd[2] = 0.; rkd[3] = 0.; rkd[4] = 0;
      rkd[5]  = 0.; rkd[6] = 1.; rkd[7] = 0.; rkd[8] = 0.; rkd[4] = 0;
      rkd[10] = rkd1[10] + rkd2[10]            + rkd2[15] * rkd1[13];
      rkd[11] = rkd1[11] + rkd2[11]            + rkd2[16] * rkd1[13];
      rkd[12] = 1.;
      rkd[13] =            rkd2[13]            +            rkd1[13];
      rkd[14] = 0.;
      rkd[15] = rkd1[15] + rkd2[10] * rkd1[17] + rkd2[15];
      rkd[16] = rkd1[16] + rkd2[11] * rkd1[17] + rkd2[16];
      rkd[17] =                       rkd1[17] + rkd2[17];
      rkd[18] = 1.;
      rkd[19] = 0.;
      rkd[20] = rkd1[20] + rkd2[10] * rkd1[22] + rkd2[15] * rkd1[23]
        +rkd2[20];
      rkd[21] = rkd1[21] + rkd2[11] * rkd1[22] + rkd2[16] * rkd1[23]
        +rkd2[21];
      rkd[22] =                       rkd1[22] + rkd2[17] * rkd1[23]
        +rkd2[22];
      rkd[23] =            rkd2[13] * rkd1[22] +            rkd1[23]
        +rkd2[23];
      rkd[24] = 1.;

    }

  }       // end of stepsize control

}    // end of rk5order

//===========================================================================
//-- Author : A.Spiridonov  26.03.98
//===========================================================================
void TrackHerabExtrapolator::rk5fast(

                      double& z_in , // z value for input parameters
                      double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                      double& error,  // acceptable errors in mm
                      double& z_out, // z value for output parameters
                      double* p_out, // output track parameters
                      double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
                      int&    ierror) const // = 0 ok, = 1 track curls
  //
  // Fifth-order Runge-Kutta method with adaptive stepsize control
  // for solution of the equation of motion of a particle with
  //       parameter qp = Q /P in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{

  static const double a[6]  = {0.0, 0.2, 0.3, 0.6, 1.0, 7.0/8.0};
  static const double c[6]  = {37.0/378.0, 0.0, 250.0/621.0, 125.0/594.0, 0.0,
                         512.0/1771.0};
  static const double c1[6] = {2825.0/27648.0, 0.0, 18575.0/48384.0, 13525.0/55296.0,
                         277.0/14336.,1.0/4.0};
  static const double b[16] = {0.0, 1.0/5.0, 3.0/40.0, 9.0/40.0, 3.0/10.0, -9.0/10.0,
                         6.0/5.0,
                         -11.0/54.0, 5.0/2.0, -70.0/27.0, 35.0/27.0,
                         1631.0/55296.0, 175.0/512.0, 575.0/13824.0,
                         44275.0/110592.0, 253.0/4096.0};

  int step_j,step4;
  double qp,hC,h;
  double k[24],x0[4],x[4],k1[24];
  double tx,ty,tx2,ty2,txty,tx2ty2;
  double Ax[6],Ay[6];

  double p1_out[5],z2_out,p2_out[5];

  //----------------------------------------------------------------
  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  ierror = (fabs(qp) > m_qpCurls) ? 1 : 0;
  h = z_out - z_in;
  hC   = h * Gaudi::Units::c_light;

  x0[0] = p_in[0];
  x0[1] = p_in[1];
  x0[2] = p_in[2];
  x0[3] = p_in[3];

  //
  //   Runge-Kutta step
  //

  int i, j, step;

  for (step = 0; step < 6; ++step){
    for(i=0; i < 4; ++i){
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j){
        x[i] += b[step_j + j] * k[j*4+i];
      } // j
    }  //i

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    tx = x[2]; ty = x[3]; tx2 = tx * tx; ty2= ty * ty; txty = tx * ty;
    tx2ty2 =  sqrt( 1.0 + tx2 + ty2 ) *  hC;

    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i){
    p_out[i]=x0[i]+c[0]*k[i]+c[2]*k[8+i]+c[3]*k[12+i]+c[5]*k[20+i];
  }
  p_out[4]=p_in[4];

  //
  //     The embedded fourth-order formula for x
  //
  p1_out[0]=x0[0]+c1[0]*k[0]+c1[2]*k[8]+c1[3]*k[12]+c1[4]*k[16]+c1[5]*k[20];

  //     Derivatives    dx/dqp
  //
  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 6; ++step) {
    x[0] = 0.0; step_j = ((step-1)*step)/2 + 1;
    for(j=0; j < step; ++j) {
      x[0] += b[step_j + j] * k1[j*4  ];
    }
    x[2] = 0.0; step_j = ((step-1)*step)/2 + 1;
    for(j=0; j < step; ++j) {
      x[2] += b[step_j + j] * k1[j*4+2];
    }

    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+2] = Ax[step];

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  rkd[20]=c[0]*k1[0]+c[2]*k1[8 ]+c[3]*k1[12]+c[5]*k1[20];
  rkd[21]=0.;
  rkd[22]=c[0]*k1[2]+c[2]*k1[10]+c[3]*k1[14]+c[5]*k1[22];
  rkd[23]=0.;
  rkd[24] = 1.;

  //      end of derivatives dx/dqp
  //
  //
  //      other derivatives

  for(i = 0; i <= 19; ++i){
    rkd[i] = 0.;
  }

  rkd[0]  = 1.;
  rkd[6]  = 1.;
  rkd[10] = h;
  rkd[12] = 1.;
  rkd[16] = h;
  rkd[18] = 1.;
  //
  //      stepsize control
  //
  if ( fabs(p1_out[0]-p_out[0])  > error ){

    if(fabs(h) > m_stepMin *3.){
      z2_out = z_in + 0.5 * h;
      rk5fast(z_in  , p_in,   error, z2_out, p2_out);
      rk5fast(z2_out, p2_out, error, z_out,  p_out );
    }
    else if(fabs(h) > m_stepMin){
      z2_out = z_in + 0.5 * h;
      rk4fast(z_in  , p_in,   z2_out, p2_out);
      rk4fast(z2_out, p2_out, z_out,  p_out );
    }
  } // end of stepsize control

}    // end of rk5fast

//===========================================================================
//-- Author : A.Spiridonov  26.03.98
//===========================================================================
void TrackHerabExtrapolator::rk5fast(                 // Without derivatives
                      double& z_in , // z value for input parameters
                      double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                      double& error,  // acceptable errors in mm
                      double& z_out, // z value for output parameters
                      double* p_out ) const // output track parameters
  //
  // Fifth-order Runge-Kutta method with adaptive stepsize control
  // for solution of the equation of motion of a particle with
  //       parameter qp = Q /P in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{

  static const double a[6]  = {0.0, 0.2, 0.3, 0.6, 1.0, 7.0/8.0};
  static const double c[6]  = {37.0/378.0, 0.0, 250.0/621.0, 125.0/594.0, 0.0,
                         512.0/1771.0};
  static const double c1[6] = {2825.0/27648.0, 0.0, 18575.0/48384.0, 13525.0/55296.0,
                         277.0/14336.0,1.0/4.0};
  static const double b[16] = {0.0, 1.0/5.0, 3.0/40.0, 9.0/40.0, 3.0/10.0, -9.0/10.0,
                         6.0/5.0,
                         -11.0/54.0, 5.0/2.0, -70.0/27.0, 35.0/27.0,
                         1631.0/55296., 175.0/512.0, 575.0/13824.0,
                         44275.0/110592.0, 253.0/4096.0};

  int step_j,step4;
  double qp,hC,h;
  double k[24],x0[4],x[4];
  double tx,ty,tx2,ty2,txty,tx2ty2;
  double Ax[6],Ay[6];

  double p1_out[5],z2_out,p2_out[5];

  //----------------------------------------------------------------

  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  h = z_out - z_in;
  hC   =  h * Gaudi::Units::c_light;

  x0[0] = p_in[0];
  x0[1] = p_in[1];
  x0[2] = p_in[2];
  x0[3] = p_in[3];

  //
  //   Runge-Kutta step
  //

  int i,j,step;

  for (step = 0; step < 6; ++step){
    for(i=0; i < 4; ++i){
      x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
      for(j=0; j < step; ++j){
        x[i] += b[step_j + j] * k[j*4+i];
      } // i
    }  // j

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    tx = x[2];
    ty = x[3];
    tx2 = tx * tx;
    ty2 = ty * ty;
    txty = tx * ty;
    tx2ty2 =  sqrt( 1.0 + tx2 + ty2 ) *  hC;
    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i){
    p_out[i]=x0[i]+c[0]*k[i]+c[2]*k[8+i]+c[3]*k[12+i]+c[5]*k[20+i];
  }

  p_out[4]=p_in[4];

  //
  //     The embedded fourth-order formula for x
  //
  p1_out[0]=x0[0]+c1[0]*k[0]+c1[2]*k[8]+c1[3]*k[12]+c1[4]*k[16]+c1[5]*k[20];

  //
  //      stepsize control
  //
  if ( fabs(p1_out[0]-p_out[0])  > error ){

    if(fabs(h) > m_stepMin *3.) {
      z2_out = z_in + 0.5 * h;
      rk5fast(z_in  , p_in,   error, z2_out, p2_out);
      rk5fast(z2_out, p2_out, error, z_out,  p_out );
    }
    else if(fabs(h) > m_stepMin ){
      z2_out = z_in + 0.5 * h;
      rk4fast(z_in  , p_in,   z2_out, p2_out);
      rk4fast(z2_out, p2_out, z_out,  p_out );
    }
  }       // end of stepsize control

}         // end of rk5fast without derivatives


//===========================================================================
//-- Author : A.Spiridonov  09.04.98
//===========================================================================
void TrackHerabExtrapolator::rk5numde(   // Numerical Derivatives ( quite slow )
                       double& z_in , // z value for input parameters
                       double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                       double& error,  // acceptable errors in mm
                       double& z_out, // z value for output parameters
                       double* p_out, // output track parameters
                       double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
                       int&    ierror) const // = 0 ok, = 1 track curls
  //
  // Fifth-order Runge-Kutta method with adaptive stepsize control
  // for solution of the equation of motion of a particle with
  //       parameter qp = Q /P in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*B(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+B(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  Derivatives are calculated by "numerical differentiation"
  //         of p_out as function of p_in
  //

{
  double qp;
  double p1_out[5],p1_in[5],d_p[5];

  //----------------------------------------------------------------
  int i,j;

  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  ierror = (fabs(qp) > m_qpCurls) ? 1 : 0;

  rk5fast(z_in  , p_in,   error, z_out, p_out);
  
  d_p[0] = d_p[1] = 100 * error ;
  d_p[2] = d_p[3] = std::min(100 * error / std::abs(z_out - z_in ), 0.01 ) ;
  d_p[4]          = 1e-7 ;

  for(j=0; j < 5 ; ++j){

    for( i=0; i < 5; ++i){
      p1_in[i] = p_in[i];
    } // i

    p1_in[j] += d_p[j];
    rk5fast(z_in  , p1_in,   error, z_out, p1_out);

    for( i=0; i < 5; ++i){
      rkd[5*j+i] = (p1_out[i]-p_out[i])/d_p[j];
    }  // i
  } // j

}    // end of rk5nuder


//===========================================================================
//-- Author : A.Spiridonov  23.03.98
//===========================================================================
void TrackHerabExtrapolator::rk4order(
                       double& z_in , // z value for input parameters
                       double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                       double& z_out, // z value for output parameters
                       double* p_out, // output track parameters
                       double* rkd,  // derivatives d p_out(1-5) / d p_in(1-5)
                       int&    ierror) const // = 0 ok, = 1 track curls
  //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{

  static const double a[4] = {0.0, 0.5, 0.5, 1.0};
  static const double c[4] = {1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0};
  static const double b[4] = {0.0, 0.5, 0.5, 1.0};

  int step4;
  double k[16],x0[4],x[4],k1[16];
  double Ax[4],Ay[4],Ax_tx[4],Ay_tx[4],Ax_ty[4],Ay_ty[4];

  //----------------------------------------------------------------
  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  double qp    = p_in[4];
  ierror = (fabs(qp) > m_qpCurls) ? 1 : 0;
  double h = z_out - z_in;
  double hC   = h * Gaudi::Units::c_light;
  x0[0] = p_in[0]; x0[1] = p_in[1];
  x0[2] = p_in[2]; x0[3] = p_in[3];
  //
  //   Runge-Kutta step
  //

  int step;
  int i;
  
  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
      }
    }

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    double tx = x[2]; 
    double ty = x[3]; 
    double tx2 = tx * tx; 
    double ty2 = ty * ty; 
    double txty = tx * ty;
    double tx2ty21= 1.0 + tx2 + ty2; 
    double I_tx2ty21 = 1.0 / tx2ty21 * qp;
    double tx2ty2 = sqrt(tx2ty21 ) ; 
    //   double I_tx2ty2 = qp * hC / tx2ty2 ; unsused ???
    tx2ty2 *= hC; 
    double tx2ty2qp = tx2ty2 * qp;
    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + (ty*m_B.x()-2.0*tx*m_B.y())*tx2ty2qp;
    Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + (tx*m_B.x()+m_B.z())*tx2ty2qp;
    Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*m_B.y()-m_B.z())*tx2ty2qp;
    Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*m_B.y()+2.0*ty*m_B.x())*tx2ty2qp;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i) {
    p_out[i]=x0[i]+c[0]*k[i]+c[1]*k[4+i]+c[2]*k[8+i]+c[3]*k[12+i];
  }
  p_out[4]=p_in[4];
  //
  //     Derivatives    dx/dqp
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  for (i = 0; i < 4; ++i ) {
    rkd[20+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  rkd[24] = 1.;
  //
  //      end of derivatives dx/dqp
  //

  //     Derivatives    dx/tx
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 1.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dtx
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else if ( i!=2 ){
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    //    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dtx

  for (i = 0; i < 4; ++i ) {
    if(i != 2) {
      rkd[10+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
    }
  }
  //      end of derivatives dx/dtx
  rkd[12] = 1.0;
  rkd[14] = 0.0;

  //     Derivatives    dx/ty
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 1.0;

  //
  //   Runge-Kutta step for derivatives dx/dty
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];           // ty fixed
      } else if(i!=3) {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }

    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    //    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dty

  for (i = 0; i < 3; ++i ) {
    rkd[15+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  //      end of derivatives dx/dty
  rkd[18] = 1.;
  rkd[19] = 0.;

  //
  //    derivatives dx/dx and dx/dy

  for(i = 0; i < 10; ++i){ rkd[i] = 0.;}
  rkd[0] = 1.; rkd[6] = 1.;

}    // end of rk4order


//===========================================================================
//-- Author : A.Spiridonov  27.03.98
//===========================================================================
void TrackHerabExtrapolator::rk4fast(
                      double& z_in , // z value for input parameters
                      double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                      double& z_out, // z value for output parameters
                      double* p_out, // output track parameters
                      double* rkd,  // derivatives d p_out[0-4] / d p_in[0-4]
                      int&    ierror) const // = 0 ok, = 1 track curls
  //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{
  static const double a[4] = {0.0, 0.5, 0.5, 1.0};
  static const double c[4] = {1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0};
  static const double b[4] = {0.0, 0.5, 0.5, 1.0};
  int step4;
  double qp,hC,h;
  double k[16],x0[4],x[4],k1[16];
  double tx,ty,tx2,ty2,txty,tx2ty2;
  double Ax[4],Ay[4];
  //----------------------------------------------------------------
  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  ierror = (fabs(qp) > m_qpCurls) ? 1 : 0;
  h = z_out - z_in; 
  hC   = h * Gaudi::Units::c_light;
  x0[0] = p_in[0]; x0[1] = p_in[1];
  x0[2] = p_in[2]; x0[3] = p_in[3];
  //
  //   Runge-Kutta step
  //

  int step, i;

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
      }
    }

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    tx = x[2]; ty = x[3]; tx2 = tx * tx; ty2 = ty * ty; txty = tx * ty;
    tx2ty2 =  sqrt( 1.0 + tx2 + ty2 ) *  hC;
    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i) {
    p_out[i]=x0[i]+c[0]*k[i]+c[1]*k[4+i]+c[2]*k[8+i]+c[3]*k[12+i];
  }
  p_out[4]=p_in[4];

  //
  //     Derivatives    dx/dqp and dtx/dqp
  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[0] = 0.0; x[2] = 0.0;
      } else {
        x[0] = b[step] * k1[step*4-4];
        x[2] = b[step] * k1[step*4-2];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+2] = Ax[step] ;

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  rkd[20] = c[0]*k1[0]+c[1]*k1[4]+c[2]*k1[8]+c[3]*k1[12];
  rkd[21] = 0.;
  rkd[22] = c[0]*k1[2]+c[1]*k1[6]+c[2]*k1[10]+c[3]*k1[14];
  rkd[23] = 0.;
  rkd[24] = 1.;

  //      end of derivatives dx/dqp
  //
  //      other derivatives

  for(i = 0; i <= 19; ++i){ rkd[i] = 0.;}
  rkd[0]  = 1.; rkd[6]  = 1.; rkd[10] = h;
  rkd[12] = 1.; rkd[16] = h;  rkd[18] = 1.;

}    // end of rk4fast


//===========================================================================
//-- Author : A.Spiridonov  27.03.98
//===========================================================================
void TrackHerabExtrapolator::rk4fast(                // Without derivatives
                      double& z_in , // z value for input parameters
                      double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
                      double& z_out, // z value for output parameters
                      double* p_out) const // output track parameters
  //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //

{
  static const double a[4] = {0.0, 0.5, 0.5, 1.0};
  static const double c[4] = {1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0};
  static const double b[4] = {0.0, 0.5, 0.5, 1.};
  int step4;
  double qp,hC,h;
  double k[16],x0[4],x[4];
  double tx,ty,tx2,ty2,txty,tx2ty2;
  double Ax[4],Ay[4];
  //----------------------------------------------------------------

  if (msgLevel(MSG::DEBUG)){
    debug() << format( "rk5order  zIn %8.1f zOut %8.1f X %7.1f y %7.1f ", 
                     z_in, z_out, p_in[0], p_in[1]) << endmsg;
  }

  qp    = p_in[4];
  h = z_out - z_in;
  hC   = h * Gaudi::Units::c_light;

  x0[0] = p_in[0];  x0[1] = p_in[1];
  x0[2] = p_in[2];  x0[3] = p_in[3];
  //
  //   Runge-Kutta step
  //

  int i, step;

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
      }
    }

    m_point.SetXYZ( x[0], x[1], z_in  + a[step] * h );
    m_B = fieldVector( m_point ) ;

    tx = x[2]; ty = x[3]; tx2 = tx * tx; ty2 = ty * ty; txty = tx * ty;
    tx2ty2 =  sqrt( 1.0 + tx2 + ty2 ) *  hC;
    Ax[step] = ( txty*m_B.x() + ty*m_B.z() - ( 1.0 + tx2 )*m_B.y() ) * tx2ty2;
    Ay[step] = (-txty*m_B.y() - tx*m_B.z() + ( 1.0 + ty2 )*m_B.x() ) * tx2ty2;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp;
    k[step4+3] = Ay[step] * qp;

  }  // end of Runge-Kutta steps
  for(i=0; i < 4; ++i) {
    p_out[i]=x0[i]+c[0]*k[i]+c[1]*k[4+i]+c[2]*k[8+i]+c[3]*k[12+i];
  }
  p_out[4]=p_in[4];

}    // end of rk4fast without derivatives

//===========================================================================
