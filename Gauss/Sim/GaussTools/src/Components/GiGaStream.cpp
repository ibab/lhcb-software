// $Id: GiGaStream.cpp,v 1.1 2002-09-26 18:11:02 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/01/22 18:20:53  ibelyaev
//  Vanya: update for newer versions of Gaudi and Geant4
//
// Revision 1.5  2001/08/12 15:42:54  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/23 13:12:28  ibelyaev
// the package restructurisation(II)
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
  , m_executeOnce           ( false            )     
  , m_execute               ( true             ) 
  ///
  , m_nameOfCnvSvc          ( "GiGaKineCnvSvc" )  
  , m_cnvSvc                (  0               )  
  ///
  , m_nameOfDataSvc         ( "EventDataSvc"   ) 
  , m_dataSvc               (  0               ) 
  ///
  , m_namesOfItems          (                  )
  , m_items                 (                  ) 
  ///
  , m_dataSelector          (                  )
  , m_leaves                ()
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
  {MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "Initialize::start" << endreq; } 
  ///
  StatusCode status = StatusCode::SUCCESS; 
  ///
  {
    status = service( m_nameOfDataSvc , m_dataSvc ) ;
    if( status.isFailure() )  
      { return Error("Unable to locate IDataProviderSvc* =" +
                     m_nameOfDataSvc , status ) ; }
    if( 0 == m_dataSvc )
      { return Error("IUnable to locate IDataProviderSvc* ="+
                     m_nameOfDataSvc          ) ; }
    m_dataSvc -> addRef();
  }
  ///
  {
    status = service( m_nameOfCnvSvc , m_cnvSvc ) ;
    if( status.isFailure() )  
      { return Error("Unable to locate IConversionSvc ="+
                     m_nameOfCnvSvc,status);}
    if( 0 == m_cnvSvc )
      { return Error("Unable to locate IConversionSvc ="+
                     m_nameOfCnvSvc       );}
    m_cnvSvc -> addRef() ;
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
          std::string levels((*pName).c_str(),pos+1);
          if( "*" == levels ) { depth = 9999999                ; }
          else                { depth = atoi( levels.c_str() ) ; }
        }    
      DataStoreItem item( obj , depth ); 
      m_items.push_back( item ); 
    }   
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "Initialize::end" << endreq; } 
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
  const IRegistry* registry = Object->registry() ;
  if( 0 == registry )
    { return Error(" loadObject:: IRegistry* points to NUL for object '" 
                   + item.path() + "'" ) ; }
  ///  
  StatusCode status = LoadObject( registry , item.depth()  , Selector ); 
  if( status.isFailure() ) 
    { return Error(" LoadObject::Failure in loading of Directory", 
                   status ); } 
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
  m_leaves.clear();
  StatusCode sc = dataMgr->objectLeaves( registry , m_leaves );
  if( sc.isFailure() )
    { return Error(" LoadObject:: Leaves could not be accessed for '" 
                   + registry->identifier() + "'" ); }
  /// loop over the leaves 
  for( Leaves::iterator leaf = m_leaves.begin() ;
       m_leaves.end() != leaf ; ++leaf )
    {
      if( 0 == *leaf ) { continue ; }           ///< CONTINUE!
      DataObject*  Object = 0 ;
      sc = registry->dataSvc()->retrieveObject( *leaf , "" , Object );
      if ( sc.isFailure() ||  0 == Object ) 
        { return Error(" LoadObject:: Leaf '" + (*leaf)->identifier() + "'" 
                       + " could not be retrieved!" ); }
      Selector->push_back( Object ) ;
      sc = LoadObject( *leaf , Level - 1 , Selector );
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
  MsgStream log( msgSvc() , name() ); 
  log <<  MSG::ERROR  <<  message  <<  endreq; 
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
  if( 0 != m_dataSvc ) { m_dataSvc -> release() ; m_dataSvc = 0 ; }
  if( 0 != m_cnvSvc  ) { m_cnvSvc  -> release() ; m_cnvSvc  = 0 ; }
  ///
  m_namesOfItems .clear () ; 
  m_dataSelector .clear () ; 
  m_items        .clear () ; 
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
// The End 
// ============================================================================


















