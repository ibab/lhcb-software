//$Id: ICondition.h,v 1.1 2003-04-23 10:06:42 sponce Exp $
#ifndef DETDESC_ICONDITION_H
#define DETDESC_ICONDITION_H 1

// Include files
#include <string>

// Framework include files
#include "GaudiKernel/IInterface.h"
#include "DetDesc/IParamSet.h"

/** the unique interface identifier 
 * ( unique interface identifier , major & minor versions)
 */
static const InterfaceID IID_ICondition ("ICondition" , 2 , 0);

/** @interface ICondition ICondition.h DetDesc/ICondition.h
 *
 *  An abstract interface describing a condition data in the transient
 *  data store (e.g. alignment, slow control, etc.).
 *
 *  @author Sebastien Ponce 
 *  @author Andrea Valassi 
 */
class ICondition : virtual public IInterface,
                   virtual public IParamSet
{

 public:

  /**
   * retrieve the unique interface identifier 
   * @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_ICondition; }

  /**
   * This method initializes the condition. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a condition.
   */
  virtual StatusCode initialize() = 0;

  /**
   * default virtual destructor
   */
  virtual ~ICondition() {};

};

#endif // DETDESC_ICONDITION_H
