// $Id: GiGaInputStream.cpp,v 1.3 2007-01-12 15:36:48 ranjard Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"

// local
#include "GiGaInputStream.h"

// ============================================================================
/** @file GiGaInputStream 
 * 
 * Implementation file for class : GiGaInputStream
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 15/01/2002 
 */ 
// ============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaInputStream )

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
GiGaInputStream::GiGaInputStream( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GiGaStream ( name , pSvcLocator ) 
{}

// ============================================================================
// Destructor
// ============================================================================
GiGaInputStream::~GiGaInputStream() {}


// ============================================================================
// Main execution
// ============================================================================
StatusCode GiGaInputStream::execute()
{
  ///
  if( !m_execute ) { return StatusCode::SUCCESS; } 
  ///
  MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << " execute:: start" << endreq; 
  ///
  if( m_executeOnce      ) { m_execute  = false; }  
  ///
  /// preload data 
  Items::const_iterator item = m_items.begin() ;
  while( item != m_items.end() )
    { m_dataSvc->addPreLoadItem( *item++ ); } 
  m_dataSvc->preLoad(); 
  ///
  StatusCode status = StatusCode::SUCCESS; 
  m_dataSelector.clear();
  item = m_items.begin() ; 
  while( item != m_items.end() && status.isSuccess() ) 
    { status = LoadObject( *item++, &m_dataSelector) ; } 
  ///  
  if( status.isFailure() ) 
    { return Error("Execute::Could not load Object="+item->path(), status); } 
  /// create the representation 
  for( IDataSelector::iterator obj1 = m_dataSelector.begin() ;
       m_dataSelector.end() != obj1 ; ++obj1 ) 
    {
      IOpaqueAddress* Address = 0 ;
      status = m_cnvSvc->createRep( *obj1 , Address ) ; 
      if( status.isFailure() ) 
        { return Error(" Error in creation of representation!"); }
      // update the registry
      (*obj1)->registry()->setAddress( Address );
    }
  /// create the references 
  for( IDataSelector::iterator obj2 = m_dataSelector.begin() ;
       m_dataSelector.end() != obj2 ; ++obj2 ) 
    {
      status = m_cnvSvc->
        fillRepRefs( (*obj2)->registry()->address(),  *obj2 ) ; 
      if( status.isFailure() ) 
        { return Error(" Error in creation of references!"); }
    }
  ///
  if( status.isFailure() ) 
    { return Error("Execute::Could not convert the IDataSelector*", status);}
  ///
  m_dataSelector.clear(); 
  ///
  log << MSG::VERBOSE << "Execute::end" << endreq;  
  ///
  return status; 
  ///
}

// ============================================================================
// The End 
// ============================================================================
