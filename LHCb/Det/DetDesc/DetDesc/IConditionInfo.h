//$Id: IConditionInfo.h,v 1.1 2001-12-13 19:20:29 andreav Exp $
#ifndef DETDESC_ICONDITIONINFO_H
#define DETDESC_ICONDITIONINFO_H 1

// Forward declarations
class IDetectorElement;
class Condition;

///---------------------------------------------------------------------------
/** @class IConditionInfo IConditionInfo.h Det/DetDesc/IConditionInfo.h

    An abstract interface to get the condition information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class IConditionInfo
{

 public:
  
  /// Get a pointer to the detector element to which the ConditionInfo belongs
  virtual IDetectorElement* detElem() const = 0 ;

  /// Get the name of the associated condition
  virtual const std::string conditionName() const = 0 ;

  /// Get a pointer to the associated condition
  virtual Condition* condition() = 0 ;

};

#endif // DETDESC_ICONDITIONINFO_H

