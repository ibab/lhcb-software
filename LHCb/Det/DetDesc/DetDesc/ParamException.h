//$Id: ParamException.h,v 1.1 2003-04-23 10:06:42 sponce Exp $
#ifndef DETDESC_PARAMEXCEPTION_H
#define DETDESC_PARAMEXCEPTION_H 1 

// Base class
#include "GaudiKernel/GaudiException.h" 

///---------------------------------------------------------------------------
/** @class ParamException ParamException.h DetDesc/ParamException.h

    Exception used in the ParamSet class.

    @author Sebastien Ponce
    @author Andrea Valassi
*///--------------------------------------------------------------------------

class ParamException : public GaudiException

{  

 public:

  /// Constructor 
  ParamException( const std::string& name );

  /// Destructor 
  virtual ~ParamException() throw();  

};

#endif  // DETDESC_PARAMEXCEPTION_H












