// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#include "DetDesc/SolidPolycone.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
// GaudiKernel
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IInspector.h"
// DetDesc
#include "DetDesc/SolidFactory.h"
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h"


//=============================================================================
/** @file SolidPolycone.cpp 
 *
 * Implementation file for class : SolidPolycone
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 24/10/2001 
 *
 */
//=============================================================================

//=============================================================================
/** Standard constructor
 *  @param Name          the name of the polycone
 *  @param Params        vector of "triplets" (z,(rMax,rMin))
 *  @param StartPhiAngle the azimuthal angle phi at which polycone "begins"
 *  @param DeltaPhiAngle the opening angle 
 */
//=============================================================================
SolidPolycone::SolidPolycone( const std::string&             Name          ,
                              const SolidPolycone::Triplets& Params        , 
                              const double                   StartPhiAngle ,
                              const double                   DeltaPhiAngle )
  : SolidBase       ( Name          ) 
  , m_triplets      ( Params        )
  , m_startPhiAngle ( StartPhiAngle ) 
  , m_deltaPhiAngle ( DeltaPhiAngle )
{
  /// check for parameters
  if( number() < 2 ) 
    { throw SolidException("SolidPolycone:: wrong number of sections!");}
  /// check for Phi
  if(    0.0 * degree > startPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle < 0 degree!"    );}
  if(    0.0 * degree > deltaPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::DeltaPhiAngle < 0 degree!"    );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle+DeltaPhiAngle>2pi");}
  /// sort the triplets
  std::sort( m_triplets.begin() , m_triplets.end() );
  ///
  for( unsigned int i = 0 ; i < number() ; ++i )
    {
      if( rMin( i ) < 0 ) 
        { throw SolidException("SolidPolycone ::rMin < 0 !");}
      if( !( rMin(i) < rMax( i ) ) )        
        { throw SolidException("SolidPolycone :: !(rMin < rMax) !");}      
    }
};

// ============================================================================
/// destructor 
// ============================================================================
SolidPolycone::~SolidPolycone()
{
  reset();
};

// ============================================================================
/** default protected  coinstructor 
 *  @param Name name of conical tube segment  
 */
// ============================================================================
SolidPolycone::SolidPolycone( const std::string& Name )
  ///
  : SolidBase       ( Name         )
  , m_triplets      ()
  , m_startPhiAngle ( 0            ) 
  , m_deltaPhiAngle ( 360 * degree )
{};

// ============================================================================
/** check for the given point (local frame)
 *  @param point point (in local reference system)
 *  @return true ifpoint is inside the solid
 */
// ============================================================================
bool SolidPolycone::isInside (  const HepPoint3D& point ) const
{
  /// look for concrete z-segment
  if( point.z() > triplets().back().first ) { return false; }
  Iterator it = 
    std::find_if( begin () , 
                  end   () ,
                  std::bind2nd( CmpZ() , point.z() ) ) ;
  /// outside!
  if( begin() == it || end() == it        ) { return false; }
  /// check for phi 
  if( 360 * degree != deltaPhiAngle() ) 
    {
      double phi = point.phi() ;
      if( phi < 0                         ) { phi += 360 * degree  ; }
      if( phi < startPhiAngle()                   ) { return false ; } 
      if( phi > startPhiAngle() + deltaPhiAngle() ) { return false ; }
    }
  /// check for radius
  const double       rho  = point.perp();
  const unsigned int i2   = it - begin();
  const unsigned int i1   = i2 - 1 ;
  ///
  const double       dzi  = 1.0 / ( z( i2 ) - z ( i1 ) ) ;
  /// check for outer radius 
  if(  rho > rMax( i1 )  +  
       ( point.z() - z( i1 ) ) * ( rMax( i2 ) - rMax( i1 ) ) * dzi ) 
    { return false; }
  /// check for inner radius  
  if( 0 < rMin( i1 )  &&  0 < rMin( i2 ) )
    {
      if(  rho < rMin( i1 )  +  
           ( point.z() - z( i1 ) ) * ( rMin( i2 ) - rMin( i1 ) ) * dzi ) 
        { return false; }
    }
  ///
  return true;
};

