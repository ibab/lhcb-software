#ifndef      __DETDESC_GEOMETRYINFO_GEOMTRYINFO_RELATIONS_H__
#define      __DETDESC_GEOMETRYINFO_GEOMTRYINFO_RELATIONS_H__ 1 




///
///  implementation of some inline methods from GeometryInfo class
///
/// Author: Vanya Belyaev
/// 


//
//
//


// pointer to the parent IGeometryInfo
inline        IGeometryInfo* GeometryInfo::parentIGeometryInfo()       
{
  if( m_gi_parentLoaded ) { return m_gi_parent; } 
  Assert( 0 != m_gi_iDetectorElement , "GeometryInfo:: IDetectorElement is not available!" );  
  IDetectorElement* ide = m_gi_iDetectorElement->parentIDetectorElement(); 
  Assert( 0 != ide                   , "GeometryInfo:: parent           is not available!" );  
  if( 0 != ide ) { m_gi_parent = ide->geometry() ; } 
  Assert( 0 != m_gi_parent           , "GeometryInfo:: parent geometry  is not available!" );  
  m_gi_parentLoaded = true; 
  return m_gi_parent;
}; 

//
// pointer to the parent IGeometryInfo (const version)
inline  const IGeometryInfo* GeometryInfo::parentIGeometryInfo() const 
{
  if( m_gi_parentLoaded ) { return m_gi_parent; } 
  Assert( 0 != m_gi_iDetectorElement , "GeometryInfo:: IDetectorElement is not available!" );  
  IDetectorElement* ide = m_gi_iDetectorElement->parentIDetectorElement(); 
  Assert( 0 != ide                   , "GeometryInfo:: parent           is not available!" );  
  const IGeometryInfo* igi = ide->geometry(); 
  Assert( 0 != igi                   , "GeometryInfo:: parent geometry  is not available!" );  
  return igi;          
}

//
// (reference to) container of children IGeometryInfo 
inline       IGeometryInfo::IGIChildrens& GeometryInfo::childIGeometryInfos()     
{
  if( !m_gi_childLoaded ) { loadChildren(); } 
  return m_gi_childrens; 
};

//
// (reference to) container of children IGeometryInfo 
inline const IGeometryInfo::IGIChildrens& GeometryInfo::childIGeometryInfos() const      
{
  if( !m_gi_childLoaded ) { loadChildren(); } 
  return m_gi_childrens; 
};


//
// begin iterator 
inline IGeometryInfo::IGIChildrens::iterator  GeometryInfo::childBegin() 
{
  if( !m_gi_childLoaded ) {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
  return m_gi_childrens.begin(); 
}; 

//
// begin iterator (const version)
inline IGeometryInfo::IGIChildrens::const_iterator  GeometryInfo::childBegin() const 
{
  if( !m_gi_childLoaded ) {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
  return m_gi_childrens.begin(); 
}; 

//
// end  iterator 
inline IGeometryInfo::IGIChildrens::iterator  GeometryInfo::childEnd()  
{
  if( !m_gi_childLoaded ) {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
  return m_gi_childrens.end(); 
};

//
// end  iterator (const version) 
inline IGeometryInfo::IGIChildrens::const_iterator  GeometryInfo::childEnd()  const 
{
  if( !m_gi_childLoaded ) {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
  return m_gi_childrens.end(); 
};



#endif       __DETDESC_GEOMETRYINFO_GEOMTRYINFO_RELATIONS_H__
