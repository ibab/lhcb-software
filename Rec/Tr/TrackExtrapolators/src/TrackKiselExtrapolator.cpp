// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// Local
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // ITrackExtrapolator::propagate" only partially overridden
  #pragma warning(push)
#endif
#include "TrackKiselExtrapolator.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackKiselExtrapolator )

/** @file
 *
 *  @author M Needham
 *  @date   02-06-2006
 */
/// TrackKiselExtrapolator constructor.
TrackKiselExtrapolator::TrackKiselExtrapolator(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent):
  TrackFieldExtrapolatorBase(type, name, parent),
  m_B(3)
{
  declareProperty("order", m_order = 3);
  declareInterface<ITrackExtrapolator>( this );
}



/// TrackKiselExtrapolator destructor.
TrackKiselExtrapolator::~TrackKiselExtrapolator()
{
}

// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
StatusCode TrackKiselExtrapolator::propagate( Gaudi::TrackVector& stateVec,
                                              double zOld,
                                              double zNew,
                                              Gaudi::TrackMatrix* transMat,
                                              LHCb::ParticleID /*pid*/) const
{
  // Bail out if already at destination
  const double dz = zNew - zOld;
  if( std::abs(dz) < TrackParameters::propagationTolerance ) { 
    if( msgLevel( MSG::DEBUG ) ) debug() << "already at required z position" << endmsg;
    if( transMat ) *transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    return StatusCode::SUCCESS ;
  }

  // prepare Q/p transport - note Kisel expects zStart as 1st argument
  double pIn[5];
  int i;
  for ( i = 0; i < 5; ++i ) { pIn[i] = stateVec[i]; }

  // return parameters
  double fQp[25];

  for( i = 0; i < 25; ++i ) { fQp[i] = 0.; }

  double pOut[5] = {0.,0.,0.,0.,0.};

  extrapolateAnalytic( pIn,pOut,fQp,zOld,zNew );

  // update the transport matrix
  if( transMat != NULL ) {
    (*transMat) = TrackMatrix( ROOT::Math::SMatrixIdentity() );    
    for ( i = 0; i < 5; ++i ) {
      for ( int j = 0; j < 5; ++j ) {
        (*transMat)(i,j) = fQp[(5*j)+i];
      }
    }
  }

  for( i = 0; i < 5; ++i ) { stateVec[i] = pOut[i]; }
  
  return StatusCode::SUCCESS;
}

