#include "GaudiKernel/IInspector.h" 
#include "GaudiKernel/StreamBuffer.h"

#include "DetDesc/SolidChild.h" 
#include "DetDesc/ClhepToStream.h" 
#include "DetDesc/ISolidFromStream.h" 

#include "CLHEP/Geometry/Transform3D.h" 
#include "CLHEP/Geometry/Point3D.h" 


//////////////////////////////////////////////////////////////////////////////////
SolidChild::SolidChild()
  : m_sc_solid     (  0    )
  , m_sc_simple    ( true  )
  , m_sc_matrix    (   0   )
{}; 
//////////////////////////////////////////////////////////////////////////////////
SolidChild::SolidChild( ISolid*                solid , 
                        const HepTransform3D*  mtrx  )
  : m_sc_solid     ( solid )
  , m_sc_simple    ( true  )
  , m_sc_matrix    (   0   ) 
{
  ///
  if( 0 == solid ) { throw SolidException("SolidChild constructor 1 , ISolid* points to NULL!");}
  ///
  if( 0 != mtrx && !(HepTransform3D::Identity == *mtrx) )
    { m_sc_matrix    = new HepTransform3D( *mtrx ); if( 0 != m_sc_matrix) { m_sc_simple = false ; } }
  else
    { m_sc_simple = true  ; }         
  ////
};
// another constructor /////////////////////////////////////////////////////////////////////////////////////////////
SolidChild::SolidChild( ISolid*              solid    , 
			const HepPoint3D&    Position , 
			const HepRotation&   Rotation )
  : m_sc_solid     ( solid )
  , m_sc_simple    ( true  )
  , m_sc_matrix    (   0   ) 
{
  ////
  if( 0 == solid ) { throw SolidException("SolidChild constructor 2 , ISolid* points to NULL!");}
  ///
  const HepPoint3D NullPoint(0,0,0);
  ///
  if( NullPoint == Position && Rotation.isIdentity() ) { m_sc_simple = true  ; }         
  else 
    {
      m_sc_matrix    = 
	new HepTransform3D( HepRotate3D( Rotation)*HepTranslate3D( -1.0 * Position ) );
      if( 0 != m_sc_matrix ) { m_sc_simple = false ; } 
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
SolidChild::~SolidChild()
{
  if( 0 != m_sc_solid  ){ delete m_sc_solid ; m_sc_solid  = 0; } 
  if( 0 != m_sc_matrix ){ delete m_sc_matrix; m_sc_matrix = 0; } 
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidChild::serialize( StreamBuffer& sb ) 
{
  ///
  if( 0 !=  m_sc_matrix ) { delete m_sc_matrix ; m_sc_matrix = 0 ; } 
  ///
  HepTransform3D Mtrx;  
  sb >>  Mtrx;
  m_sc_simple = false ;  
  ISolidFromStream cnstr;
  m_sc_solid  = cnstr( sb ) ;
  ///
  return sb;
} 
////////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidChild::serialize( StreamBuffer& sb ) const
{ return sb << matrix() << *solid() ;  } 
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidChild::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this;
  return s->acceptInspector( pInspector ) ; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidChild::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef ( m_sc_solid , this , "ChildSolid" ); 
  ///
  return true;
  ///
}; 
//////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& SolidChild::printOut     ( std::ostream& os ) const
{ return os << "\tSolidChild with " << solid(); };
//////////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    SolidChild::printOut     ( MsgStream&    os ) const
{ return os << "\tSolidChild with " << solid(); };
//////////////////////////////////////////////////////////////////////////////////////////////////////








