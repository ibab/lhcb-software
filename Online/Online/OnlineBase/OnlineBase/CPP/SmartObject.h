#ifndef __SMARTOBJECT_H__
#define __SMARTOBJECT_H__
#ifdef  __cplusplus
#include "NET/defs.h"

/// Useful template class for object, which may fail
/** @class SmartObject
 *
 *   template <class Type> class SmartObject
 *
 *   SmartObject class:
 *   - Hold a status word, which may be filled and checked.
 *   - Automatic conversion to requested parent class
 *
 *   @author M.Frank
 */
template <class Type> class SmartObject {
protected:
  //@Man: Protected member variables
  ///  Status Code
  int m_status;
public:
  //@Man: Public member functions
  //  SmartObject::SmartObject
  //  ========================
  ///  Standard Constructor: Returns reference to SmartObject instance
  //  Arguments: None
  SmartObject() : m_status(1) {
  }

  //  SmartObject::~SmartObject
  //  ========================
  ///  Standard Destructor
  //   Arguments: None
  virtual ~SmartObject() {
  }

  //  SmartObject::Status
  //  ====================
  ///  Returns the Status code of the Condition object.
  //  Arguments: None
  //  Return Value:   1             Success state
  //                  errno         Error code, which occurred during execution
  int Status () const {
    return m_status;
  }

  //  SmartObject::Ok
  //  ===============
  ///  Checks the Status code of the Condition object.
  //  Arguments: None
  //  Return Value:   TRUE   Condition in success state
  //                  FALSE  Error occurred during execution
  int Ok () const {
    return (m_status&1);
  }

  //  SmartObject::operator Type*
  //  ==========================
  ///  Automatic conversion to pointer if required.
  //
  //  Arguments: None
  //  Return Value:     Type* 
  virtual operator Type* () { 
    return (Type*)this; 
  }
};
#endif      /* __cplusplus      */
#endif      /* __SMARTOBJECT_H__    */
