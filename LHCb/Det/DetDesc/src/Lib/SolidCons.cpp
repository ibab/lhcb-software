/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/08/09 16:48:02  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
// GaudiKernel
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IInspector.h"
// DetDesc
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidTrd.h"
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h"

// ============================================================================
/** @file SolidCons.cpp 
 *
 *  Implementation of class SolidCons 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param name                  name of conical tube segment
 *  @param ZHalfLength           half-length 
 *  @param OuterRadiusAtMinusZ   outer radius at minus Z 
 *  @param OuterRadiusAtPlusZ    outer radius at plus  Z 
 *  @param InnerRadiusAtMinusZ   inner radius at minus Z 
 *  @param InnerRadiusAtPlusZ    inner radius at plus  Z 
 *  @param StartPhiAngle         start phi angle 
 *  @param DeltaPhiAngle         delta phi angle 
 *  @param CoverModel            covering model 
 *  @exception SolidException    wrong parameters range 
 */
// ============================================================================
SolidCons::SolidCons( const std::string & name  ,
                      double ZHalfLength        , 
                      double OuterRadiusMinusZ  ,
                      double OuterRadiusPlusZ   ,
                      double InnerRadiusMinusZ  , 
                      double InnerRadiusPlusZ   , 
                      double StartPhiAngle      , 
                      double DeltaPhiAngle      ,
                      int    CoverModel         )
  : SolidBase                ( name               )
  , m_cons_zHalfLength       ( ZHalfLength        )
  , m_cons_outerRadiusMinusZ ( OuterRadiusMinusZ  )
  , m_cons_outerRadiusPlusZ  ( OuterRadiusPlusZ   )
  , m_cons_innerRadiusMinusZ ( InnerRadiusMinusZ  )
  , m_cons_innerRadiusPlusZ  ( InnerRadiusPlusZ   )
  , m_cons_startPhiAngle     ( StartPhiAngle      )
  , m_cons_deltaPhiAngle     ( DeltaPhiAngle      )
  , m_cons_coverModel        ( CoverModel         )        
{
  ///
  if( 0 >= zHalfLength()                                )
    { throw SolidException("SolidCons ::ZHalfLength       is not positive!");}
  if( 0 >= outerRadiusAtMinusZ()                        ) 
    { throw SolidException("SolidCons ::OuterRadiusMinusZ is not positive!");}
  if( 0 >= outerRadiusAtPlusZ()                         ) 
    { throw SolidException("SolidCons ::OuterRadiusPlusZ  is not positive!");}
  if( 0 >  innerRadiusAtMinusZ()                        ) 
    { throw SolidException("SolidCons ::InnerRadiusMinusZ is negative !   ");}
  if( innerRadiusAtMinusZ() >= outerRadiusAtMinusZ()    ) 
    { throw SolidException("SolidCons:InnerRadiusMinusZ>=OuterRadiusMinusZ!");}
  if( 0 >  innerRadiusAtPlusZ()                         ) 
    { throw SolidException("SolidCons ::InnerRadiusPlusZ  is negative !   ");}
  if( innerRadiusAtPlusZ() >= outerRadiusAtPlusZ()      )
    { throw SolidException("SolidCons::InnerRadiusPlusZ>=OuterRadiusPlusZ!");}
  if( -180.0 * degree > startPhiAngle()                 ) 
    { throw SolidException("SolidCons::StartPhiAngle <-pi!");}
  if(  360.0 * degree < startPhiAngle()                 ) 
    { throw SolidException("SolidCons ::StartPhiAngle >2pi degree!");}
  if(    0.0 * degree > deltaPhiAngle()                 ) 
    { throw SolidException("SolidCons ::DeltaPhiAngle < 0 degree!"    );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) 
    { throw SolidException("SolidCons ::StartPhiAngle+DeltaPhiAngle>2pi");}
  ///
};

// ============================================================================
/// destructor 
// ============================================================================
SolidCons::~SolidCons()
{
  reset();
};


// ============================================================================
/** default protected  coinstructor 
 *  @param Name name of conical tube segment  
 */
