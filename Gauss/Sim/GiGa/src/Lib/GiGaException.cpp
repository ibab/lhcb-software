/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
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

