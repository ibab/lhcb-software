#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
#include <numeric> 


#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/TransientStore.h" 

#include "GaudiKernel/MsgStream.h"

#include "DetDesc/GeometryInfo.h" 

#include "DetDesc/IPVolume_predicates.h"


//
// constructors
// 

//
// create "ghost"
GeometryInfo::GeometryInfo( IDetectorElement*            de          , 
			    IDataProviderSvc*            datasvc , 
			    IMessageSvc*                 messsvc ) 
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
  , m_gi_dataSvc          (   datasvc   )
  , m_gi_messSvc          (   messsvc   )
{
  //
  //  MsgStream log( msgSvc(), "GeometryInfo::constructor" );
  //  log << MSG::VERBOSE << " Constructor 1 is invoked " << endreq;  
  Assert( 0 != m_gi_iDetectorElement , "Detector element points to NULL!" );
  //
};

//
//
//

// create orphan
GeometryInfo::GeometryInfo( IDetectorElement*            de          ,
			    const std::string&           LogVol      , 
			    IDataProviderSvc*            datasvc     , 
			    IMessageSvc*                 messsvc     ) 
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
  , m_gi_dataSvc          (   datasvc   )
  , m_gi_messSvc          (   messsvc   )
{
  //
  //  MsgStream log( msgSvc(), "GeometryInfo::constructor" );
  //  log << MSG::VERBOSE << " Constructor 2 is invoked " << endreq;  
  Assert( 0 != m_gi_iDetectorElement , "Detector element points to NULL!" );
  //
};

//
//
//

// create regular  with numeric replica path 
GeometryInfo::GeometryInfo( IDetectorElement*            de          ,
			    const std::string&           LogVol      , 
			    const std::string&           Support     ,
			    const ILVolume::ReplicaPath& replicaPath , 
			    IDataProviderSvc*            datasvc , 
			    IMessageSvc*                 messsvc ) // create "regular" 
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
  , m_gi_dataSvc          (   datasvc   )
  , m_gi_messSvc          (   messsvc   )
{
  //
  //  MsgStream log( msgSvc(), "GeometryInfo::constructor" );
  //  log << MSG::VERBOSE << " Constructor 3 is invoked " << endreq;  
  Assert( 0 != m_gi_iDetectorElement , "Detector element points to NULL!" );
  //
};
  
//
//
//

// create regular  with name path 
GeometryInfo::GeometryInfo( IDetectorElement*            de              ,
			    const std::string&           LogVol          , 
			    const std::string&           Support         ,
			    const std::string&           ReplicaNamePath , 
			    IDataProviderSvc*            datasvc         , 
			    IMessageSvc*                 messsvc         ) // create "regular" 
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
  , m_gi_dataSvc          (   datasvc       )
  , m_gi_messSvc          (   messsvc       )
{
  //
  //  MsgStream log( msgSvc(), "GeometryInfo::constructor" );
  //  log << MSG::VERBOSE << " Constructor 3 is invoked " << endreq;  
  Assert( 0 != m_gi_iDetectorElement , "Detector element points to NULL!" );
  //
};
  
//
//
//
GeometryInfo::~GeometryInfo()
{
  //
  StatusCode sc = reset(0);
  Assert( sc.isSuccess() , " GeometryInfo::Error in destructor " );
  //
};

//
//
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
  //  if( Level < 0 ) { m_gi_iDetectorElement = 0; };
  if( Level < 0 ) { ; };
  //
  m_gi_childLoaded = false; 
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  //
  return StatusCode::SUCCESS;
};

//
//
//

std::ostream& GeometryInfo::printOut( std::ostream& os ) const
{
  os << "GeometryInfo::name=" <<  dataObj()->fullpath() << ";" ; 
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
  return os;
};

//
//
//

MsgStream& GeometryInfo::printOut( MsgStream& os ) const
{
  os << "GeometryInfo::name=" << dataObj()->fullpath() << ";" ; 
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
  return os;
};

//
//
//

//
//
//


ILVolume* GeometryInfo::findLogical() const 
{
  ILVolume* lv = 0;
  if( !hasLVolume() ) { return lv; }
  //
  try   
    { 
      SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
      if( 0 != ptr ) { lv = ptr.operator->(); }
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


//
//
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
    {  Assert( false , "GeometryInfo::findLogical(), exception caught in retrieving IGI="+name , Exception ) ; } ///  throw new exception 
  catch ( ... ) 
    {  Assert( false , "GeometryInfo::findLogical(), unknown exception caught in retrieving IGI="+name     ) ; } ///  throw new exception 
  ///
  Assert( 0 != gi , "GeometryInfo::geoByName(), unable to retrieve IGeometryInfo, named="+name ) ;
  ///
  return gi; 
};


//
//
//


IDataProviderSvc* GeometryInfo::findDataSvc() const 
{
  ///
  IDataDirectory* dir   = dataObj()->directory();
  ///
  Assert( 0 != dir   , " GeoemtryInfo::findDataSvc(): IDataDirectory*   point to NULL! " );
  ///
  TransientStore* store = dir->store();
  ///
  Assert( 0 != store , " GeometryInfo::findDataSvc(): TransientStore*   points to NULL! ");
  ///
  IDataProviderSvc* svc = store->dataSvc(); 
  ///
  Assert( 0 != svc   , " GeometryInfo::findDataSvc(): IDataProviderSvc* points to NULL! ");
  ///
  m_gi_dataSvc = svc; 
  ///
  return m_gi_dataSvc;
};

//
//
//

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

//
//
//

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

//
//
//