// ============================================================================
SolidCons::SolidCons( const std::string& Name )
  ///
  : SolidBase                ( Name                )
  , m_cons_zHalfLength       ( 1000000             )
  , m_cons_outerRadiusMinusZ ( 1000000             )
  , m_cons_outerRadiusPlusZ  ( 1000000             )
  , m_cons_innerRadiusMinusZ ( 0                   )
  , m_cons_innerRadiusPlusZ  ( 0                   )
  , m_cons_startPhiAngle     ( 0                   )
  , m_cons_deltaPhiAngle     ( 360 * degree        )
  , m_cons_coverModel        ( 0                   )
{};


// ============================================================================
/** check for the given point (local frame)
 *  @param point point (in local reference system)
 *  @return true ifpoint is inside the solid
 */
// ============================================================================
bool SolidCons::isInside (  const HepPoint3D& point ) const
{  
  /// check z-planes 
  if( abs( point.z() ) > zHalfLength() ) { return false; }
  /// check for radius 
  const double rho = point.perp(); 
  if( rho < iR_z( point.z() ) ) { return false; }
  if( rho > oR_z( point.z() ) ) { return false; }
  /// check for phi 
  double phi = point.phi();
  if( phi < 0 ) { phi  += 360.0 * degree ; };
  if( phi < startPhiAngle()                   ) { return false; } 
  if( phi > startPhiAngle() + deltaPhiAngle() ) { return false; }
  ///
  return true;
};

