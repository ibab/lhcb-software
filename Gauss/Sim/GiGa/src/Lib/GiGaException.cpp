/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/15 20:54:25  ibelyaev
/// package restructurisation
/// 
/// ===========================================================================
#define   GiGa_GIGAEXCEPTION_CPP 1 
/// ===========================================================================
// GiGa 
#include "GiGa/GiGaException.h"

/// ===========================================================================
/// constructor 
/// ===========================================================================
GiGaException::GiGaException( const std::string & message ,          
			      const StatusCode  & sc      )          
  : GaudiException( message , "*GiGaException*" , sc ) {};                    

/// ===========================================================================
/// constructor
/// ===========================================================================
GiGaException::GiGaException( const std::string   & message   ,
			      const GaudiException& Exception ,
			      const StatusCode    & sc        )   
  : GaudiException( message , "*GiGaException*"   , sc , Exception ) {};        

/// ===========================================================================
/// destructor  
/// ===========================================================================
GiGaException::~GiGaException(){};                       

/// ===========================================================================
/// clone (virtual constructor)
/// ===========================================================================
GaudiException* GiGaException::clone() const                                  
{ return  new GiGaException(*this); };                        

/// ===========================================================================