void TrackKiselExtrapolator::getCoefficients( const double x, const double y,
					      double Xi  [3][3]      , double Yi  [3][3],
					      double Xii [3][3][3]   , double Yii [3][3][3], 
					      double Xiii[3][3][3][3], double Yiii[3][3][3][3]) const
{
  const double xx = x*x;
  const double xy = x*y;
  const double yy = y*y;

  const double x2 = x*2;
  const double x4 = x*4;
  const double xx31 = xx*3+1;
  const double xx33 = xx*3+3;
  const double xx82 = xx*8+2;
  const double xx86 = xx*8+6;
  const double xx153 = xx*15+3;
  const double xx159 = xx*15+9;

  const double y2 = y*2;
  const double y4 = y*4;
  const double yy31 = yy*3+1;
  const double yy33 = yy*3+3;
  const double yy82 = yy*8+2;
  const double yy86 = yy*8+6;
  const double yy153 = yy*15+3;
  const double yy159 = yy*15+9;
  const double xxyy2 = 2*(xx-yy);

  const double xx1053 = y*(30*xx+xx159);
  const double yy1053 = x*(30*yy+yy159);


  double  X1[3]      = { xy, -xx-1, y };
  
  double X2[3][3]    =   { {          x*yy31,             -y*xx31,   2*yy+1 },
			   {         -y*xx31,              x*xx33,    -2*xy },
			   {           yy-xx,               -2*xy,       -x } };
  
  double X3[3][3][3] = { { {        xy*yy159,      -xx*yy153-yy31,   y*yy86 },
			   {  -xx*yy153-yy31,            xy*xx159,  -x*yy82 },
			   {   y2*(-xxyy2+1),             -x*yy82,    -3*xy } },
			 { {  -xx*yy153-yy31,            xy*xx159,  -x*yy82 },
			   {        xy*xx159, -3*(5*xx*xx+6*xx+1),   y*xx82 },
			   {    x2*(xxyy2+1),              y*xx82,     xx31 } },
			 { {  y*(-6*xx+yy31),       x*(xx31-6*yy),   -4*xy  },
			   { x*(3*xx-6*yy+1),            3*y*xx31,   xxyy2  },
			   {           -4*xy,               xxyy2,      -y  } 
			 }};
  double X1x[3]       =     { y, -x2, 0 };

  double X2x[3][3]    =   { {  yy31,  -6*xy,   0 },
			    { -6*xy, 3*xx31, -y2 },
			    {   -x2,    -y2,  -1 } };

  double X3x[3][3][3] = { { {         y*yy159,   -x2*yy153,     0 },
			    {       -x2*yy153,      xx1053, -yy82 },
			    {           -8*xy,       -yy82,  -3*y } },
			  { {       -x2*yy153,      xx1053, -yy82 },
			    {          xx1053,   -x4*xx159, 16*xy },
			    { 2*(6*xx-2*yy+1),       16*xy,   6*x } },
			  { {          -12*xy, 9*xx-6*yy+1,   -y4 },
			    {     9*xx-6*yy+1,       18*xy,    x4 },
			    {             -y4,          x4,     0 } 
			  }};
  
  double X1y[3]       =     { x, 0, 1 };
  
  double X2y[3][3]    =   { {  6*xy, -xx31,  y4 },
			    { -xx31,     0, -x2 },
			    {    y2,   -x2,   0 } };

  double X3y[3][3][3] = { { {       yy1053,  -y2*xx153, 16*yy+yy86 },
			    {    -y2*xx153,    x*xx159,     -16*xy },
			    { yy82-2*xxyy2,     -16*xy,       -3*x } },
			  { {    -y2*xx153,    x*xx159,     -16*xy },
			    {      x*xx159,          0,       xx82 },
			    {        -8*xy,       xx82,          0 } },
			  { { -6*xx+9*yy+1,     -12*xy,        -x4 },
			    {       -12*xy,     3*xx31,        -y4 },
			    {          -x4,        -y4,         -1 } 
			  }};


  double Y1[3]       =     { yy+1, -xy, -x };

  double Y2[3][3]    =   { {  y*yy33, -x*yy31,  -2*xy },
			   { -x*yy31,  y*xx31, 2*xx+1 },
			   {   -2*xy,   xx-yy,     -y } };

  double Y3[3][3][3] = { { { 3*(5*yy*yy+6*yy+1),      -xy*yy159, -x*yy82 },
			   {          -xy*yy159,  xx*yy153+yy31,  y*xx82 },
			   {            -x*yy82, -y2*(-xxyy2+1),   -yy31 } },
			 { {          -xy*yy159,  xx*yy153+yy31,  y*xx82 },
			   {      xx*yy153+yy31,      -xy*xx159, -x*xx86 },
			   {             y*xx82,  -x2*(xxyy2+1),    3*xy } },
			 { {          -3*x*yy31,  y*(6*xx-yy31),   xxyy2 },
			   {      y*(6*xx-yy31),  x*(6*yy-xx31),    4*xy },
			   {              xxyy2,           4*xy,       x } 
			 }};
  double Y1x[3]       =     { 0, -y, -1 };
  
  double Y2x[3][3]    =   { {     0, -yy31, -y2 },
			    { -yy31,  6*xy,  x4 },
			    {   -y2,    x2,   0 } };

  double Y3x[3][3][3] = { { {        0,         -y*yy159,       -yy82 },
			    { -y*yy159,         x2*yy153,       16*xy },
			    {    -yy82,             8*xy,           0 } },
			  { { -y*yy159,         x2*yy153,       16*xy }, 
			    { x2*yy153,          -xx1053, -16*xx-xx86 },
			    {    16*xy, -2*(6*xx-2*yy+1),         3*y } },
			  { {  -3*yy31,            12*xy,          x4 },
			    {    12*xy,     -9*xx+6*yy-1,          y4 },
			    {       x4,               y4,           1 } 
			  }};

  double Y1y[3]       =     { y2, -x, 0 };

  double Y2y[3][3]    =   { { 3*yy31, -6*xy, -x2 },
			    {  -6*xy,  xx31,   0 },
			    {    -x2,   -y2,  -1 } };

  double  Y3y[3][3][3] = { { {    y4*yy159,      -yy1053, -16*xy },
			     {     -yy1053,     y2*xx153,   xx82 },
			     {      -16*xy, 4*xx-12*yy-2,   -6*y } },
			   { {     -yy1053,     y2*xx153,   xx82 },
			     {    y2*xx153,     -x*xx159,      0 },
			     {        xx82,         8*xy,    3*x } },
			   { {      -18*xy,  6*xx-9*yy-1,    -y4 },
			     { 6*xx-9*yy-1,        12*xy,     x4 },
			     {         -y4,           x4,      0 } 
			   }};

     
  for( int i=0;i<3; ++i ){
    Xi[0][i] = X1 [i];
    Xi[1][i] = X1x[i];
    Xi[2][i] = X1y[i];
  }

  for( int i=0;i<3; ++i ){
    Yi[0][i] = Y1 [i];
    Yi[1][i] = Y1x[i];
    Yi[2][i] = Y1y[i];
  }

  for( int i=0;i<3; ++i ) for( int j=0;j<3; ++j ){
    Xii[0][i][j] = X2 [i][j];
    Xii[1][i][j] = X2x[i][j];
    Xii[2][i][j] = X2y[i][j];
  }

  for( int i=0;i<3; ++i ) for( int j=0;j<3; ++j ){
    Yii[0][i][j] = Y2 [i][j];
    Yii[1][i][j] = Y2x[i][j];
    Yii[2][i][j] = Y2y[i][j];
  }
 
  for( int i=0;i<3; ++i ) for( int j=0;j<3; ++j )for( int k=0;k<3; ++k ){
    Xiii[0][i][j][k] = X3 [i][j][k];
    Xiii[1][i][j][k] = X3x[i][j][k];
    Xiii[2][i][j][k] = X3y[i][j][k];
  }

  for( int i=0;i<3; ++i ) for( int j=0;j<3; ++j )for( int k=0;k<3; ++k ){
    Yiii[0][i][j][k] = Y3 [i][j][k];
    Yiii[1][i][j][k] = Y3x[i][j][k];
    Yiii[2][i][j][k] = Y3y[i][j][k];
  }
}

