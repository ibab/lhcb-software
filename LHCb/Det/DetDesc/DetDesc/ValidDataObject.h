//$Id: ValidDataObject.h,v 1.8 2007-11-09 17:10:06 marcocle Exp $
#ifndef DETDESC_VALIDDATAOBJECT_H
#define DETDESC_VALIDDATAOBJECT_H 1

#include <string>

// Base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/Time.h"

// Unique ID of the class
#include "DetDesc/CLIDValidDataObject.h"

// Forward declarations
class StatusCode;

///---------------------------------------------------------------------------
/** @class ValidDataObject ValidDataObject.h DetDesc/ValidDataObject.h

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
  
  /// Copy constructor 
  ValidDataObject( const ValidDataObject& obj );
  
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
  inline static  const CLID& classID() { return CLID_ValidDataObject; }

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream( std::ostream& s ) const;

 public:

  // Implementation of IValidity

  /// Check if the data object has a well defined validity range
  virtual bool isValid() const ;

  /// Check if the data object is valid at the specified time
  virtual bool isValid( const Gaudi::Time& t ) const;

  /// Get start of validity
  virtual const Gaudi::Time& validSince() const;

  /// Get end of validity
  virtual const Gaudi::Time& validTill() const;

  /// Set validity range
  virtual void setValidity( const Gaudi::Time& since, const Gaudi::Time& till );  

  /// Set start of validity
  virtual void setValiditySince( const Gaudi::Time& since );  

  /// Set end of validity
  virtual void setValidityTill( const Gaudi::Time& till );   
  
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
  Gaudi::Time m_validSince;
  /// End of validity
  Gaudi::Time m_validUntil;
  
  /// Flag to force/inhibit the update regardless of the validity
  UpdateModeFlag   m_updateMode;
  
};

#endif // DETDESC_VALIDDATAOBJECT_H
