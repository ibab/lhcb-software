//$Id: ValidDataObject.h,v 1.3 2001-12-14 17:54:41 andreav Exp $
#ifndef DETDESC_VALIDDATAOBJECT_H
#define DETDESC_VALIDDATAOBJECT_H 1

#include <string>

// Base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"

// Unique ID of the class
#include "DetDesc/CLIDValidDataObject.h"

// Forward declarations
class ITime;
class StatusCode;

///---------------------------------------------------------------------------
/** @class ValidDataObject ValidDataObject.h Det/DetCond/ValidDataObject.h

    A simple DataObject implementing the IValidity interface.

    @author Andrea Valassi 
    @date February 2001
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
  virtual bool isValid();

  /// Check if the data object is valid at the specified time
  virtual bool isValid( const ITime& t );

  /// Get start of validity
  virtual const ITime& validSince();

  /// Get end of validity
  virtual const ITime& validTill();

  /// Set validity range
  virtual void setValidity( const ITime& since, const ITime& till );  

  /// Set start of validity
  virtual void setValiditySince( const ITime& since );  

  /// Set end of validity
  virtual void setValidityTill( const ITime& till );   
  
  /// Update the validity range (foreseen for tree-like structures)
  virtual StatusCode updateValidity();
 
 private:

  // IValidity data

  /// Start of validity
  ITime* m_validSince;

  /// End of validity
  ITime* m_validTill;
  
};

#endif // DETDESC_VALIDDATAOBJECT_H