void TrackKiselExtrapolator::integrateField( const Gaudi::XYZPoint& p0,
                                             const Gaudi::XYZPoint& p1,
                                             const Gaudi::XYZPoint& p2,
                                             double si  [3]      , double Si  [3],
                                             double sii [3][3]   , double Sii [3][3], 
                                             double siii[3][3][3], double Siii[3][3][3]) const
{
  double dz = p2.z() - p0.z();

  m_B[0] = fieldVector(p0) ;
  m_B[1] = fieldVector(p1) ;
  m_B[2] = fieldVector(p2) ;

  // no other way...
  double B[3][3];
  unsigned int i = 0;
  for (std::vector<XYZVector>::const_iterator iter = m_B.begin(); iter != m_B.end(); ++iter, ++i){
    B[i][0] = iter->x();
    B[i][1] = iter->y();
    B[i][2] = iter->z();
  } //iter

  // coefficients
  static const double c1[3] = {  1,  4,  1}; // /=6.
  static const double c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.
  static const double c3[3][3][3] = { { { 35, 20, -1},{-124,-256,  20},{-19,-52,-1} }, 
				{ {524,176,-52},{1760,2240,-256},{-52,176,20} }, 
				{ {125,-52,-19},{1028,1760,-124},{125,524,35} } };  // /=45360.
    
  static const double C1[3]       =     {  1,  2,  0}; // /=6.
  static const double C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
  static const double C3[3][3][3] = { { { 85, 28, -5},{   4, -80,   4},{-17,-20, 1} }, 
				{ {494,200,-46},{1256,1376,-184},{-94,  8,14} }, 
				{ { 15,-12, -3},{ 252, 432, -36},{ 21, 84, 3} } }; // /=90720.
    
  // integrate field
  
  for( int x=0; x<3; ++x){	
    si[x]=0;
    for( int n=0; n<3; ++n ) si[x] += c1[n]*B[n][x];	  
    si[x] *= dz/6.;
	 
    Si[x]=0;
    for(int n=0; n<3; ++n ) Si[x] += C1[n]*B[n][x];
    Si[x] *= dz*dz/6.;
	
    for( int y=0; y<3; ++y ){    
      sii[x][y] = 0;
      for(int n=0; n<3; ++n)
	for(int m=0; m<3; ++m) sii[x][y] += c2[n][m]*B[n][x]*B[m][y];
      sii[x][y] *= dz*dz/360.;
	   
      Sii[x][y] = 0;
      for(int n=0; n<3; ++n)
	for(int m=0; m<3; m++) Sii[x][y] += C2[n][m]*B[n][x]*B[m][y];	       
      Sii[x][y] *= dz*dz*dz/2520.;

      for( int z=0; z<3; ++z ){
	siii[x][y][z] = 0;
	for(int n=0; n<3; ++n)
	  for(int m=0; m<3; ++m)
	    for(int k=0; k<3; ++k)
	      siii[x][y][z] += c3[n][m][k]*B[n][x]*B[m][y]*B[k][z];
	siii[x][y][z] *= dz*dz*dz/45360.;
	   
	Siii[x][y][z] = 0;
	for(int n=0; n<3; ++n)
	  for(int m=0; m<3; ++m)
	    for(int k=0; k<3; ++k)
	      Siii[x][y][z] += C3[n][m][k]*B[n][x]*B[m][y]*B[k][z];	       
	Siii[x][y][z] *= dz*dz*dz*dz/90720.;
	       
      }
    }
  }
}
  
