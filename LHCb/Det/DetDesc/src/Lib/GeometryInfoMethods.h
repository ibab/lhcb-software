#ifndef    __DETDESC_GEOMETRYINFO_GEOMETRYINFO_METHODS_H__
#define    __DETDESC_GEOMETRYINFO_GEOMETRYINFO_METHODS_H__ 1 


#include "GeometryInfo.h"

///
///  implementation of some methods from GeometryInfo class 
///
/// Author Vanya Belyaev 
///

//
//
//
inline StatusCode GeometryInfo::fullGeoInfoForPoint( const HepPoint3D&        point      , 
						     const int                level      , 
						     IGeometryInfo*&          start      , 
						     ILVolume::PVolumePath&   volumePath )
{
  // reset output values 
  start = 0         ; 
  if( !volumePath.empty() )            { volumePath.clear();          }
  if( !isInside( point )  )            { return StatusCode::FAILURE ; }       // RETURN
  //
  IGeometryInfo* gi = belongsTo( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() )  { return StatusCode::FAILURE ; }       // RETURN 
  start = gi;
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point )  , level , volumePath ); 
};

//
//
//
inline StatusCode GeometryInfo::fullGeoInfoForPoint( const HepPoint3D&        point      , 
						     const int                level      , 
						     std::string&             start      , 
						     ILVolume::PVolumePath&   volumePath )
{
  // reset output values 
  start = std::string("");
  if( !volumePath.empty() ) { volumePath.clear()       ; } 
  if( !isInside( point ) ) { return StatusCode::FAILURE; }  
  //
  IGeometryInfo* gi = belongsTo    ( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() ) { return StatusCode::FAILURE ; }
  start             = belongsToPath( point , -1 );
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point ) , level , volumePath ); 
  //
};

//
//
//
inline StatusCode GeometryInfo::fullGeoInfoForPoint( const HepPoint3D&        point      , 
						     const int                level      , 
						     IGeometryInfo*&          start      , 
						     ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values 
  start = 0         ; 
  if( !replicaPath.empty() )            { replicaPath.clear();         }
  if( !isInside( point )   )            { return StatusCode::FAILURE ; }       // RETURN
  //
  IGeometryInfo* gi = belongsTo( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() )   { return StatusCode::FAILURE ; }       // RETURN 
  start = gi; 
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point )  , level , replicaPath ); 
};

//
//
//
inline StatusCode GeometryInfo::fullGeoInfoForPoint( const HepPoint3D&        point      , 
						     const int                level      , 
						     std::string&             start      , 
						     ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values 
  start = std::string("");
  if( !replicaPath.empty() ) { replicaPath.clear()       ; }
  if( !isInside( point )   ) { return StatusCode::FAILURE; }  
  //
  IGeometryInfo* gi = belongsTo    ( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() ) { return StatusCode::FAILURE ; }
  start             = belongsToPath( point , -1 );
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point ) , level , replicaPath ); 
};



//
//
//
inline ILVolume* GeometryInfo::lvolume    ( IGeometryInfo*               start       , 
					    const ILVolume::ReplicaPath& replicaPath )
{
  if( 0 == start || 0 == start->lvolume() ){ return 0; }  // starting point for address is invalid! 
  //
  ILVolume* lv = start->lvolume(); 
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
		    replicaPath.end  ()  , 
		    volumePath           ).isFailure() ) { return 0; } 
  //
  if( volumePath.empty() ) { return lv; } 
  //
  IPVolume* pv = *(volumePath.rbegin()); // get "the last" element 
  //
  return ( 0 == pv ? 0 : pv->lvolume() );  
};

//
//
//
inline std::string GeometryInfo::lvolumePath( IGeometryInfo*               start       , 
					      const ILVolume::ReplicaPath& replicaPath )
{
  // starting point for address is invalid! 
  if( 0 == start || 0 == start->lvolume() ){ return std::string("") ; } 
  //
  ILVolume* lv = start->lvolume(); 
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
		    replicaPath.end  ()  , 
		    volumePath           ).isFailure() ) { return 0; } 
  //
  if( volumePath.empty() ) { return start->lvolumeName(); } 
  //
  IPVolume* pv = *(volumePath.rbegin()); // get "the last" element 
  //
  return ( 0 == pv ? std::string("") : pv->lvolumeName() );  
};


//
//
//
//
//
//
inline StatusCode GeometryInfo::location   ( std::string&                 start , 
					     ILVolume::ReplicaPath&       replicaPath ) const
{
  start = m_gi_supportName;
  replicaPath.clear();
  replicaPath.insert( replicaPath.end() , supportPath().begin() , supportPath().end() ); 
  //
  return StatusCode::SUCCESS;
};

//
//
//


