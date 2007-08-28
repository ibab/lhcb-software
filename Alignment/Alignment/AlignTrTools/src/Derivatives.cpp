/*
 *  Derivatives
 *
 *
 */
//from GAUDI
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Derivatives.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace Gaudi;


/// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( Derivatives );



/// Standard Concstructor:
Derivatives::Derivatives( const std::string &type,
			  const std::string& name,
			  const IInterface* parent )
  : GaudiTool( type, name, parent ),
    m_n_elements(0),
    m_tr_pars(0),
    m_n_dofs(0)
{
  declareInterface<IDerivatives>(this);
}

/// Destructor:
Derivatives::~Derivatives(){};

StatusCode Derivatives::initialize( const int &n_tr_pars,
			  const int &n_elements,
			  const std::vector<bool> &dofs ) {
  m_n_elements = n_elements;
  m_tr_pars = n_tr_pars;
  m_dofs = dofs;
  for ( unsigned int i = 0; i < m_dofs.size();i++ )
    if ( m_dofs[i] )
      m_n_dofs++;
  m_derLC.resize( m_tr_pars );
  m_derGB.resize( m_n_elements * m_n_dofs );
  info() << "Number of degrees of freedom: " << m_n_dofs << endreq;
  info() << "Number of local parameters: " << m_tr_pars << endreq;
  info() << "Number of objects to align: " << m_n_elements << endreq;
  return StatusCode::SUCCESS;
}

StatusCode Derivatives::SetGlobal( const Gaudi::XYZVector &slope, const
                        Gaudi::XYZVector &origin,  /// Slope and origin vectors of track
			const Gaudi::Transform3D &deltas, /// transformation matrix with shifts & rotations
			std::vector<double> &m_derGB, // vector containing global derivatives
			const int &station, // detector element number in list
			const double &z,  // z-position of this det. element
			const double &sta ) { // stereo-angle of this det element
  // ============================================
  // slope is a vector obtained from the track state at the z of the hit
  // z is the z-position of the hit
  // station is the rank of the alignable object
  // in the list of alignable objects
  // ============================================
  double tx = slope.x();
  double ty = slope.y();
  double x0,y0;
  double z0 = z;
  if(z0==0.){
    x0 = origin.x();
    y0 = origin.y();
  }else
    {
      x0 = origin.x() -tx*z0;
      y0 = origin.y() -ty*z0;
    }

  Gaudi::XYZVector shifts;
  Gaudi::Rotation3D d_rotations;
  deltas.GetDecomposition( d_rotations, shifts); //retrieve rotations & shifts from transformation matrix
  // determine rotations about the 3 axis from the transform3d matrix
  double xx,xy,xz,yy,yx,yz,zz,zx,zy;
  d_rotations.GetComponents(xx,xy,xz,yx,yy,yz,zx,zy,zz);
  Gaudi::RotationX Rx(acos(xy));
  Gaudi::RotationY Ry(acos(xx));
  Gaudi::RotationZ Rz(acos(xz));

  double d_alpha = Rx.Angle(); // d gamma is rotation about z-axis
  double d_beta = Ry.Angle(); // d gamma is rotation about z-axis
  double d_gamma = Rz.Angle(); // d gamma is rotation about z-axis
  double d_z = shifts.z(); // d_z is shift along z-axis

  int dof_cnt = 0; // count the degrees of freedom
  // shift in co-ordinate along measurement:
  if ( m_dofs[0] ) {
    m_derGB[station] = 1.0;
    dof_cnt++;
  }

  // shift in co-ordinate perpendicular to measurement:
  if ( m_dofs[1] ) {
    m_derGB[station+dof_cnt*m_n_elements] = 0.0;
    dof_cnt++;
  }

  // shift along z co-ordinate
  if ( m_dofs[2] ) {
    m_derGB[station+dof_cnt*m_n_elements] =
      //      (tx * cos( sta ) + ty * sin( sta ) ) * cos ( d_gamma ) +
      //      ( -tx * sin( sta ) + ty * cos ( sta ) ) * sin ( d_gamma );
      -(tx * cos( sta ) + ty * sin( sta ) );
    dof_cnt++;
  }


  // rotation about local measurement-axis
  if ( m_dofs[3] ) {
    //    m_derGB[station+dof_cnt*m_n_elements] =  tx*z+x; here is left
    m_derGB[station+dof_cnt*m_n_elements] = -(ty*z+y0)*(tx*cos(sta)+ty*sin(sta));
    dof_cnt++;
  }

  // rotation about local perpendicular-axis
  if ( m_dofs[4] ) {
    m_derGB[station+dof_cnt*m_n_elements] = (tx*z+x0)*(tx*cos(sta)+ty*sin(sta));
    dof_cnt++;
  }

  // rotation about z-axis
  if ( m_dofs[5] ) {
    m_derGB[station+dof_cnt*m_n_elements] =
      //      (d_z + z) * ( tx*cos( sta ) + ty*sin( sta ) ) * (-1.0)*sin( d_gamma )
      //      + (d_z + z) * ( -1.0*tx*sin( sta ) + ty*cos( sta ) ) * cos( d_gamma );
      z*tx*sin(sta)-z*ty*cos(sta) + x0*sin(sta) -y0*cos(sta);
    dof_cnt++;
  }

  return StatusCode::SUCCESS;
}

