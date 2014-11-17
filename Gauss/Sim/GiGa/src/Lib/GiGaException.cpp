// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/08/12 15:42:49  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.2  2001/07/23 13:12:11  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.1  2001/07/15 20:54:25  ibelyaev
/// package restructurisation
/// 
// ============================================================================
#define   GiGa_GIGAEXCEPTION_CPP 1 
// ============================================================================
// GiGa 
#include "GiGa/GiGaException.h"

// ============================================================================
/// constructor 
// ============================================================================
GiGaException::GiGaException( const std::string & message ,          
			      const StatusCode  & sc      )          
  : GaudiException( message , "*GiGaException*" , sc ) {}                    

// ============================================================================
/// constructor
// ============================================================================
GiGaException::GiGaException( const std::string   & message   ,
			      const GaudiException& Exception ,
			      const StatusCode    & sc        )   
  : GaudiException( message , "*GiGaException*"   , sc , Exception ) {}

// ============================================================================
/// destructor  
// ============================================================================
GiGaException::~GiGaException() throw(){}

// ============================================================================
/// clone (virtual constructor)
// ============================================================================
GaudiException* GiGaException::clone() const                                  
{ return  new GiGaException(*this); }

// ============================================================================

