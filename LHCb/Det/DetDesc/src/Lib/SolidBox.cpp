/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// GaudiKernel
#include "GaudiKernel/IInspector.h"
// FDetDesc 
#include "DetDesc/SolidFactory.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 

/// ===========================================================================
/** @file SolidBox.cpp
 *
 *  implementation of class SolidBox 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru  
 *  @date xx/xx/xxxx
 */
/// ===========================================================================

/// ===========================================================================
/// Factory busness 
/// ===========================================================================
static const  SolidFactory<SolidBox>         s_Factory;
const        ISolidFactory&SolidBoxFactory = s_Factory;
/// ===========================================================================

/// ===========================================================================
/** standard(public)  constructor 
 *  @param      Name           name of this box  
 *  @param      xHalf          half-size of the box in x-direction
 *  @param      yHalf          half-size of the box in y-direction
 *  @param      zHalf          half-size of the box in z-direction
 *  @exception  SolidException wrong parameters range 
 */
/// ===========================================================================
SolidBox::SolidBox( const std::string& Name  ,
                    const double       xHalf , 
                    const double       yHalf , 
                    const double       zHalf )
  : SolidBase        ( Name  ) 
  , m_box_xHalfLength( xHalf )
  , m_box_yHalfLength( yHalf )
  , m_box_zHalfLength( zHalf )
{
  if( 0 >= xHalfLength() )
    { throw SolidException("SolidBox(): XHalfLength is non positive! "); }
  if( 0 >= yHalfLength() )
    { throw SolidException("SolidBox(): YHalfLength is non positive! "); }
  if( 0 >= zHalfLength() )
    { throw SolidException("SolidBox(): ZHalfLength is non positive! "); }
};

/// ===========================================================================
/** default (protected) constructor 
 */
/// ===========================================================================
SolidBox::SolidBox()
  : SolidBase        ("anonymous box")
  , m_box_xHalfLength( 100000.0 ) 
  , m_box_yHalfLength( 100000.0 ) 
  , m_box_zHalfLength( 100000.0 ) 
{};

/// ===========================================================================
/// destructor 
/// ===========================================================================
SolidBox::~SolidBox(){ reset(); };

/// ===========================================================================
/** - serialization for reading
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param      s               reference to stream buffer
 *  @exception  SolidException  wrong parameters range 
 *  @return reference to stream buffer
 */
/// ===========================================================================
StreamBuffer& SolidBox::serialize( StreamBuffer& s )       
{
  /// 
  reset();
  /// serialize the base class 
  SolidBase::serialize( s ) ;  
  /// serialize own data 
  s >> m_box_xHalfLength 
    >> m_box_yHalfLength 
    >> m_box_zHalfLength ;
  ///
  if( 0 >= xHalfLength() )
    { throw SolidException("SolidBox: XHalfLength is non positive!"); }
  if( 0 >= yHalfLength() )
    { throw SolidException("SolidBox: YHalfLength is non positive!"); }
  if( 0 >= zHalfLength() )
    { throw SolidException("SolidBox: ZHalfLength is non positive!"); }
  ///
  return s ;
};

/// ===========================================================================
/** - serialization for writing
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
/// ===========================================================================
StreamBuffer& SolidBox::serialize( StreamBuffer& s ) const 
{
  /// serialize the base class 
  SolidBase::serialize( s ) ;
  /// serialize own data 
  return s << xHalfLength() 
           << yHalfLength() 
           << zHalfLength() ; 
}; 

/// ===========================================================================
/** calculate the intersection points("ticks") with a given line. 
 *  Input - line, paramterised by  x_vect = Point + Vector * T 
 *  "tick" is just a value of T, at which the intersection occurs
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points (=size of Ticks container)
 */
/// ===========================================================================
inline unsigned int 
SolidBox::intersectionTicks( const HepPoint3D & point  ,
                             const HepVector3D& vect   ,
                             ISolid::Ticks    & ticks  ) const
{  ///
  ticks.clear(); 
  /// find intersection ticks with x-planes
  SolidTicks::LineIntersectsTheX( point , vect ,        xHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheX( point , vect , -1.0 * xHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  /// find intersection ticks with y-planes
  SolidTicks::LineIntersectsTheY( point , vect ,        yHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheY( point , vect , -1.0 * yHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  /// find intersection ticks with z-planes
  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  ///  
};

/// ===========================================================================
/** printout to STD/STL stream    
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */ 
/// ===========================================================================
std::ostream& SolidBox::printOut  ( std::ostream&  os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return
    os << "[" 
       << " xsize[mm]=" << std::setw(12) << xsize() / millimeter
       << " ysize[mm]=" << std::setw(12) << ysize() / millimeter 
       << " zsize[mm]=" << std::setw(12) << zsize() / millimeter << "]" ;
};

/// ===========================================================================
/** printout to Gaudi  stream    
 *  @param os Gaudi stream
 *  @return reference to the stream 
 */ 
/// ===========================================================================
MsgStream&    SolidBox::printOut  ( MsgStream&     os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  return 
    os << "[" 
       << " xsize[mm]=" << std::setw(12) << xsize() / millimeter
       << " ysize[mm]=" << std::setw(12) << ysize() / millimeter 
       << " zsize[mm]=" << std::setw(12) << zsize() / millimeter << "]" ;
};

/// ===========================================================================
/** - retrieve the pointer to "simplified" solid - "cover"
 *  - implementation of ISolid abstract interface 
 *  The Box is the most simple shape
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
/// ===========================================================================
const ISolid* SolidBox::cover() const { return this; };

/// ===========================================================================
/** - retrieve the pointer to "the most simplified cover" 
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::coverTop() 
 *  The Box is the most simple shape
 *  @see ISolid 
 *  @see SolidBase 
 *  @return pointer to the most simplified cover 
 */
/// ===========================================================================
const ISolid* SolidBox::coverTop () const { return this; };


/// ===========================================================================
