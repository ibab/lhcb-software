#ifndef     __DETDESC_SOLID_SOLIDTICKS_H__
#define     __DETDESC_SOLID_SOLIDTICKS_H__ 1 


#include <cmath>
#include <algorithm>

#include "GaudiKernel/ISolid.h" 

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

//
// SolidTicks - a collection of technical methods for manipulation of  
//               ISolid::Tick ans ISolid::Ticks. Also some additional 
//               mathematical utilities are supplied.
//
// Author:      Vanya Belyaev  
// Date:        10.02.2000 
//
//

namespace SolidTicks
{

  ///
  ///
  /// Sort Ticks, eliminate duplicates and remove Adjancent Ticks 
  ///
  inline unsigned int  RemoveAdjancentTicks( ISolid::Ticks    & ticks , // tick is NOT assumed to be sorted !!!
                                             const HepPoint3D & point , 
                                             const HepVector3D & vect  , 
                                             const ISolid     & solid )
    {
      /// (1) sort container  
      std::sort( ticks.begin() , ticks.end() ) ; 
      /// (2) eliminate duplicates and (3) shrink container 
      ticks.erase( std::unique( ticks.begin() , ticks.end() )  , ticks.end() ); 
      { /// no adjancent ticks! 
        if     ( ticks.size() <  2 )    { ticks.clear() ; return 0 ; }         /// RETURN !!!
        else if( ticks.size() == 2 ) 
          {
            ISolid::Tick tick1 = *( ticks.begin ()  ) ; 
            ISolid::Tick tick2 = *( ticks.rbegin()  ) ;
            ISolid::Tick tick  = 0.5 * ( tick1 + tick2 ) ;  
            if( solid.isInside( point + vect * tick ) ) { return 2 ; }    /// RETURN 
            else                        { ticks.clear() ; return 0 ; }    /// RETURN 
          }  
        /// perform removing of adjancent  ticks
        std::vector<unsigned int> tmp ; 
        ISolid::Tick tickNext = 0.0   ;
        ISolid::Tick tickPrev = 0.0   ; 
        bool         boolPrev = true  ; 
        bool         boolNext = true  ; 
        for ( ISolid::Ticks::iterator it = ticks.begin() ; it != ticks.end() ; ++it ) 
          {
            // the last point is to be treated in a specific way
            if     ( ticks.end   () != it + 1 ) 
              { tickNext = 0.5 * ( (*it) + *(it+1) ) ; boolNext = solid.isInside( point + vect * tickNext );  }  
            //
            /// get the index 
            unsigned int index = it - ticks.begin();                       
            /// to write the last  tick it is enought to have the previous interval "inside" 
            if      ( ticks.end  () == it + 1 ) { if( !boolPrev             ) { tmp.push_back( index ) ; } }
            /// to write the first tick it is enought to have the first    interval "inside" 
            else if ( ticks.begin() == it     ) { if( !boolNext             ) { tmp.push_back( index ) ; } }
            /// to write the "regular" tick, it should separate 2 different zones! 
            else                                { if(  boolPrev == boolNext ) { tmp.push_back( index ) ; } }
            ///
            boolPrev = boolNext; 
            tickPrev = tickNext;
          }
        ///
        std::vector<unsigned int>::reverse_iterator cri = tmp.rbegin();
        while( cri != tmp.rend() ) { ticks.erase( ticks.begin() + *cri++ );  }
        ///
      }
      ///
      return ticks.size();
    }
  
