// $Id: GiGaMagFieldUniform.cpp,v 1.9 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaMagFieldFactory.h"
/// local 
#include "GiGaMagFieldUniform.h"

/** @file 
 *  
 *  implementation of class GiGaMagFieldUniform 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaMagField( GiGaMagFieldUniform) ;

// ============================================================================
// ============================================================================
GiGaMagFieldUniform::GiGaMagFieldUniform( const std::string& nick , 
                                          ISvcLocator* loc) 
  : GiGaMagFieldBase ( nick , loc )
  , m_Bx( 0 ) 
  , m_By( 0 ) 
  , m_Bz( 0 ) 
{
  declareProperty("Bx" , m_Bx );
  declareProperty("By" , m_By );
  declareProperty("Bz" , m_Bz );
};

// ============================================================================
// ============================================================================
GiGaMagFieldUniform::~GiGaMagFieldUniform(){};

// ============================================================================
/** initialization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldUniform::initialize()
{
  /// initialize the base class 
  StatusCode sc = GiGaMagFieldBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc);}
  ///
  Print("initialized succesfully") ;
  return StatusCode::SUCCESS;  
};

// ============================================================================
/** finalization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldUniform::finalize  ()
{
  ///
  Print("finalization") ;
  ///
  return GiGaMagFieldBase::finalize();
};


// ============================================================================
// ============================================================================
void GiGaMagFieldUniform::GetFieldValue 
( const double [4]    , 
  double*          B  ) const 
{
  ///
  *(B+0) = (double) m_Bx ;
  *(B+1) = (double) m_By ;
  *(B+2) = (double) m_Bz ;
  *(B+2) = (double) m_Bz ;
  ///
};

// ============================================================================
// The END 
// ============================================================================

