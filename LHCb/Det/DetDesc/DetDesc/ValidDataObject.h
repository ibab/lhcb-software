//$Id: ValidDataObject.h,v 1.4 2005-04-22 13:10:41 marcocle Exp $
#ifndef DETDESC_VALIDDATAOBJECT_H
#define DETDESC_VALIDDATAOBJECT_H 1

#include <string>

// Base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/TimePoint.h"

// Unique ID of the class
#include "DetDesc/CLIDValidDataObject.h"

// Forward declarations
class StatusCode;

///---------------------------------------------------------------------------
/** @class ValidDataObject ValidDataObject.h Det/DetCond/ValidDataObject.h

    A simple DataObject implementing the IValidity interface.

    @author Andrea Valassi 
    @date February 2001
    @author Marco Clemencic 
    @date February 2005
*///--------------------------------------------------------------------------

class ValidDataObject : public DataObject, 
			virtual public IValidity

{

 public: 
  
  /// Default constructor
  ValidDataObject();

  /// Constructor 
  ValidDataObject( const ITime& since, const ITime& till );
  
  /// Copy constructor 
  ValidDataObject( ValidDataObject& obj );
  
  /*
  /// Overloaded copy operator: deep copy all contents (virtual!)
  virtual ValidDataObject& operator= ( ValidDataObject& obj );
  */

  /// Update using another instance of this class: deep copy all 
  /// contents, except for the properties of a generic DataObject
  virtual void update ( ValidDataObject& obj );

  /// Destructor 
  virtual ~ValidDataObject();
  
 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_ValidDataObject; };

 public:

  // Implementation of IValidity

  /// Check if the data object has a well defined validity range
  virtual bool isValid() const ;

  /// Check if the data object is valid at the specified time
  virtual bool isValid( const ITime& t ) const;

  /// Get start of validity
  virtual const ITime& validSince() const;

  /// Get end of validity
  virtual const ITime& validTill() const;

  /// Set validity range
  virtual void setValidity( const ITime& since, const ITime& till );  

  /// Set start of validity
  virtual void setValiditySince( const ITime& since );  

  /// Set end of validity
  virtual void setValidityTill( const ITime& till );   
  
// ================================================
// ValidDataObject methods
// ================================================

  /// Update the current object and all the useds one (children, 
  /// references, etc) only if needed, of course.
  /// This method must be reimplemented for more complex objects.
  virtual StatusCode update();

  /// Possible stata of the update flag
  enum UpdateModeFlag {
    DEFAULT,
    ALWAYS_VALID,
    FORCE_UPDATE
  };

  // setters and getters for update mode

  /// Return the update mode:
  /// ValidDataObject::DEFAULT -> use the actual validity <br>
  /// ValidDataObject::ALWAYS_VALID -> never update
  /// ValidDataObject::FORCE_UPDATE -> force an update even if still valid
  ///                                  (then back to DEFAULT behaviour)
  virtual const UpdateModeFlag &updateMode() const;

  /// Set the mode for update
  virtual void setUpdateMode(UpdateModeFlag mode);

  virtual void defaultUpdateMode();
  virtual void forceUpdateMode();
  virtual void neverUpdateMode();
 
 private:

  // IValidity data
  /// Start of validity
  TimePoint m_validSince;
  /// End of validity
  TimePoint m_validUntil;
  
  /// Flag to force/inhibit the update regardless of the validity
  UpdateModeFlag   m_updateMode;
  
};

#endif // DETDESC_VALIDDATAOBJECT_H
