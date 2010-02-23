// $Id: $
// Include files 



// local
#include "BeamLineInfo.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : BeamLineInfo
//
// 2009-09-16 : Neal Gauvin
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamLineInfo::BeamLineInfo() : 
  m_axis1(1.,0.,0.), 
  m_axis2(0.,1.,0.), 
  m_axis3(0.,0.,1.),
  m_sigma1(0.), 
  m_sigma2(0.), 
  m_sigma3(0.),
  m_nbentries(0)
{
}

//=============================================================================
// Destructor
//=============================================================================
BeamLineInfo::~BeamLineInfo() {} 

//=============================================================================
// Set parameters
//=============================================================================
void BeamLineInfo::SetNbEntries( int & nb ){
  m_nbentries = nb;
}


void BeamLineInfo::SetIPPosition( Gaudi::XYZPoint & pos ){ m_ipPosition = pos; }
void BeamLineInfo::SetIPPosition( double v1, double v2, double v3 ){ 
  m_ipPosition.SetX(v1); m_ipPosition.SetY(v2); m_ipPosition.SetZ(v3);
}


//Eigenvectors are given as the columns of a matrix
void BeamLineInfo::SetEigenVectors( Gaudi::Matrix3x3 & mat ){
  m_axis1 = Gaudi::XYZVector( mat(0,0), mat(1,0), mat(2,0) );
  m_axis2 = Gaudi::XYZVector( mat(0,1), mat(1,1), mat(2,1) );
  m_axis3 = Gaudi::XYZVector( mat(0,2), mat(1,2), mat(2,2) );
}

void BeamLineInfo::SetXEigenVector( Gaudi::XYZVector & vec ){ m_axis1 = vec; }
void BeamLineInfo::SetYEigenVector( Gaudi::XYZVector & vec ){ m_axis2 = vec; }
void BeamLineInfo::SetZEigenVector( Gaudi::XYZVector & vec ){ m_axis3 = vec; }
void BeamLineInfo::SetZEigenVector( double v1, double v2, double v3 ){
  m_axis3 = Gaudi::XYZVector( v1, v2, v3 );
}
void BeamLineInfo::SetEigenValues( double & v1, double & v2, double & v3 ){
  m_sigma1 = v1; m_sigma2 = v2; m_sigma3 = v3; 
}
void BeamLineInfo::SetEigenValues( Gaudi::Vector3 & vec ){
  m_sigma1 = vec.At(0); m_sigma2 = vec.At(1); m_sigma3 = vec.At(2);
}
void BeamLineInfo::SetEigenValues( Gaudi::XYZPoint & vec ){
  m_sigma1 = vec.x(); m_sigma2 = vec.y(); m_sigma3 = vec.z();
}

//=============================================================================
//  From a XYZVector, get a Vector3, easier to handle
//=============================================================================
Gaudi::Vector3 BeamLineInfo::MkV( Gaudi::XYZVector & vec ){
  return Gaudi::Vector3( vec.x(), vec.y(), vec.z() );
}
Gaudi::XYZVector BeamLineInfo::MkV( Gaudi::Vector3 & vec ){
  return Gaudi::XYZVector( vec.At(0), vec.At(1), vec.At(2) );
}

//=============================================================================
//  Nice print out of the mean interaction point with errors
//=============================================================================
string BeamLineInfo::PrintIP(){
  stringstream s;
  s <<"( " << m_ipPosition.x() <<" +- "<< sqrt(abs(m_sigma1)/m_nbentries) 
    <<", "<< m_ipPosition.y() <<" +- "<< sqrt(abs(m_sigma2)/m_nbentries) 
    <<", "<< m_ipPosition.z() <<" +- "<< sqrt(abs(m_sigma3)/m_nbentries) 
    <<" )";
  return s.str();
}

