
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IConversionSvc.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartDataPtr.h"




/// local 
#include "GiGaStream.h" 



static const   AlgFactory<GiGaStream>                      s_GiGaStreamFactory; 
const         IAlgFactory&             GiGaStreamFactory = s_GiGaStreamFactory; 


///
/// constructor
///

GiGaStream::GiGaStream( const std::string& StreamName     , 
                        ISvcLocator*       ServiceLocator )
  : Algorithm( StreamName , ServiceLocator ) 
  /// 
  , m_ExecuteOnce           ( false            )     
  , m_Execute               ( true             ) 
  ///
  , m_NameOfConversionSvc   ( "GiGaKineCnvSvc" )  
  , m_ConversionSvc         (  0               )  
  ///
  , m_NameOfDataProviderSvc ( "EventDataSvc"   ) 
  , m_DataProviderSvc       (  0               ) 
  ///
  , m_NameOfDataManagerSvc  ( "EventDataSvc"   ) 
  , m_DataManagerSvc        (  0               ) 
  ///
  , m_NamesOfStreamItems    (                  )
  , m_DataSelector          (                  )
  , m_StreamItems           (                  ) 
  ///
  , m_FillGiGaStream        ( true             ) 
{
  ///
  m_NameOfDataManagerSvc = m_NameOfDataProviderSvc; 
  ///
  declareProperty( "ExecuteOnce"         , m_ExecuteOnce           ) ; 
  declareProperty( "ConversionSvcName"   , m_NameOfConversionSvc   ) ; 
  declareProperty( "DataProviderSvcName" , m_NameOfDataProviderSvc ) ;
  declareProperty( "DataManagerSvcName"  , m_NameOfDataManagerSvc  ) ;
  declareProperty( "StreamItems"         , m_NamesOfStreamItems    ) ;   
  declareProperty( "FillGiGaStream"      , m_FillGiGaStream        ) ; 
  ///
};

///
///
///

StatusCode GiGaStream::initialize()
{
  ///
  {MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "Initialize::start" << endreq; } 
  ///
  StatusCode status = StatusCode::SUCCESS; 
  ///
  {
    status = serviceLocator()->getService( m_NameOfDataProviderSvc          , 
					   IID_IDataProviderSvc             , 
					   (IInterface*&) m_DataProviderSvc ) ;
    if( status.isFailure() )  
      { return Error("Initialize::Unable to locate IDataProviderSvc interface with name \t"+m_NameOfDataProviderSvc,status);}
    if( 0 == m_DataProviderSvc )
      { return Error("Initialize::Unable to locate IDataProviderSvc interface with name \t"+m_NameOfDataProviderSvc       );}
  }
  ///
  {
    status = serviceLocator()->getService( m_NameOfDataManagerSvc          , 
					   IID_IDataManagerSvc             , 
					   (IInterface*&) m_DataManagerSvc ) ;
    
    if( status.isFailure() )  
      { return Error("Initialize::Unable to locate IDataManagerSvc interface with name \t"+m_NameOfDataManagerSvc,status);}
    if( 0 == m_DataManagerSvc )
      { return Error("Initialize::Unable to locate IDataManagerSvc interface with name \t"+m_NameOfDataManagerSvc       );}
  }
  ///
  {
    status = serviceLocator()->getService( m_NameOfConversionSvc            , 
					   IID_IConversionSvc               , 
					   (IInterface*&) m_ConversionSvc ) ;
    if( status.isFailure() )  
      { return Error("Initialize::Unable to locate IConversionSvc interface with name \t"+m_NameOfConversionSvc,status);}
    if( 0 == m_DataManagerSvc )
      { return Error("Initialize::Unable to locate IConversionSvc interface with name \t"+m_NameOfConversionSvc       );}
  }
  ///
  ///
  /// 
  for( Names::const_iterator pName = m_NamesOfStreamItems.begin(); m_NamesOfStreamItems.end() != pName ; ++pName ) 
    { 
      std::string obj            = *pName ; 
      int depth                  = 1      ; 
      std::string::size_type pos = pName->find('#');
      if( std::string::npos != pos ) 
	{ 
	  obj.erase(pos);
	  std::string levels((*pName).c_str(),pos+1);
	  if( "*" == levels ) { depth = 9999999              ;}
	  else                { depth = atoi(levels.c_str());}
	}    
      DataStoreItem item( obj , depth ); 
      m_StreamItems.push_back(item); 
    }   
  ///
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "Initialize::end" << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  ///
};  


