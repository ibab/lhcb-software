//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/IConditionsDBAddress.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBADDRESS_H
#define DETCOND_ICONDITIONSDBADDRESS_H 1

// Base class
#include "DetCond/IConditionAddress.h"

///---------------------------------------------------------------------------
/** @class IConditionsDBAddress IConditionsDBAddress.h Det/DetCond/IConditionsDBAddress.h

    Abstract interface to CERN-IT CondDB opaque addresses.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionsDBAddress : virtual public IConditionAddress {

 public:

  // ConditionsDB-specific methods

  /// Get the folder name in the CondDB
  virtual const std::string folderName    ( ) = 0 ;

  /// Get the tag name in the CondDB
  virtual const std::string tagName       ( ) = 0 ;

  /// Get the storage type for the string stored in the CondDB
  virtual const unsigned char& stringType ( ) = 0 ;

};

#endif // DETCOND_ICONDITIONSDBADDRESS_H

