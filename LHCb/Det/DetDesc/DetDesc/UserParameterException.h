//$Id: UserParameterException.h,v 1.2 2003-01-17 14:03:38 sponce Exp $
#ifndef DETDESC_USERPARAMETEREXCEPTION_H
#define DETDESC_USERPARAMETEREXCEPTION_H 1 

// Base class
#include "GaudiKernel/GaudiException.h" 

///---------------------------------------------------------------------------
/** @class UserParameterException UserParameterException.h DetDesc/UserParameterException.h

    Exception used in the UserParameterSet class.

    @author Andrea Valassi
    @date December 2001
*///--------------------------------------------------------------------------

class UserParameterException : public GaudiException

{  

 public:

  /// Constructor 
  UserParameterException( const std::string& name );

  /// Destructor 
  virtual ~UserParameterException() throw();  

};

#endif  // DETDESC_USERPARAMETEREXCEPTION_H