StatusCode Derivatives::SetLocal( std::vector<double> &m_derLC,
				  const int &rank,
				  const double &z,
				  const double &stereo_angle ) {


  //C  info() << "m_tr_pars: " << m_tr_pars << " " << rank  << " " << z << " " << stereo_angle  <<endreq;
  if ( m_tr_pars == 1 )
    m_derLC[0] = cos( stereo_angle );
  else if ( m_tr_pars == 2 ) {
    m_derLC[0] = cos( stereo_angle );
    m_derLC[1] = z * cos ( stereo_angle );
  } else if ( m_tr_pars == 4 ) {
    m_derLC[0] = cos( stereo_angle );
    m_derLC[1] = z * cos ( stereo_angle );
    m_derLC[2] = sin( stereo_angle );
    m_derLC[3] = z * sin( stereo_angle );
  } else if ( m_tr_pars == 5 ) {
    m_derLC[0] = cos( stereo_angle );
    m_derLC[1] = z * cos ( stereo_angle );
    m_derLC[2] = z * z * cos( stereo_angle );
    m_derLC[3] = sin( stereo_angle );
    m_derLC[4] = z * sin( stereo_angle );
  } else if ( m_tr_pars == 6 ) {
    m_derLC[0] = cos( stereo_angle );
    m_derLC[1] = z * cos ( stereo_angle );
    m_derLC[2] = z * z * cos( stereo_angle );
    m_derLC[4] = sin( stereo_angle );
    m_derLC[5] = z * sin( stereo_angle );
    m_derLC[6] = z * z * sin( stereo_angle );
  }
  return StatusCode::SUCCESS;
}



double Derivatives::GetGlobalDerivative_X(){
  return  m_globalderiv_X;
}

double Derivatives::GetGlobalDerivative_Y(){
  return  m_globalderiv_Y;
}

double Derivatives::GetGlobalDerivative_Z(){
  return  m_globalderiv_Z;
}

double Derivatives::GetGlobalDerivative_RotZ(){
  return m_globalderiv_RotZ;
}

void Derivatives::SetTrueSlope(double slope[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_TrackSlope[i] = slope[i];
  }
}
void Derivatives::SetTrueOrigin(double origin[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_TrackOrigin[i] = origin[i];
  }
}

void Derivatives::SetModulePos(double modulepos[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModulePos[i] = modulepos[i];
  }
}

void Derivatives::SetModuleRot(double modulerot[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModuleRot[i] = modulerot[i];
  }
}

void Derivatives::SetModuleOffset(double moduleoff[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModuleOffset[i] = moduleoff[i];
  }
}
void Derivatives::SetStereo(double ster){
  m_Stereo = ster;
}
