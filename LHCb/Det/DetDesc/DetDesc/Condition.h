//$Id: Condition.h,v 1.10 2006-10-25 13:45:00 marcocle Exp $
#ifndef DETDESC_CONDITION_H
#define DETDESC_CONDITION_H 1

#include <string>

// Base classes
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
class Condition : public ParamValidDataObject {

 public: 
  
  /// Default constructor
  Condition();

  /// Copy constructor 
  Condition (const Condition& obj);
  
  /// Update using another ValidDataObject: deep copy all contents, 
  /// except for the properties of a generic DataObject
  virtual void update ( ValidDataObject& obj);

  /// Prepare an XML string representing the condition.
  virtual std::string toXml(const std::string &name = std::string()) const;

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
  inline static  const CLID& classID() { return CLID_Condition; }

};

#endif // DETDESC_CONDITION_H