//=============================================================================
//  Get parameters
//=============================================================================
int & BeamLineInfo::GetNbEntries(){ return m_nbentries; }
Gaudi::XYZPoint & BeamLineInfo::GetIPPosition(){ return m_ipPosition; }
Gaudi::XYZPoint BeamLineInfo::GetIPPositionErrors(){ 
  return Gaudi::XYZPoint( sqrt(m_sigma1/m_nbentries), 
                          sqrt(m_sigma2/m_nbentries), 
                          sqrt(m_sigma3/m_nbentries) ); 
}
Gaudi::XYZVector & BeamLineInfo::GetXEigenVector(){ return m_axis1; };
Gaudi::XYZVector & BeamLineInfo::GetYEigenVector(){ return m_axis2; };
Gaudi::XYZVector & BeamLineInfo::GetZEigenVector(){ return m_axis3; };
Gaudi::XYZPoint BeamLineInfo::GetEigenValues(){ 
  return Gaudi::XYZPoint( m_sigma1, m_sigma2, m_sigma3 );
  
}      
double & BeamLineInfo::GetXEigenValue(){ return m_sigma1; }
double & BeamLineInfo::GetYEigenValue(){ return m_sigma2; }
double & BeamLineInfo::GetZEigenValue(){ return m_sigma3; }

//=============================================================================
// Move to given frame
//=============================================================================
void BeamLineInfo::MoveIPToFrame( Gaudi::Transform3D & frame ){
  m_ipPosition = frame * m_ipPosition;
}
void BeamLineInfo::MoveZEVToFrame( Gaudi::Transform3D & frame ){
  m_axis3 = frame * m_axis3;
}
void BeamLineInfo::MoveToFrame( Gaudi::Transform3D & frame ){
  MoveIPToFrame( frame );
  MoveZEVToFrame( frame );
  m_axis1 = frame * m_axis1;
  m_axis2 = frame * m_axis2;
  Gaudi::XYZPoint dump = frame * GetEigenValues();
  SetEigenValues( dump );
}


//=============================================================================
//  Set X,Y,Z eigenvectors as being the vectors closest to the X,Y,Z axis, 
//     respectively, with positive eigenvalues.
//=============================================================================
void BeamLineInfo::SetOrder(){


  Gaudi::Vector3 ev[3] = { MkV(m_axis1), MkV(m_axis2), MkV(m_axis3) };
  double eval[3] = { m_sigma1, m_sigma2, m_sigma3 };
  
  
  // Eigenvectors that is most along x,y,z
  Gaudi::Vector3 tmp[3];
  double tmpd[3];

  for(size_t ivec=0; ivec<3; ++ivec) {
    // clumsy way of finding the largest coordinate
    size_t icoord = 0;
    for(size_t jcoord = 1; jcoord<3; ++jcoord)
      if( abs( ev[ivec].At(icoord) ) < abs( ev[ivec].At(jcoord) ) )
        icoord = jcoord;
    tmp[icoord] = ev[ivec];
    tmpd[icoord] = eval[ivec];
    // turn the vector around if the direction is wrong.
    if( tmp[icoord].At(icoord) < 0 ){ 
      tmp[icoord] *= -1;
      //change the sign of the eigenvalue as well.
      tmpd[icoord] *= -1;
    } 
  }
  m_axis1 = MkV(tmp[0]); m_axis2 = MkV(tmp[1]); m_axis3 = MkV(tmp[2]);
  m_sigma1 = tmpd[0]; m_sigma2 = tmpd[1]; m_sigma3 = tmpd[2];
}

//=============================================================================
// return true if the beam has moved radially by a factor > threshold
//=============================================================================
bool BeamLineInfo::HasMoved( Gaudi::XYZPoint & oldip, double & threshold ){

  if( oldip.z() < -1000. ) return false;
  
  double x = m_ipPosition.x() - oldip.x();
  double y = m_ipPosition.y() - oldip.y();
  if( std::sqrt( x*x + y*y ) > threshold ) return true;
  
  return false;
  
}

//=============================================================================
