//$Id: UserParameterException.h,v 1.1 2001-12-11 10:02:20 sponce Exp $
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
  virtual ~UserParameterException();  

};

#endif  // DETDESC_USERPARAMETEREXCEPTION_H












