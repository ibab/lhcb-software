#ifndef     __DETDESC_GEOMETRYINFO_GEOMETRYINFO_INLINE_H__
#define     __DETDESC_GEOMETRYINFO_GEOMETRYINFO_INLINE_H__ 1 




#include "GeometryInfo.h"
#include "GeometryInfo.icpp"


///
inline MsgStream& operator<<( MsgStream& os , const GeometryInfo& gi ) { return gi.printOut(os); };
///
inline MsgStream& operator<<( MsgStream& os , const GeometryInfo* gi ) 
{ return  ( (0 == gi) ?  (os << "GeometryInfo* points to NULL") : gi->printOut(os) ); };
///
inline bool GeometryInfo::hasLVolume () const { return m_gi_has_logical; };
inline bool GeometryInfo::hasSupport () const { return m_gi_has_support; } 
///
inline const HepTransform3D& GeometryInfo::matrix()  const 
{
  ///
  if( 0 != m_gi_matrix ) { return *m_gi_matrix                                                        ; }   // matrix exists
  ///
  try                    { m_gi_matrix = calculateMatrix()                                            ; }   // calculate matrix
  catch ( const GaudiException&  GE )   
    { Assert( false , "GeometryInfo::matrix() : exception caught" , GE    ) ; } 
  catch (...)   
    { Assert( false , "GeometryInfo::matrix() : unknown exception caught" ) ; } 
  ///
  if( 0 == m_gi_matrix ) { m_gi_matrix = new HepTransform3D() ; } 
  ///
  Assert( 0 != m_gi_matrix , "GeometryInfo::matrix() : unable to calculate matrix!" );
  ///  
  return *m_gi_matrix;
};
//
inline const HepTransform3D& GeometryInfo::matrixInv() const 
{
  if( 0 != m_gi_matrixInv )   { return *m_gi_matrixInv             ; }      // matrix is calculated
  m_gi_matrixInv = new HepTransform3D( matrix().inverse() );     // calculate matrix 
  if( 0 == m_gi_matrixInv )   { m_gi_matrixInv = new HepTransform3D; }      // default value 
  return *m_gi_matrixInv;
};
///
inline HepPoint3D GeometryInfo::toLocal ( const HepPoint3D& GlobalPoint ) const { return ( matrix   () * GlobalPoint ); };
///
inline HepPoint3D GeometryInfo::toGlobal( const HepPoint3D& localPoint  ) const { return ( matrixInv() * localPoint  ); };
///
inline bool GeometryInfo::isInside ( const HepPoint3D& globalPoint )  const
{
  //
  if( hasLVolume() && 0 != lvolume() ) { return lvolume()->isInside( toLocal( globalPoint ) ); }
  //
  return  isInsideDaughter( globalPoint ) ;
};       
///
inline const std::string&  GeometryInfo::lvolumeName() const { return m_gi_lvolumeName; };
///
inline ILVolume* GeometryInfo::lvolume    () const  
{
  if( !hasLVolume() )     { return  0              ; }     
  if( 0 != m_gi_lvolume ) { return  m_gi_lvolume          ; } 
  try                     { m_gi_lvolume  = findLogical() ; }
  catch ( const GaudiException& Exception ) 
    { Assert( false , "GeometryInfo::lvolume() : exception caught" , Exception ) ; } 
  catch (...)           
    { Assert( false , "GeometryInfo::lvolume() : unknown exception caught" ) ; } 
  ///
  Assert( 0 != m_gi_lvolume , "GeometryInfo::lvolume() : unable to load ILVolume!" );
  ///
  /// !! gi_has_volume = false                                      // NB!!!!! ATTENTION!!!!
  ///
  return m_gi_lvolume; 
}; 
// look for daughter to which the given point belongs to 
inline std::string GeometryInfo::belongsToPath( const HepPoint3D& globalPoint )
{
  if( !isInside( globalPoint )                     ) { return std::string(""); } 
  if( !childLoaded() && loadChildren().isFailure() ) { return std::string(""); }  
  //
  IGeometryInfo::IGIChildrens::iterator it  =                                  // look through children
    std::find_if( childBegin() , childEnd  () , IGeometryInfo_isInside(globalPoint) ) ;
  //
  return ( childEnd() == it ) ? std::string("") : *(m_gi_childrensNames.begin()+(it-childBegin())); 
};
// look for daughter to which the given point belongs to 
inline IGeometryInfo* GeometryInfo::belongsTo ( const HepPoint3D& globalPoint )
{
  if( !isInside( globalPoint )                     ) { return 0; } 
  if( !childLoaded() && loadChildren().isFailure() ) { return 0; }   
  //
  IGeometryInfo::IGIChildrens::iterator it  = 
    std::find_if( childBegin() , childEnd  () , IGeometryInfo_isInside(globalPoint) ) ;
  //
  return ( childEnd() == it ? 0 : *it );   
};
// be careful! use first "isInside" method! 
inline std::string    GeometryInfo::belongsToPath( const HepPoint3D& globalPoint , 
						   const int         level       )
{
  if( 0 == level ){  return detElem()->name() ; } 
  IGeometryInfo* gi =  belongsTo( globalPoint );
  return ( 0 == gi ) ? detElem()->name()     : gi->belongsToPath( globalPoint , level - 1 ) ; 
  //
};
// be careful! use first "isInside" method! 
inline IGeometryInfo* GeometryInfo::belongsTo    ( const HepPoint3D& globalPoint , 
						   const int         level       )
{
  if( level == 0 ) { return this;  }                // do not look throug  daughters!
  IGeometryInfo* gi = belongsTo( globalPoint );
  return  ( ( 0 == gi ) ? this : gi->belongsTo( globalPoint , level - 1 ) ); 
};
///





#endif   // __DETDESC_GEOMETRYINFO_GEOMETRYINFO_INLINE_H__