void TrackKiselExtrapolator::extrapolateAnalytic( const double T_in[], // input track parameters (x,y,tx,ty,Q/p)
						  double T_out[], // output track parameters
						  double J[], // output covariance matrix
						  const double z_in,
						  double z_out   // extrapolate to this z position
						  ) const
{
  //
  //  Part of the exact extrapolation formula with error (c_light*B*dz)^4/4!
  //
  double qp_in = T_in[4];
  double dz = z_out - z_in;

  // construct coefficients 

  double tx   = T_in[2], ty   = T_in[3];
    
  double   A1[3][3] ,      B1[3][3],
    A2[3][3][3]   , B2[3][3][3], 
    A3[3][3][3][3], B3[3][3][3][3];

  getCoefficients( tx, ty, A1, B1, A2, B2, A3, B3 );

  double t2   = 1. + tx*tx + ty*ty;
  double t    = std::sqrt( t2 );
  double h    = qp_in*Gaudi::Units::c_light;
  double ht = h*t;
  
  double s1[3], s2[3][3], s3[3][3][3], S1[3], S2[3][3], S3[3][3][3];

  // get the points and field
  XYZPoint r0 = XYZPoint(T_in[0], T_in[1],z_in );   
  XYZPoint r2 = XYZPoint( T_in[0] + T_in[2]*dz,T_in[1] + T_in[3]*dz, z_out );
  XYZPoint r1 = r0 + 0.5*(r2-r0);

  m_B[0] = fieldVector( r0 ) ;
  m_B[1] = fieldVector( r1 ) ;
  m_B[2] = fieldVector( r2 ) ;

  double Sy = (7*m_B[0].y() + 6*m_B[1].y()-m_B[2].y() )*dz*dz/96.;
  r1.SetX(T_in[0] + 0.5*tx*dz + ht*Sy*A1[0][1]);
  r1.SetY(T_in[1] + 0.5*ty*dz + ht*Sy*B1[0][1]);

  Sy = (m_B[0].y() + 2*m_B[1].y() )*dz*dz/6.;
  r2.SetX(T_in[0] + tx*dz + ht*Sy*A1[0][1]);
  r2.SetY(T_in[1] + ty*dz + ht*Sy*B1[0][1]);

  integrateField( r0,r1,r2, s1, S1, s2, S2, s3, S3 );

  double sA1 =0, sA2 =0, sA3 =0, sB1 =0, sB2 =0, sB3 =0;
  double sA1x=0, sA2x=0, sA3x=0, sB1x=0, sB2x=0, sB3x=0;
  double sA1y=0, sA2y=0, sA3y=0, sB1y=0, sB2y=0, sB3y=0;

  double SA1 =0, SA2 =0, SA3 =0, SB1 =0, SB2 =0, SB3 =0;
  double SA1x=0, SA2x=0, SA3x=0, SB1x=0, SB2x=0, SB3x=0;
  double SA1y=0, SA2y=0, SA3y=0, SB1y=0, SB2y=0, SB3y=0;


  for( int n=0; n<3; ++n ){
    sA1  += s1[n]*A1[0][n];
    sA1x += s1[n]*A1[1][n];
    sA1y += s1[n]*A1[2][n];
    sB1  += s1[n]*B1[0][n];
    sB1x += s1[n]*B1[1][n];
    sB1y += s1[n]*B1[2][n];
    SA1  += S1[n]*A1[0][n];
    SA1x += S1[n]*A1[1][n];
    SA1y += S1[n]*A1[2][n];
    SB1  += S1[n]*B1[0][n];
    SB1x += S1[n]*B1[1][n];
    SB1y += S1[n]*B1[2][n];

    if(m_order>1) 
      for( int m=0; m<3;++m){
	sA2  += s2[n][m]*A2[0][n][m];
	sA2x += s2[n][m]*A2[1][n][m];
	sA2y += s2[n][m]*A2[2][n][m];
	sB2  += s2[n][m]*B2[0][n][m];
	sB2x += s2[n][m]*B2[1][n][m];
	sB2y += s2[n][m]*B2[2][n][m];
 
	SA2  += S2[n][m]*A2[0][n][m];
	SA2x += S2[n][m]*A2[1][n][m];
	SA2y += S2[n][m]*A2[2][n][m];
	SB2  += S2[n][m]*B2[0][n][m];
	SB2x += S2[n][m]*B2[1][n][m];
	SB2y += S2[n][m]*B2[2][n][m];

	if(m_order>2)
	  for(int k=0; k<3; ++k ){
	    sA3  += s3[n][m][k]*A3[0][n][m][k];
	    sA3x += s3[n][m][k]*A3[1][n][m][k];
	    sA3y += s3[n][m][k]*A3[2][n][m][k];
	    sB3  += s3[n][m][k]*B3[0][n][m][k];
	    sB3x += s3[n][m][k]*B3[1][n][m][k];
	    sB3y += s3[n][m][k]*B3[2][n][m][k];
 
	    SA3  += S3[n][m][k]*A3[0][n][m][k];
	    SA3x += S3[n][m][k]*A3[1][n][m][k];
	    SA3y += S3[n][m][k]*A3[2][n][m][k];
	    SB3  += S3[n][m][k]*B3[0][n][m][k];
	    SB3x += S3[n][m][k]*B3[1][n][m][k];
	    SB3y += S3[n][m][k]*B3[2][n][m][k];
	  } //k
      } // m 
  } // n

  T_out[0] = T_in[0] + tx*dz + ht*( SA1 + ht*(SA2 + ht*SA3) );
  T_out[1] = T_in[1] + ty*dz + ht*( SB1 + ht*(SB2 + ht*SB3) );
  T_out[2] = T_in[2] + ht*( sA1 + ht*(sA2 + ht*sA3) );
  T_out[3] = T_in[3] + ht*( sB1 + ht*(sB2 + ht*sB3) );	      
  T_out[4] = T_in[4];
  
  // derivatives '_x

  J[0] = 1; J[1] = 0; J[2] = 0; J[3] = 0; J[4] = 0;
  
  // derivatives '_y

  J[5] = 0; J[6] = 1; J[7] = 0; J[8] = 0; J[9] = 0;
  
  
  // derivatives '_tx

  J[10] = dz + h*tx*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1x + ht*(SA2x + ht*SA3x) );
  J[11] =      h*tx*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1x + ht*(SB2x + ht*SB3x) );
  J[12] =  1 + h*tx*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1x + ht*(sA2x + ht*sA3x) );
  J[13] =      h*tx*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1x + ht*(sB2x + ht*sB3x) );
  J[14] = 0;

 
  // derivatives '_ty

  J[15] =      h*ty*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1y + ht*(SA2y + ht*SA3y) );
  J[16] = dz + h*ty*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1y + ht*(SB2y + ht*SB3y) );
  J[17] =      h*ty*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1y + ht*(sA2y + ht*sA3y) );
  J[18] =  1 + h*ty*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1y + ht*(sB2y + ht*sB3y) );
  J[19] = 0;


  // derivatives '_qp

  J[20] = Gaudi::Units::c_light*t*( SA1 + ht*( 2*SA2 + 3*ht*SA3 ) );
  J[21] = Gaudi::Units::c_light*t*( SB1 + ht*( 2*SB2 + 3*ht*SB3 ) );
  J[22] = Gaudi::Units::c_light*t*( sA1 + ht*( 2*sA2 + 3*ht*sA3 ) );
  J[23] = Gaudi::Units::c_light*t*( sB1 + ht*( 2*sB2 + 3*ht*sB3 ) );
  J[24] = 1;
    
}
