/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
// GaudiKernel
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/StreamBuffer.h" 
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/IPVolume_predicates.h"
#include "DetDesc/DetDesc.h"
// STD and STL 
#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
#include <numeric> 
// local 
#include "GeometryInfo.h" 
#include "GeometryInfoException.h" 

// ============================================================================
/** @file GeometryInfo.cpp
 *  implementation of class GeometryInfo 
 *  @see GeoemtryInfo.h 
 *  @see GeometryInfo.icpp
 *  @see IGeometryInfo 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

unsigned long GeometryInfo::m_count = 0;

IDataProviderSvc* GeometryInfo::dataSvc() 
{ return DetDesc::detSvc(); }

// create "ghost" 
GeometryInfo::GeometryInfo( IDetectorElement*            de      ) 
  : m_gi_has_logical      (     false   )
  , m_gi_lvolumeName      (      ""     )
  , m_gi_lvolume          (       0     )
  , m_gi_matrix           (       0     )
  , m_gi_matrixInv        (       0     ) 
  , m_gi_has_support      (    false    )
  , m_gi_supportName      (      ""     )
  , m_gi_support          (       0     ) 
  , m_gi_supportPath      (             )
  , m_gi_supportNamePath  (      ""     )
  , m_gi_iDetectorElement (      de     )
  , m_gi_childLoaded      (    false    ) 
  , m_gi_childrens        (             ) 
  , m_gi_childrensNames   (             )
{
  if( 0 == de  ) 
    { throw GeometryInfoException("IDetectorElement* points to NULL!") ; }
  addRef();
};
// create orphan 
GeometryInfo::GeometryInfo( IDetectorElement*            de          ,
                            const std::string&           LogVol      ) 
  : m_gi_has_logical      (    true     )
  , m_gi_lvolumeName      (   LogVol    )
  , m_gi_lvolume          (       0     )
  , m_gi_matrix           (       0     )
  , m_gi_matrixInv        (       0     ) 
  , m_gi_has_support      (    false    )
  , m_gi_supportName      (      ""     )
  , m_gi_support          (       0     ) 
  , m_gi_supportPath      (             )
  , m_gi_supportNamePath  (      ""     )
  , m_gi_iDetectorElement (      de     )
  , m_gi_parentLoaded     (    false    )
  , m_gi_parent           (      0      )
  , m_gi_childLoaded      (    false    ) 
  , m_gi_childrens        (             ) 
  , m_gi_childrensNames   (             )
{
  if( 0 == de  ) 
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }
  addRef();
};
/// create regular  with numeric replica path 
GeometryInfo::GeometryInfo( IDetectorElement*            de          ,
                            const std::string&           LogVol      , 
                            const std::string&           Support     ,
                            const ILVolume::ReplicaPath& replicaPath )
  : m_gi_has_logical      (    true     )
  , m_gi_lvolumeName      (   LogVol    )
  , m_gi_lvolume          (       0     )
  , m_gi_matrix           (       0     )
  , m_gi_matrixInv        (       0     ) 
  , m_gi_has_support      (    true     )
  , m_gi_supportName      (   Support   )
  , m_gi_support          (       0     ) 
  , m_gi_supportPath      ( replicaPath )
  , m_gi_supportNamePath  (      ""     )
  , m_gi_iDetectorElement (      de     )
  , m_gi_parentLoaded     (    false    )
  , m_gi_parent           (      0      )
  , m_gi_childLoaded      (    false    ) 
  , m_gi_childrens        (             ) 
  , m_gi_childrensNames   (             )
{
  if( 0 == de  ) 
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }
  addRef();
};  
/// create regular  with name path 
GeometryInfo::GeometryInfo( IDetectorElement*            de              ,
                            const std::string&           LogVol          , 
                            const std::string&           Support         ,
                            const std::string&           ReplicaNamePath ) 
  : m_gi_has_logical      (    true         )
  , m_gi_lvolumeName      (   LogVol        )
  , m_gi_lvolume          (       0         )
  , m_gi_matrix           (       0         )
  , m_gi_matrixInv        (       0         ) 
  , m_gi_has_support      (    true         )
  , m_gi_supportName      (   Support       )
  , m_gi_support          (       0         ) 
  , m_gi_supportPath      (                 )
  , m_gi_supportNamePath  ( ReplicaNamePath )
  , m_gi_iDetectorElement (      de         )
  , m_gi_parentLoaded     (    false        )
  , m_gi_parent           (      0          )
  , m_gi_childLoaded      (    false        ) 
  , m_gi_childrens        (                 ) 
  , m_gi_childrensNames   (                 )
{
  if( 0 == de  ) 
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }
  addRef();
};
//
StreamBuffer& GeometryInfo::serialize( StreamBuffer& sb ) 
{
  ///
  reset() ; 
  ///
  unsigned long tmp1, tmp2 ; 
  ///
  sb >> tmp1 ;   m_gi_has_logical = tmp1 ? true : false ;
  sb >> m_gi_lvolumeName ; 
  sb >> tmp2 ;   m_gi_has_support = tmp2 ? true : false ;
  if( m_gi_has_support ) 
    { 
      sb >> m_gi_supportName     ;
      sb >> m_gi_supportNamePath ; 
      {
        unsigned long n ;
        unsigned long i ;
        sb >> n ; m_gi_supportPath.clear()   ; 
        while( n-- > 0 ) { sb >> i ; m_gi_supportPath.push_back( i ) ; }
      }
    } 
  ///
  return sb;
};
//
StreamBuffer& GeometryInfo::serialize( StreamBuffer& sb ) const 
{
  ///
  sb << (unsigned long) m_gi_has_logical 
     <<                 m_gi_lvolumeName 
     << (unsigned long) m_gi_has_support ;
  if( m_gi_has_support ) 
    { 
      sb << m_gi_supportName  ;
      sb << m_gi_supportNamePath ; 
      ///
      sb << m_gi_supportPath.size() ; 
      for( unsigned i = 0 ; i < m_gi_supportPath.size() ; ++i ) 
        { sb << m_gi_supportPath[i] ; }
      ///
    }
  return sb;
};
/// IInspectable 
bool GeometryInfo::acceptInspector( IInspector* pInspector ) 
{
  return 0 != pInspector? true : false ;
};
//
bool GeometryInfo::acceptInspector( IInspector* pInspector ) const  
{
  return 0 != pInspector? true : false ;
};
//
GeometryInfo::~GeometryInfo()
{
  reset();
  release();
};
//
StatusCode  GeometryInfo::reset( const int Level )
{
  //
  m_gi_lvolume = 0 ; 
  //
  if( 0 != m_gi_matrix    ){ delete m_gi_matrix   ; m_gi_matrix    = 0; }  
  if( 0 != m_gi_matrixInv ){ delete m_gi_matrixInv; m_gi_matrixInv = 0; }  
  //
  m_gi_support = 0;
  //
  std::for_each( m_gi_childrens.begin () ,
                 m_gi_childrens.end   () ,
                 std::mem_fun(&IGeometryInfo::reset) );
  m_gi_childLoaded = false; 
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  //
  return StatusCode::SUCCESS;
};
//
std::ostream& GeometryInfo::printOut( std::ostream& os ) const
{
  os << "GeometryInfo::name=" <<  detElem()->name() << ";" ; 
  if( !hasLVolume() ) { os << " no LVolume associated ('ghost');"; }
  else 
    { 
      os << " LVolume=" << lvolumeName() << ";";
      if( !hasSupport() ) { os << "('orphan');"; } 
      else
        {
          os << " supported by " << m_gi_supportName << " with ReplicaPath=("; 
          std::copy( m_gi_supportPath.begin() , 
                     m_gi_supportPath.end() , 
                     std::ostream_iterator<ILVolume::ReplicaType>(os,",") );
          os << ") ";
          os << "(" << m_gi_supportNamePath << ");";
        }
    }
  //
  return os << std::endl;
};
//
MsgStream& GeometryInfo::printOut( MsgStream& os ) const
{
  os << "GeometryInfo::name=" << detElem()->name() << ";" ; 
  if( !hasLVolume() ) { os << " no LVolume associated ('ghost');"; }
  else 
    { 
      os << " LVolume=" << lvolumeName() << ";";
      if( !hasSupport() ) { os << "('orphan');"; } 
      else
        {
          os << " supported by " << m_gi_supportName << " with ReplicaPath=("; 
          ILVolume::ReplicaPath::iterator ci = supportPath().begin(); 
          while( supportPath().end() != ci ) { os << *ci++ << "," ; }
          os << ") ";
          os << "(" << m_gi_supportNamePath << ");";
        }
    }
  //
  return os << endreq ;
};
//
ILVolume* GeometryInfo::findLogical() const 
{
  ILVolume* lv = 0;
  if( !hasLVolume() ) { return lv; }
  //
  try   
    { 
      SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
      if( 0 != ptr ) { lv = ptr ; }
    }
  catch ( const GaudiException& Exception ) 
    {  Assert( false , 
               "GeometryInfo::findLogical(), exception in retrieving LV=" + 
               lvolumeName() , Exception ); }   ///  throw new exception 
  catch (...)
    {  Assert( false , 
               "GeometryInfo::findLogical(), exception in retrieving LV=" + 
               lvolumeName() ); }   ///  throw new exception 
  ///
  Assert( 0 != lv , 
          "GeometryInfo::findLogical(), unable to retrieve LV=" + 
          lvolumeName() ) ; 
  ///  
  return lv;  
};
//
IGeometryInfo* GeometryInfo::geoByName( const std::string& name ) const 
{
  IGeometryInfo* gi = 0;
  try         
    { 
      SmartDataPtr<IDetectorElement> ptr( dataSvc() , name );
      if( 0 != ptr )      { gi = ptr->geometry(); } 
    }
  catch ( const GaudiException& Exception ) 
    {  Assert( false , 
               "GeometryInfo::findLogical(), exception in retrieving IGI=" + 
               name , Exception ) ; } ///  throw new exception 
  catch ( ... ) 
    {  Assert( false ,
               "GeometryInfo::findLogical(), exception in retrieving IGI=" + 
               name     ) ; } ///  throw new exception 
  ///
  Assert( 0 != gi ,
          "GeometryInfo::geoByName(), unable to retrieve IGeometryInfo="
          + name ) ;
  ///
  return gi; 
};
///
StatusCode GeometryInfo::loadChildren() const 
{
  ///
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();  
  m_gi_childLoaded = false; 
  ///  
  Assert( 0 != m_gi_iDetectorElement, 
          "GeometryInfo::loadChildren IDetectorElement is not availble!");
  ///
  IDetectorElement::IDEContainer& IDEchildrens = 
    m_gi_iDetectorElement->childIDetectorElements(); 
  IDetectorElement::IDEContainer::iterator it = IDEchildrens.begin(); 
  while( IDEchildrens.end() != it ) 
    {
      IDetectorElement* ide = *it++; 
      try{ 
        IGeometryInfo* igi   = 0 ; 
        DataObject*    pObj  = 0 ; 
        std::string    path("")  ;  
        if( 0 != ide  ) { igi  = ide->geometry();                }
        if( 0 != ide  ) { pObj = dynamic_cast<DataObject*>(ide); }
        if( 0 != pObj ) { path = pObj->fullpath();               }
        if( 0 != igi &&  0 != path.size() )
          { m_gi_childrensNames.push_back( path ); 
          m_gi_childrens.push_back( igi  ); }    
      }
      catch( const GaudiException& Exception ) 
        {  Assert( false , 
                   "GeometryInfo::loadChildren(): exception caught!" , 
                   Exception ) ; }   ///  throw new exception 
      catch(...)                   
        {  Assert( false , 
                   "GeometryInfo::loadChildren(): unknown exception caught!") ; 
        }   ///  throw new exception 
    } 
  //
  m_gi_childLoaded = true; 
  //
  return StatusCode::SUCCESS;
};
//
HepTransform3D* GeometryInfo::calculateMatrix() const 
{
  //
  if( 0 != m_gi_matrix ) { delete m_gi_matrix; m_gi_matrix = 0; }
  //
  if( !hasLVolume()  ) 
    { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }
  if( !hasSupport()  ) 
    { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }
  //
  ///
  IGeometryInfo* gi = 0;
  ILVolume::ReplicaPath replicaPath; 
  StatusCode sc = location( gi , replicaPath );
  //
  Assert( sc.isSuccess()     , 
          " GeometryInfo::calculateMatrix() - support is not found!");
  Assert( 0 != gi            , 
          " GeometryInfo::calculateMatrix() - support is not found!");
  Assert( 0 != gi->lvolume() , 
          " GeometryInfo::calculateMatrix() - support has no LVolume!");
  //
  if( this == ( const IGeometryInfo*) gi ) 
    { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }
  //
  // 
  ILVolume* lv = gi->lvolume();
  ILVolume::PVolumePath volumePath;
  StatusCode st = 
    lv->traverse( supportPath().begin() , 
                  supportPath().end  () , volumePath );
  //
  Assert( st.isSuccess()  , 
          " GeometryInfo::calculateMatrix() - could not traverse the path! ");
  //
  /** calculate "relative matrix" - empty path results in 
   *  "Identity" transformation! 
   */
  HepTransform3D  Mtrx,init; 
  Mtrx = std::accumulate( volumePath.begin() , 
                          volumePath.end  () , 
                          init               , 
                          IPVolume_accumulateMatrix() );
  //
  //  calculate the total matrix 
  //  ( forces( via call of matrix() ) calculation of matrix for "gi" )
  HepTransform3D* pM = 0;
  try   { pM = new  HepTransform3D(  Mtrx*(gi->matrix()) );}
  catch ( const GaudiException& Exception )
    {  Assert( false , 
               "GeometryInfo::calculateMatrix(): exception caught!" , 
               Exception ) ; }   ///  throw new exception 
  catch (...)                
    {  Assert( false , 
               "GeometryInfo::calculateMatrix(): unknown exception caught!") ; 
    }   ///  throw new exception 
  ///
  return pM;
  //
};
///



/** query the interface
 *  @param ID interface identifier
 *  @param ppI placeholder for returned interface
 *  @return status code
 */ 
StatusCode GeometryInfo::queryInterface( const InterfaceID& ID , void** ppI ) 
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IGeometryInfo::interfaceID()  == ID ) 
    { *ppI = static_cast<IGeometryInfo*> ( this ) ; } 
  else if ( ISerialize:: interfaceID()    == ID )
    { *ppI = static_cast<ISerialize*>    ( this ) ; } 
  else if ( IInterface:: interfaceID()    == ID ) 
    { *ppI = static_cast<IInterface*>    ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE                  ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS; 
};

/// add reference
unsigned long GeometryInfo::addRef () { return ++m_count ; }
/// release 
unsigned long GeometryInfo::release() { return  0 < m_count ? --m_count : 0 ;}


