
/*
 *  Derivatives
 *  author F. MACIUC
 *  date 8.02.2008
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

StatusCode Derivatives::initialize( int n_tr_pars,
			  int n_elements,
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
			std::vector<double> &derGB, // vector containing global derivatives
			int station, // detector element number in list
			double z,  // z-position of this det. element
			double sta ) { // stereo-angle of this det element
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
      x0 = origin.x() -tx*origin.z();
      y0 = origin.y() -ty*origin.z();
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

//  double d_alpha = Rx.Angle(); // d gamma is rotation about z-axis
//  double d_beta = Ry.Angle(); // d gamma is rotation about z-axis
//  double d_gamma = Rz.Angle(); // d gamma is rotation about z-axis
//  double d_z = shifts.z(); // d_z is shift along z-axis

  int dof_cnt = 0; // count the degrees of freedom
  // shift in co-ordinate along measurement:
  if ( m_dofs[0] ) {
    derGB[station] = 1.0;
    dof_cnt++;
  }

  // shift in co-ordinate perpendicular to measurement:
  if ( m_dofs[1] ) {
    derGB[station+dof_cnt*m_n_elements] = 0.0;
    dof_cnt++;
  }

  // shift along z co-ordinate
  if ( m_dofs[2] ) {
    derGB[station+dof_cnt*m_n_elements] =
      //      (tx * cos( sta ) + ty * sin( sta ) ) * cos ( d_gamma ) +
      //      ( -tx * sin( sta ) + ty * cos ( sta ) ) * sin ( d_gamma );
      -(tx * cos( sta ) + ty * sin( sta ) );
    dof_cnt++;
  }


  // rotation about local measurement-axis
  if ( m_dofs[3] ) {
    //    derGB[station+dof_cnt*m_n_elements] =  tx*z+x; here is left
    derGB[station+dof_cnt*m_n_elements] = -(ty*z+y0)*(tx*cos(sta)+ty*sin(sta));
    dof_cnt++;
  }

  // rotation about local perpendicular-axis
  if ( m_dofs[4] ) {
    derGB[station+dof_cnt*m_n_elements] = -(tx*z+x0)*(tx*cos(sta)+ty*sin(sta));
    dof_cnt++;
  }

  // rotation about z-axis
  if ( m_dofs[5] ) {
    derGB[station+dof_cnt*m_n_elements] =
      //      (d_z + z) * ( tx*cos( sta ) + ty*sin( sta ) ) * (-1.0)*sin( d_gamma )
      //      + (d_z + z) * ( -1.0*tx*sin( sta ) + ty*cos( sta ) ) * cos( d_gamma );
      z*tx*sin(sta)-z*ty*cos(sta) + x0*sin(sta) -y0*cos(sta);
    dof_cnt++;
  }

  return StatusCode::SUCCESS;
}





bool Derivatives::SetGlobalN( const std::vector<double> &tvec,
				    const std::vector<double> &delta,
				    const Gaudi::XYZPoint &plane,
				    const Gaudi::XYZPoint &sensor,
				    std::vector<double> &derGB,
				    int station, 
				    const Gaudi::XYZVector & Straj, 
				    const Gaudi::XYZVector & Mtraj ) {
  // ============================================
  // station is the rank of the alignable object 
  // in the list of alignable objects
  // ============================================

  double x0,tx,y0,ty, dtau,dz,alpha, dalpha,dbeta,dgamma;

  double kx,ky,kz,mx,my,mz;
  alpha=0;
  x0=tvec[0];
  tx=tvec[1];
  y0=tvec[2];
  ty=tvec[3];
  dtau=delta[0];
  dz=delta[1];
  dalpha=delta[2];
  dbeta=delta[3];
  dgamma=delta[4];
  double xp=plane.X();
  double yp=plane.Y();
  double zp=plane.Z();
  double xc=sensor.X()-xp;
  double yc=sensor.Y()-yp;
  double zc=sensor.Z()-zp;


  kx=Mtraj.X()/sqrt(Mtraj.Mag2()); ky=Mtraj.Y()/sqrt(Mtraj.Mag2()); kz=Mtraj.Z()/sqrt(Mtraj.Mag2()); //trajectory in the direction of measurement
  mx=Straj.X()/sqrt(Straj.Mag2()); my=Straj.Y()/sqrt(Straj.Mag2()); mz=Straj.Z()/sqrt(Straj.Mag2()); //strip trajectory 

  
  int dof_cnt = 0; // count the degrees of freedom
  // shift in co-ordinate along measurement:
  if ( m_dofs[0] ) {
    derGB[station] = 1.0; 
    dof_cnt++;
  }

  // shift in co-ordinate perpendicular to measurement:
  if ( m_dofs[1] ) {
    derGB[station+dof_cnt*m_n_elements] = 0.0; 
    dof_cnt++;
  }

  // shift along z co-ordinate
  if ( m_dofs[2] ) {

     derGB[station+dof_cnt*m_n_elements]= (-(my*tx) + mx*ty)/ 
   (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty + kx*mz*ty); 

    dof_cnt++;
  }


  // rotation about local measurement-axis
  if ( m_dofs[3] ) {

   derGB[station+dof_cnt*m_n_elements]=
((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty + kx*mz*ty)* 
      (mz*x0 + my*ty*x0 - mz*xc - my*ty*xc - mz*xp - my*ty*xp -  
        my*tx*y0 + mx*ty*yc + my*tx*yp + mx*zc + mz*tx*zp) +  
     (kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
      (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
        mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
        mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
        mx*ty*zp))/ 
   pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
       kx*mz*ty,2.); 

    dof_cnt++;
  }

  // rotation about local perpendicular-axis
  if ( m_dofs[4] ) {

    derGB[station+dof_cnt*m_n_elements]=((kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
      (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
        mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
        mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
        mx*ty*zp) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
        kz*mx*ty + kx*mz*ty)* 
      (-(mx*ty*x0) + my*tx*xc + mx*ty*xp + mz*y0 + mx*tx*y0 -  
        mz*yc - mx*tx*yc - mz*yp - mx*tx*yp + my*zc + mz*ty*zp)) 
    /pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	   kx*mz*ty,2.);
    dof_cnt++;
  }

  // rotation about z-axis
  if ( m_dofs[5] ) {

     derGB[station+dof_cnt*m_n_elements] =((-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
      (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
        mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
        mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
        mx*ty*zp) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
        kz*mx*ty + kx*mz*ty)* 
      (-(mx*x0) + mz*tx*xc + mx*xp - my*y0 + mz*ty*yc + my*yp -  
        mx*tx*zc - my*ty*zc - mx*tx*zp - my*ty*zp))/ 
   pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	 kx*mz*ty,2.);
    dof_cnt++;
  }

  return true;
}




StatusCode Derivatives::SetLocal( std::vector<double> &derLC,
				  int rank,
				  double z,
				  double dz,
				  double stereo_angle ) {


  //C  info() << "m_tr_pars: " << m_tr_pars << " " << rank  << " " << z << " " << stereo_angle  <<endreq;
  if ( m_tr_pars == 1 )
    derLC[0] = cos( stereo_angle );
  else if ( m_tr_pars == 2 ) {
    derLC[0] = cos( stereo_angle );
    derLC[1] = z * cos ( stereo_angle );
  } else if ( m_tr_pars == 4 ) {
    derLC[0] = cos( stereo_angle );
    derLC[1] = z * cos ( stereo_angle );
    derLC[2] = sin( stereo_angle );
    derLC[3] = z * sin( stereo_angle );
  } else if ( m_tr_pars == 5 ) {
    derLC[0] = cos( stereo_angle );
    derLC[1] = z * cos ( stereo_angle );
    derLC[2] = z * z * cos( stereo_angle );
    derLC[3] = sin( stereo_angle );
    derLC[4] = z * sin( stereo_angle );
  } else if ( m_tr_pars == 6 ) {
    derLC[0] = cos( stereo_angle );
    derLC[1] = z * cos ( stereo_angle );
    derLC[2] = z * z * cos( stereo_angle );
    derLC[4] = sin( stereo_angle );
    derLC[5] = z * sin( stereo_angle );
    derLC[6] = z * z * sin( stereo_angle );
  }
  return StatusCode::SUCCESS;
}





bool Derivatives::SetLocalN( std::vector<double> &derLC, 
				  int rank, 
                                  const std::vector<double> &tvec,
				  const std::vector<double> &delta,
                                  const Gaudi::XYZPoint &plane,
                                  const Gaudi::XYZPoint &sensor,
				  const Gaudi::XYZVector & Straj,
                                  const Gaudi::XYZVector & Mtraj ) {
  

  double x0,tx,y0,ty,dtau,dz,alpha, dalpha,dbeta,dgamma;
  double kx,ky,kz,mx,my,mz;
  double stereo_angle=0;
  alpha=0.;
  x0=tvec[0];
  tx=tvec[1];
  y0=tvec[2];
  ty=tvec[3];
  dtau=delta[0];
  dz=delta[1];
  dalpha=delta[2];
  dbeta=delta[3];
  dgamma=delta[4];
  double xp = plane.X();
  double yp = plane.Y(); 
  double zp = plane.Z();
  double xc = sensor.X()-xp;
  double yc = sensor.Y()-yp;
  double zc = sensor.Z()-zp;

  kx=Mtraj.X()/sqrt(Mtraj.Mag2()); ky=Mtraj.Y()/sqrt(Mtraj.Mag2()); kz=Mtraj.Z()/sqrt(Mtraj.Mag2());
  mx=Straj.X()/sqrt(Straj.Mag2()); my=Straj.Y()/sqrt(Straj.Mag2()); mz=Straj.Z()/sqrt(Straj.Mag2()); //strip trajectory 

  double z=0.0;
  //C  info() << "m_tr_pars: " << m_tr_pars << " " << rank  << " " << z << " " << stereo_angle  <<endreq; 
  if ( fabs(derLC[0]) > 1.0e50 
       || fabs(derLC[1]) > 1.0e50 
       || fabs(derLC[2]) > 1.0e50 
       || fabs(derLC[3]) > 1.0e50 ) {
    info() << "SetlocalN (1): derLC = " << derLC << endreq;
    info() << "                tvec = " << tvec << endreq;
    info() << "               delta = " << delta << endreq;
    info() << "               plane = " << plane << endreq;
    info() << "              sensor = " << sensor << endreq;
    info() << "               Straj = " << Straj << endreq;
    info() << "               Mtraj = " << Mtraj << endreq;
  }
  if ( m_tr_pars == 1 )  
    derLC[0] = 0.0;
  else if ( m_tr_pars == 2 ) {
    derLC[0] = 0.0;
    derLC[1] = 0.0;
  } else if ( m_tr_pars == 4 ) {

    derLC[0]=((-my + mz*ty)*(ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
			       kz*mx*ty + kx*mz*ty) +  
		dbeta*((kz*my - ky*mz - ky*mx*tx + kx*my*tx)*(-my + mz*ty) -  
		       mx*ty*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			      kx*mz*ty)) + dgamma* 
		((kz*mx - kx*mz + ky*mx*ty - kx*my*ty)*(-my + mz*ty) +  
		 (mz + my*ty)*(ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
			       kz*mx*ty + kx*mz*ty)) +  
		dalpha*(-(mx*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			      kx*mz*ty)) +  
			(-my + mz*ty)*(-(kz*mx*tx) + kx*mz*tx - kz*my*ty +  
				       ky*mz*ty)))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,2.); 



    derLC[1]=(dz*(kz*my - ky*mz)*(-(my*tx) + mx*ty) -  
		dz*my*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
		       kx*mz*ty) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
				    kz*mx*ty + kx*mz*ty)* 
		(-(mz*y0) + mz*yc + mz*yp - my*zc - my*zp) +  
		(kz*my - ky*mz)*(-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc +  
				 my*xp - mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc +  
				 mz*tx*yc - mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
				 my*tx*zp + mx*ty*zp) +  
		dgamma*((kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
			(-(mz*y0) + mz*yc + mz*yp - my*zc - my*zp) +  
			(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			 kx*mz*ty)*(-(my*y0) + my*yp + mz*zp) +  
			(kz*my - ky*mz)*(mz*x0 + my*ty*x0 - mz*xc - my*ty*xc -  
					 mz*xp - my*ty*xp - my*tx*y0 + mx*ty*yc + my*tx*yp +  
					 mx*zc + mz*tx*zp)) +  
		dalpha*((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			 kx*mz*ty)*(mz*xc - mx*zc - mx*zp) +  
			(-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
			(-(mz*y0) + mz*yc + mz*yp - my*zc - my*zp) +  
			(-(kz*mx) + kx*mz)* 
			(-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			 mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			 mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
			 mx*ty*zp) + (kz*my - ky*mz)* 
			(-(mx*x0) + mz*tx*xc + mx*xp - my*y0 + mz*ty*yc +  
			 my*yp - mx*tx*zc - my*ty*zc - mx*tx*zp - my*ty*zp)) 
		+ dbeta*((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			  kx*mz*ty)*(my*xc + mx*y0 - mx*yc - mx*yp) +  
			 (kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
			 (-(mz*y0) + mz*yc + mz*yp - my*zc - my*zp) +  
			 (-(ky*mx) + kx*my)* 
			 (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			  mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			  mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
			  mx*ty*zp) + (kz*my - ky*mz)* 
			 (-(mx*ty*x0) + my*tx*xc + mx*ty*xp + mz*y0 +  
			  mx*tx*y0 - mz*yc - mx*tx*yc - mz*yp - mx*tx*yp +  
			  my*zc + mz*ty*zp)))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,2.) - (2.*(kz*my - ky*mz)* 
			  (dz*(-(my*tx) + mx*ty)* 
			   (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			    kx*mz*ty) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
					 kz*mx*ty + kx*mz*ty)* 
			   (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			    mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			    mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
			    mx*ty*zp) + dgamma* 
			   ((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			     kx*mz*ty)*(mz*x0 + my*ty*x0 - mz*xc - my*ty*xc -  
					mz*xp - my*ty*xp - my*tx*y0 + mx*ty*yc +  
					my*tx*yp + mx*zc + mz*tx*zp) +  
			    (kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
			    (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			     mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			     mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
			     my*tx*zp + mx*ty*zp)) +  
			   dalpha*((-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
				   (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
				    mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
				    mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
				    my*tx*zp + mx*ty*zp) +  
				   (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
				    kx*mz*ty)*(-(mx*x0) + mz*tx*xc + mx*xp - my*y0 +  
					       mz*ty*yc + my*yp - mx*tx*zc - my*ty*zc -  
					       mx*tx*zp - my*ty*zp)) +  
			   dbeta*((kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
				  (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
				   mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
				   mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
				   my*tx*zp + mx*ty*zp) +  
				  (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
				   kx*mz*ty)*(-(mx*ty*x0) + my*tx*xc + mx*ty*xp +  
					      mz*y0 + mx*tx*y0 - mz*yc - mx*tx*yc - mz*yp -  
					      mx*tx*yp + my*zc + mz*ty*zp))))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,3.); 




    derLC[2]=((mx - mz*tx)*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			      kx*mz*ty) + dgamma*((mx - mz*tx)* 
						  (kz*mx - kx*mz + ky*mx*ty - kx*my*ty) -  
						  my*tx*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
							 kx*mz*ty)) + dbeta* 
		((kz*my - ky*mz - ky*mx*tx + kx*my*tx)*(mx - mz*tx) +  
		 (mz + mx*tx)*(ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
			       kz*mx*ty + kx*mz*ty)) +  
		dalpha*(-(my*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			      kx*mz*ty)) +  
			(mx - mz*tx)*(-(kz*mx*tx) + kx*mz*tx - kz*my*ty +  
				      ky*mz*ty)))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,2.); 
 

    derLC[3]=(dz*(-(kz*mx) + kx*mz)*(-(my*tx) + mx*ty) +  
		dz*mx*(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
		       kx*mz*ty) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
				    kz*mx*ty + kx*mz*ty)* 
		(mz*x0 - mz*xc - mz*xp + mx*zc + mx*zp) +  
		(-(kz*mx) + kx*mz)*(-(my*x0) + mz*ty*x0 + my*xc -  
				    mz*ty*xc + my*xp - mz*ty*xp + mx*y0 - mz*tx*y0 -  
				    mx*yc + mz*tx*yc - mx*yp + mz*tx*yp - my*tx*zc +  
				    mx*ty*zc - my*tx*zp + mx*ty*zp) +  
		dgamma*((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			 kx*mz*ty)*(my*x0 - my*xc - my*xp + mx*yc) +  
			(kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
			(mz*x0 - mz*xc - mz*xp + mx*zc + mx*zp) +  
			(-(kz*mx) + kx*mz)* 
			(mz*x0 + my*ty*x0 - mz*xc - my*ty*xc - mz*xp -  
			 my*ty*xp - my*tx*y0 + mx*ty*yc + my*tx*yp + mx*zc +  
			 mz*tx*zp) + (ky*mx - kx*my)* 
			(-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			 mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			 mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
			 mx*ty*zp)) + dalpha* 
		((-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
		 (mz*x0 - mz*xc - mz*xp + mx*zc + mx*zp) +  
		 (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
		  kx*mz*ty)*(mz*yc - my*zc - my*zp) +  
		 (-(kz*my) + ky*mz)* 
		 (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
		  mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
		  mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
		  mx*ty*zp) + (-(kz*mx) + kx*mz)* 
		 (-(mx*x0) + mz*tx*xc + mx*xp - my*y0 + mz*ty*yc +  
		  my*yp - mx*tx*zc - my*ty*zc - mx*tx*zp - my*ty*zp)) 
		+ dbeta*((kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
			 (mz*x0 - mz*xc - mz*xp + mx*zc + mx*zp) +  
			 (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			  kx*mz*ty)*(-(mx*x0) + mx*xp + mz*zp) +  
			 (-(kz*mx) + kx*mz)* 
			 (-(mx*ty*x0) + my*tx*xc + mx*ty*xp + mz*y0 +  
			  mx*tx*y0 - mz*yc - mx*tx*yc - mz*yp - mx*tx*yp +  
			  my*zc + mz*ty*zp)))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,2.) - (2.*(-(kz*mx) + kx*mz)* 
			  (dz*(-(my*tx) + mx*ty)* 
			   (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			    kx*mz*ty) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
					 kz*mx*ty + kx*mz*ty)* 
			   (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			    mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			    mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
			    mx*ty*zp) + dgamma* 
			   ((ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
			     kx*mz*ty)*(mz*x0 + my*ty*x0 - mz*xc - my*ty*xc -  
					mz*xp - my*ty*xp - my*tx*y0 + mx*ty*yc +  
					my*tx*yp + mx*zc + mz*tx*zp) +  
			    (kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
			    (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
			     mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
			     mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
			     my*tx*zp + mx*ty*zp)) +  
			   dalpha*((-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
				   (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
				    mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
				    mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
				    my*tx*zp + mx*ty*zp) +  
				   (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
				    kx*mz*ty)*(-(mx*x0) + mz*tx*xc + mx*xp - my*y0 +  
					       mz*ty*yc + my*yp - mx*tx*zc - my*ty*zc -  
					       mx*tx*zp - my*ty*zp)) +  
			   dbeta*((kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
				  (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
				   mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
				   mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc -  
				   my*tx*zp + mx*ty*zp) +  
				  (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
				   kx*mz*ty)*(-(mx*ty*x0) + my*tx*xc + mx*ty*xp +  
					      mz*y0 + mx*tx*y0 - mz*yc - mx*tx*yc - mz*yp -  
					      mx*tx*yp + my*zc + mz*ty*zp))))/ 
      pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
	  kx*mz*ty,3.); 



  } else if ( m_tr_pars == 5 ) {

    // this shall be changed 
    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 0.0;
    derLC[3] = 0.0;
    derLC[4] = 0.0;


  } else if ( m_tr_pars == 6 ) {
    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 0.0;
    derLC[4] = 0.0;
    derLC[5] = 0.0;
    derLC[6] = 0.0;
  }


  if ( fabs(derLC[0]) > 1.0e50 
       || fabs(derLC[1]) > 1.0e50 
       || fabs(derLC[2]) > 1.0e50 
       || fabs(derLC[3]) > 1.0e50 ) {
    info() << "SetlocalN (2): derLC = " << derLC << endreq;
    info() << "                tvec = " << tvec << endreq;
    info() << "               delta = " << delta << endreq;
    info() << "               plane = " << plane << endreq;
    info() << "              sensor = " << sensor << endreq;
    info() << "               Straj = " << Straj << endreq;
    info() << "               Mtraj = " << Mtraj << endreq;
  }

  return true;
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




double Derivatives::Model(const std::vector<double> &tvec,
			  const std::vector<double> &delta,
			  const Gaudi::XYZVector &plane,
			  const Gaudi::XYZVector &sensor,				    
                          const Gaudi::XYZVector & Straj,
                          const Gaudi::XYZVector & Mtraj ) {

  double x0,tx,y0,ty,dtau,dz,alpha, dalpha,dbeta,dgamma;
  double kx,ky,kz,mx,my,mz;
  
  alpha=0.;
  x0=tvec[0];
  tx=tvec[1];
  y0=tvec[2];
  ty=tvec[3];
  dtau=delta[0];
  dz=delta[1];
  dalpha=delta[2];
  dbeta=delta[3];
  dgamma=delta[4];
  double xp,yp,zp,xc,yc,zc;
  xp=plane.X(); 
  yp=plane.Y(); 
  zp=plane.Z();
  xc=sensor.X() - xp; 
  yc=sensor.Y() - yp;
  zc=sensor.Z() - zp;

  kx=Mtraj.X()/sqrt(Mtraj.Mag2()); 
  ky=Mtraj.Y()/sqrt(Mtraj.Mag2()); 
  kz=Mtraj.Z()/sqrt(Mtraj.Mag2());
  mx=Straj.X()/sqrt(Straj.Mag2()); 
  my=Straj.Y()/sqrt(Straj.Mag2()); 
  mz=Straj.Z()/sqrt(Straj.Mag2()); //strip trajectory 

  if ( isnan(kx) || isnan(ky) || isnan(kz) ) {
    debug() << "Model: kx = " << kx << ", " << ky << ", " << kz << endreq;
    debug() << "       xc = " << xc << ", " << yc << ", " << zc << endreq;
    debug() << "       mx = " << mx << ", " << my << ", " << mz << endreq;
    debug() << "     tvec = " << tvec << endreq;
    debug() << "    delta = " << delta << endreq;
    debug() << "    plane = " << plane << endreq;
    debug() << "   sensor = " << sensor << endreq;
    debug() << "    Mtraj = " << Mtraj << endreq;
    debug() << "    Straj = " << Straj << endreq;
    return kx;
  }
  double modelF= dtau + (dz*(-(my*tx) + mx*ty)* 
       (ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
         kx*mz*ty) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
         kz*mx*ty + kx*mz*ty)* 
       (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
         mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
         mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
         mx*ty*zp) + 
	dgamma*((ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
            kz*mx*ty + kx*mz*ty)* 
          (mz*x0 + my*ty*x0 - mz*xc - my*ty*xc - mz*xp -  
            my*ty*xp - my*tx*y0 + mx*ty*yc + my*tx*yp + mx*zc +  
            mz*tx*zp) + (kz*mx - kx*mz + ky*mx*ty - kx*my*ty)* 
          (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
            mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
            mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
            mx*ty*zp)) + 
        dalpha*((-(kz*mx*tx) + kx*mz*tx - kz*my*ty + ky*mz*ty)* 
          (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
            mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
            mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
            mx*ty*zp) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
            kz*mx*ty + kx*mz*ty)* 
          (-(mx*x0) + mz*tx*xc + mx*xp - my*y0 + mz*ty*yc +  
            my*yp - mx*tx*zc - my*ty*zc - mx*tx*zp - my*ty*zp)) +
        dbeta*((kz*my - ky*mz - ky*mx*tx + kx*my*tx)* 
          (-(my*x0) + mz*ty*x0 + my*xc - mz*ty*xc + my*xp -  
            mz*ty*xp + mx*y0 - mz*tx*y0 - mx*yc + mz*tx*yc -  
            mx*yp + mz*tx*yp - my*tx*zc + mx*ty*zc - my*tx*zp +  
            mx*ty*zp) + (ky*mx - kx*my + kz*my*tx - ky*mz*tx -  
            kz*mx*ty + kx*mz*ty)* 
          (-(mx*ty*x0) + my*tx*xc + mx*ty*xp + mz*y0 +  
            mx*tx*y0 - mz*yc - mx*tx*yc - mz*yp - mx*tx*yp +  
            my*zc + mz*ty*zp)))/ 
    pow(ky*mx - kx*my + kz*my*tx - ky*mz*tx - kz*mx*ty +  
      kx*mz*ty,2.); 
  if ( isnan(modelF) ) {
      error() << "Error in calculating model function!" << endreq;
      error() << "Model: kx = " << kx << ", " << ky << ", " << kz << endreq;
      error() << "       xc = " << xc << ", " << yc << ", " << zc << endreq;
      error() << "       mx = " << mx << ", " << my << ", " << mz << endreq;
      error() << "     tvec = " << tvec << endreq;
      error() << "    delta = " << delta << endreq;
      error() << "    plane = " << plane << endreq;
      error() << "   sensor = " << sensor << endreq;
      error() << "    Mtraj = " << Mtraj << endreq;
      error() << "    Straj = " << Straj << endreq;
  }
  return modelF;

}
