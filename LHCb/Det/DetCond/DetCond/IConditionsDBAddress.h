//$Id: IConditionsDBAddress.h,v 1.2 2001-11-27 18:17:03 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBADDRESS_H
#define DETCOND_ICONDITIONSDBADDRESS_H 1

///---------------------------------------------------------------------------
/** @class IConditionsDBAddress IConditionsDBAddress.h Det/DetCond/IConditionsDBAddress.h

    Abstract interface to CERN-IT CondDB opaque addresses,
    specifying folder name, tag name, string technology.
    The event time is NOT specified by the address.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionsDBAddress {

 public:

  /// Get the folder name in the CondDB
  virtual const std::string folderName    ( ) = 0 ;

  /// Get the tag name in the CondDB
  virtual const std::string tagName       ( ) = 0 ;

  /// Get the storage type for the string stored in the CondDB
  virtual const unsigned char& stringType ( ) = 0 ;

};

#endif // DETCOND_ICONDITIONSDBADDRESS_H

