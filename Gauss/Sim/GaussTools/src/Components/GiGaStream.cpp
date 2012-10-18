// $Id: GiGaStream.cpp,v 1.5 2003-01-23 09:36:56 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/12/13 16:58:45  ibelyaev
//  put modifications and bug fixes
//
// Revision 1.3  2002/12/07 14:41:45  ibelyaev
//  add new Calo stuff
//
// Revision 1.2  2002/12/03 17:22:05  ibelyaev
//  fix the bug with conversion of hierarchy of objects
//
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartDataPtr.h"
/// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
/// local
#include "GiGaStream.h" 

// ============================================================================
/** @file GiGaStream.cpp 
 *  
 *  Implemenation of class GiGaStream
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
GiGaStream::~GiGaStream(){};

// ============================================================================
/// constructor
// ============================================================================
GiGaStream::GiGaStream( const std::string& StreamName     , 
                        ISvcLocator*       ServiceLocator )
  : Algorithm( StreamName , ServiceLocator ) 
  /// 
  , m_executeOnce           ( false                     )     
  , m_execute               ( true                      )
  ///
  , m_nameOfCnvSvc          ( IGiGaCnvSvcLocation::Kine )  
  , m_cnvSvc                (  0                        )  
  ///
  , m_nameOfDataSvc         ( "EventDataSvc"   ) 
  , m_dataSvc               (  0               ) 
  ///
  , m_namesOfItems          (                  )
  , m_items                 (                  ) 
  ///
  , m_dataSelector          (                  )
  ///
{
  ///
  declareProperty( "ExecuteOnce"         , m_executeOnce    ) ; 
  declareProperty( "ConversionSvcName"   , m_nameOfCnvSvc   ) ; 
  declareProperty( "DataProviderSvcName" , m_nameOfDataSvc  ) ;
  declareProperty( "StreamItems"         , m_namesOfItems   ) ;   
  ///
};

// ============================================================================
// initialize method 
// ============================================================================
StatusCode GiGaStream::initialize()
{
  ///
  MsgStream msg( msgSvc() , name() ); 
  if (outputLevel() == MSG::VERBOSE) {    
    msg << MSG::VERBOSE << "Initialize::start" << endmsg; 
  }
  
  ///
  StatusCode status = StatusCode::SUCCESS; 
  ///
  {
    status = service( m_nameOfDataSvc , m_dataSvc , true ) ;
    if( status.isFailure() )  
      { return Error("Unable to locate IDataProviderSvc* =" +
                     m_nameOfDataSvc , status ) ; }
    if( 0 == m_dataSvc )
      { return Error("IUnable to locate IDataProviderSvc* ="+
                     m_nameOfDataSvc          ) ; }
  }
  ///
  {
    status = service( m_nameOfCnvSvc , m_cnvSvc , true ) ;
    if( status.isFailure() )  
      { return Error("Unable to locate IConversionSvc ="+
                     m_nameOfCnvSvc,status);}
    if( 0 == m_cnvSvc )
      { return Error("Unable to locate IConversionSvc ="+
                     m_nameOfCnvSvc       );}
  }
  /// 
  for( Names::const_iterator pName = m_namesOfItems.begin(); 
       m_namesOfItems.end() != pName ; ++pName ) 
    { 
      std::string obj            = *pName ; 
      int depth                  = 1      ; 
      std::string::size_type pos = pName->find('#');
      if( std::string::npos != pos ) 
        { 
          obj.erase(pos);
          std::string levels(*pName,pos+1,pName->size());
          if( "*" == levels ) { depth = 9999999                ; }
          else                { depth = atoi( levels.c_str() ) ; }
        }    
      DataStoreItem item( obj , depth ); 
      m_items.push_back( item ); 
    }   
  ///
  // properties will be printed if asked for or in "MSG::DEBUG" mode
  if ( outputLevel() == MSG::DEBUG || outputLevel() == MSG::VERBOSE ) { 
    printProps(); 
  }
  if ( outputLevel() == MSG::VERBOSE ) {
    msg << MSG::VERBOSE << "Initialize::end" << endmsg; 
  }

  ///
  return StatusCode::SUCCESS; 
  ///
};

// ============================================================================
/** Load objects pointed by Item  and put it into Selector 
 *  @param Item object tree 
 *  @param Selector data selector 
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaStream::LoadObject( const DataStoreItem& item     , 
                                   IDataSelector*       Selector ) 
{
  ///
  if( item.depth() <=0   ) { return StatusCode::SUCCESS; } 
  ///
  if( 0 == m_dataSvc     ) 
    { return Error(" LoadObject::IDataProvoiderSvc points to NULL!" ) ; } 
  ///
  SmartDataPtr<DataObject> Object( m_dataSvc , item.path() ); 
  ///
  if( !Object            ) 
    { return Error(" LoadObject::DataObject* points to NULL for Oject '" 
                   + item.path() + "'" ) ; } 
  if( 0 == Selector      ) 
    { return Error(" LoadObject::IDataSelector* points to NULL for Object '"
                   + item.path() + "'" ) ; } 
  ///
  Selector->push_back( Object );
  // load other levels 
  const IRegistry* registry = Object->registry() ;
  if( 0 == registry )
    { return Error(" LoadObject:: IRegistry* points to NUL for object '" 
                   + item.path() + "'" ) ; }
  ///  
  StatusCode sc = LoadObject( registry , item.depth() - 1 , Selector ) ; 
  if( sc.isFailure() ) 
    { return Error(" LoadObject::Failure in loading of Directory", sc ); } 
  ///
  return StatusCode::SUCCESS; 
  ///
}; 

// ============================================================================
/** Load objects pointed by Item  and put it into Selector 
 *  @param registry object directory 
 *  @param Level    number of levels  
 *  @param Selector data selector 
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaStream::LoadObject( const IRegistry*     registry ,
                                   const int            Level    ,   
                                   IDataSelector*       Selector ) 
{
  /// success? 
  if( Level <= 0 ) { return StatusCode::SUCCESS ; }
  ///
  if( 0 == registry            ) 
    { return Error("LoadObject:: IRegistry* points to NULL "         ); } 
  if( 0 == Selector            ) 
    { return Error("LoadObject:: IDataSelector* points to NULL "     ); }
  if( 0 == registry->dataSvc() ) 
    { return Error("LoadObject:: IDataProviderSvc* points to NULL "  ); }
  ///
  SmartIF<IDataManagerSvc> dataMgr( registry->dataSvc() );
  if( !dataMgr ) 
    { return Error(" LoadObject:: IDataManagerSvc* points to NULL for '"
                   + registry->identifier() + "'" ); }
  ///
  Leaves leaves;
  StatusCode sc = dataMgr->objectLeaves( registry , leaves );
  if( sc.isFailure() )
    { return Error(" LoadObject:: Leaves could not be accessed for '" 
                   + registry->identifier() + "'" ); }
  /// loop over the leaves 
  for( Leaves::iterator leaf = leaves.begin() ; leaves.end() != leaf ; ++leaf )
    {
      if( 0 == *leaf ) { continue ; }           ///< CONTINUE!
      DataObject*  Object = 0 ;
      sc = registry->dataSvc()->retrieveObject( *leaf , "" , Object );
      if ( sc.isFailure() ||  0 == Object ) 
        { return Error(" LoadObject:: Leaf '" + (*leaf)->identifier() + "'" 
                       + " could not be retrieved!" ); }
      Selector->push_back( Object ) ;
      sc = LoadObject( Object->registry() , Level - 1 , Selector );
      if( sc.isFailure() ) 
        { return Error(" LoadObject:: Could not process the leaf '" +
                       (*leaf)->identifier() + "'" ); } 
    }
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
/** Miscellaneous function to simplify the typing 
 *  @param message message to be printed 
 *  @param status status code to be returned 
 */
