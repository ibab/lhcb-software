//$Id: IConditionInfo.h,v 1.3 2003-04-16 08:06:08 cattanem Exp $
#ifndef DETDESC_ICONDITIONINFO_H
#define DETDESC_ICONDITIONINFO_H 1

#include "GaudiKernel/IInterface.h"
// Forward declarations
class IDetectorElement;
class Condition;

/** the unique interface identifier 
 * ( unique interface identifier , major & minor versions)
 */
static const InterfaceID IID_IConditionInfo( "IConditionInfo" , 1 , 0 );

///---------------------------------------------------------------------------
/** @class IConditionInfo IConditionInfo.h Det/DetDesc/IConditionInfo.h

    An abstract interface to get the condition information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class IConditionInfo : virtual public  IInterface
{

 public:
  virtual ~IConditionInfo() { }
  
  /** retrieve the uniqie interface identifier 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IConditionInfo; }
  
  /// Get a pointer to the detector element to which the ConditionInfo belongs
  virtual IDetectorElement* detElem() const = 0 ;

  /// Get the name of the associated condition
  virtual const std::string conditionName() const = 0 ;

  /// Get a pointer to the associated condition
  virtual Condition* condition() = 0 ;

};

#endif // DETDESC_ICONDITIONINFO_H