inline std::string GeometryInfo::lvolumePath( const std::string&           start       , 
					      const ILVolume::ReplicaPath& replicaPath )
{
  if( std::string("") == start ) { return std::string(""); }
  //
  IGeometryInfo* gi = geoByName( start );
  //
  return 0 == gi ? std::string("") : lvolumePath( gi , replicaPath );
};

//
//
//

inline ILVolume* GeometryInfo::lvolume    ( const std::string&           start       , 
					    const ILVolume::ReplicaPath& replicaPath )
{
  if( std::string("") == start ) { return 0; }
  //
  IGeometryInfo* gi = geoByName( start ); 
  //
  return 0 == gi ? 0 : lvolume( gi , replicaPath );
};

//
//
//

inline StatusCode GeometryInfo::location   ( IGeometryInfo*&              start , 
					     ILVolume::ReplicaPath&       replicaPath ) const
{
  replicaPath.clear();
  replicaPath.insert( replicaPath.end() , supportPath().begin() , supportPath().end() ); 
  //
  //
  if( !hasSupport() )  { start = 0; return StatusCode::SUCCESS; }  
  //
  //
  if( 0 == m_gi_support )
    {
      IGeometryInfo* gi = 0;
      if( std::string("") != m_gi_supportName ) { gi = geoByName( m_gi_supportName ); }
      if( 0 !=  gi ) { m_gi_support = gi ; }
    }
  //
  start = m_gi_support; 
  if( 0 == start ) { replicaPath.clear(); } 
  //
  return  ( ( 0 != start ) ? StatusCode::SUCCESS : StatusCode::FAILURE );  
};


//
//
// 


// retrive the replica path (misterious "rpath")  
inline       ILVolume::ReplicaPath& GeometryInfo::supportPath() const      
{
  //
  if( !m_gi_has_support         )  { return m_gi_supportPath ; }  
  // if replica path is known already 
  if( !m_gi_supportPath.empty() )  { return m_gi_supportPath; } 
  // try to extract the replica path from its name (parse the name path) 
  ///
  if( 0 == m_gi_support ) { m_gi_support = geoByName( m_gi_supportName ) ; } 
  ///
  ILVolume* lv = m_gi_support->lvolume(); 
  std::string aux( m_gi_supportNamePath ); 
  while( 0 != aux.size() && 0 != lv ) 
    {
      // find separator 
      std::string::size_type pos     = aux.find_first_of('/');  
      std::string            physVol ("") ; 
      if( std::string::npos == pos )  
	{ physVol  = aux                   ; aux=""                       ; }
      else 
	{ physVol = aux.substr( 0 , pos )  ; aux = aux.substr( pos + 1  ) ; } 
      if( 0 != physVol.size() ) 
	{
	  // get physical volume by name
	  IPVolume* pv = (*lv)[ physVol ]; 
	  // find this name in the sequence 
	  ILVolume::PVolumes::const_iterator it = std::find( lv->pvBegin() , lv->pvEnd() , pv ) ; 
	  Assert( lv->pvEnd() != it , "GeometryInfo:: fatal error during retrieve of Replica Path" );
	  // extract index and put it into replica path   
	  m_gi_supportPath.push_back( it - lv->pvBegin() ); 
	}
    } 
  //
  return m_gi_supportPath; 
  //
};

///
///
/// reset to the initial state 
inline const IGeometryInfo*  GeometryInfo::reset() const 
{
  /// reset logical volume 
  m_gi_lvolume = 0; 

  /// reset matrices 
  if( 0 != m_gi_matrix          ) { delete m_gi_matrix    ; m_gi_matrix    = 0 ; } 
  if( 0 != m_gi_matrixInv       ) { delete m_gi_matrixInv ; m_gi_matrixInv = 0 ; } 

  /// reset support 
  m_gi_support      = 0      ; 
  
  /// reset parent 
  m_gi_parentLoaded = false  ;  
  m_gi_parent       = 0      ;  
  
  // reset childs
  // due to stupid NT-compiler we must to write the following ugly stuff instead of nice statement: 
  // std::for_each( childBegin() , childEnd() , std::mem_fun(&IGeometryInfo::reset) );
  if( m_gi_childLoaded ) 
    { 
      for( IGeometryInfo::IGIChildrens::const_iterator iter = childBegin() ; iter != childEnd() ; ++iter ) 
	{ if( 0 != (*iter) ) { (*iter)->reset();} } 
      
      m_gi_childrens.clear()      ; 
      m_gi_childrensNames.clear() ; 
      m_gi_childLoaded  = false   ;
    }
  ///
  ///  
  return this;
};

///
///
///


#endif  // __DETDESC_GEOMETRYINFO_GEOMETRYINFO_METHODS_H__