  ///
  ///
  /// Sort Ticks, eliminate duplicates and remove Adjancent Ticks 
  ///
  inline unsigned int  RemoveAdjancentTicks( ISolid::Ticks      & ticks   , /// tick is assumed to be sorted !!!
                                             const HepPoint3D   & point   , 
                                             const HepVector3D  & vect    , 
                                             const ISolid::Tick & tickMin , 
                                             const ISolid::Tick & tickMax , 
                                             const ISolid       & solid   )
    {
      ///
      if( tickMin >= tickMax ) { ticks.clear(); return 0 ; }                    /// RETURN !!!
      /// sort, esliminate duplicates and shrink 
      /// (1) sort container  
      ///      std::sort( ticks.begin() , ticks.end() ) ; 
      /// (2) eliminate duplicates and (3) shrink container 
      ///   ticks.erase( std::unique( ticks.begin() , ticks.end() )  , ticks.end() ); 
      ///
      { /// check for tickMin and tickMax 
        /// remove extra ticks from container
        { /// find the first element greater or equal than tickMin
          ISolid::Ticks::iterator iter = 
            std::find_if( ticks.begin() , ticks.end() , 
                          std::bind2nd( std::greater_equal<const ISolid::Tick>() , tickMin ) );
          /// Element found! shrink container!
          ticks.erase( ticks.begin() , iter ); 
          if( !ticks.empty() ) { ticks.insert( ticks.begin() , tickMin ); } 
        }
        ///
        if( ticks.size() < 2 )                      { ticks.clear() ; return 0;  } /// RETURN !!!
        ///
        { /// search for elements which are greater or equal than tickMax 
          ISolid::Ticks::iterator iter = 
            std::find_if( ticks.begin() , ticks.end() , 
                          std::bind2nd( std::greater_equal<const ISolid::Tick>() , tickMax ) );
          /// found element wich is greater !  remove it (and larger), put tickMax into container!                 
          ticks.erase( iter , ticks.end() ); 
          if( !ticks.empty() ) { ticks.push_back( tickMax ); } 
        }
      }
      { /// no adjancent ticks! 
        if     ( ticks.size() <  2 )    { ticks.clear() ; return 0 ; }         /// RETURN !!!
        else if( ticks.size() == 2 ) 
          {
            ISolid::Tick tick1 = *( ticks.begin ()  ) ; 
            ISolid::Tick tick2 = *( ticks.rbegin()  ) ;
            ISolid::Tick tick  = 0.5 * ( tick1 + tick2 ) ;  
            if( solid.isInside( point + vect * tick ) ) { return 2 ; }    /// RETURN 
            else                        { ticks.clear() ; return 0 ; }    /// RETURN 
          }  
        /// perform removing of adjancent  ticks
        std::vector<unsigned int> tmp ; 
        ISolid::Tick tickNext = 0.0   ;
        ISolid::Tick tickPrev = 0.0   ; 
        bool         boolPrev = true  ; 
        bool         boolNext = true  ; 
        for ( ISolid::Ticks::iterator it = ticks.begin() ; it != ticks.end() ; ++it ) 
          {
            // the last point is to be treated in a specific way
            if( ticks.end  () != it + 1 ) 
              { tickNext = 0.5 * ( (*it) + *(it+1) ) ; boolNext = solid.isInside( point + vect * tickNext );  }  
            /// get the index 
            unsigned int index = it - ticks.begin();           
            /// to write the last  tick it is enought to have the previous interval "inside" 
            if      ( ticks.end  () == it + 1 ) { if( !boolPrev             ) { tmp.push_back( index ) ; } }
            /// to write the first tick it is enought to have the first    interval "inside" 
            else if ( ticks.begin() == it     ) { if( !boolNext             ) { tmp.push_back( index ) ; } }
            /// to write the "regular" tick, it should separate 2 different zones! 
            else                                { if ( boolPrev == boolNext ) { tmp.push_back( index ) ; } }
            ///
            boolPrev = boolNext; 
            tickPrev = tickNext;
          }
        ///
        std::vector<unsigned int>::reverse_iterator cri = tmp.rbegin();
        while( cri != tmp.rend() ) { ticks.erase( ticks.begin() + *cri++ );  }
        ///
      }
      ///
      return ticks.size();
      ///
    }
  
  
  ///
  ///
  /// solving the quadratic equation:  a*x*x + b*x + c = 0; 
  template < class OUTPUTTYPE > 
    inline unsigned int SolveQuadraticEquation( const double a   , 
                                                const double b   , 
                                                const double c   ,
                                                OUTPUTTYPE   out ) 
    {
      if( 0 == a )                       /// it is a linear equation:  b*x + c = 0 
        {
          // no solution!
          if( b == 0 ) { return 0 ; }   // RETURN !!! 
          // 1 solution!
          *out++ = -1.0 * c / b ; 
          *out++ = -1.0 * c / b ;       // double the solutions 
          return 1;                     // RETURN !!!   
        }
      double d = b * b - 4.0 * a * c ; 
      /// no solutions 
      if(  d < 0  )   { return 0; }     // RETURN !!!
      // 1 or 2 solution
      d = sqrt( d )                  ;   
      *out++ = 0.5 * ( -b - d ) / a  ; 
      *out++ = 0.5 * ( -b + d ) / a  ; 
      // return number of solutions;
      return 0 == d ? 1 : 2;            // RETURN !!! 
    };
  
  
  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with sphere of radius Radius
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheSphere( const HepPoint3D  & point  , 
                                                  const HepVector3D & vect   , 
                                                  const double        radius ,
                                                  OUTPUTTYPE          out    )
    {
      ///
      /// sphere with non-positive radius is not able to intersect the line! 
      if( radius <= 0 ) { return 0 ; } 
      /// line with null direction vector is not to intersect the sphere! 
      double v2 = vect.mag2(); 
      if( v2 <= 0     ) { return 0 ; }
      double p2 = point.mag2()    ; 
      double pv = point.dot(vect) ; 
      /// It is equivalent to the equation
      /// ( Point + Vector * Tick )^2 = R^2
      /// it is quadratic equation!  a*x^2+b*x+c=0
      double a = v2                   ; 
      double b = 2.0*pv               ; 
      double c = p2 - radius * radius ;
      // solve the equation! 
      return SolidTicks::SolveQuadraticEquation( a , b , c , out );
    }
  
  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with cylinder of radius Radius
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheCylinder( const HepPoint3D  & point  , 
                                                    const HepVector3D & vect   , 
                                                    const double        radius ,
                                                    OUTPUTTYPE          out    )
    
