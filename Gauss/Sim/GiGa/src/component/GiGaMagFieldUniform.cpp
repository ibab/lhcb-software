// $Id: GiGaMagFieldUniform.cpp,v 1.7 2002-03-13 16:22:12 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/12 15:42:51  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.5  2001/07/27 17:03:20  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:27  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaMagFieldFactory.h"
/// local 
#include "GiGaMagFieldUniform.h"

/** implementation of class GiGaMagFieldUniform 
 * 
 *  @author Vanya Belyaev 
 */

// ============================================================================
// ============================================================================
static const  GiGaMagFieldFactory<GiGaMagFieldUniform>         s_Factory ;
const        IGiGaMagFieldFactory&GiGaMagFieldUniformFactory = s_Factory ; 

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
  ///
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
void GiGaMagFieldUniform::GetFieldValue ( const double [4], 
                                          double *B  ) const 
{
  ///
  *(B+0) = (double) m_Bx ;
  *(B+1) = (double) m_By ;
  *(B+2) = (double) m_Bz ;
  ///
};

// ============================================================================

