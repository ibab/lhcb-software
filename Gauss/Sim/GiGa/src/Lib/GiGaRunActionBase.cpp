// $Id: GiGaRunActionBase.cpp,v 1.5 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
// ============================================================================
/// GiGa 
#include "GiGa/GiGaRunActionBase.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaRunActionBase
 * 
 *  @date 26/07/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param Name name of this concrete instance of Run Action Object 
 *  @param Loc  pointer to Service Locator 
 */
// ============================================================================
GiGaRunActionBase::GiGaRunActionBase( const std::string Name ,
                                      ISvcLocator*      Loc  )
  : GiGaBase( Name , Loc )
{};

// ============================================================================
/// destructor
// ============================================================================
GiGaRunActionBase::~GiGaRunActionBase()
{};

// ============================================================================
/** identification 
 *  @return name of concrete inteface instance 
 */
// ============================================================================
const std::string& GiGaRunActionBase::name() const 
{ return GiGaBase::name() ; }

// ============================================================================
/** initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::initialize  ()
{ 
  StatusCode sc = GiGaBase::initialize();   
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!",sc); }
  ///
  Print("GiGaRunActionBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::finalize  ()
{
  /// 
  Print("GiGaRunActionBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize(); 
};

// ============================================================================
// The END 
// ============================================================================

