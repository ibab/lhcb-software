
#include "GaudiKernel/ISolid.h" 
#include "DetDesc/SolidChild.h" 

#include "CLHEP/Geometry/Transform3D.h" 
#include "CLHEP/Geometry/Point3D.h" 


//
// constructor
SolidChild::SolidChild( ISolid*                solid , 
                        const HepTransform3D*  mtrx  )
  : m_sc_solid     ( solid )
  , m_sc_simple    ( true  )
  , m_sc_matrix    (   0   ) 
{
  
  if( 0 == solid ) { throw SolidException("SolidChild constructor 1 , ISolid* points to NULL!");}
  
  if( 0 != mtrx && !(HepTransform3D::Identity == *mtrx) )
    { m_sc_matrix    = new HepTransform3D( *mtrx ); if( 0 != m_sc_matrix) { m_sc_simple = false ; } }
  else{                                                                                   m_sc_simple = true  ; }         
  
};

//
// another constructor 
SolidChild::SolidChild( ISolid*              solid    , 
			const HepPoint3D&    Position , 
			const HepRotation&   Rotation )
  : m_sc_solid     ( solid )
  , m_sc_simple    ( true  )
  , m_sc_matrix    (   0   ) 
{
  
  if( 0 == solid ) { throw SolidException("SolidChild constructor 2 , ISolid* points to NULL!");}
  
  const HepPoint3D NullPoint(0,0,0);
  
  if( NullPoint == Position && Rotation.isIdentity() ) { m_sc_simple = true  ; }         
  else 
    {
      m_sc_matrix    = 
	new HepTransform3D( HepTranslate3D( -1.0 * Position ) * HepRotate3D( Rotation ) );
      if( 0 != m_sc_matrix ) { m_sc_simple = false ; } 
    }
  
};

//
//
//

SolidChild::~SolidChild()
{
  if( 0 != m_sc_solid  ){ delete m_sc_solid ; m_sc_solid  = 0; } 
  if( 0 != m_sc_matrix ){ delete m_sc_matrix; m_sc_matrix = 0; } 
};