// ============================================================================
/** serialization for reading
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidCons::serialize( StreamBuffer& sb ) 
{
  /// reset 
  reset();
  /// serialize the base class
  SolidBase::serialize( sb );
  /// serialize own members 
  sb >>  m_cons_zHalfLength       
     >>  m_cons_outerRadiusMinusZ 
     >>  m_cons_outerRadiusPlusZ  
     >>  m_cons_innerRadiusMinusZ 
     >>  m_cons_innerRadiusPlusZ  
     >>  m_cons_startPhiAngle    
     >>  m_cons_deltaPhiAngle    
     >>  m_cons_coverModel ;   
  ///
  if( 0 >= zHalfLength()                                ) 
    { throw SolidException("SolidCons::ZHalfLength is not positive!");}
  if( 0 >= outerRadiusAtMinusZ()                        ) 
    { throw SolidException("SolidCons::OuterRadiusMinusZ is not positive!");}
  if( 0 >= outerRadiusAtPlusZ()                         ) 
    { throw SolidException("SolidCons::OuterRadiusPlusZ  is not positive!");}
  if( 0 >  innerRadiusAtMinusZ()                        ) 
    { throw SolidException("SolidCons::InnerRadiusMinusZ is negative !  ");}
  if( innerRadiusAtMinusZ() >= outerRadiusAtMinusZ()    ) 
    { throw SolidException("SolidCons::InnerRadiusMinusZ>=OuterRadiusMinusZ!");}
  if( 0 >  innerRadiusAtPlusZ()                         ) 
    { throw SolidException("SolidCons ::InnerRadiusPlusZ  is negative !   ");}
  if( innerRadiusAtPlusZ() >= outerRadiusAtPlusZ()      ) 
    { throw SolidException("SolidCons ::InnerRadiusPlusZ>=OuterRadiusPlusZ!");}
  if( -180.0 * degree > startPhiAngle()                 ) 
    { throw SolidException("SolidCons ::StartPhiAngle  is <-pi");}
  if(  360.0 * degree < startPhiAngle()                 ) 
    { throw SolidException("SolidCons ::StartPhiAngle>2pi");}
  if(    0.0 * degree > deltaPhiAngle()                 ) 
    { throw SolidException("SolidCons ::DeltaPhiAngle <0 "   );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) 
    { throw SolidException("SolidCons ::StartPhiAngle+DeltaPhiAngle>2pi");}
  ///
  return sb; 
};

// ============================================================================
/** serialization for writing
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidCons::serialize( StreamBuffer& sb ) const 
{
  /// serialize the base class 
  SolidBase::serialize( sb ) ;
  return   sb <<  m_cons_zHalfLength       
              <<  m_cons_outerRadiusMinusZ 
              <<  m_cons_outerRadiusPlusZ  
              <<  m_cons_innerRadiusMinusZ 
              <<  m_cons_innerRadiusPlusZ  
              <<  m_cons_startPhiAngle    
              <<  m_cons_deltaPhiAngle    
              <<  m_cons_coverModel ;   
};

// ============================================================================/
/** -# retrieve the pointer to "simplified" solid - "cover"
 *    - for Model = 0 
 *        -# the cover for the general conical tube segment is 
 *           the conical tube 
 *        -# the cover for the conical tube is conical cylinder 
 *        -# the cover for the conical cylinder is TRD 
 *    - for Model != 0 
 *        -# the cover for conical tube serment is conical cylinder segment 
 *        -# the cover for conical cylinder segment is conical cylinder 
 *        -# the cover for conical cylinder is TRD 
 *  -# implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidCons::cover () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; } 
  //  
  ISolid* cov = 0 ;
  if( 0 == m_cons_coverModel ) 
    {
      // cover for conical tube segment is conical tube 
      if (   0.0   * degree  != startPhiAngle  () || 
             360.0 * degree  != deltaPhiAngle  () )  
        { cov = 
            new SolidCons  ("Cover for " + name     () , 
                            zHalfLength             () , 
                            outerRadiusAtMinusZ     () , 
                            outerRadiusAtPlusZ      () , 
                            innerRadiusAtMinusZ     () , 
                            innerRadiusAtPlusZ      () ); }
      // cover for conical tube is "conical cylinder"  
      else if ( 0.0 != innerRadiusAtMinusZ ()  || 
                0.0 != innerRadiusAtPlusZ  ()  )
        { cov = 
            new SolidCons  ("Cover for " + name     () , 
                            zHalfLength             () , 
                            outerRadiusAtMinusZ     () , 
                            outerRadiusAtPlusZ      () );}
      // cover for "conical cylinder" is TRD  
      else
        { cov = 
            new SolidTrd  ("Cover for " + name     () , 
                           zHalfLength             () ,
                           outerRadiusAtMinusZ     () , 
                           outerRadiusAtMinusZ     () , 
                           outerRadiusAtPlusZ      () , 
                           outerRadiusAtPlusZ      () ); } 
    }
  else
    {
      /// cover for conical tube segment is conical cylinder segment 
      if ( 0.0 != innerRadiusAtMinusZ() || 
           0.0 != innerRadiusAtPlusZ () )
        { cov = 
            new SolidCons  ("Cover for " + name     () , 
                            zHalfLength             () , 
                            outerRadiusAtMinusZ     () , 
                            outerRadiusAtPlusZ      () , 
                            0.0 * mm                   ,  
                            0.0 * mm                   , 
                            startPhiAngle           () , 
                            deltaPhiAngle           () , 
                            m_cons_coverModel          );}
      /// cover for conical cylinder segment is conical cylinder 
      else if ( 0.0   * degree  != startPhiAngle  () || 
                360.0 * degree  != deltaPhiAngle  () )  
        { cov = 
            new SolidCons  ("Cover for " + name     () , 
                            zHalfLength             () , 
                            outerRadiusAtMinusZ     () , 
                            outerRadiusAtPlusZ      () , 
                            innerRadiusAtMinusZ     () , 
                            innerRadiusAtPlusZ      () ,
                            0.0 * degree               , 
                            360.0 * degree             , 
                            m_cons_coverModel          ); }
      /// cover for "conical cylinder" is TRD  
      else                                                       
        { cov = 
            new SolidTrd  ("Cover for " + name     () , 
                           zHalfLength             () ,
                           outerRadiusAtMinusZ     () , 
                           outerRadiusAtMinusZ     () , 
                           outerRadiusAtPlusZ      () , 
                           outerRadiusAtPlusZ      () ); } 
    }
  /// 
  if( 0 == cov ) { return this; } 
  m_cover = cov ; 
  return m_cover ;
};

// ============================================================================
/** calculate the intersection points("ticks") with a given line.
 *  Input - line, paramterised by  x_vect = Point + Vector * T
 *  "tick" is just a value of T, at which the intersection occurs
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points (=size of Ticks container)
 */
