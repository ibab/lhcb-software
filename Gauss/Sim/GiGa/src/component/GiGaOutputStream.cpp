// $Id: GiGaOutputStream.cpp,v 1.1 2002-01-22 18:20:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
// local
#include "GiGaOutputStream.h"

// ============================================================================
/** @file GiGaOutputStream 
 * 
 * Implementation file for class : GiGaOutputStream
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 15/01/2002 
 */ 
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<GiGaOutputStream>          s_Factory ;
const        IAlgFactory& GiGaOutputStreamFactory = s_Factory ; 

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
GiGaOutputStream::GiGaOutputStream( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GiGaStream ( name , pSvcLocator ) 
{};

// ============================================================================
// Destructor
// ============================================================================
GiGaOutputStream::~GiGaOutputStream() {};

// ============================================================================
// Main execution
// ============================================================================
StatusCode GiGaOutputStream::execute()
{
  ///
  if( !m_execute ) { return StatusCode::SUCCESS; } 
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << " execute:: start" << endreq; } 
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
  ///
  m_dataSelector.clear(); 
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "Execute::end" << endreq; } 
  ///
  return status; 
  ///
}; 

// ============================================================================
// The End 
// ============================================================================
