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
    : m_scaleFactor(1),
      m_min_FL{ {0.,0.,0.} },
      m_Dxyz{ {0.,0.,0.} },
      m_Nxyz{ {0,0,0} }
  {
  }

  
  MagneticFieldGrid::FieldVector
  MagneticFieldGrid::fieldVectorClosestPoint( const Gaudi::XYZPoint& r ) const 
  {
    // round of till the nearest bin -- i,j,k are unsigned so we can do less
    // comparisons, i.e. we can skip the comparison with 0...
    unsigned i = int( (r.x() - m_min_FL[0])/m_Dxyz[0] + 0.5 ) ;
    unsigned j = int( (r.y() - m_min_FL[1])/m_Dxyz[1] + 0.5 ) ;
    unsigned k = int( (r.z() - m_min_FL[2])/m_Dxyz[2] + 0.5 ) ;
    // get the field
    FieldVector fvec(0,0,0) ;
    if( i < m_Nxyz[0] && j < m_Nxyz[1] && k < m_Nxyz[2] ) {
      const auto& Q = m_Q[m_Nxyz[0]*( m_Nxyz[1]*k + j ) + i] ;
      fvec.SetXYZ( scaleFactor() * Q.x(),
		           scaleFactor() * Q.y(),
		           scaleFactor() * Q.z() ) ;
    }
    return fvec ;
  }

  MagneticFieldGrid::FieldGradient
  MagneticFieldGrid::fieldGradient( const Gaudi::XYZPoint& r ) const 
  {
    FieldGradient rc ;

    double x = r.x() - m_min_FL[0] ; unsigned i = int( x/m_Dxyz[0] ) ;
    double y = r.y() - m_min_FL[1] ; unsigned j = int( y/m_Dxyz[1] ) ;
    double z = r.z() - m_min_FL[2] ; unsigned k = int( z/m_Dxyz[2] ) ;

    // true interpolation is too expensive, is it? let's start with this
    if( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 ) {
      auto ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );

      int dijk[3] ;
      dijk[0] = 1 ;
      dijk[1] = m_Nxyz[0] ;
      dijk[2] = m_Nxyz[0] * m_Nxyz[1] ;
      for(int col=0; col<3; ++col) {
        auto  dQdX = m_Q[ ijk000 + dijk[col] ] - m_Q[ ijk000 ] ;
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
    vec bf{0,0,0} ;

    // i,j,k are unsigned so we can skip the test whether 
    // they are positive...
    double x = r.x() - m_min_FL[0] ; unsigned i =  int(x/m_Dxyz[0]) ;
    double y = r.y() - m_min_FL[1] ; unsigned j =  int(y/m_Dxyz[1]) ;
    double z = r.z() - m_min_FL[2] ; unsigned k =  int(z/m_Dxyz[2]) ;
    
    if( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 ) {
      
      // auxiliary variables defined at the vertices of the cube that
      // contains the (x, y, z) point where the field is interpolated
      
      float hx1 = ( x-i*m_Dxyz[0] )/m_Dxyz[0]; auto hx0 = 1.0f-hx1;
      float hy1 = ( y-j*m_Dxyz[1] )/m_Dxyz[1]; auto hy0 = 1.0f-hy1;
      float hz1 = ( z-k*m_Dxyz[2] )/m_Dxyz[2]; auto hz0 = 1.0f-hz1;
      
      auto h000 = hx0*hy0*hz0; auto h100 = hx1*hy0*hz0;
      auto ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );
      bf  = h000*m_Q[ijk000]+h100*m_Q[ijk000+1] ;
      auto h001 = hx0*hy0*hz1; auto h101 = hx1*hy0*hz1;
      auto ijk001 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j )     + i );
      bf += h001*m_Q[ijk001]+h101*m_Q[ijk001+1] ;
      auto h010 = hx0*hy1*hz0; auto h110 = hx1*hy1*hz0;
      auto ijk010 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1 ) + i );
      bf += h010*m_Q[ijk010]+h110*m_Q[ijk010+1] ;
      auto h011 = hx0*hy1*hz1; auto h111 = hx1*hy1*hz1;
      auto ijk011 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1)  + i );
      bf += h011*m_Q[ijk011]+h111*m_Q[ijk011+1] ;

      bf *= float(scaleFactor());
      
    }
    return { bf.x(), bf.y(), bf.z() };
  }
}
