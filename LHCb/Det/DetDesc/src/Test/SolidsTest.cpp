// $Id: SolidsTest.cpp,v 1.1 2002-05-13 18:29:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// DetDesc 
#include "DetDesc/Solids.h"
#include "DetDesc/DetDesc.h"

/** @file 
 *  Simple test application to perform primmitive test of properties 
 *  of some  solids
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 13 May 2002 
 */

int main()
{  

  const std::string stars( 80 , '*' );

  std::cout 
    << stars                             << std::endl 
    << " Test of some primitive solids " << std::endl 
    << stars                             << std::endl ;
  

  typedef ISolid::Tick          Tick     ;
  typedef ISolid::Ticks         Ticks    ;
  typedef Ticks::const_iterator iterator ;
  
  {
    ISolid* solid = new SolidBox("Box" , 1 * m , 1.5 * m , 2 * m );
    
    std::cout 
      << stars                             << std::endl 
      << " SolidBox "                      << std::endl 
      << stars                             << std::endl ;
    
    std::cout << solid << std::endl;    
    Ticks ticks;
    const HepVector3D vect ( 0 , 0 , 1 );
    for( int i = 0 ; i < 20 ; ++i ) 
      {
        const double x = -2 * m + i * ( 4 * m / 20  ) ;
        const HepPoint3D point( x , 0.0 , -10.0 * m ) ;
        const unsigned int n = 
          solid->intersectionTicks( point , vect  , ticks );
        std::cout << " point " << point << " #ticks=" << n << " (";
        for( iterator tick = ticks.begin() ; ticks.end() != tick ; ++tick )
          { std::cout << DetDesc::print( *tick ) << ","; } 
        std::cout << ") " << std::endl;
      }
    
  };

  {
    ISolid* solid = new SolidTrd("Trd"   , 2.0 * m , 
                                 0.5 * m , 1.0 * m , 
                                 1.0 * m , 1.5 * m );
    
    std::cout 
      << stars                             << std::endl 
      << " SolidTrd "                      << std::endl 
      << stars                             << std::endl ;

    std::cout << solid << std::endl;    
    Ticks ticks;
    const HepVector3D vect ( 0 , 0 , 1 );
    for( int i = 0 ; i < 20 ; ++i ) 
      {
        const double x = -2 * m + i * ( 4 * m / 20  ) ;
        const HepPoint3D point( x , 0.0 , -10.0 * m ) ;
        const unsigned int n = 
          solid->intersectionTicks( point , vect  , ticks );
        std::cout << " point " << point << " #ticks=" << n << " (";
        for( iterator tick = ticks.begin() ; ticks.end() != tick ; ++tick )
          { std::cout << DetDesc::print( *tick ) << ","; } 
        std::cout << ") " << std::endl;
      }
    
    
  };

  {
    ISolid* solid = new SolidSphere("Sphere"   , 1.0 * m );
    
    std::cout 
      << stars                             << std::endl 
      << " SolidSphere "                   << std::endl 
      << stars                             << std::endl ;
    
    std::cout << solid << std::endl;    
    Ticks ticks;
    const HepVector3D vect ( 0 , 0 , 1 );
    for( int i = 0 ; i < 20 ; ++i ) 
      {
        const double x = -2 * m + i * ( 4 * m / 20  ) ;
        const HepPoint3D point( x , 0.0 , -10.0 * m ) ;
        const unsigned int n = 
          solid->intersectionTicks( point , vect  , ticks );
        std::cout << " point " << point << " #ticks=" << n << " (";
        for( iterator tick = ticks.begin() ; ticks.end() != tick ; ++tick )
          { std::cout << DetDesc::print( *tick ) << ","; } 
        std::cout << ") " << std::endl;
      }
    
  };

  {
    ISolid* solid = new SolidTubs("Tubs"  , 
                                  1.0 * m , 
                                  0.9 * m , 
                                  0.5 * m );
    
    std::cout 
      << stars                             << std::endl 
      << " SolidTubs "                     << std::endl 
      << stars                             << std::endl ;
    
    std::cout << solid << std::endl;    
    Ticks ticks;
    const HepVector3D vect ( 0 , 0 , 1 );
    for( int i = 0 ; i < 20 ; ++i ) 
      {
        const double x = -2 * m + i * ( 4 * m / 20  ) ;
        const HepPoint3D point( x , 0.0 , -10.0 * m ) ;
        const unsigned int n = 
          solid->intersectionTicks( point , vect  , ticks );
        std::cout << " point " << point << " #ticks=" << n << " (";
        for( iterator tick = ticks.begin() ; ticks.end() != tick ; ++tick )
          { std::cout << DetDesc::print( *tick ) << ","; } 
        std::cout << ") " << std::endl;
      }
    
  };

  {
    ISolid* solid = new SolidCons("Cons"  , 
                                  1.0 * m , 
                                  2.0 * m , 
                                  1.0 * m , 
                                  0.5 * m ,
                                  0.5 * m );
    
    std::cout 
      << stars                             << std::endl 
      << " SolidCons "                     << std::endl 
      << stars                             << std::endl ;
    
    std::cout << solid << std::endl;    
    Ticks ticks;
    const HepVector3D vect ( 0 , 0 , 1 );
    for( int i = 0 ; i < 20 ; ++i ) 
      {
        const double x = -2 * m + i * ( 4 * m / 20  ) ;
        const HepPoint3D point( x , 0.0 , -10.0 * m ) ;
        ticks.clear();
        const unsigned int n = 
          solid->intersectionTicks( point , vect  , ticks );
        std::cout << " point " << point << " #ticks=" << n << " (";
        for( iterator tick = ticks.begin() ; ticks.end() != tick ; ++tick )
          { std::cout << DetDesc::print( *tick ) << ","; } 
        std::cout << ") " << std::endl;
      }
    
  };

};

// ============================================================================
// The END 
// ============================================================================