// ============================================================================
unsigned int 
SolidCons::intersectionTicks ( const HepPoint3D & point  ,      
                               const HepVector3D& vect   ,      
                               ISolid::Ticks    & ticks  ) const
{
  /// line with numm direction vector is not able to intersect any solid 
  if( vect.mag2() <= 0 )  { return 0 ;}  ///< RETURN!!!
  /// intersect with z-planes 
  SolidTicks::LineIntersectsTheZ( point                       , 
                                  vect                        , 
                                  -1.0 * zHalfLength()        , 
                                  std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point                       , 
                                  vect                        ,     
                                  zHalfLength()               , 
                                  std::back_inserter( ticks ) );   
  /// intersect with phi 
  if( ( 0 != startPhiAngle() ) || ( 360 * degree != deltaPhiAngle() ) )
    {
      SolidTicks::LineIntersectsThePhi( point                             , 
                                        vect                              , 
                                        startPhiAngle()                   , 
                                        std::back_inserter( ticks )       ); 
      SolidTicks::LineIntersectsThePhi( point                             , 
                                        vect                              , 
                                        startPhiAngle() + deltaPhiAngle() , 
                                        std::back_inserter( ticks )       ); 
    }   
  /// intersect with outer conical surface
  SolidTicks::LineIntersectsTheCone( point                           , 
                                     vect                            , 
                                     outerRadiusAtMinusZ()           , 
                                     outerRadiusAtPlusZ ()           , 
                                     -1.0 * zHalfLength ()           , 
                                     zHalfLength        ()           , 
                                     std::back_inserter( ticks )     );   
  /// intersect with inner conical surface
  if( ( 0 < innerRadiusAtPlusZ() ) || ( 0 < innerRadiusAtMinusZ() )  )
    {
      SolidTicks::LineIntersectsTheCone( point                       , 
                                         vect                        , 
                                         innerRadiusAtMinusZ()       , 
                                         innerRadiusAtPlusZ ()       , 
                                         -1.0 * zHalfLength ()       , 
                                         zHalfLength        ()       , 
                                         std::back_inserter( ticks ) );     
    }
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
};

// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream&  SolidCons::printOut      ( std::ostream&  os ) const
{
  /// serialize the base class
  SolidCons::printOut( os );
  /// serialize members
  os << "[" ;
  os << " sizeZ[mm]"          <<  zLength()            / millimeter 
     << " outerRadiusPZ[mm]=" << outerRadiusAtPlusZ () / millimeter  
     << " outerRadiusMZ[mm]=" << outerRadiusAtMinusZ() / millimeter ;
  if( 0 < innerRadiusAtPlusZ() ) 
    { os << " innerRadiusPZ[mm]=" <<  innerRadiusAtPlusZ() / millimeter ; }
  if( 0 < innerRadiusAtMinusZ() ) 
    { os << " innerRadiusMZ[mm]=" <<  innerRadiusAtMinusZ() / millimeter ; }
  if( 0 * degree != startPhiAngle() || 
      360 * degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / degree ;
      os << " deltaPhiAngle[deg]" << startPhiAngle() / degree ;
    }
  return os << "]";
};

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&     SolidCons::printOut      ( MsgStream&     os ) const
{
  /// serialize the base class
  SolidCons::printOut( os );
  /// serialize members
  os << "[" ;
  os << " sizeZ[mm]"          <<  zLength()            / millimeter 
     << " outerRadiusPZ[mm]=" << outerRadiusAtPlusZ () / millimeter  
     << " outerRadiusMZ[mm]=" << outerRadiusAtMinusZ() / millimeter ;
  if( 0 < innerRadiusAtPlusZ() ) 
    { os << " innerRadiusPZ[mm]=" <<  innerRadiusAtPlusZ() / millimeter ; }
  if( 0 < innerRadiusAtMinusZ() ) 
    { os << " innerRadiusMZ[mm]=" <<  innerRadiusAtMinusZ() / millimeter ; }
  if( 0 * degree != startPhiAngle() || 
      360 * degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / degree ;
      os << " deltaPhiAngle[deg]" << deltaPhiAngle() / degree ;
    }
  return os << "]";
};

// ============================================================================