    {
      ///
      /// Cylinder with non-positive radius is not able to intersect the line! 
      if( radius <= 0 ) { return 0 ; } 
      
      /// line with null direction vector is not able to intersect the cylinder! 
      double v2 = vect.x()*vect.x() + vect.y()*vect.y() ; 
      if( v2 <= 0     ) { return 0 ; }
      
      double p2 = point.x() * point.x() + point.y() * point.y() ; 
      double pv = point.x() * vect.x()  + point.y() * vect.y()  ; 
      
      /// It is equivalent to the equation
      /// ( Point + Vector * Tick )^2 = R^2
      /// it is quadratic equation!  a*x^2+b*x+c=0
      double a = v2                   ; 
      double b = 2.0*pv               ; 
      double c = p2 - radius * radius ;
      // solve the equation! 
      return SolidTicks::SolveQuadraticEquation( a , b , c , out );

    }
  
  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with x-plane x=X
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheX( const HepPoint3D  & point  , 
                                             const HepVector3D & vect   , 
                                             const double        X      ,
                                             OUTPUTTYPE          out   )
    
    {
      
      /// line with null vector in X-direction is not able to intersect the x-plane! 
      if( 0 == vect.x() ) { return 0; } 
      *out++ = ( X - point.x() ) / vect.x() ; 
      return 1;      
    }
  

  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with y-plane y=Y
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheY( const HepPoint3D  & point  , 
                                             const HepVector3D & vect   , 
                                             const double        Y      ,
                                             OUTPUTTYPE          out    )
    
