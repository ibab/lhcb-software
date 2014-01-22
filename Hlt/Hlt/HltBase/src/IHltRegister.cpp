// $Id: IHltRegister.cpp,v 1.1 2009-12-23 10:34:13 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IAlgorithm.h"
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/IHltRegister.h"
// ============================================================================
/** @file 
 *  Implementation file for class  Hlt::IRegister 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-17 
 */
// ============================================================================
/*  constructor from the registrator and algorithm 
 *  open&lock  the transaction  
 *  @exception throw GaudiException for invalid Hlt::IRrgister::lock 
 *  @exception throw GaudiException for invalid arguments 
 *  @param reg pointer to Hlt::IRegister interface
 *  @param alg the algorithm
 */
// ============================================================================
Hlt::IRegister::Lock::Lock 
(       Hlt::IRegister*  reg , const IAlgorithm* alg ) 
  : m_register  { reg } 
  , m_algorithm { alg } 
{
  // ==========================================================================
  if ( !m_register ) 
  {
    throw GaudiException ( "Hlt::IRegister* points to NULL" , 
                           "Hlt::IRegister::Lock"           ,
                           StatusCode::FAILURE              ) ;
  }
  // ==========================================================================
  if ( !m_algorithm ) 
  {
    throw GaudiException ( "IAlgorithm*     points to NULL" , 
                           "Hlt::IRegister::Lock"           ,
                           StatusCode::FAILURE              ) ;
  }
  // ==========================================================================
  StatusCode sc = m_register -> lock ( m_algorithm ) ;
  //
  if ( sc.isFailure() ) 
  {
    throw GaudiException ( "Unable to lock Hlt::IRegister " , 
                           "Hlt::IRegister::Lock"           , sc ) ;
  }
  // ==========================================================================
}
// ============================================================================
/*  destructor (close & unlock transaction)
 *  @exception throw exception for invalid Hlt::IRrgister::unlock 
 */
// ============================================================================
Hlt::IRegister::Lock::~Lock()
{
  // ==========================================================================
  if ( !m_register ) 
  {
    throw GaudiException ( "Hlt::IRegister* points to NULL" , 
                           "Hlt::IRegister::~Lock"          ,
                           StatusCode::FAILURE              ) ;
  }
  // ==========================================================================
  if ( !m_algorithm ) 
  {
    throw GaudiException ( "IAlgorithm*     points to NULL" , 
                           "Hlt::IRegister::~Lock"          ,
                           StatusCode::FAILURE              ) ;
  }
  // ==========================================================================
  StatusCode sc = m_register -> unlock ( m_algorithm ) ;
  //
  if ( sc.isFailure() ) 
  {
    throw GaudiException ( "Unable to unlock Hlt::IRegister " , 
                           "Hlt::IRegister::~Lock"          , sc ) ;
  }
  // ==========================================================================
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
Hlt::IRegister:: ~IRegister() {}
// ============================================================================
// Return the unique interface ID 
// ============================================================================
const InterfaceID& Hlt::IRegister::interfaceID() 
{
  static const InterfaceID s_IID {"Hlt::IRegister" , 2 , 0 } ;
  return s_IID ;
}
// ============================================================================  

// ============================================================================
// The END 
// ============================================================================
