// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidTubs.cpp,v 1.4 2001-03-13 11:58:08 mato Exp $

#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/SolidException.h"

#include "CLHEP/Units/PhysicalConstants.h"


// constructor: //////////////////////////////////////////////////////////////////
SolidTubs::SolidTubs( const std::string& name          ,
                      const double       ZHalfLength   , 
                      const double       OuterRadius   ,
                      const double       InnerRadius   , 
                      const double       StartPhiAngle , 
                      const double       DeltaPhiAngle , 
                      const int          CoverModel    )  
  : m_tubs_name           ( name          )
  , m_tubs_zHalfLength    ( ZHalfLength   )
  , m_tubs_outerRadius    ( OuterRadius   )
  , m_tubs_innerRadius    ( InnerRadius   )
  , m_tubs_startPhiAngle  ( StartPhiAngle )                         //  NB!
  , m_tubs_deltaPhiAngle  ( DeltaPhiAngle )                         //  NB! 
  //
  , m_tubs_coverModel     ( CoverModel    )
  , m_tubs_cover          (        0      ) 
  //
{
  //
  if( 0 >= ZHalfLength )
    { throw SolidException("SolidTubs::ZHalfLength is not positive!"); } 
  if( 0 >= OuterRadius )
    { throw SolidException("SolidTubs::OuterRadius is not positive!"); } 
  if( 0 >  InnerRadius ) 
    { throw SolidException("SolidTubs::InnerRadius is negative    !"); } 
  if( InnerRadius >= OuterRadius ) 
    { throw SolidException("SolidTubs::InnerRadius >= OuterRadius !"); } 
  if( -180.0 * degree > StartPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle is < -180 degree! "); } 
  if(  360.0 * degree < StartPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle is >  360 degree! "); } 
  if(    0.0 * degree > DeltaPhiAngle ) 
    { throw SolidException("SolidTubs::DeltaPhiAngle is <    0 degree! "); } 
  if(  360.0 * degree < StartPhiAngle + DeltaPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle+DeltaPhiAngle > 360 degree!"); } 
  //
};
///////////////////////////////////////////////////////////////////////////////////////
SolidTubs::~SolidTubs()
{
  if( this == m_tubs_cover ){                    m_tubs_cover = 0 ; } 
  if(    0 != m_tubs_cover ){ delete m_tubs_cover; m_tubs_cover = 0 ; }
};
/// fictive default constructor ////////////////////////////////////////////////////////
SolidTubs::SolidTubs()
  : m_tubs_name           ( "nnnamed Tubs")
  , m_tubs_zHalfLength    ( 10000000      )
  , m_tubs_outerRadius    ( 10000000      )
  , m_tubs_innerRadius    ( 0             )
  , m_tubs_startPhiAngle  ( 0             )                         
  , m_tubs_deltaPhiAngle  ( 360*degree    )                         
  //
  , m_tubs_coverModel     (        0      )
  , m_tubs_cover          (        0      ) 
  //
{};
/// serialization for reading //////////////////////////////////////////////////////////
StreamBuffer& SolidTubs::serialize( StreamBuffer& s ) 
{
  ///
  if( 0 != m_tubs_cover ) { delete m_tubs_cover ; m_tubs_cover = 0 ; } 
  ///
  s >> m_tubs_name           
    >> m_tubs_zHalfLength    
    >> m_tubs_outerRadius    
    >> m_tubs_innerRadius    
    >> m_tubs_startPhiAngle  
    >> m_tubs_deltaPhiAngle  
    >> m_tubs_coverModel    ;
  ///
  if( 0 >= zHalfLength() )
    { throw SolidException("SolidTubs::ZHalfLength is not positive!"); } 
  if( 0 >= outerRadius() )
    { throw SolidException("SolidTubs::OuterRadius is not positive!"); } 
  if( 0 >  innerRadius() ) 
    { throw SolidException("SolidTubs::InnerRadius is negative    !"); } 
  if( innerRadius() >= outerRadius() ) 
    { throw SolidException("SolidTubs::InnerRadius >= OuterRadius !"); } 
  if( -180.0 * degree > startPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle is < -180 degree! "); } 
  if(  360.0 * degree < startPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle is >  360 degree! "); } 
  if(    0.0 * degree > deltaPhiAngle() ) 
    { throw SolidException("SolidTubs::DeltaPhiAngle is <    0 degree! "); } 
  if(  360.0 * degree < startPhiAngle() + deltaPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle+DeltaPhiAngle > 360 degree!"); } 
  ///
  return s;
  ///
};
/// serialization for writing ////////////////////////////////////////////////////////////
StreamBuffer& SolidTubs::serialize( StreamBuffer& s ) const
{
  return s << typeName() 
           << m_tubs_name           
           << m_tubs_zHalfLength    
           << m_tubs_outerRadius    
           << m_tubs_innerRadius    
           << m_tubs_startPhiAngle  
           << m_tubs_deltaPhiAngle  
           << m_tubs_coverModel    ;
  ///
};
/**  Construction of the covering solid 
     Model == 0 :
     cover for the tube segment  is the tube
     cover for the tube          is the cylinder 
     cover for the cylinder      is the box
     Model != 0 :
     cover for tube     segment  is the cylinder segment 
     cover for cylinder segment  is the cylinder
     cover for cylinder          is the box  
*/
const ISolid*           SolidTubs::cover         () const 
{
  if( 0 != m_tubs_cover ) { return m_tubs_cover; }              // cover is calculated already 
  //
  ISolid* cov = 0 ;  
  if( 0 == m_tubs_coverModel ) 
    { 
      if( 0.0*degree  != startPhiAngle() || 360.0*degree  != deltaPhiAngle()    ) 
        { cov = new SolidTubs("Cover for " + name() , zHalfLength() , outerRadius() , innerRadius() ); }
      else if ( 0.0 != innerRadius() )                              
        { cov = new SolidTubs("Cover for " + name() , zHalfLength() , outerRadius()                 ); }
      else                                                          
        { cov = new SolidBox ("Cover for " + name() , outerRadius() , outerRadius() , zHalfLength() ); }
    }
  else 
    {
      if ( 0.0 != innerRadius() )                              
        { cov = new SolidTubs("Cover for " + name() , zHalfLength() , outerRadius() , 0.0 * mm      , 
                              startPhiAngle() , deltaPhiAngle() , m_tubs_coverModel ); }
      else if( 0.0*degree  != startPhiAngle() || 360.0*degree  != deltaPhiAngle()    ) 
        { cov = new SolidTubs("Cover for " + name() , zHalfLength() , outerRadius() , innerRadius() ,
                              0.0 * degree , 360.0 * degree , m_tubs_coverModel ); }
      else                                                          
        { cov = new SolidBox ("Cover for " + name() , outerRadius() , outerRadius() , zHalfLength() ); }
    }
  //
  if( 0 == cov ) { return this; } 
  //
  m_tubs_cover = cov ; 
  //
  return m_tubs_cover;
};
/** calculate the intersection points("ticks") with a given line. 
    Input - line, parametrised as (Point + Vector * Tick) 
    "Tick" is just a value of parameter, at which the intercestion occurs 
    Return the number of intersection points (=size of Ticks container)   
*/
inline unsigned int SolidTubs::intersectionTicks( const HepPoint3D& point  ,
                                                  const HepVector3D& vect   ,
                                                  ISolid::Ticks   & ticks  ) const
{
  ticks.clear(); 
  // line with null direction vector is not able to intersect any solid. 
  if( vect.mag2() == 0 )                  { return 0 ; }                     // RETURN! 
  // find intersection points ("ticks") with cylinder (outer radius) 
  SolidTicks::LineIntersectsTheCylinder( point , vect , outerRadius() , std::back_inserter( ticks ) ); 
  // find intersection points("ticks") with cylinder (inner radius) 
  if( innerRadius() > 0 ) 
    { SolidTicks::LineIntersectsTheCylinder( point , vect , innerRadius() , std::back_inserter( ticks ) ); }
  // find intersection points("ticks") with z-planes 
  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , std::back_inserter( ticks ) ); 
  ///
  if( 0 != startPhiAngle() || ( 360 * degree !=  deltaPhiAngle() ) )
    {
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle()                   , std::back_inserter( ticks ) );
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle() + deltaPhiAngle() , std::back_inserter( ticks ) );
    }
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );    
};
/** calculate the intersection points("ticks") with a given line. 
    Input - line, parametrised as (Point + Vector * Tick) 
    "Tick" is just a value of parameter, at which the intercestion occurs 
    Return the number of intersection points (=size of Ticks container)   
*/
inline unsigned int SolidTubs::intersectionTicks( const HepPoint3D  & point   ,
                                                  const HepVector3D  & vect    ,
                                                  const ISolid::Tick& tickMin ,
                                                  const ISolid::Tick& tickMax ,
                                                  ISolid::Ticks     & ticks   ) const
{
  intersectionTicks( point , vect , ticks );
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin, tickMax , *this );  
};
///////////////////////////////////////////////////////////////////////////////////////////
bool SolidTubs::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this;
  return s->acceptInspector( pInspector ); 
};
///////////////////////////////////////////////////////////////////////////////////////////
bool SolidTubs::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef ( &m_tubs_name          , this , "TubsName"    ) ; 
  pInspector->inspectByRef ( &m_tubs_zHalfLength   , this , "Z-size/2"    ) ; 
  pInspector->inspectByRef ( &m_tubs_outerRadius   , this , "OuterRadius" ) ; 
  pInspector->inspectByRef ( &m_tubs_innerRadius   , this , "InnerRadius" ) ; 
  pInspector->inspectByRef ( &m_tubs_startPhiAngle , this , "StartPhi"    ) ; 
  pInspector->inspectByRef ( &m_tubs_deltaPhiAngle , this , "DeltaPhi"    ) ; 
  pInspector->inspectByRef ( &m_tubs_coverModel    , this , "CoverModel"  ) ; 
  ///
  return true;
  ///
};
//////////////////////////////////////////////////////////////////////////////////////
std::ostream&  SolidTubs::printOut      ( std::ostream&  os ) const
{
  os << "\t" << typeName()  << "\tname="  << name()
     << std::endl  
     << "\t\t\tzLength           [mm] = " << std::setw(12) << zLength()       / millimeter
     << std::endl  
     << "\t\t\touterRadius       [mm] = " << std::setw(12) << outerRadius()   / millimeter
     << std::endl  ;
  if( innerRadius() > 0 ) 
    { os << "\t\t\tinnerRadius       [mm] = " << std::setw(12) << innerRadius() / millimeter << std::endl ; }
  if( startPhiAngle() != 0 * degree || deltaPhiAngle() != 360 * degree ) 
    { 
      os << "\t\t\tstartPhiAngle [degree] = " << std::setw(12) << startPhiAngle() / degree    
         << std::endl 
         << "\t\t\tdeltaPhiAngle [degree] = " << std::setw(12) << deltaPhiAngle() / degree  
         << std::endl ;
    }
  ///
  return os;
};
//////////////////////////////////////////////////////////////////////////////////////
MsgStream&     SolidTubs::printOut      ( MsgStream&     os ) const
{
  os << "\t" << typeName()  << "\tname="  << name()
     << endreq     
     << "\t\t\tzLength           [mm] = " << std::setw(12) << zLength()       / millimeter
     << endreq     
     << "\t\t\touterRadius       [mm] = " << std::setw(12) << outerRadius()   / millimeter
     << endreq     ;
  if( innerRadius() > 0 ) 
    { os << "\t\t\tinnerRadius       [mm] = " << std::setw(12) << innerRadius() / millimeter << endreq ; }
  if( startPhiAngle() != 0 * degree || deltaPhiAngle() != 360 * degree ) 
    { 
      os << "\t\t\tstartPhiAngle [degree] = " << std::setw(12) << startPhiAngle() / degree    
         << endreq    
         << "\t\t\tdeltaPhiAngle [degree] = " << std::setw(12) << deltaPhiAngle() / degree  
         << endreq    ;
    }
  ///
  return os;
};
//////////////////////////////////////////////////////////////////////////////////////