    {
      
      /// line with null vector in Y-direction is not able to intersect the y-plane! 
      if( 0 == vect.y() ) { return 0; } 
      *out++ = ( Y - point.y() ) / vect.y() ; 
      return 1;      
    }

  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with z-plane z=Z
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheZ( const HepPoint3D & point  , 
                                             const HepVector3D& vect   , 
                                             const double       Z      ,
                                             OUTPUTTYPE         out    )
    
    {
      
      /// line with null vector in Z-direction is not able to intersect the z-plane! 
      if( 0 == vect.z() ) { return 0; } 
      *out++ = ( Z - point.z() ) / vect.z() ; 
      return 1;      
    }

  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with half-plane phi=Phi
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsThePhi( const HepPoint3D  & point  , 
                                               const HepVector3D & vect   , 
                                               const double        Phi    ,
                                               OUTPUTTYPE          out    )
    
    {
      double sinphi = sin( Phi ) ; 
      double cosphi = cos( Phi ) ; 
      double d      = vect.x() * sinphi - vect.y() * cosphi ; 
      if( 0 == d ) { return 0; } 
      *out++ = ( point.y() * cosphi - point.x() * sinphi ) / d ; 
      return 1; 
    }

  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with cone theta=Theta
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheTheta( const HepPoint3D & point  , 
                                                 const HepPoint3D & vect   , 
                                                 const double       Theta  ,
                                                 OUTPUTTYPE         out    )
    
    {
      ///
      /// it is equivalent to solve the equation
      ///  cos^2(x^2+y^2)=sin^2*z^2
      
      double sinthe = sin( Theta )    ; 
      double costhe = cos( Theta )    ;
      
      double c2     = costhe * costhe ; 
      double s2     = sinthe * sinthe ; 
      
      double a = c2 *  vect.z() *  vect.x() + c2 *  vect.z() *  vect.y() - s2 *  vect.z() *  vect.z() ; 
      double b = c2 *  vect.x() * point.x() + c2 *  vect.y() * point.y() - s2 *  vect.z() * point.z() ;
      double c = c2 * point.x() * point.x() + c2 * point.y() * point.y() - s2 * point.z() * point.z() ;
      b *= 2.0; 
      
      // return result
      return SolidTicks::SolveQuadraticEquation( a , b, c, out );
    }

  ///
  ///
  /// find intersection ticks for the line parametrized as Point + Vector * Tick 
  /// with conical surface 
  template < class OUTPUTTYPE > 
    inline unsigned int  LineIntersectsTheCone( const HepPoint3D  & point  , 
                                                const HepVector3D & vect   , 
                                                const double        r1     ,
                                                const double        r2     ,
                                                const double        z1     ,
                                                const double        z2     ,
                                                OUTPUTTYPE          out    )
    
    {
      // it is equivalent to the equation
      // (x^2+y^2)=(r(z))^2
      // 
      // r(z) = r1 + (z-z1)*(r2-r1)/(z2-z1) = 
      //      = r1 + (z-z1)*(drdz)  = 
      //      = r1 - z1*drdz + drdz*(pz+vz*t) = 
      //      = r1 - z1*drdz + drdz*pz + drdz*vz*t = 
      //      = p1 + p2*t ; 
      double drdz = (r2-r1)/(z2-z1)               ;
      double p1   = r1 - z1*drdz + drdz*point.z() ;
      double p2   = drdz * point.z()              ;
      
      double a = vect.x () * vect.x () + vect.y () * vect.y () ;
      a -= p2*p2 ; 
      double b = vect.x () * point.x() + vect.y () * point.y() ;
      b -= p2*p1 ;
      b *= 2.0   ; 
      double c = point.x() * point.x() + point.y() * point.y() ; 
      c -= p1*p1 ;
      
      // return result
      return SolidTicks::SolveQuadraticEquation( a , b, c, out );
    }

}; // end of namespace SolidTicks

///
///
///

#endif   //   __DETDESC_SOLID_SOLIDTICKS_H__






