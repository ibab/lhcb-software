//$Id: Condition.h,v 1.4 2003-04-23 10:06:41 sponce Exp $
#ifndef DETDESC_CONDITION_H
#define DETDESC_CONDITION_H 1

#include <string>

// Base classes
#include "DetDesc/ICondition.h"
#include "DetDesc/ParamValidDataObject.h"

// Unique ID of the class
#include "DetDesc/CLIDCondition.h"

/** @class Condition Condition.h DetDesc/Condition.h
 *
 *  A DataObject implementing the IParamSet, IValidity and
 *  ICondition interfaces.
 *  This is the basic type of condition data in the transient data store
 *  (alignment, slow control, etc.).
 *
 *  @author Sebastien Ponce 
 *  @author Andrea Valassi 
 */
class Condition : public ParamValidDataObject, 
                  virtual public ICondition
{

 public: 
  
  /// Default constructor
  Condition();

  /// Constructor 
  Condition (const ITime& since, const ITime& till);
  
  /// Copy constructor 
  Condition (Condition& obj);
  
  /// Update using another Condition: deep copy all contents, 
  /// except for the properties of a generic DataObject
  virtual void update (Condition& obj);

  /// Destructor 
  virtual ~Condition();
  
 public:

  /**
   * This method initializes the condition. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a condition.
   */
  virtual StatusCode initialize();

 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_Condition; };
  
public:

  // Implementation of IInterface
  virtual unsigned long addRef();
  virtual unsigned long release();
  virtual StatusCode queryInterface( const InterfaceID& , void** );
  
};

#endif // DETDESC_CONDITION_H
