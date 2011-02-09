// $Id: MagneticFieldGrid.cpp,v 1.1 2009-09-01 14:50:30 wouter Exp $
// Include files 

// local
#include "DetDesc/MagneticFieldGrid.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MagneticFieldGrid
//
// 2008-07-26 : Marco Cattaneo
// 2009-06-05 : Wouter Hulsbergen
//-----------------------------------------------------------------------------

//=============================================================================
// Return the field vector fvec at the point xyz
//=============================================================================

namespace LHCb 
{
  
  MagneticFieldGrid::MagneticFieldGrid()
    : m_scaleFactor(1) 
  {
    m_Dxyz[0] = 0.0;
    m_Dxyz[1] = 0.0;
    m_Dxyz[2] = 0.0;
    m_Nxyz[0] = 0;
    m_Nxyz[1] = 0;
    m_Nxyz[2] = 0;
    m_min_FL[0] = 0.0;
    m_min_FL[1] = 0.0;
    m_min_FL[2] = 0.0;
  }

  
  MagneticFieldGrid::FieldVector
  MagneticFieldGrid::fieldVectorClosestPoint( const Gaudi::XYZPoint& r ) const 
  {
    // round of till the nearest bin
    size_t i = int( (r.x() - m_min_FL[0])/m_Dxyz[0] + 0.5 ) ;
    size_t j = int( (r.y() - m_min_FL[1])/m_Dxyz[1] + 0.5 ) ;
    size_t k = int( (r.z() - m_min_FL[2])/m_Dxyz[2] + 0.5 ) ;
    // get the field
    FieldVector fvec(0,0,0) ;
    if( i < m_Nxyz[0] && j < m_Nxyz[1] && k < m_Nxyz[2] ) {
      const FieldVector& thisQ = m_Q[m_Nxyz[0]*( m_Nxyz[1]*k + j ) + i] ;
      fvec.SetXYZ( scaleFactor() * thisQ.x(),
		   scaleFactor() * thisQ.y(),
		   scaleFactor() * thisQ.z() ) ;
    }
    return fvec ;
  }

  MagneticFieldGrid::FieldGradient
  MagneticFieldGrid::fieldGradient( const Gaudi::XYZPoint& r ) const 
  {
    FieldGradient rc ;

    double x = r.x() - m_min_FL[0] ;  
    double y = r.y() - m_min_FL[1] ;
    double z = r.z() - m_min_FL[2] ;
    size_t i = int( x/m_Dxyz[0] ) ;
    size_t j = int( y/m_Dxyz[1] ) ;
    size_t k = int( z/m_Dxyz[2] ) ;

    // true interpolation is too expensive, is it? let's start with this
    if( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 ) {
      int ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );

      int dijk[3] ;
      dijk[0] = 1 ;
      dijk[1] = m_Nxyz[0] ;
      dijk[2] = m_Nxyz[0] * m_Nxyz[1] ;
      for(int col=0; col<3; ++col) {
	FieldVector dQdX = m_Q[ ijk000 + dijk[col] ] - m_Q[ ijk000 ] ;
	rc(0,col) = scaleFactor() * dQdX.x() / m_Dxyz[col] ;
	rc(1,col) = scaleFactor() * dQdX.y() / m_Dxyz[col] ;
	rc(2,col) = scaleFactor() * dQdX.z() / m_Dxyz[col] ;
      }
    }
    return  rc ;
  }

  MagneticFieldGrid::FieldVector
  MagneticFieldGrid::fieldVector( const Gaudi::XYZPoint& r ) const 
  {
    //  Interpolate the field on a cube
    FieldVector bf(0,0,0) ;

    double x = r.x() - m_min_FL[0] ;  
    double y = r.y() - m_min_FL[1] ;
    double z = r.z() - m_min_FL[2] ;
    size_t i = int( x/m_Dxyz[0] ) ;
    size_t j = int( y/m_Dxyz[1] ) ;
    size_t k = int( z/m_Dxyz[2] ) ;
    
    if( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 ) {
      
      int ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );
      int ijk001 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j )     + i );
      int ijk010 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1 ) + i );
      int ijk011 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1)  + i );
      int ijk100 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j)      + i + 1 );
      int ijk101 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j)      + i + 1 );
      int ijk110 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1)  + i + 1 );
      int ijk111 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1 ) + i + 1 );
      
      // auxiliary variables defined at the vertices of the cube that
      // contains the (x, y, z) point where the field is interpolated
      double cx000,cx001,cx010,cx011,cx100,cx101,cx110,cx111,
	cy000,cy001,cy010,cy011,cy100,cy101,cy110,cy111,
	cz000,cz001,cz010,cz011,cz100,cz101,cz110,cz111;
      
      cx000 = (m_Q)[ ijk000 ].x();
      cy000 = (m_Q)[ ijk000 ].y();
      cz000 = (m_Q)[ ijk000 ].z();
      cx001 = (m_Q)[ ijk001 ].x();
      cy001 = (m_Q)[ ijk001 ].y();
      cz001 = (m_Q)[ ijk001 ].z();
      cx010 = (m_Q)[ ijk010 ].x();
      cy010 = (m_Q)[ ijk010 ].y();
      cz010 = (m_Q)[ ijk010 ].z();
      cx011 = (m_Q)[ ijk011 ].x();
      cy011 = (m_Q)[ ijk011 ].y();
      cz011 = (m_Q)[ ijk011 ].z();
      cx100 = (m_Q)[ ijk100 ].x();
      cy100 = (m_Q)[ ijk100 ].y();
      cz100 = (m_Q)[ ijk100 ].z();
      cx101 = (m_Q)[ ijk101 ].x();
      cy101 = (m_Q)[ ijk101 ].y();
      cz101 = (m_Q)[ ijk101 ].z();
      cx110 = (m_Q)[ ijk110 ].x();
      cy110 = (m_Q)[ ijk110 ].y();
      cz110 = (m_Q)[ ijk110 ].z();
      cx111 = (m_Q)[ ijk111 ].x();
      cy111 = (m_Q)[ ijk111 ].y();
      cz111 = (m_Q)[ ijk111 ].z();
      
      double hx1 = ( x-i*m_Dxyz[0] )/m_Dxyz[0];
      double hy1 = ( y-j*m_Dxyz[1] )/m_Dxyz[1];
      double hz1 = ( z-k*m_Dxyz[2] )/m_Dxyz[2];
      double hx0 = 1.0-hx1;
      double hy0 = 1.0-hy1;
      double hz0 = 1.0-hz1;
      
      double h000 = hx0*hy0*hz0;
      double h001 = hx0*hy0*hz1;
      double h010 = hx0*hy1*hz0;
      double h011 = hx0*hy1*hz1;
      double h100 = hx1*hy0*hz0;
      double h101 = hx1*hy0*hz1;
      double h110 = hx1*hy1*hz0;
      double h111 = hx1*hy1*hz1;
      
      bf.SetX ( cx000*h000 + cx001*h001 + cx010*h010 + cx011*h011 +
		cx100*h100 + cx101*h101 + cx110*h110 + cx111*h111) ;
      bf.SetY ( cy000*h000 + cy001*h001 + cy010*h010 + cy011*h011 +
		cy100*h100 + cy101*h101 + cy110*h110 + cy111*h111 );
      bf.SetZ ( cz000*h000 + cz001*h001 + cz010*h010 + cz011*h011 +
		cz100*h100 + cz101*h101 + cz110*h110 + cz111*h111 );
      bf *= scaleFactor() ;
    }

    return bf ;
  }

}