/// ============================================================================
/** serialization for writing
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidPolycone::serialize( StreamBuffer& sb ) const 
{
  /// serialize the base class 
  SolidBase::serialize( sb ) ;
  /// serialze the triplets 
  sb << number() ;
  for( unsigned int  i = 0 ; i < number()  ; ++i ) 
    { sb << z( i ) << rMax( i ) << rMin( i ) ; }
  ///
  return sb << startPhiAngle() << deltaPhiAngle() ;
};


/// ============================================================================
/** serialization for reading
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidPolycone::serialize( StreamBuffer& sb ) 
{
  /// reset
  reset();
  m_triplets.clear();
  /// serialize the base class 
  SolidBase::serialize( sb ) ;
  Triplets::size_type num;
  sb >> num ;
  if( num < 2 ) 
    { throw SolidException("SolidPolycone:: serialize wrong number!");}
  while( num-- > 0 )
    {
      double zz , rmx , rmn;
      sb >> zz >> rmx >> rmn ;
      m_triplets.push_back( Triplet( zz , Pair( rmx , rmn ) ) );
    }
  ///
  sb >> m_startPhiAngle >> m_deltaPhiAngle ;
  ///
  /// check for Phi
  if(    0.0 * degree > startPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle < 0 degree!"    );}
  if(    0.0 * degree > deltaPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::DeltaPhiAngle < 0 degree!"    );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle+DeltaPhiAngle>2pi");}
  /// sort the triplets
  std::sort( m_triplets.begin() , m_triplets.end() );
  ///
  for( unsigned int i = 0 ; i < number() ; ++i )
    {
      if( rMin( i ) < 0 ) 
        { throw SolidException("SolidPolycone ::rMin < 0 !");}
      if( !( rMin(i) < rMax( i ) ) )        
        { throw SolidException("SolidPolycone :: !(rMin < rMax) !");}      
    }
  ///
  return sb;
};

// ============================================================================/
/** -# retrieve the pointer to "simplified" solid - "cover"
 *  -# implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidPolycone::cover () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; } 
  //  
  ISolid* cov = 0 ;
  if( 0.0 != startPhiAngle() || 360 * degree != deltaPhiAngle() )
    { cov = new SolidPolycone( "Cover for " + name () , triplets() ); }  
  else
    {
      double rmxmx = rMin( 0 ) ;
      double rmnmn = rMax( 0 ) ;
      double zmxmx = 0         ;
      ///
      for( unsigned int i = 0 ; i < number() ; ++i )
        {
          if( rMax  ( i )   > rmxmx ) { rmxmx = rMax   ( i )   ; }
          if( rMin  ( i )   < rmnmn ) { rmnmn = rMin   ( i )   ; }
          if( abs( z( i ) ) > zmxmx ) { zmxmx = abs( z ( i ) ) ; }
        }
      cov = new SolidTubs( "Cover for " + name () , zmxmx , rmxmx , rmnmn ) ;
    }
  ///
  if( 0 == cov ) { return this ; }
  ///
  m_cover = cov ;
  ///
  return m_cover;
};

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param stream STD/STL stream
 *  @return reference to the stream 
 */
// ============================================================================
std::ostream&  SolidPolycone::printOut ( std::ostream& os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  /// serialize the members 
  os << "["
     << " number of planes: " << number() ;
  for( unsigned int i = 0 ; i < number() ; ++i ) 
    {
      os << "(Z[mm]="    << z    ( i ) / millimeter 
         << ",Rmax[mm]=" << rMax ( i ) / millimeter 
         << ",Rmin[mm]=" << rMax ( i ) / millimeter 
         << ")";
    }
  ///
  if( 0   * degree != startPhiAngle() || 
      360 * degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / degree ;
      os << " deltaPhiAngle[deg]" << startPhiAngle() / degree ;
    }
  ///
  return os ;
  ///
};

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&  SolidPolycone::printOut ( MsgStream& os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  /// serialize the members 
  os << "["
     << " number of planes: " << number() ;
  for( unsigned int i = 0 ; i < number() ; ++i ) 
    {
      os << "(Z[mm]="    << z    ( i ) / millimeter 
         << ",Rmax[mm]=" << rMax ( i ) / millimeter 
         << ",Rmin[mm]=" << rMax ( i ) / millimeter 
         << ")";
    }
  ///
  if( 0   * degree != startPhiAngle() || 
      360 * degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / degree ;
      os << " deltaPhiAngle[deg]" << startPhiAngle() / degree ;
    }
  ///
  return os ;
  ///
};

// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  -# Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int 
SolidPolycone::intersectionTicks ( const HepPoint3D & Point  ,  
                                   const HepVector3D& Vector ,  
                                   ISolid::Ticks    & ticks  ) const 
{
  /// line with null direction vector is not able to intersect any solid!
  if( Vector.mag2() <= 0 ) { return 0 ; }
  /// intersect with phi 
  if( ( 0            != startPhiAngle() ) || 
      ( 360 * degree != deltaPhiAngle() ) )
    {
      SolidTicks::LineIntersectsThePhi( Point                             , 
                                        Vector                            , 
                                        startPhiAngle()                   , 
                                        std::back_inserter( ticks )       ); 
      SolidTicks::LineIntersectsThePhi( Point                             ,
                                        Vector                            , 
                                        startPhiAngle() + deltaPhiAngle() , 
                                        std::back_inserter( ticks )       ); 
    }
  /// intersect with first z-plane 
  SolidTicks::LineIntersectsTheZ( Point        , 
                                  Vector       , 
                                  z( 0 )       , 
                                  std::back_inserter( ticks ) ); 
  /// loop over all other surfaces 
  for( unsigned int i = 1 ; i < number() ; ++i )
    {
      /// intersect with outer conical surface 
      SolidTicks::LineIntersectsTheCone( Point                       , 
                                         Vector                      , 
                                         rMax ( i - 1 )              , 
                                         rMax ( i     )              , 
                                         z    ( i - 1 )              , 
                                         z    ( i     )              , 
                                         std::back_inserter( ticks ) );   
      /// intersect with inner conical surface 
      if( ( 0 < rMin( i - 1 ) ) || ( 0 < rMin ( i ) ) ) 
        {
          SolidTicks::LineIntersectsTheCone( Point                       , 
                                             Vector                      , 
                                             rMin ( i - 1 )              , 
                                             rMin ( i     )              , 
                                             z    ( i - 1 )              , 
                                             z    ( i     )              , 
                                             std::back_inserter( ticks ) );   
          
        }      
      /// intersect with z-plane 
      SolidTicks::LineIntersectsTheZ( Point        , 
                                      Vector       , 
                                      z( i )       , 
                                      std::back_inserter( ticks ) ); 
    }
  ///
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );  
  ///  
};

//=============================================================================
// The end 
//=============================================================================