///
///
///

StatusCode GiGaStream::execute()
{
  ///
  if( !m_Execute         ) { return StatusCode::SUCCESS; } 
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "Execute::start" << endreq; } 
  ///
  if( m_ExecuteOnce      ) { m_Execute  = false; }  
  ///
  Items::const_iterator item = m_StreamItems.begin() ; 
  while( item != m_StreamItems.end() ) { m_DataProviderSvc->addPreLoadItem( *item++ ); } 
  ///
  m_DataProviderSvc->preLoad(); 
  ///
  m_DataSelector.clear();
  ///
  StatusCode status = StatusCode::SUCCESS; 
  item = m_StreamItems.begin() ; 
  while( item != m_StreamItems.end() && status.isSuccess() ) { status = LoadObject( *item++, &m_DataSelector) ; } 
  ///  
  if( status.isFailure() ) { return Error("Execute::Could not load Object="+item->path(), status); } 
  ///
  if( m_FillGiGaStream   ) { status = m_ConversionSvc->createReps( &m_DataSelector )             ; }            
  ///
  if( status.isFailure() ) { return Error("Execute::Could not convert the IDataSelector*", status);}
  ///
  m_DataSelector.clear(); 
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "Execute::end" << endreq; } 
  ///
  return status; 
  ///
}; 


///
///
///

StatusCode GiGaStream::LoadObject( const DataStoreItem& item , IDataSelector* Selector ) 
{
  ///
  if( item.depth() <=0   ) { return StatusCode::SUCCESS; } 
  ///
  SmartDataPtr<DataObject> Object( m_DataProviderSvc , item.path() ); 
  ///
  if( 0 == Object        ) { return Error("LoadObjectItem::DataObject* points to NULL for Oject="    +item.path() ); } 
  if( 0 == Selector      ) { return Error("LoadObjectItem::IDataSelector* points to NULL for Oject=" +item.path() ); } 
  ///
  Selector->push_back( Object ); 
  ///
  IDataDirectory* iDir = Object->directory(); 
  if( 0 == iDir          ) { return Error("LoadObjectItem::IDataDirectory* points to NULL for Oject="+item.path() ); } 
  ///  
  StatusCode status = LoadObject( iDir , item.depth() -1 , Selector ); 
  if( status.isFailure() ) { return Error("LoadObjectItem::Failure in loading of Directory", status ); } 
  ///
  return StatusCode::SUCCESS; 
  ///
}; 

///
///
///

StatusCode GiGaStream::LoadObject( IDataDirectory* CurrentDir , const int Level , IDataSelector* Selector ) 
{
  ///
  if( Level <=0              ) { return StatusCode::SUCCESS; } 
  ///
  if( 0 == CurrentDir        ) { return Error("LoadObjectDirIDataDirectory* points to NULL " ); } 
  if( 0 == Selector          ) { return Error("LoadObjectDirIDataSelector* points to NULL "  ); }
  ///
  ///
  for( IDataDirectory::DirIterator obj = CurrentDir->begin(); CurrentDir->end() != obj ; ++obj )
    {     
      DataObject* Object = 0 ; 
      StatusCode status = m_DataProviderSvc->retrieveObject( CurrentDir , (*obj)->name() , Object )    ;
      if( status.isFailure() ) { return Error("LoadObjectDir::Could not retrieve"+(*obj)->name(), status )   ; } 
      if( 0 == Object        ) { return Error("LoadObjectDir::Could not retrieve"+(*obj)->name()         )   ; } 
      ///
      Selector->push_back( Object ); 
      ///
      IDataDirectory* SubDir = Object->directory(); 
      if( 0 == SubDir        ) { return Error("LoadObjectDir::Could not retrieve directory"+(*obj)->name() ) ; } 
      status = LoadObject( SubDir , Level - 1 , Selector );
      if( status.isFailure() ) { return Error("LoadObjectDir::Could not retrieve subdirectory")              ; }    
    }
  ///
  return StatusCode::SUCCESS;
  ///
};

///
///
///

StatusCode GiGaStream::Error( const std::string& message , const StatusCode& status ) 
{
  MsgStream log( msgSvc() , name() ); log << MSG::ERROR << message << endreq; 
  return status;  
};  

///
///
///

StatusCode GiGaStream::finalize()
{
  ///
  m_NamesOfStreamItems.clear() ; 
  m_DataSelector.clear      () ; 
  m_StreamItems.clear       () ; 
  ///
  return StatusCode::SUCCESS;
  ///
};


















