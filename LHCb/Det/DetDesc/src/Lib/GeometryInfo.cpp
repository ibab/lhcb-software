/// STD and STL 
#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
#include <numeric> 
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
/// GaudiKernel
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/TransientStore.h" 
#include "GaudiKernel/StreamBuffer.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
/// DetDesc 
#include "DetDesc/IPVolume_predicates.h"
/// local 
#include "GeometryInfo.h" 
#include "GeometryInfoException.h" 


// create "ghost" ////////////////////////////////////////////////////////////////////////////////////////////
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
  , m_gi_dataSvc          (   0         )
  , m_gi_messSvc          (   0         )
{
  if( 0 == de  ) { throw GeometryInfoException("Constructor: IDetectorElement* points to NULL!") ; }
  ISvcLocator* svc = Gaudi::svcLocator(); 
  if( 0 == svc ) { throw GeometryInfoException("Constructor: ISvsLoctaor*      points to NULL!") ; }
  svc->addRef();
  //
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svc->service( tmp , m_gi_dataSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == dataSvc() ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svc->service( tmp , m_gi_messSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == msgSvc () ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    msgSvc()->addRef();
  }
  ///
};
// create orphan ///////////////////////////////////////////////////////////////////////////////////////////
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
  , m_gi_dataSvc          (      0      )
  , m_gi_messSvc          (      0      )
{
  if( 0 == de  ) { throw GeometryInfoException("Constructor: IDetectorElement* points to NULL!"    ) ; }
  ISvcLocator* svc = Gaudi::svcLocator(); 
  if( 0 == svc ) { throw GeometryInfoException("Constructor: ISvsLoctaor*      points to NULL!"    ) ; }
  svc->addRef();
  //
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svc->service( tmp , m_gi_dataSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == dataSvc() ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svc->service( tmp , m_gi_messSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == msgSvc () ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    msgSvc()->addRef();
  }
  ///
};
/// create regular  with numeric replica path /////////////////////////////////////////////////////////////
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
  , m_gi_dataSvc          (      0      )
  , m_gi_messSvc          (      0      )
{
  if( 0 == de  ) { throw GeometryInfoException("Constructor: IDetectorElement* points to NULL!"    ) ; }
  ISvcLocator* svc = Gaudi::svcLocator(); 
  if( 0 == svc ) { throw GeometryInfoException("Constructor: ISvsLoctaor*      points to NULL!"    ) ; }
  svc->addRef();
  //
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svc->service( tmp , m_gi_dataSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == dataSvc() ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svc->service( tmp , m_gi_messSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == msgSvc () ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    msgSvc()->addRef();
  }
  ///
};  
/// create regular  with name path ///////////////////////////////////////////////////////////////////////
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
  , m_gi_dataSvc          (      0          )
  , m_gi_messSvc          (      0          )
{
  if( 0 == de  ) { throw GeometryInfoException("Constructor: IDetectorElement* points to NULL!"    ) ; }
  ISvcLocator* svc = Gaudi::svcLocator(); 
  if( 0 == svc ) { throw GeometryInfoException("Constructor: ISvsLoctaor*      points to NULL!"    ) ; }
  svc->addRef();
  //
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svc->service( tmp , m_gi_dataSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == dataSvc() ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svc->service( tmp , m_gi_messSvc ) ; 
    if( sc.isFailure() ) { throw GeometryInfoException("Constructor: could not locate "+tmp, 0 , sc ) ; }
    if( 0 == msgSvc () ) { throw GeometryInfoException("Constructor: could not locate "+tmp         ) ; }
    msgSvc()->addRef();
  }
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
      for( unsigned i = 0 ; i < m_gi_supportPath.size() ; ++i ) { sb << m_gi_supportPath[i] ; }
      ///
    }
  return sb;
};
/// IInspectable ///////////////////////////////////////////////////////////////////////////////////////////
bool GeometryInfo::acceptInspector( IInspector* pInspector ) 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef       (  lvolume()             , this , "HasLogical"  ); 
  ///
  const IGeometryInfo* gi = this; 
  return gi->acceptInspector( pInspector );
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool GeometryInfo::acceptInspector( IInspector* pInspector ) const  
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef       ( &m_gi_has_logical      , this , "HasLogical"  ); 
  pInspector->inspectByRef       ( &m_gi_lvolumeName      , this , "LogVolName"  );
  pInspector->inspectByRef       ( &m_gi_has_support      , this , "HasSupport"  );
  pInspector->inspectContByValue (  m_gi_supportPath      , this , "ReplicaPath" );
  pInspector->inspectContByValue (  m_gi_supportNamePath  , this , "NamePath"    );
  pInspector->inspectByRef       (  m_gi_iDetectorElement , this , "DetElement"  );
  ///
  return true; 
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
GeometryInfo::~GeometryInfo()
{
  //
  // StausCode sc = reset(0);
  // Assert( sc.isSuccess() , " GeometryInfo::Error in destructor " );
  //
};
////////////////////////////////////////////////////////////////////////////////////////////////////// 
StatusCode  GeometryInfo::reset( const int Level )
{
  //
  m_gi_lvolume = 0 ; 
  //
  if( 0 != m_gi_matrix    ){ delete m_gi_matrix   ; m_gi_matrix    = 0; }  
  if( 0 != m_gi_matrixInv ){ delete m_gi_matrixInv; m_gi_matrixInv = 0; }  
  //
  m_gi_support = 0;
  //  if( Level < 0 ) { m_gi_iDetectorElement = 0; };
  if( Level < 0 ) { ; };
  //
  m_gi_childLoaded = false; 
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  //
  return StatusCode::SUCCESS;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
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
	  std::copy( m_gi_supportPath.begin() , m_gi_supportPath.end() , std::ostream_iterator<ILVolume::ReplicaType>(os,",") );
	  os << ") ";
	  os << "(" << m_gi_supportNamePath << ");";
	}
    }
  //
  return os << std::endl;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////
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
    {  Assert( false , "GeometryInfo::findLogical(), exception caught in retrieving LV="+lvolumeName() , Exception ); }   ///  throw new exception 
  catch (...)
    {  Assert( false , "GeometryInfo::findLogical(), unknown exception caught in retrieving LV="+lvolumeName() ); }   ///  throw new exception 
  ///
  Assert( 0 != lv , "GeometryInfo::findLogical(), unable to retrieve LV="+lvolumeName() ) ; 
  ///  
  return lv;  
};
///////////////////////////////////////////////////////////////////////////////////////////////////
IGeometryInfo* GeometryInfo::geoByName( const std::string& name ) const 
{
  IGeometryInfo* gi = 0;
  try         
    { 
      SmartDataPtr<IDetectorElement> ptr( dataSvc() , name );
      if( 0 != ptr )      { gi = ptr->geometry(); } 
    }
  catch ( const GaudiException& Exception ) 
    {  Assert( false , "GeometryInfo::findLogical(), exception caught in retrieving IGI="+name , Exception ) ; } ///  throw new exception 
  catch ( ... ) 
    {  Assert( false , "GeometryInfo::findLogical(), unknown exception caught in retrieving IGI="+name     ) ; } ///  throw new exception 
  ///
  Assert( 0 != gi , "GeometryInfo::geoByName(), unable to retrieve IGeometryInfo, named="+name ) ;
  ///
  return gi; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////
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
	  { m_gi_childrensNames.push_back( path ); m_gi_childrens.push_back( igi  ); }    
      }
      catch( const GaudiException& Exception ) 
	{  Assert( false , "GeometryInfo::loadChildren(): exception caught!" , Exception ) ; }   ///  throw new exception 
      catch(...)                   
	{  Assert( false , "GeometryInfo::loadChildren(): unknown exception caught!"     ) ; }   ///  throw new exception 
    } 
  //
  m_gi_childLoaded = true; 
  //
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////////////////
HepTransform3D* GeometryInfo::calculateMatrix() const 
{
  //
  if( 0 != m_gi_matrix ) { delete m_gi_matrix; m_gi_matrix = 0; }
  //
  if( !hasLVolume()  ) { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }   // by agreement 
  if( !hasSupport()  ) { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }   // by agreement 
  //
  ///
  IGeometryInfo* gi = 0;
  ILVolume::ReplicaPath replicaPath; 
  StatusCode sc = location( gi , replicaPath );
  //
  Assert( sc.isSuccess()     , " GeometryInfo::calculateMatrix() - support is not found!");
  Assert( 0 != gi            , " GeometryInfo::calculateMatrix() - support is not found!");
  Assert( 0 != gi->lvolume() , " GeometryInfo::calculateMatrix() - support has no LVolume!");
  //
  if( this == ( const IGeometryInfo*) gi ) { m_gi_matrix = new HepTransform3D(); return m_gi_matrix; }    // SELFSUPPORT!!!!
  //
  // 
  ILVolume* lv = gi->lvolume();
  ILVolume::PVolumePath volumePath;
  StatusCode st = lv->traverse( supportPath().begin() , supportPath().end  () , volumePath );
  //
  Assert( st.isSuccess()  , " GeometryInfo::calculateMatrix() - could not traverse the path! ");
  //
  // calculate "relative matrix" - empty path results in "Identity" transformation! 
  HepTransform3D  Mtrx,init; 
  Mtrx = std::accumulate( volumePath.begin() , volumePath.end  ()          , 
                          init               , IPVolume_accumulateMatrix() );
  //
  //  calculate the total matrix 
  //  ( forces( via call of matrix() ) calculation of matrix for "gi" )
  HepTransform3D* pM = 0;
  try   { pM = new  HepTransform3D(  Mtrx*(gi->matrix()) );}
  catch ( const GaudiException& Exception )
    {  Assert( false , "GeometryInfo::calculateMatrix(): exception caught!" , Exception ) ; }   ///  throw new exception 
  catch (...)                
    {  Assert( false , "GeometryInfo::calculateMatrix(): unknown exception caught!"     ) ; }   ///  throw new exception 
  ///
  return pM;
  //
};
////////////////////////////////////////////////////////////////////////////////////////////////////////



