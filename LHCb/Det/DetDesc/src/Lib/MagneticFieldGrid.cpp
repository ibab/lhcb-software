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
      // m_min_FL{ {0.,0.,0.} },
      // m_Dxyz{ {0.,0.,0.} },
      // m_invDxyz{ {0.,0.,0.} },
      // m_Nxyz{ {0,0,0} },
      m_scaleFactor_V(1.,1.,1.,0.),
      m_min_FL_V ( 0.,0.,0.,0.),
      m_Dxyz_V   ( 0.,0.,0.,0.),
      m_invDxyz_V( 0.,0.,0.,0.),
      m_Nxyz_V   { {0,0,0} }
 { }

  MagneticFieldGrid::FieldVector
  MagneticFieldGrid::fieldVectorClosestPoint( const Gaudi::XYZPoint& r ) const
  {

    // // Original method
    // FieldVector fvec(0,0,0) ;
    // // round of till the nearest bin -- i,j,k are unsigned so we can do less
    // // comparisons, i.e. we can skip the comparison with 0...
    // unsigned i = int( (r.x() - m_min_FL[0])*m_invDxyz[0] + 0.5 ) ;
    // unsigned j = int( (r.y() - m_min_FL[1])*m_invDxyz[1] + 0.5 ) ;
    // unsigned k = int( (r.z() - m_min_FL[2])*m_invDxyz[2] + 0.5 ) ;
    // // get the field
    // if ( i < m_Nxyz[0] && j < m_Nxyz[1] && k < m_Nxyz[2] ) 
    // {
    //   const auto& Q = m_Q[m_Nxyz[0]*( m_Nxyz[1]*k + j ) + i] ;
    //   fvec.SetXYZ( scaleFactor() * Q.x(),
    //                scaleFactor() * Q.y(),
    //                scaleFactor() * Q.z() ) ;
    // }

    // Vectorised way
    
    const Vec4f _r( r.x(), r.y(), r.z(), 0.0 );
    
    const Vec4ui ijk ( truncate_to_int( ( ( _r -  m_min_FL_V ) * m_invDxyz_V ) + 0.5 ) );
    const auto _i(ijk[0]), _j(ijk[1]), _k(ijk[2]);
    
    // // Check indices
    // if ( i != _i || j != _j || k != _k )
    // {
    //   std::cout << "fieldVectorClosestPoint : ijk Problem"
    //             << " i= " << i << " " << _i
    //             << " j= " << j << " " << _j
    //             << " k= " << k << " " << _k
    //             << std::endl;
    // }
    
    // get the field
    const Vec4f fvec_V = 
      ( _i < m_Nxyz_V[0] && _j < m_Nxyz_V[1] &&  _k < m_Nxyz_V[2] ?
        m_scaleFactor_V * ( m_Q_V[m_Nxyz_V[0]*( m_Nxyz_V[1]*_k + _j ) + _i] ) : 
        Vec4f(0.,0.,0.,0.) );

    // // compare final results
    // if ( fabs( fvec_V[0] - fvec.x() ) > 1e-5 ||
    //      fabs( fvec_V[1] - fvec.y() ) > 1e-5 ||
    //      fabs( fvec_V[2] - fvec.z() ) > 1e-5 )
    // {
    //   std::cout << "fieldVectorClosestPoint : Old= " << fvec 
    //             << " New= (" << fvec_V[0] << "," << fvec_V[1] << "," << fvec_V[2] << ")" 
    //             << std::endl;
    // }
    
    return { fvec_V[0], fvec_V[1], fvec_V[2] };
  }

  MagneticFieldGrid::FieldGradient
  MagneticFieldGrid::fieldGradient( const Gaudi::XYZPoint& r ) const
  {

    // // Original method

    // FieldGradient rc ;

    // double x = r.x() - m_min_FL[0] ; unsigned i = int( x*m_invDxyz[0] ) ;
    // double y = r.y() - m_min_FL[1] ; unsigned j = int( y*m_invDxyz[1] ) ;
    // double z = r.z() - m_min_FL[2] ; unsigned k = int( z*m_invDxyz[2] ) ;

    // // true interpolation is too expensive, is it? let's start with this
    // if( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 ) 
    // {
    //   auto ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );

    //   int dijk[3] ;
    //   dijk[0] = 1 ;
    //   dijk[1] = m_Nxyz[0] ;
    //   dijk[2] = m_Nxyz[0] * m_Nxyz[1] ;
    //   for(int col=0; col<3; ++col) 
    //   {
    //     auto dQdX = m_Q[ ijk000 + dijk[col] ] - m_Q[ ijk000 ];
    //     rc(0,col) = scaleFactor() * dQdX.x() * m_invDxyz[col] ;
    //     rc(1,col) = scaleFactor() * dQdX.y() * m_invDxyz[col] ;
    //     rc(2,col) = scaleFactor() * dQdX.z() * m_invDxyz[col] ;
    //   }
    // }

    // Vectorised way
    FieldGradient _rc ;
 
    const Vec4f _r( r.x(), r.y(), r.z(), 0.0 );
    
    const Vec4ui ijk ( truncate_to_int( ( _r - m_min_FL_V ) * m_invDxyz_V ) );
    const auto _i(ijk[0]), _j(ijk[1]), _k(ijk[2]);
    
    if ( _i < (m_Nxyz_V[0]-1) && 
         _j < (m_Nxyz_V[1]-1) && 
         _k < (m_Nxyz_V[2]-1) ) 
    {
      const auto ijk000 = ( m_Nxyz_V[0]*( m_Nxyz_V[1]*_k + _j ) + _i );
      
      const Vec4i dijk( 1, m_Nxyz_V[0], m_Nxyz_V[0] * m_Nxyz_V[1], 0 );
      for ( int col = 0; col<3; ++col ) 
      {
        const auto dQdX = ( m_Q_V[ ijk000 + dijk[col] ] - m_Q_V[ ijk000 ] ) * m_scaleFactor_V;
        _rc(0,col) = dQdX[0] * m_invDxyz_V[col] ;
        _rc(1,col) = dQdX[1] * m_invDxyz_V[col] ;
        _rc(2,col) = dQdX[2] * m_invDxyz_V[col] ;
      }
    }
    
    // // compare
    // if ( fabs( rc[0][0] - _rc[0][0] ) > 1e-5 ||
    //      fabs( rc[0][1] - _rc[0][1] ) > 1e-5 ||
    //      fabs( rc[0][2] - _rc[0][2] ) > 1e-5 ||
    //      fabs( rc[1][0] - _rc[1][0] ) > 1e-5 ||
    //      fabs( rc[1][1] - _rc[1][1] ) > 1e-5 ||
    //      fabs( rc[1][2] - _rc[1][2] ) > 1e-5 ||
    //      fabs( rc[2][0] - _rc[2][0] ) > 1e-5 ||
    //      fabs( rc[2][1] - _rc[2][1] ) > 1e-5 ||
    //      fabs( rc[2][2] - _rc[2][2] ) > 1e-5 )
    // {
    //   std::cout << "fieldGradient Return Problem :" << std::endl
    //             << " i= " << i << " " << _i
    //             << " j= " << j << " " << _j
    //             << " k= " << k << " " << _k
    //             << std::endl
    //             << " Old=" << std::endl << rc << std::endl 
    //             << " New=" << std::endl << _rc << std::endl;
    // }
 
    return _rc ;
  }

  MagneticFieldGrid::FieldVector
  MagneticFieldGrid::fieldVector( const Gaudi::XYZPoint& r ) const
  {

    // // Force test the other two methods ....
    // fieldVectorClosestPoint(r);
    // fieldGradient(r);

    // // Original version

    // //  Interpolate the field on a cube
    // vec bf{0,0,0} ;

    // // i,j,k are unsigned so we can skip the test whether
    // // they are positive...
    // double x = r.x() - m_min_FL[0] ; unsigned i = int(x*m_invDxyz[0]) ;
    // double y = r.y() - m_min_FL[1] ; unsigned j = int(y*m_invDxyz[1]) ;
    // double z = r.z() - m_min_FL[2] ; unsigned k = int(z*m_invDxyz[2]) ;

    // if ( i < m_Nxyz[0]-1 && j < m_Nxyz[1]-1 && k < m_Nxyz[2]-1 )
    // {
    //   // auxiliary variables defined at the vertices of the cube that
    //   // contains the (x, y, z) point where the field is interpolated

    //   float hx1 = ( (x*m_invDxyz[0]) - i ); auto hx0 = 1.0f-hx1;
    //   float hy1 = ( (y*m_invDxyz[1]) - j ); auto hy0 = 1.0f-hy1;
    //   float hz1 = ( (z*m_invDxyz[2]) - k ); auto hz0 = 1.0f-hz1;

    //   auto h000 = hx0*hy0*hz0; auto h100 = hx1*hy0*hz0;
    //   auto ijk000 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );
    //   bf  = h000*m_Q[ijk000]+h100*m_Q[ijk000+1] ;

    //   auto h001 = hx0*hy0*hz1; auto h101 = hx1*hy0*hz1;
    //   auto ijk001 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j )     + i );
    //   bf += h001*m_Q[ijk001]+h101*m_Q[ijk001+1] ;

    //   auto h010 = hx0*hy1*hz0; auto h110 = hx1*hy1*hz0;
    //   auto ijk010 = ( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1 ) + i );
    //   bf += h010*m_Q[ijk010]+h110*m_Q[ijk010+1] ;

    //   auto h011 = hx0*hy1*hz1; auto h111 = hx1*hy1*hz1;
    //   auto ijk011 = ( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1)  + i );
    //   bf += h011*m_Q[ijk011]+h111*m_Q[ijk011+1] ;

    //   bf *= float(scaleFactor());
    // }

    // Vectorised way

    Vec4f bf_V(0.,0.,0.,0.);

    const Vec4f _r( r.x(), r.y(), r.z(), 0.0 );
    
    const auto abc = ( _r - m_min_FL_V ) * m_invDxyz_V;
    const Vec4ui ijk ( truncate_to_int( abc ) );
    const auto _i(ijk[0]), _j(ijk[1]), _k(ijk[2]);

    // // compare indices
    // if ( i != _i || j != _j || k != _k )
    // {
    //   std::cout << "fieldVector : ijk Problem"
    //             << " i= " << i << " " << _i
    //             << " j= " << j << " " << _j
    //             << " k= " << k << " " << _k
    //               << std::endl;
    // }
    
    if ( _i < m_Nxyz_V[0]-1 && 
         _j < m_Nxyz_V[1]-1 && 
         _k < m_Nxyz_V[2]-1 )
    {
      // auxiliary variables defined at the vertices of the cube that
      // contains the (x, y, z) point where the field is interpolated

      const auto ijk000 = ( m_Nxyz_V[0]*( m_Nxyz_V[1]*_k     + _j )     + _i );
      const auto ijk001 = ( m_Nxyz_V[0]*( m_Nxyz_V[1]*(_k+1) + _j )     + _i );
      const auto ijk010 = ( m_Nxyz_V[0]*( m_Nxyz_V[1]*_k     + _j + 1 ) + _i );
      const auto ijk011 = ( m_Nxyz_V[0]*( m_Nxyz_V[1]*(_k+1) + _j + 1 ) + _i );

      const auto h1 = abc - Vec4f( _i, _j, _k, 0.0 );
      const auto h0 = 1.0f - h1;

      const auto h1x(h1[0]), h1y(h1[1]), h1z(h1[2]);
      const auto h0x(h0[0]), h0y(h0[1]), h0z(h0[2]);
 
      const auto h000 = h0x*h0y*h0z; 
      const auto h001 = h0x*h0y*h1z;
      const auto h010 = h0x*h1y*h0z;
      const auto h011 = h0x*h1y*h1z;

      const auto h100 = h1x*h0y*h0z;
      const auto h101 = h1x*h0y*h1z;
      const auto h110 = h1x*h1y*h0z;
      const auto h111 = h1x*h1y*h1z;

      bf_V = ( ( h000*m_Q_V[ijk000] + h100*m_Q_V[ijk000+1] ) +
               ( h001*m_Q_V[ijk001] + h101*m_Q_V[ijk001+1] ) +
               ( h010*m_Q_V[ijk010] + h110*m_Q_V[ijk010+1] ) +
               ( h011*m_Q_V[ijk011] + h111*m_Q_V[ijk011+1] ) ) * m_scaleFactor_V;

      // Try a few alternatives to see which is faster...

      // const Vec4f a(h0y,h0y,h1y,h1y);
      // const Vec4f b(h0z,h1z,h0z,h1z);
      // const auto H = Vec8f(Vec4f(h0x),Vec4f(h1x)) * Vec8f(a,a) * Vec8f(b,b);

      // bf_V = ( ( H[0]*m_Q_V[ijk000] + H[4]*m_Q_V[ijk000+1] ) +
      //          ( H[1]*m_Q_V[ijk001] + H[5]*m_Q_V[ijk001+1] ) +
      //          ( H[2]*m_Q_V[ijk010] + H[6]*m_Q_V[ijk010+1] ) +
      //          ( H[3]*m_Q_V[ijk011] + H[7]*m_Q_V[ijk011+1] ) ) * m_scaleFactor_V;

      // const auto A = Vec4f(h0y,h0y,h1y,h1y) * Vec4f(h0z,h1z,h0z,h1z);
      // const auto H = h0x * A;
      // const auto I = h1x * A;

      // bf_V = ( ( H[0]*m_Q_V[ijk000] + I[0]*m_Q_V[ijk000+1] ) +
      //          ( H[1]*m_Q_V[ijk001] + I[1]*m_Q_V[ijk001+1] ) +
      //          ( H[2]*m_Q_V[ijk010] + I[2]*m_Q_V[ijk010+1] ) +
      //          ( H[3]*m_Q_V[ijk011] + I[3]*m_Q_V[ijk011+1] ) ) * m_scaleFactor_V;

    }
    
    // // compare final vectors
    // if ( fabs( bf_V[0] - bf.x() ) > 1e-5 ||
    //      fabs( bf_V[1] - bf.y() ) > 1e-5 ||
    //      fabs( bf_V[2] - bf.z() ) > 1e-5 )
    // {
    //   std::cout << "fieldVector : Old= " << bf 
    //               << " New= (" << bf_V[0] << "," << bf_V[1] << "," << bf_V[2] << ")" 
    //             << std::endl;
    // }
  
    return { bf_V[0], bf_V[1], bf_V[2] };
  }

}