// ============================================================================
StatusCode GiGaStream::Error( const std::string& message , 
                              const StatusCode&  status  ) 
{
  MsgStream msg( msgSvc() , name() ); 
  msg <<  MSG::ERROR  <<  message  <<  endmsg; 
  return status;  
};  

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStream::finalize()
{
  ///
  /// reset the preload list 
  if( 0 != m_dataSvc ) { m_dataSvc -> resetPreLoad()             ;}
  // reelase the servcies 
  if( 0 != m_dataSvc ) { m_dataSvc -> release () ; m_dataSvc = 0 ; }
  if( 0 != m_cnvSvc  ) { m_cnvSvc  -> release () ; m_cnvSvc  = 0 ; }
  ///
  m_namesOfItems .clear () ; 
  m_dataSelector .clear () ; 
  m_items        .clear () ; 
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
/** perform the printout of properties
 */
// ============================================================================
long GiGaStream::printProps ( ) const
{

  // print ALL properties
  MsgStream msg( msgSvc() , name() ); 
  typedef std::vector<Property*> Properties;
  const Properties& properties = this->getProperties() ;
  msg << MSG::DEBUG << "List of ALL properties of "
      << System::typeinfoName( typeid( *this ) ) << "/" << this->name()
      << "  #properties = " << properties.size() << endmsg ;
  for ( Properties::const_reverse_iterator property
          = properties.rbegin() ;
        properties.rend() != property ; ++property )
  {
    msg << MSG::DEBUG << "Property ['Name': Value] = "
        << ( **property)
        << endmsg ;
  }
  return properties.size() ;
}

// ============================================================================
// The End 
// ============================================================================


















